/*
 * ============================================================================
 * VHD/VHDX Native Retrim & Compaction Tool
 * License: Public Domain
 * 
 * Compile instructions (GCC / MinGW-w64):
 * gcc vhdtrim.c -o vhdtrim.exe -lvirtdisk -lole32
 * 
 * Usage:
 * vhdcompact.exe "C:\Path\To\VirtualDisk.vhd"
 * 
 * Note: Must be run as Administrator!
 * ============================================================================
 */

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

// Require at least Windows 8 / Server 2012 for Advanced VHD/TRIM APIs
#define _WIN32_WINNT 0x0602 

#include <windows.h>
#include <virtdisk.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

// ----------------------------------------------------------------------------
// Custom DSM Structs (Avoids GCC/MinGW header conflicts on older versions)
// ----------------------------------------------------------------------------
#define MY_IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES \
    CTL_CODE(IOCTL_STORAGE_BASE, 0x0501, METHOD_BUFFERED, FILE_WRITE_ACCESS)

typedef struct _MY_DEVICE_MANAGE_DATA_SET_ATTRIBUTES {
    DWORD Size;
    DWORD Action;
    DWORD Flags;
    DWORD ParameterBlockOffset;
    DWORD ParameterBlockLength;
    DWORD DataSetRangesOffset;
    DWORD DataSetRangesLength;
} MY_DEVICE_MANAGE_DATA_SET_ATTRIBUTES;

typedef struct _MY_DEVICE_DATA_SET_RANGE {
    LONGLONG  StartingOffset;
    DWORDLONG LengthInBytes;
} MY_DEVICE_DATA_SET_RANGE;

#define MAX_TRIM_RANGES 512

typedef struct _MY_TRIM_DSM_INPUT {
    MY_DEVICE_MANAGE_DATA_SET_ATTRIBUTES Dsm;
    MY_DEVICE_DATA_SET_RANGE             Ranges[MAX_TRIM_RANGES];
} MY_TRIM_DSM_INPUT;

// ----------------------------------------------------------------------------
// Privilege Check
// ----------------------------------------------------------------------------
BOOL IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }
    return fRet;
}

