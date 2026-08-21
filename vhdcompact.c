/*
 * ============================================================================
 * VHD Compaction Tool (Corrected & Spec-Compliant)
 * License: Public Domain
 * 
 * Compile instructions (GCC / MinGW-w64):
 * gcc vhdcompact.c -Os -s -o vhdcompact.exe -lole32 -mconsole
 * 
 * Usage:
 * vhdcompact.exe "C:\Path\To\VirtualDisk.vhd"
 * ============================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#pragma pack(push, 1)

/* VHD Footer structure (512 bytes) - must be at end of file */
typedef struct {
    char     magic[8];              /* "conectix" */
    uint32_t features;
    uint32_t version;
    uint64_t data_offset;
    uint32_t timestamp;
    char     creator_app[4];        
    uint32_t creator_version;
    uint32_t creator_host_os;       
    uint64_t original_size;
    uint64_t current_size;
    uint16_t geometry_cylinders;
    uint8_t  geometry_heads;
    uint8_t  geometry_sectors;
    uint32_t disk_type;             /* 2=dynamic, 3=differencing */
    uint32_t checksum;
    uint8_t  uuid[16];
    uint8_t  saved;
    uint8_t  reserved[427];
} VHD_FOOTER;

/* Dynamic Disk Header (1024 bytes) */
typedef struct {
    char     magic[8];              /* "cxsparse" */
    uint64_t data_offset;
    uint64_t table_offset;
    uint32_t version;
    uint32_t max_table_entries;
    uint32_t block_size;
    uint32_t checksum;
    uint8_t  parent_uuid[16];
    uint32_t parent_timestamp;
    uint32_t reserved1;
    char     parent_name[512];
    uint8_t  parent_locators[192];
    uint8_t  reserved2[256];
} DYNAMIC_HEADER;

#pragma pack(pop)

/* Constants */
#define VHD_FOOTER_SIZE         512
#define DYNAMIC_HEADER_SIZE     1024
#define BLOCK_SIZE_DEFAULT      (2 * 1024 * 1024) 
#define SECTOR_SIZE             512

/* Endianness Helper Macros */
#define BE32(x) __builtin_bswap32(x)
#define BE64(x) __builtin_bswap64(x)

/* Calculate VHD checksum (One's complement of byte sum) */
static uint32_t calculate_checksum(const void *data, size_t size) {
    const uint8_t *ptr = (const uint8_t *)data;
    uint32_t sum = 0;
    
    for (size_t i = 0; i < size; i++) {
        sum += ptr[i];
    }
    return ~sum;
}

/* Check if all zeros in buffer */
static BOOL is_all_zeros(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i += sizeof(uint64_t)) {
        if (((uint64_t*)buf)[i / sizeof(uint64_t)] != 0)
            return FALSE;
    }
    return TRUE;
}

/* Seek helper */
static BOOL seek_file(HANDLE hFile, uint64_t offset, DWORD moveMethod) {
    LARGE_INTEGER li;
    li.QuadPart = offset;
    return SetFilePointerEx(hFile, li, NULL, moveMethod);
}

