/*
 * FreeDOS VHD Image Creator
 * Converts raw disk images (.img) or Physical MS-DOS drives to Microsoft VHD
 * Environment: MS-DOS / DJGPP (GCC for DOS)
 * Compile: gcc -o vhdmaker.exe vhdmaker.c -O2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

/* Include DPMI headers if compiling under MS-DOS GCC (DJGPP) */
#ifdef __DJGPP__
#include <dpmi.h>
#include <go32.h>
#include <sys/farptr.h>
#include <sys/movedata.h>
#endif

#define VHD_FOOTER_SIZE     512
#define VHD_SIGNATURE       "conectix"
#define VHD_FEATURE_NONE    0x00000000
#define VHD_FEATURE_SAVE    0x00000001
#define VHD_VERSION         0x00010002
#define VHD_TYPE_FIXED      2
#define VHD_TYPE_DYNAMIC    3 

/* VHD Footer structure (512 bytes) - must be packed */
typedef struct __attribute__((packed)) {
    char        signature[8];
    uint8_t     features[4];
    uint8_t     version[4];
    uint8_t     data_offset[8];
    uint8_t     creation_time[4];
    char        creator_app[4];
    uint8_t     version_major[2];
    uint8_t     version_minor[2];
    char        creator_os[4];
    uint8_t     orig_size[8];
    uint8_t     current_size[8];
    uint8_t     cylinders[2];
    uint8_t     heads[1];
    uint8_t     sectors_per_track[1];
    uint8_t     sector_type[4];
    uint8_t     unique_id[16];
    uint8_t     saved_state[1];
    uint8_t     reserved[427];
    uint8_t     checksum[4];
} vhdf_footer_t;

/* Function prototypes */
void print_usage(const char *prog);
uint32_t calculate_checksum(uint8_t *data, uint32_t len);
void write_be32(uint8_t *buf, uint32_t val);
void write_be64(uint8_t *buf, uint64_t val);
void fill_footer(vhdf_footer_t *footer, uint64_t orig_size, 
                 uint64_t current_size, time_t timestamp, uint8_t vhd_type);
void list_drives(void);
int get_drive_info(uint8_t drive_num, uint64_t *total_sectors, uint16_t *sector_size);
int read_physical_sectors(uint8_t drive_num, uint64_t lba, uint16_t count, void *buffer, int dos_seg);