// ----------------------------------------------------------------------------
// Native NTFS Volume Retrim
// ----------------------------------------------------------------------------
void TrimNTFSVolume(LPCWSTR volumeName) {
    wprintf(L"  -> Analyzing & Trimming Volume: %s\n", volumeName);

    // 1. Open volume for Read/Write to send IOCTLs
    HANDLE hVol = CreateFileW(volumeName, GENERIC_READ | GENERIC_WRITE, 
                              FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
                              OPEN_EXISTING, 0, NULL);
    if (hVol == INVALID_HANDLE_VALUE) {
        wprintf(L"     [!] Failed to open volume. Error: %lu\n", GetLastError());
        return;
    }

    // 2. Read NTFS Volume Data
    NTFS_VOLUME_DATA_BUFFER ntfsData = {0};
    DWORD bytesReturned;
    if (!DeviceIoControl(hVol, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, 
                         &ntfsData, sizeof(ntfsData), &bytesReturned, NULL)) {
        wprintf(L"     [!] Not an NTFS volume or cannot read metadata.\n");
        CloseHandle(hVol);
        return;
    }

    DWORD bytesPerCluster = ntfsData.BytesPerCluster;
    wprintf(L"     [i] Cluster size: %lu. Total clusters: %llu\n", bytesPerCluster, ntfsData.TotalClusters.QuadPart);

    // Buffer for 1MB of bitmap data (approx 8 million clusters mapped at a time)
    DWORD bitmapBufSize = sizeof(VOLUME_BITMAP_BUFFER) + (1024 * 1024);
    PVOLUME_BITMAP_BUFFER bitmap = (PVOLUME_BITMAP_BUFFER)malloc(bitmapBufSize);
    
    STARTING_LCN_INPUT_BUFFER inputLcn = {0};
    inputLcn.StartingLcn.QuadPart = 0;

    MY_TRIM_DSM_INPUT trimInput = {0};
    trimInput.Dsm.Size = sizeof(MY_DEVICE_MANAGE_DATA_SET_ATTRIBUTES);
    trimInput.Dsm.Action = 1; // DeviceDsmAction_Trim
    trimInput.Dsm.DataSetRangesOffset = (DWORD)offsetof(MY_TRIM_DSM_INPUT, Ranges);

    int rangeCount = 0;
    LONGLONG currentFreeLcn = -1;
    LONGLONG freeLcnCount = 0;
    LONGLONG totalTrimmedBytes = 0;

    wprintf(L"     [*] Scanning kernel allocation bitmap and dispatching TRIM commands...\n");

    // 3. Iterate over the entire disk bitmap
    while (1) {
        BOOL bRet = DeviceIoControl(hVol, FSCTL_GET_VOLUME_BITMAP,
            &inputLcn, sizeof(inputLcn),
            bitmap, bitmapBufSize,
            &bytesReturned, NULL);
        
        DWORD err = GetLastError();
        if (!bRet && err != ERROR_MORE_DATA && err != ERROR_SUCCESS) {
            break; // Finished or Error
        }

        // Calculate valid bits mapped in the returned buffer chunk safely
        LONGLONG bytesOfBitmap = bytesReturned - offsetof(VOLUME_BITMAP_BUFFER, Buffer);
        LONGLONG validBits = bytesOfBitmap * 8;
        if (validBits > bitmap->BitmapSize.QuadPart) {
            validBits = bitmap->BitmapSize.QuadPart;
        }

        // Parse bit by bit
        for (LONGLONG bit = 0; bit < validBits; bit++) {
            LONGLONG byteIdx = bit / 8;
            int bitIdx = bit % 8;
            BOOL isAllocated = (bitmap->Buffer[byteIdx] & (1 << bitIdx)) != 0;

            if (!isAllocated) {
                if (currentFreeLcn == -1) {
                    currentFreeLcn = bitmap->StartingLcn.QuadPart + bit; // Start of a free block
                    freeLcnCount = 1;
                } else {
                    freeLcnCount++; // Continue free block
                }
            } else {
                if (currentFreeLcn != -1) {
                    // We hit allocated space, commit the previous contiguous free range
                    trimInput.Ranges[rangeCount].StartingOffset = currentFreeLcn * bytesPerCluster;
                    trimInput.Ranges[rangeCount].LengthInBytes = freeLcnCount * bytesPerCluster;
                    rangeCount++;
                    totalTrimmedBytes += (freeLcnCount * bytesPerCluster);

                    // If range buffer is full, dispatch to disk
                    if (rangeCount == MAX_TRIM_RANGES) {
                        trimInput.Dsm.DataSetRangesLength = rangeCount * sizeof(MY_DEVICE_DATA_SET_RANGE);
                        DeviceIoControl(hVol, MY_IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES, 
                                        &trimInput, sizeof(trimInput), NULL, 0, &bytesReturned, NULL);
                        rangeCount = 0;
                    }
                    currentFreeLcn = -1;
                    freeLcnCount = 0;
                }
            }
        }

        if (!bRet && err == ERROR_MORE_DATA) {
            inputLcn.StartingLcn.QuadPart = bitmap->StartingLcn.QuadPart + validBits;
        } else {
            break; // No more data to process
        }
    }

    // Flush any remaining trailing free ranges
    if (currentFreeLcn != -1) {
        trimInput.Ranges[rangeCount].StartingOffset = currentFreeLcn * bytesPerCluster;
        trimInput.Ranges[rangeCount].LengthInBytes = freeLcnCount * bytesPerCluster;
        rangeCount++;
        totalTrimmedBytes += (freeLcnCount * bytesPerCluster);
    }
    if (rangeCount > 0) {
        trimInput.Dsm.DataSetRangesLength = rangeCount * sizeof(MY_DEVICE_DATA_SET_RANGE);
        DeviceIoControl(hVol, MY_IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES, 
                        &trimInput, sizeof(trimInput), NULL, 0, &bytesReturned, NULL);
    }

    free(bitmap);
    CloseHandle(hVol);
    
    wprintf(L"     [+] Retrim complete! Trimmed approx %.2f MB.\n", (double)totalTrimmedBytes / (1024.0 * 1024.0));
}

