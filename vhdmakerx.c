/*
 * FreeDOS VHD Image Creator with File Exclusion Support
 * Converts raw disk images (.img) or physical MS-DOS drives to Microsoft VHD.
 * Supports zeroing out specified files on FAT32 and NTFS partitions.
 *
 * Environment: MS-DOS / DJGPP (GCC for DOS)
 * Compile: gcc -o vhdmaker vhdmaker.c -O2
 *# Convert primary physical drive hd0 to disk.vhd and zero out PAGEFILE.SYS
vhdmaker -x PAGEFILE.SYS hd0 output.vhd
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

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

#define MAX_EXCLUSIONS      1024
#define SECTOR_SIZE         512

/* --- STRUCTURES --- */

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

typedef struct __attribute__((packed)) {
    uint8_t  boot_indicator;
    uint8_t  start_head;
    uint16_t start_sector_cyl;
    uint8_t  sys_ind;
    uint8_t  end_head;
    uint16_t end_sector_cyl;
    uint32_t start_lba;
    uint32_t size_lba;
} mbr_part_entry_t;

typedef struct __attribute__((packed)) {
    uint8_t         bootstrap[446];
    mbr_part_entry_t partitions[4];
    uint16_t        signature;
} mbr_sector_t;

typedef struct __attribute__((packed)) {
    uint8_t  jmp[3];
    char     oem[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_dir_entries;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint32_t fat_size_32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
} fat32_bs_t;

typedef struct __attribute__((packed)) {
    char     name[11];
    uint8_t  attr;
    uint8_t  nt_reserved;
    uint8_t  creation_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} fat32_dir_entry_t;

typedef struct __attribute__((packed)) {
    uint8_t  jmp[3];
    char     oem[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  zeros1[3];
    uint16_t unused1;
    uint8_t  media_descriptor;
    uint16_t unused2;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t unused3;
    uint32_t unused4;
    uint64_t total_sectors;
    uint64_t mft_lcn;
    uint64_t mft_mirr_lcn;
    int8_t   clusters_per_mft_record;
} ntfs_bs_t;

typedef struct __attribute__((packed)) {
    char     magic[4];       /* "FILE" */
    uint16_t update_seq_off;
    uint16_t update_seq_cnt;
    uint64_t lsn;
    uint16_t seq_num;
    uint16_t hard_link_count;
    uint16_t attr_offset;
    uint16_t flags;          /* 0x01 = In Use, 0x02 = Directory */
    uint32_t real_size;
    uint32_t allocated_size;
} ntfs_mft_header_t;

typedef struct __attribute__((packed)) {
    uint32_t type;           /* 0x30 = File Name, 0x80 = Data */
    uint32_t length;
    uint8_t  non_resident;
    uint8_t  name_len;
    uint16_t name_offset;
    uint16_t flags;
    uint16_t attr_id;
} ntfs_attr_header_t;

typedef struct {
    uint64_t start_lba;
    uint64_t num_sectors;
} lba_extent_t;

/* --- GLOBAL EXCLUSION MAP --- */

lba_extent_t excluded_extents[MAX_EXCLUSIONS];
int exclusion_count = 0;

/* --- FUNCTION PROTOTYPES --- */

void print_usage(const char *prog);
uint32_t calculate_checksum(uint8_t *data, uint32_t len);
void write_be32(uint8_t *buf, uint32_t val);
void write_be64(uint8_t *buf, uint64_t val);
void fill_footer(vhdf_footer_t *footer, uint64_t orig_size, uint64_t current_size, time_t timestamp, uint8_t vhd_type);

void list_drives(void);
int get_drive_info(uint8_t drive_num, uint64_t *total_sectors, uint16_t *sector_size);
int read_physical_sectors(uint8_t drive_num, uint64_t lba, uint16_t count, void *buffer, int dos_seg);

int read_abstract_sectors(int is_physical, uint8_t drive_num, FILE *src_file, uint64_t lba, uint16_t count, void *buffer, int dos_seg);
void add_exclusion(uint64_t lba, uint64_t count);
void apply_exclusions(uint64_t base_lba, uint16_t sector_count, uint8_t *buffer);

void scan_partitions_and_exclude(int is_physical, uint8_t drive_num, FILE *src_file, const char *target_file, int dos_seg);
void scan_fat32(int is_physical, uint8_t drive_num, FILE *src_file, uint64_t part_lba, const char *target_file, int dos_seg);
void scan_ntfs(int is_physical, uint8_t drive_num, FILE *src_file, uint64_t part_lba, const char *target_file, int dos_seg);

/* --- MAIN --- */

int main(int argc, char *argv[])
{
    FILE *src_file = NULL;
    FILE *dst_file = NULL;
    char *input_file = NULL;
    char *output_file = NULL;
    char *exclude_filename = NULL;
    uint8_t vhd_type = VHD_TYPE_FIXED;
    
    int is_physical = 0;
    uint8_t drive_num = 0x80;
    int dos_sel = -1, dos_seg = -1;

    size_t chunk_sectors = 64; 
    size_t buffer_size = chunk_sectors * SECTOR_SIZE;          
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
        } else if (strcmp(argv[i], "-f") == 0) {
            vhd_type = VHD_TYPE_FIXED;
        } else if (strcmp(argv[i], "-d") == 0) {
            vhd_type = VHD_TYPE_DYNAMIC;
        } else if ((strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--exclude") == 0) && (i + 1 < argc)) {
            exclude_filename = argv[++i];
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

    if (strncmp(input_file, "hd", 2) == 0 && input_file[2] >= '0' && input_file[2] <= '9') {
        is_physical = 1;
        drive_num = 0x80 + (input_file[2] - '0');
    }

    printf("FreeDOS VHD Image Creator v1.2 (Exclusion Enabled)\n");
    printf("==================================================\n");
    printf("Input:   %s\n", input_file);
    printf("Output:  %s\n", output_file);
    if (exclude_filename) printf("Exclude: %s\n", exclude_filename);

    if (is_physical) {
#ifndef __DJGPP__
        fprintf(stderr, "Error: Physical drive access requires MS-DOS and DJGPP compiler.\n");
        return 1;
#else
        uint64_t total_sectors;
        uint16_t sector_sz;
        
        if (!get_drive_info(drive_num, &total_sectors, &sector_sz)) {
            fprintf(stderr, "Error: BIOS failed to read physical drive 0x%02X\n", drive_num);
            return 1;
        }
        
        source_size = total_sectors * sector_sz;
        
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

    printf("Size:    %llu bytes (%.2f MB)\n", (unsigned long long)source_size, source_size / (1024.0 * 1024.0));

    if (exclude_filename) {
        printf("\nParsing File System Exclusions...\n");
        scan_partitions_and_exclude(is_physical, drive_num, src_file, exclude_filename, dos_seg);
    }

    buffer = malloc(buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory\n");
        goto cleanup;
    }

    dst_file = fopen(output_file, "wb");
    if (dst_file == NULL) {
        fprintf(stderr, "Error: Cannot create output file: %s\n", strerror(errno));
        goto cleanup;
    }

    timestamp = time(NULL);
    memset(&footer, 0, sizeof(footer));
    fill_footer(&footer, source_size, source_size, timestamp, vhd_type);

    printf("\nConverting...\n");

    uint64_t offset = 0;
    while (offset < source_size) {
        size_t chunk = (source_size - offset > buffer_size) ? buffer_size : (source_size - offset);
        uint64_t current_lba = offset / SECTOR_SIZE;
        uint16_t sector_count = chunk / SECTOR_SIZE;

        if (!read_abstract_sectors(is_physical, drive_num, src_file, current_lba, sector_count, buffer, dos_seg)) {
            fprintf(stderr, "\nError reading source at LBA %llu\n", (unsigned long long)current_lba);
            goto cleanup;
        }

        apply_exclusions(current_lba, sector_count, buffer);

        bytes_read = chunk;
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

/* --- EXCLUSION ENGINE IMPLEMENTATION --- */

void add_exclusion(uint64_t lba, uint64_t count)
{
    if (exclusion_count >= MAX_EXCLUSIONS) return;
    excluded_extents[exclusion_count].start_lba = lba;
    excluded_extents[exclusion_count].num_sectors = count;
    exclusion_count++;
    printf("  [Exclusion] Zeroing LBA range: %llu to %llu (%llu sectors)\n",
           (unsigned long long)lba, (unsigned long long)(lba + count - 1), (unsigned long long)count);
}

void apply_exclusions(uint64_t base_lba, uint16_t sector_count, uint8_t *buffer)
{
    if (exclusion_count == 0) return;
    int i, s;
    for (s = 0; s < sector_count; s++) {
        uint64_t cur_lba = base_lba + s;
        for (i = 0; i < exclusion_count; i++) {
            if (cur_lba >= excluded_extents[i].start_lba &&
                cur_lba < (excluded_extents[i].start_lba + excluded_extents[i].num_sectors)) {
                memset(buffer + (s * SECTOR_SIZE), 0x00, SECTOR_SIZE);
                break;
            }
        }
    }
}

int read_abstract_sectors(int is_physical, uint8_t drive_num, FILE *src_file, uint64_t lba, uint16_t count, void *buffer, int dos_seg)
{
    if (is_physical) {
        return read_physical_sectors(drive_num, lba, count, buffer, dos_seg);
    } else {
        if (fseeko(src_file, lba * SECTOR_SIZE, SEEK_SET) != 0) return 0;
        return (fread(buffer, SECTOR_SIZE, count, src_file) == count);
    }
}

/* --- FILESYSTEM PARSERS --- */

void scan_partitions_and_exclude(int is_physical, uint8_t drive_num, FILE *src_file, const char *target_file, int dos_seg)
{
    uint8_t sector[SECTOR_SIZE];
    if (!read_abstract_sectors(is_physical, drive_num, src_file, 0, 1, sector, dos_seg)) return;

    mbr_sector_t *mbr = (mbr_sector_t*)sector;
    if (mbr->signature != 0xAA55) {
        printf("  No valid MBR found. Scanning LBA 0 directly...\n");
        scan_fat32(is_physical, drive_num, src_file, 0, target_file, dos_seg);
        scan_ntfs(is_physical, drive_num, src_file, 0, target_file, dos_seg);
        return;
    }

    int i;
    for (i = 0; i < 4; i++) {
        uint8_t type = mbr->partitions[i].sys_ind;
        uint32_t start_lba = mbr->partitions[i].start_lba;
        if (type == 0 || mbr->partitions[i].size_lba == 0) continue;

        printf(" Partition %d (Type 0x%02X) at LBA %u:\n", i + 1, type, start_lba);
        
        if (type == 0x0B || type == 0x0C || type == 0x0C) {
            scan_fat32(is_physical, drive_num, src_file, start_lba, target_file, dos_seg);
        } else if (type == 0x07) { /* NTFS or exFAT */
            scan_fat32(is_physical, drive_num, src_file, start_lba, target_file, dos_seg);
            scan_ntfs(is_physical, drive_num, src_file, start_lba, target_file, dos_seg);
        }
    }
}

void scan_fat32(int is_physical, uint8_t drive_num, FILE *src_file, uint64_t part_lba, const char *target_file, int dos_seg)
{
    uint8_t sector[SECTOR_SIZE];
    if (!read_abstract_sectors(is_physical, drive_num, src_file, part_lba, 1, sector, dos_seg)) return;

    fat32_bs_t *bs = (fat32_bs_t*)sector;
    if (bs->bytes_per_sector != 512 || bs->num_fats == 0 || bs->fat_size_32 == 0) return;

    printf("  -> Valid FAT32 Volume Detected.\n");

    uint32_t fat_begin = part_lba + bs->reserved_sectors;
    uint32_t cluster_begin = fat_begin + (bs->num_fats * bs->fat_size_32);
    uint32_t current_cluster = bs->root_cluster;

    /* Format search name to 8.3 FAT format */
    char dos_name[11];
    memset(dos_name, ' ', 11);
    int p = 0, j = 0;
    while (target_file[p] && target_file[p] != '.' && j < 8) {
        dos_name[j++] = toupper((unsigned char)target_file[p++]);
    }
    if (target_file[p] == '.') {
        p++;
        j = 8;
        while (target_file[p] && j < 11) {
            dos_name[j++] = toupper((unsigned char)target_file[p++]);
        }
    }

    /* Process Root Directory */
    uint8_t cluster_buf[SECTOR_SIZE * 64];
    uint32_t spc = bs->sectors_per_cluster;
    
    uint64_t root_lba = cluster_begin + ((current_cluster - 2) * spc);
    if (!read_abstract_sectors(is_physical, drive_num, src_file, root_lba, spc, cluster_buf, dos_seg)) return;

    fat32_dir_entry_t *dir = (fat32_dir_entry_t*)cluster_buf;
    int entries = (spc * SECTOR_SIZE) / sizeof(fat32_dir_entry_t);
    int i;

    for (i = 0; i < entries; i++) {
        if (dir[i].name[0] == 0x00) break;
        if (dir[i].name[0] == 0xE5) continue;
        if (dir[i].attr & 0x10) continue; /* Skip subdirectories for brevity */

        if (memcmp(dir[i].name, dos_name, 11) == 0) {
            uint32_t file_cluster = ((uint32_t)dir[i].first_cluster_high << 16) | dir[i].first_cluster_low;
            printf("  -> Match Found! Target File First Cluster: %u\n", file_cluster);

            /* Follow FAT Chain */
            while (file_cluster >= 2 && file_cluster < 0x0FFFFFF8) {
                uint64_t data_lba = cluster_begin + ((file_cluster - 2) * spc);
                add_exclusion(data_lba, spc);

                /* Read FAT entry for next cluster */
                uint32_t fat_sector_off = (file_cluster * 4) / SECTOR_SIZE;
                uint32_t fat_byte_off = (file_cluster * 4) % SECTOR_SIZE;

                if (!read_abstract_sectors(is_physical, drive_num, src_file, fat_begin + fat_sector_off, 1, sector, dos_seg)) break;
                file_cluster = (*(uint32_t*)(sector + fat_byte_off)) & 0x0FFFFFFF;
            }
            return;
        }
    }
}

void scan_ntfs(int is_physical, uint8_t drive_num, FILE *src_file, uint64_t part_lba, const char *target_file, int dos_seg)
{
    uint8_t sector[SECTOR_SIZE];
    if (!read_abstract_sectors(is_physical, drive_num, src_file, part_lba, 1, sector, dos_seg)) return;

    ntfs_bs_t *bs = (ntfs_bs_t*)sector;
    if (memcmp(bs->oem, "NTFS    ", 8) != 0) return;

    printf("  -> Valid NTFS Volume Detected.\n");

    uint64_t mft_lba = part_lba + (bs->mft_lcn * bs->sectors_per_cluster);
    uint8_t mft_buf[1024]; /* Standard MFT Record Size */
    int mft_record_sectors = 1024 / SECTOR_SIZE;

    /* Scan first 256 MFT Records for matching file */
    int r;
    for (r = 0; r < 256; r++) {
        if (!read_abstract_sectors(is_physical, drive_num, src_file, mft_lba + (r * mft_record_sectors), mft_record_sectors, mft_buf, dos_seg)) break;

        ntfs_mft_header_t *mft = (ntfs_mft_header_t*)mft_buf;
        if (memcmp(mft->magic, "FILE", 4) != 0 || !(mft->flags & 0x01)) continue;

        /* Traverse Attributes */
        uint16_t attr_off = mft->attr_offset;
        int found_file = 0;

        while (attr_off + sizeof(ntfs_attr_header_t) < 1024) {
            ntfs_attr_header_t *attr = (ntfs_attr_header_t*)(mft_buf + attr_off);
            if (attr->type == 0xFFFFFFFF || attr->length == 0) break;

            /* Type 0x30: $FILE_NAME */
            if (attr->type == 0x30 && !attr->non_resident) {
                uint8_t *val = mft_buf + attr_off + *(uint16_t*)(mft_buf + attr_off + 20);
                uint8_t name_len = val[64];
                uint16_t *wchar_name = (uint16_t*)(val + 66);
                
                char ascii_name[256];
                int k;
                for (k = 0; k < name_len && k < 255; k++) ascii_name[k] = (char)wchar_name[k];
                ascii_name[k] = '\0';

                if (strcasecmp(ascii_name, target_file) == 0) {
                    printf("  -> Match Found in MFT Record %d: %s\n", r, ascii_name);
                    found_file = 1;
                }
            }

            /* Type 0x80: $DATA (Parse Non-Resident Data Runs if file matched) */
            if (attr->type == 0x80 && found_file) {
                if (!attr->non_resident) {
                    printf("  -> File content is resident inside MFT record (no data clusters).\n");
                } else {
                    /* Parse Data Runs */
                    uint16_t run_off = *(uint16_t*)(mft_buf + attr_off + 32);
                    uint8_t *run = mft_buf + attr_off + run_off;
                    
                    int64_t current_lcn = 0;
                    while (*run != 0x00) {
                        uint8_t len_bytes = *run & 0x0F;
                        uint8_t off_bytes = (*run >> 4) & 0x0F;
                        run++;

                        uint64_t run_len = 0;
                        int i;
                        for (i = 0; i < len_bytes; i++) run_len |= ((uint64_t)run[i]) << (i * 8);
                        run += len_bytes;

                        int64_t run_off_val = 0;
                        for (i = 0; i < off_bytes; i++) run_off_val |= ((uint64_t)run[i]) << (i * 8);
                        /* Sign extend if negative */
                        if (off_bytes > 0 && (run[off_bytes - 1] & 0x80)) {
                            for (i = off_bytes; i < 8; i++) run_off_val |= ((uint64_t)0xFF) << (i * 8);
                        }
                        run += off_bytes;

                        current_lcn += run_off_val;
                        uint64_t lba = part_lba + (current_lcn * bs->sectors_per_cluster);
                        add_exclusion(lba, run_len * bs->sectors_per_cluster);
                    }
                }
                return;
            }

            attr_off += attr->length;
        }
    }
}

/* --- DOS & GENERAL UTILITIES --- */

void print_usage(const char *prog)
{
    printf("Usage: %s [options] <input> <output.vhd>\n", prog);
    printf("\nOptions:\n");
    printf("  -l, --list             List physical BIOS hard drives and exit\n");
    printf("  -f                     Fixed-size VHD (default)\n");
    printf("  -x, --exclude <file>   Exclude specific file contents (zeros LBA range)\n");
    printf("  -h                     Show help\n");
    printf("\nExamples:\n");
    printf("  %s freedos.img freedos.vhd\n", prog);
    printf("  %s -x PAGEFILE.SYS hd0 backup.vhd\n", prog);
}

void list_drives(void)
{
#ifndef __DJGPP__
    printf("Physical drive listing requires MS-DOS and DJGPP.\n");
#else
    uint8_t drive;
    int found = 0;
    printf("Detected BIOS Hard Drives:\n--------------------------\n");
    for (drive = 0x80; drive <= 0x8F; drive++) {
        uint64_t sectors;
        uint16_t sector_sz;
        if (get_drive_info(drive, &sectors, &sector_sz)) {
            uint64_t total_bytes = sectors * sector_sz;
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
    
    dos_seg = __dpmi_allocate_dos_memory((0x42 + 15) / 16, &dos_sel);
    if (dos_seg == -1) return 0;

    _farpokew(_dos_ds, dos_seg * 16, 0x0042);
    regs.h.ah = 0x48;
    regs.h.dl = drive_num;
    regs.x.ds = dos_seg;
    regs.x.si = 0;
    __dpmi_simulate_real_mode_interrupt(0x13, &regs);

    int success = 0;
    if ((regs.x.flags & 1) == 0 && regs.h.ah == 0) {
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
    
    _farpokeb(_dos_ds, dos_seg * 16 + 0, 0x10);
    _farpokeb(_dos_ds, dos_seg * 16 + 1, 0);
    _farpokew(_dos_ds, dos_seg * 16 + 2, count);
    _farpokew(_dos_ds, dos_seg * 16 + 4, 16);
    _farpokew(_dos_ds, dos_seg * 16 + 6, dos_seg);
    _farpokel(_dos_ds, dos_seg * 16 + 8, (uint32_t)lba);
    _farpokel(_dos_ds, dos_seg * 16 + 12, (uint32_t)(lba >> 32));

    regs.h.ah = 0x42;
    regs.h.dl = drive_num;
    regs.x.ds = dos_seg;
    regs.x.si = 0;
    
    __dpmi_simulate_real_mode_interrupt(0x13, &regs);
    if (regs.x.flags & 1) return 0;

    dosmemget(dos_seg * 16 + 16, count * SECTOR_SIZE, buffer);
    return 1;
#else
    return 0;
#endif
}

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

void fill_footer(vhdf_footer_t *footer, uint64_t orig_size, uint64_t current_size, time_t timestamp, uint8_t vhd_type)
{
    memcpy(footer->signature, VHD_SIGNATURE, 8);
    write_be32(footer->features, vhd_type == VHD_TYPE_FIXED ? VHD_FEATURE_NONE : VHD_FEATURE_SAVE);
    write_be32(footer->version, VHD_VERSION);
    write_be64(footer->data_offset, 0xFFFFFFFFFFFFFFFFULL);
    
    uint32_t vhd_time = (uint32_t)(timestamp - 946684800);
    write_be32(footer->creation_time, vhd_time);
    
    memcpy(footer->creator_app, "win9x", 4);
    write_be32(footer->version_minor, VHD_VERSION);
    memcpy(footer->creator_os, "W2k ", 4);
    write_be64(footer->orig_size, orig_size);
    write_be64(footer->current_size, current_size);
    
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
    
    uint32_t id_val = vhd_time ^ 0xDEADBEEF;
    footer->unique_id[0] = id_val & 0xFF;
    footer->unique_id[1] = (id_val >> 8) & 0xFF;
    footer->unique_id[2] = (id_val >> 16) & 0xFF;
    footer->unique_id[3] = (id_val >> 24) & 0xFF;
    
    footer->saved_state[0] = 0;
    
    memset(footer->checksum, 0, 4);
    uint32_t chksum = calculate_checksum((uint8_t *)footer, VHD_FOOTER_SIZE);
    write_be32(footer->checksum, chksum);
}