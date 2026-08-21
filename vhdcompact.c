/*
 * ============================================================================
 * VHD/VHDX Compaction Tool
 * License: Public Domain
 * 
 * Compile instructions (GCC / MinGW-w64):
 * gcc vhdcompact.c -o vhdcompact.exe -lvirtdisk -lole32
 * 
 * Usage:
 * vhdcompact.exe "C:\Path\To\VirtualDisk.vhd"
 * ============================================================================
 */

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

// Require at least Windows 7 / Server 2008 R2 for Virtual Disk API
#define _WIN32_WINNT 0x0601 

#include <windows.h>
#include <virtdisk.h>
#include <stdio.h>
#include <stdlib.h>

int wmain(int argc, wchar_t *argv[]) {
    if (argc < 2) {
        wprintf(L"Usage: %s <Path to VHD/VHDX file>\n", argv[0]);
        wprintf(L"Note: The VHD must not be in use (mounted) by another process.\n");
        return 1;
    }

    LPCWSTR vhdPath = argv[1];
    HANDLE hVhd = INVALID_HANDLE_VALUE;
    DWORD result;

    // 1. Configure the storage type. 
    // Using UNKNOWN allows the API to auto-detect VHD vs VHDX.
    VIRTUAL_STORAGE_TYPE storageType;
    storageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN;
    memset(&storageType.VendorId, 0, sizeof(GUID)); // GUID_NULL

    // 2. Configure open parameters
    OPEN_VIRTUAL_DISK_PARAMETERS openParams;
    memset(&openParams, 0, sizeof(openParams));
    openParams.Version = OPEN_VIRTUAL_DISK_VERSION_1;

    // 3. Open the Virtual Disk
    // VIRTUAL_DISK_ACCESS_METAOPS is the required access mask for compaction
    wprintf(L"Opening virtual disk: %s\n", vhdPath);
    result = OpenVirtualDisk(
        &storageType,
        vhdPath,
        VIRTUAL_DISK_ACCESS_METAOPS, 
        OPEN_VIRTUAL_DISK_FLAG_NONE,
        &openParams,
        &hVhd
    );

    if (result != ERROR_SUCCESS) {
        wprintf(L"Error: Failed to open virtual disk. Win32 Error Code: %lu\n", result);
        if (result == ERROR_SHARING_VIOLATION) {
            wprintf(L"Hint: The file is in use. Make sure the VM is off and the VHD is unmounted.\n");
        }
        return 1;
    }

    // 4. Configure compact parameters
    COMPACT_VIRTUAL_DISK_PARAMETERS compactParams;
    memset(&compactParams, 0, sizeof(compactParams));
    compactParams.Version = COMPACT_VIRTUAL_DISK_VERSION_1;

    // 5. Compact the Virtual Disk
    wprintf(L"Compacting virtual disk... This may take a while depending on file size.\n");
    result = CompactVirtualDisk(
        hVhd,
        COMPACT_VIRTUAL_DISK_FLAG_NONE,
        &compactParams,
        NULL // Run synchronously
    );

    if (result != ERROR_SUCCESS) {
        wprintf(L"Error: Failed to compact virtual disk. Win32 Error Code: %lu\n", result);
        CloseHandle(hVhd);
        return 1;
    }

    wprintf(L"Success: Virtual disk compacted successfully!\n");

    // 6. Cleanup
    CloseHandle(hVhd);
    return 0;
}