static int compact_vhd(const wchar_t *input_path, const wchar_t *temp_path) {
    HANDLE hIn, hOut;
    VHD_FOOTER footer;
    DYNAMIC_HEADER dynHeader;
    uint32_t *bat = NULL, *newBatEntries = NULL;
    uint8_t *blockBuffer = NULL;
    DWORD bytesRead, bytesWritten;
    uint32_t writtenBlocks = 0;
    uint64_t totalBlocksAllocated = 0;
    
    hIn = CreateFileW(input_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hIn == INVALID_HANDLE_VALUE) {
        wprintf(L"Error: Cannot open input file.\n");
        return 1;
    }
    
    /* Read footer from end of file */
    seek_file(hIn, -(int64_t)VHD_FOOTER_SIZE, FILE_END);
    if (!ReadFile(hIn, &footer, VHD_FOOTER_SIZE, &bytesRead, NULL)) {
        wprintf(L"Error: Cannot read VHD footer\n");
        CloseHandle(hIn);
        return 1;
    }
    
    if (memcmp(footer.magic, "conectix", 8) != 0 || BE32(footer.disk_type) != 2) {
        wprintf(L"Error: Not a valid dynamic VHD file.\n");
        CloseHandle(hIn);
        return 1;
    }
    
    /* Safely seek to dynamic header using internal offset */
    seek_file(hIn, BE64(footer.data_offset), FILE_BEGIN);
    if (!ReadFile(hIn, &dynHeader, DYNAMIC_HEADER_SIZE, &bytesRead, NULL)) {
        wprintf(L"Error: Cannot read dynamic header\n");
        CloseHandle(hIn);
        return 1;
    }
    
    if (memcmp(dynHeader.magic, "cxsparse", 8) != 0) {
        wprintf(L"Error: Invalid dynamic disk header\n");
        CloseHandle(hIn);
        return 1;
    }
    
    /* Process Endianness */
    uint32_t blockCount = BE32(dynHeader.max_table_entries);
    uint32_t blockSizeDefault = BE32(dynHeader.block_size);
    if (blockSizeDefault == 0) blockSizeDefault = BLOCK_SIZE_DEFAULT;
    
    /* Calculate precise sizes (Blocks include a Sector Bitmap) */
    uint32_t bitmapSize = ((blockSizeDefault / SECTOR_SIZE) / 8 + SECTOR_SIZE - 1) / SECTOR_SIZE * SECTOR_SIZE;
    uint32_t totalBlockSize = bitmapSize + blockSizeDefault;
    uint32_t batSize = (blockCount * sizeof(uint32_t) + SECTOR_SIZE - 1) / SECTOR_SIZE * SECTOR_SIZE;
    
    bat = (uint32_t *)malloc(batSize);
    newBatEntries = (uint32_t *)malloc(batSize);
    blockBuffer = (uint8_t *)malloc(totalBlockSize);
    
    if (!bat || !newBatEntries || !blockBuffer) {
        wprintf(L"Error: Memory allocation failed\n");
        goto cleanup;
    }
    
    /* Read BAT */
    seek_file(hIn, BE64(dynHeader.table_offset), FILE_BEGIN);
    if (!ReadFile(hIn, bat, batSize, &bytesRead, NULL)) {
        wprintf(L"Error: Cannot read BAT\n");
        goto cleanup;
    }
    
    for (uint32_t i = 0; i < blockCount; i++) {
        if (BE32(bat[i]) != 0xFFFFFFFF) totalBlocksAllocated++;
        newBatEntries[i] = 0xFFFFFFFF; /* Default to unallocated */
    }
    wprintf(L"Total blocks: %u, Allocated blocks: %llu\n", blockCount, totalBlocksAllocated);
    
    hOut = CreateFileW(temp_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hOut == INVALID_HANDLE_VALUE) {
        wprintf(L"Error: Cannot create output file.\n");
        goto cleanup;
    }
    
    /* File Layout: Footer Copy (512) -> DynHeader (1024) -> BAT -> Data Blocks -> Footer (512) */
    uint64_t currentWriteOffset = VHD_FOOTER_SIZE + DYNAMIC_HEADER_SIZE;
    uint64_t newTableOffset = currentWriteOffset;
    currentWriteOffset += batSize; 
    
    wprintf(L"Processing blocks...\n");
    for (uint32_t i = 0; i < blockCount; i++) {
        uint32_t oldSectorOffset = BE32(bat[i]);
        if (oldSectorOffset == 0xFFFFFFFF) continue;
        
        seek_file(hIn, (uint64_t)oldSectorOffset * SECTOR_SIZE, FILE_BEGIN);
        if (!ReadFile(hIn, blockBuffer, totalBlockSize, &bytesRead, NULL)) {
            wprintf(L"Warning: Failed to read block %u\n", i);
            continue;
        }
        
        /* Only check the DATA for zeros, skip the sector bitmap */
        if (is_all_zeros(blockBuffer + bitmapSize, blockSizeDefault)) continue;
        
        /* Write to new file */
        newBatEntries[i] = (uint32_t)(currentWriteOffset / SECTOR_SIZE);
        seek_file(hOut, currentWriteOffset, FILE_BEGIN);
        
        if (!WriteFile(hOut, blockBuffer, totalBlockSize, &bytesWritten, NULL)) {
            wprintf(L"Error: Failed to write block %u\n", i);
            CloseHandle(hOut);
            goto cleanup;
        }
        writtenBlocks++;
        currentWriteOffset += totalBlockSize;
        
        if ((i + 1) % 100 == 0) wprintf(L"Progress: %u/%u blocks processed\n", i + 1, blockCount);
    }
    
    /* Finalize and Write new BAT (Must be Big-Endian) */
    seek_file(hOut, newTableOffset, FILE_BEGIN);
    for (uint32_t i = 0; i < blockCount; i++) {
        newBatEntries[i] = BE32(newBatEntries[i]);
    }
    WriteFile(hOut, newBatEntries, batSize, &bytesWritten, NULL);
    
    /* Update and Write Dynamic Header */
    dynHeader.table_offset = BE64(newTableOffset);
    dynHeader.checksum = 0;
    dynHeader.checksum = BE32(calculate_checksum(&dynHeader, DYNAMIC_HEADER_SIZE));
    seek_file(hOut, VHD_FOOTER_SIZE, FILE_BEGIN);
    WriteFile(hOut, &dynHeader, DYNAMIC_HEADER_SIZE, &bytesWritten, NULL);
    
    /* Update and Write Footer (at beginning and end) */
    footer.data_offset = BE64(VHD_FOOTER_SIZE);
    footer.checksum = 0;
    footer.checksum = BE32(calculate_checksum(&footer, VHD_FOOTER_SIZE));
    
    seek_file(hOut, 0, FILE_BEGIN);
    WriteFile(hOut, &footer, VHD_FOOTER_SIZE, &bytesWritten, NULL);
    
    seek_file(hOut, currentWriteOffset, FILE_BEGIN);
    WriteFile(hOut, &footer, VHD_FOOTER_SIZE, &bytesWritten, NULL);
    
    seek_file(hOut, currentWriteOffset + VHD_FOOTER_SIZE, FILE_BEGIN);
    SetEndOfFile(hOut);
    
    CloseHandle(hOut);
    wprintf(L"Compaction complete! Original blocks: %u, Written blocks: %u\n", blockCount, writtenBlocks);

cleanup:
    if (bat) free(bat);
    if (newBatEntries) free(newBatEntries);
    if (blockBuffer) free(blockBuffer);
    CloseHandle(hIn);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <path_to_vhd_file>\n", argv[0]);
        return 1;
    }
    
    printf("WARNING: Manual VHD compaction carries risk of data loss!\n");
    printf("Make a backup before proceeding.\n\n");
    
    wchar_t input_path[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, argv[1], -1, input_path, MAX_PATH);
    
    wchar_t temp_path[MAX_PATH];
    swprintf(temp_path, MAX_PATH, L"%ls.compacting", input_path);
    
    int result = compact_vhd(input_path, temp_path);
    
    if (result == 0) {
        DeleteFileW(input_path);
        MoveFileW(temp_path, input_path);
        wprintf(L"Original file replaced successfully!\n");
    }
    
    return result;
}