int main(int argc, char *argv[])
{
    FILE *src_file = NULL;
    FILE *dst_file = NULL;
    char *input_file = NULL;
    char *output_file = NULL;
    uint8_t vhd_type = VHD_TYPE_FIXED;
    
    int is_physical = 0;
    uint8_t drive_num = 0x80;
    int dos_sel = -1, dos_seg = -1;

    /* Max 64 sectors (32KB) per read to avoid old BIOS limits */
    size_t chunk_sectors = 64; 
    size_t buffer_size = chunk_sectors * 512;          
    unsigned char *buffer = NULL;
    uint64_t source_size = 0;
    size_t bytes_read, total_written = 0;
    vhdf_footer_t footer;
    time_t timestamp;
    int i;

    for (i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0) {
            list_drives();
            return 0;
        } else if ((strcmp(argv[i], "-f") == 0) && (i + 1 < argc)) {
            vhd_type = VHD_TYPE_FIXED;
        } else if ((strcmp(argv[i], "-d") == 0) && (i + 1 < argc)) {
            vhd_type = VHD_TYPE_DYNAMIC;
        } else if (strncmp(argv[i], "-", 1) == 0) {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        } else if (input_file == NULL) {
            input_file = argv[i];
        } else if (output_file == NULL) {
            output_file = argv[i];
        } else {
            fprintf(stderr, "Too many arguments\n");
            print_usage(argv[0]);
            return 1;
        }
    }

    if (input_file == NULL || output_file == NULL) {
        fprintf(stderr, "Error: Input and output paths required\n");
        print_usage(argv[0]);
        return 1;
    }

    /* Check if input is a physical drive (e.g. hd0, hd1) */
    if (strncmp(input_file, "hd", 2) == 0 && input_file[2] >= '0' && input_file[2] <= '9') {
        is_physical = 1;
        drive_num = 0x80 + (input_file[2] - '0');
    }

    printf("FreeDOS VHD Image Creator v1.1\n");
    printf("==================================\n");
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n", output_file);
    printf("Type:   %s\n", vhd_type == VHD_TYPE_FIXED ? "Fixed" : "Dynamic");

    if (is_physical) {
#ifndef __DJGPP__
        fprintf(stderr, "Error: Physical drive access requires MS-DOS and DJGPP compiler.\n");
        return 1;
#else
        uint64_t total_sectors;
        uint16_t sector_size;
        
        if (!get_drive_info(drive_num, &total_sectors, &sector_size)) {
            fprintf(stderr, "Error: BIOS failed to read physical drive 0x%02X\n", drive_num);
            return 1;
        }
        
        source_size = total_sectors * sector_size;
        
        /* Allocate DOS conventional memory for INT 13h buffer (16 bytes DAP + 32KB data) */
        dos_seg = __dpmi_allocate_dos_memory((16 + buffer_size + 15) / 16, &dos_sel);
        if (dos_seg == -1) {
            fprintf(stderr, "Error: Could not allocate DOS memory for disk transfers.\n");
            return 1;
        }
#endif
    } else {
        struct stat st;
        if (stat(input_file, &st) != 0) {
            fprintf(stderr, "Error: Cannot access input file '%s': %s\n", input_file, strerror(errno));
            return 1;
        }
        source_size = st.st_size;
        
        src_file = fopen(input_file, "rb");
        if (src_file == NULL) {
            fprintf(stderr, "Error: Cannot open input file: %s\n", strerror(errno));
            return 1;
        }
    }

    printf("Size:   %llu bytes (%.2f MB)\n", (unsigned long long)source_size, source_size / (1024.0 * 1024.0));

    buffer = malloc(buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory\n");
        return 1;
    }

    dst_file = fopen(output_file, "wb");
    if (dst_file == NULL) {
        fprintf(stderr, "Error: Cannot create output file: %s\n", strerror(errno));
        return 1;
    }

    timestamp = time(NULL);
    memset(&footer, 0, sizeof(footer));
    fill_footer(&footer, source_size, source_size, timestamp, vhd_type);

    printf("\nConverting...\n");

    /* Main copying loop */
    uint64_t offset = 0;
    while (offset < source_size) {
        size_t chunk = (source_size - offset > buffer_size) ? buffer_size : (source_size - offset);
        
        if (is_physical) {
            uint64_t lba = offset / 512;
            uint16_t count = chunk / 512;
            
            if (!read_physical_sectors(drive_num, lba, count, buffer, dos_seg)) {
                fprintf(stderr, "\nError: Physical read failed at LBA %llu\n", (unsigned long long)lba);
                goto cleanup;
            }
            bytes_read = chunk;
        } else {
            bytes_read = fread(buffer, 1, chunk, src_file);
            if (bytes_read == 0) break;
        }

        if (fwrite(buffer, 1, bytes_read, dst_file) != bytes_read) {
            fprintf(stderr, "\nError: Write to VHD failed\n");
            goto cleanup;
        }
        
        total_written += bytes_read;
        offset += bytes_read;
        
        if (total_written % (1024 * 1024) < buffer_size) {
            printf("  %llu MB written...\r", (unsigned long long)(total_written / (1024 * 1024)));
            fflush(stdout);
        }
    }

    printf("  Done!                            \n");

    /* Fixed VHD requires ONLY a footer appended at the very end of the file */
    if (vhd_type == VHD_TYPE_FIXED) {
        printf("Appending VHD footer... ");
        if (fwrite(&footer, 1, VHD_FOOTER_SIZE, dst_file) != VHD_FOOTER_SIZE) {
            fprintf(stderr, "Failed\n");
            goto cleanup;
        }
        printf("Done!\n");
    }

cleanup:
    if (src_file) fclose(src_file);
    if (dst_file) fclose(dst_file);
    if (buffer) free(buffer);
#ifdef __DJGPP__
    if (dos_sel != -1) __dpmi_free_dos_memory(dos_sel);