// ----------------------------------------------------------------------------
// Main Application
// ----------------------------------------------------------------------------
int wmain(int argc, wchar_t *argv[]) {
    if (argc < 2) {
        wprintf(L"Usage: %s <Path to VHD/VHDX file>\n", argv[0]);
        return 1;
    }

    if (!IsElevated()) {
        wprintf(L"ERROR: This tool requires Administrator privileges to mount and trim volumes.\n");
        return 1;
    }

    LPCWSTR vhdPath = argv[1];
    HANDLE hVhd = INVALID_HANDLE_VALUE;
    DWORD result;

    VIRTUAL_STORAGE_TYPE storageType;
    storageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN;
    memset(&storageType.VendorId, 0, sizeof(GUID)); 

    OPEN_VIRTUAL_DISK_PARAMETERS openParams = {0};
    openParams.Version = OPEN_VIRTUAL_DISK_VERSION_1;

    wprintf(L"1. Opening virtual disk: %s\n", vhdPath);
    result = OpenVirtualDisk(&storageType, vhdPath, VIRTUAL_DISK_ACCESS_ALL, OPEN_VIRTUAL_DISK_FLAG_NONE, &openParams, &hVhd);
    if (result != ERROR_SUCCESS) {
        wprintf(L"Error: Failed to open virtual disk. Win32 Code: %lu\n", result);
        return 1;
    }

    wprintf(L"2. Attaching virtual disk for native retrim...\n");
    ATTACH_VIRTUAL_DISK_PARAMETERS attachParams = {0};
    attachParams.Version = ATTACH_VIRTUAL_DISK_VERSION_1;
    
    // Attach without assigning drive letters to avoid cluttering Explorer
    result = AttachVirtualDisk(hVhd, NULL, ATTACH_VIRTUAL_DISK_FLAG_NO_DRIVE_LETTER, 0, &attachParams, NULL);
    
    if (result == ERROR_SUCCESS) {
        wprintf(L"   Virtual disk attached. Waiting 3 seconds for OS to map volumes...\n");
        Sleep(3000); 

        WCHAR physicalDrivePath[MAX_PATH];
        DWORD pathSize = sizeof(physicalDrivePath);
        if (GetVirtualDiskPhysicalPath(hVhd, &pathSize, physicalDrivePath) == ERROR_SUCCESS) {
            
            int vhdDiskNumber = -1;
            swscanf(physicalDrivePath, L"\\\\.\\PhysicalDrive%d", &vhdDiskNumber);
            
            if (vhdDiskNumber != -1) {
                wprintf(L"3. Scanning volumes on physical disk %d...\n", vhdDiskNumber);
                
                WCHAR volName[MAX_PATH];
                HANDLE hVolEnum = FindFirstVolumeW(volName, MAX_PATH);
                if (hVolEnum != INVALID_HANDLE_VALUE) {
                    do {
                        // Strip trailing backslash required by CreateFile
                        size_t len = wcslen(volName);
                        if (len > 0 && volName[len-1] == L'\\') volName[len-1] = L'\0';
                        
                        HANDLE hVol = CreateFileW(volName, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                        if (hVol != INVALID_HANDLE_VALUE) {
                            VOLUME_DISK_EXTENTS extents = {0};
                            DWORD retBytes;
                            if (DeviceIoControl(hVol, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &extents, sizeof(extents), &retBytes, NULL)) {
                                for (DWORD i = 0; i < extents.NumberOfDiskExtents; i++) {
                                    if (extents.Extents[i].DiskNumber == (DWORD)vhdDiskNumber) {
                                        TrimNTFSVolume(volName); // Found a volume on our VHD! Retrim it.
                                        break;
                                    }
                                }
                            }
                            CloseHandle(hVol);
                        }
                        volName[len-1] = L'\\'; // Put trailing slash back for FindNextVolume
                    } while (FindNextVolumeW(hVolEnum, volName, MAX_PATH));
                    FindVolumeClose(hVolEnum);
                }
            }
        } else {
            wprintf(L"   [Warning] Could not get VHD physical path. Skipping retrim.\n");
        }

        wprintf(L"4. Detaching virtual disk...\n");
        DetachVirtualDisk(hVhd, DETACH_VIRTUAL_DISK_FLAG_NONE, 0);
    } else {
        wprintf(L"   [Warning] Failed to attach VHD (Code %lu). Skipping retrim.\n", result);
    }

    wprintf(L"5. Compacting virtual disk...\n");
    COMPACT_VIRTUAL_DISK_PARAMETERS compactParams = {0};
    compactParams.Version = COMPACT_VIRTUAL_DISK_VERSION_1;
    
    result = CompactVirtualDisk(hVhd, COMPACT_VIRTUAL_DISK_FLAG_NONE, &compactParams, NULL);
    
    if (result != ERROR_SUCCESS) {
        wprintf(L"Error: Compaction failed. Win32 Error Code: %lu\n", result);
    } else {
        wprintf(L"Success: Virtual disk compacted successfully!\n");
    }

    CloseHandle(hVhd);
    return 0;
}