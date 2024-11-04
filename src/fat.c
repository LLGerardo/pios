#include "fat.h"
#include "sd.h"
#include <string.h>
#include <ctype.h>

static struct boot_sector *bs;
static char fat_table[CLUSTER_SIZE];
static unsigned int root_sector;

int fatInit() {
    char bootSector[SECTOR_SIZE];
    
    if (sd_readblock(0, (unsigned char*)bootSector, 1) != SECTOR_SIZE) {
        return SD_ERROR;
    }
    
    bs = (struct boot_sector *)bootSector;

    if (bs->boot_signature != 0xaa55) { 
        return SD_ERROR;
    }

    if (strncmp(bs->fs_type, "FAT12   ", 8) != 0) {
        return SD_ERROR;
    }

    unsigned int fat_size = bs->num_sectors_per_fat * SECTOR_SIZE;
    if (sd_readblock(bs->num_reserved_sectors, (unsigned char*)fat_table, bs->num_sectors_per_fat) != fat_size) {
        return SD_ERROR;
    }

    root_sector = bs->num_fat_tables * bs->num_sectors_per_fat + 
                  bs->num_reserved_sectors + bs->num_hidden_sectors;

    return SD_OK;
}

struct file fatOpen(const char *filename) {
    struct file fd = {0};
    char uppercase_filename[12];
    int i;

    for (i = 0; filename[i] && i < 11; i++) {
        uppercase_filename[i] = toupper(filename[i]);
    }
    uppercase_filename[i] = '\0';

    char root_dir[bs->num_root_dir_entries * sizeof(struct root_directory_entry)];
    if (sd_readblock(root_sector, (unsigned char*)root_dir, 
                     (bs->num_root_dir_entries * sizeof(struct root_directory_entry) + SECTOR_SIZE - 1) / SECTOR_SIZE) 
        != bs->num_root_dir_entries * sizeof(struct root_directory_entry)) {
        return fd;
    }

    struct root_directory_entry *entry;
    for (i = 0; i < bs->num_root_dir_entries; i++) {
        entry = (struct root_directory_entry *)&root_dir[i * sizeof(struct root_directory_entry)];
        
        if (strncmp(entry->file_name, uppercase_filename, 11) == 0) {
            fd.rde = *entry;
            fd.start_cluster = entry->cluster;
            return fd;
        }
    }
    return fd;
}

int fatRead(struct file *fd, void *buffer, uint32_t size) {
    uint32_t bytes_read = 0;
    uint32_t cluster_size = bs->num_sectors_per_cluster * bs->bytes_per_sector;
    char cluster_data[CLUSTER_SIZE];
    uint32_t current_cluster = fd->start_cluster;

    while (bytes_read < size && bytes_read < fd->rde.file_size) {
        uint32_t cluster_sector = ((current_cluster - 2) * bs->num_sectors_per_cluster) + root_sector + 
                                  (bs->num_root_dir_entries * sizeof(struct root_directory_entry) + SECTOR_SIZE - 1) / SECTOR_SIZE;
        if (sd_readblock(cluster_sector, (unsigned char*)cluster_data, bs->num_sectors_per_cluster) != cluster_size) {
            return SD_ERROR;
        }

        uint32_t bytes_to_copy = (size - bytes_read < cluster_size) ? size - bytes_read : cluster_size;
        if (bytes_to_copy > fd->rde.file_size - bytes_read) {
            bytes_to_copy = fd->rde.file_size - bytes_read;
        }
        memcpy((char*)buffer + bytes_read, cluster_data, bytes_to_copy);

        bytes_read += bytes_to_copy;

        if (bytes_read < size && bytes_read < fd->rde.file_size) {
            current_cluster = get_next_cluster(current_cluster);
            if (current_cluster >= 0xFF8) {
                break; // End of file
            }
        }
    }

    return bytes_read;
}

static uint16_t get_next_cluster(uint16_t current_cluster) {
    uint32_t fat_offset = current_cluster * 3 / 2;
    uint16_t fat_entry = *(uint16_t *)&fat_table[fat_offset];

    if (current_cluster & 1) {
        return fat_entry >> 4;
    } else {
        return fat_entry & 0x0FFF;
    }
}