#endif

    return 0;
}

void print_usage(const char *prog)
{
    printf("Usage: %s [options] <input> <output.vhd>\n", prog);
    printf("\nOptions:\n");
    printf("  -l, --list  List physical BIOS hard drives and exit\n");
    printf("  -f          Fixed-size VHD (default, recommended)\n");
    printf("  -h          Show this help message\n");
    printf("\nInput Types:\n");
    printf("  File:       Any .img or .raw disk file\n");
    printf("  Physical:   hd0 (1st drive), hd1 (2nd drive), etc.\n");
    printf("\nExamples:\n");
    printf("  %s freedos.img freedos.vhd\n", prog);
    printf("  %s hd0 backup.vhd\n", prog);
}

/* -------------------------------------------------------------
 *  MS-DOS DJGPP PHYSICAL DRIVE ROUTINES
 * ------------------------------------------------------------- */

void list_drives(void)
{
#ifndef __DJGPP__
    printf("Physical drive listing requires MS-DOS and DJGPP.\n");
#else
    uint8_t drive;
    int found = 0;
    
    printf("Detected BIOS Hard Drives:\n");
    printf("--------------------------\n");
    
    for (drive = 0x80; drive <= 0x8F; drive++) {
        uint64_t sectors;
        uint16_t sector_size;
        
        if (get_drive_info(drive, &sectors, &sector_size)) {
            uint64_t total_bytes = sectors * sector_size;
            printf("  hd%d (0x%02X) : %llu bytes (%.2f GB)\n", 
                   drive - 0x80, drive, (unsigned long long)total_bytes, 
                   (double)total_bytes / (1024.0 * 1024.0 * 1024.0));
            found++;
        }
    }
    
    if (found == 0) printf("No extended INT13h hard drives found.\n");
#endif
}

int get_drive_info(uint8_t drive_num, uint64_t *total_sectors, uint16_t *sector_size)
{
#ifdef __DJGPP__
    __dpmi_regs regs = {0};
    int dos_sel, dos_seg;
    
    /* Allocate 0x42 bytes in DOS Memory for result buffer */
    dos_seg = __dpmi_allocate_dos_memory((0x42 + 15) / 16, &dos_sel);
    if (dos_seg == -1) return 0;

    _farpokew(_dos_ds, dos_seg * 16, 0x0042); /* Indicate size of buffer */

    regs.h.ah = 0x48; /* Get Drive Parameters Extension */
    regs.h.dl = drive_num;
    regs.x.ds = dos_seg;
    regs.x.si = 0;
    __dpmi_simulate_real_mode_interrupt(0x13, &regs);

    int success = 0;
    if ((regs.x.flags & 1) == 0 && regs.h.ah == 0) {
        /* Read total sectors (QWORD) and sector size (WORD) from returned struct */
        *total_sectors = _farpeekl(_dos_ds, dos_seg * 16 + 16) | 
                         ((uint64_t)_farpeekl(_dos_ds, dos_seg * 16 + 20) << 32);
        *sector_size = _farpeekw(_dos_ds, dos_seg * 16 + 24);
        success = 1;
    }
    
    __dpmi_free_dos_memory(dos_sel);
    return success;
#else
    return 0;
#endif
}

int read_physical_sectors(uint8_t drive_num, uint64_t lba, uint16_t count, void *buffer, int dos_seg)
{
#ifdef __DJGPP__
    __dpmi_regs regs = {0};
    
    /* Disk Address Packet (DAP) layout at dos_seg:0x0000 */
    _farpokeb(_dos_ds, dos_seg * 16 + 0, 0x10);       /* DAP size */
    _farpokeb(_dos_ds, dos_seg * 16 + 1, 0);          /* Reserved */
    _farpokew(_dos_ds, dos_seg * 16 + 2, count);      /* Sectors to read */
    _farpokew(_dos_ds, dos_seg * 16 + 4, 16);         /* Buffer Offset: 0x0010 */
    _farpokew(_dos_ds, dos_seg * 16 + 6, dos_seg);    /* Buffer Segment */
    _farpokel(_dos_ds, dos_seg * 16 + 8, (uint32_t)lba);           /* LBA Low */
    _farpokel(_dos_ds, dos_seg * 16 + 12, (uint32_t)(lba >> 32));  /* LBA High */

    regs.h.ah = 0x42; /* Extended Read */
    regs.h.dl = drive_num;
    regs.x.ds = dos_seg;
    regs.x.si = 0;    /* DAP is at ds:si (dos_seg:0000) */
    
    __dpmi_simulate_real_mode_interrupt(0x13, &regs);

    if (regs.x.flags & 1) return 0; /* Carry Flag set = Error */

    /* Retrieve chunk from dos_seg:0x0010 */
    dosmemget(dos_seg * 16 + 16, count * 512, buffer);
    return 1;
#else
    return 0;
#endif
}

/* -------------------------------------------------------------
 *  VHD FORMATTING ROUTINES
 * ------------------------------------------------------------- */

void write_be32(uint8_t *buf, uint32_t val)
{
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

void write_be64(uint8_t *buf, uint64_t val)
{
    buf[0] = (val >> 56) & 0xFF;
    buf[1] = (val >> 48) & 0xFF;
    buf[2] = (val >> 40) & 0xFF;
    buf[3] = (val >> 32) & 0xFF;
    buf[4] = (val >> 24) & 0xFF;
    buf[5] = (val >> 16) & 0xFF;
    buf[6] = (val >> 8) & 0xFF;
    buf[7] = val & 0xFF;
}

uint32_t calculate_checksum(uint8_t *data, uint32_t len)
{
    uint32_t sum = 0;
    uint32_t i;
    for (i = 0; i < len; i++) sum += data[i];
    return ~sum;
}

void fill_footer(vhdf_footer_t *footer, uint64_t orig_size,
                 uint64_t current_size, time_t timestamp, uint8_t vhd_type)
{
    memcpy(footer->signature, VHD_SIGNATURE, 8);
    write_be32(footer->features, vhd_type == VHD_TYPE_FIXED ? VHD_FEATURE_NONE : VHD_FEATURE_SAVE);
    write_be32(footer->version, VHD_VERSION);
    write_be64(footer->data_offset, 0xFFFFFFFFFFFFFFFFULL);
    
    /* VHD timestamp is seconds since Jan 1 2000 12:00:00 AM UTC */
    uint32_t vhd_time = (uint32_t)(timestamp - 946684800);
    write_be32(footer->creation_time, vhd_time);
    
    memcpy(footer->creator_app, "win9x", 4);
    write_be32(footer->version_minor, VHD_VERSION);
    memcpy(footer->creator_os, "W2k ", 4);
    write_be64(footer->orig_size, orig_size);
    write_be64(footer->current_size, current_size);
    
    /* Geometry */
    if (orig_size >= 1024ULL * 1024 * 63 * 16) {
        uint64_t cylinders = (orig_size + 63) / (1024 * 63 * 16);
        footer->cylinders[0] = (cylinders >> 8) & 0xFF;
        footer->cylinders[1] = cylinders & 0xFF;
        footer->heads[0] = 16;
        footer->sectors_per_track[0] = 63;
    } else {
        uint64_t sectors = (orig_size + 511) / 512;
        footer->cylinders[0] = 0;
        footer->cylinders[1] = (sectors <= 1024) ? 1 : 2;
        footer->heads[0] = 2;
        footer->sectors_per_track[0] = sectors > 1024 ? 32 : 17;
    }
    
    write_be32(footer->sector_type, vhd_type);
    
    /* Unique ID */
    uint32_t id_val = vhd_time ^ 0xDEADBEEF;
    footer->unique_id[0] = id_val & 0xFF;
    footer->unique_id[1] = (id_val >> 8) & 0xFF;
    footer->unique_id[2] = (id_val >> 16) & 0xFF;
    footer->unique_id[3] = (id_val >> 24) & 0xFF;
    
    footer->saved_state[0] = 0;
    
    /* Checksum (Computed after everything is set to 0) */
    memset(footer->checksum, 0, 4);
    uint32_t chksum = calculate_checksum((uint8_t *)footer, VHD_FOOTER_SIZE);
    write_be32(footer->checksum, chksum);
}