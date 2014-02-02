#include <stdio.h>
#include <string.h>

#include "sd.h"

#include "sd-reader/sd_raw.h"
#include "sd-reader/fat.h"
#include "sd-reader/partition.h"

struct partition_struct* partition = NULL;
struct fat_fs_struct* fs = NULL;
struct fat_dir_struct* root = NULL;

struct fat_dir_entry_struct file_dir;
struct fat_file_struct* file = NULL;

int sd_init()
{
    if (!sd_raw_init()) {
        printf("Error: sd_raw_init failed\n");
        return 0;
    }

    partition = partition_open(
            sd_raw_read,  sd_raw_read_interval,
            sd_raw_write, sd_raw_write_interval,
            0);
    if (!partition) {
        printf("Error: partition_open failed\n");
        return 0;
    }

    fs = fat_open(partition);
    if (!fs) {
        printf("Error: fat_open failed\n");
        return 0;
    }

    struct fat_dir_entry_struct root_dir;
    fat_get_dir_entry_of_path(fs, "/", &root_dir);
    root = fat_open_dir(fs, &root_dir);

    sd_next_song();

    return 1;
}

void sd_next_song()
{
    // Flag to prevent infinite loop if we don't have any .mp3 files
    int reset_already = 0;

    while (1)
    {
        // If we've reached the last file, reset the directory
        if (!fat_read_dir(root, &file_dir))
        {
            fat_reset_dir(root);

            // Don't loop forever if we've got no .mp3 files
            if (reset_already)  return;

            // Set this flag so that if we get here again we abort.
            reset_already = 1;
        }

        // Check to see if this directory entry has mp3 as its extension
        else if (!strcmp(
                    "mp3", file_dir.long_name +
                           strlen(file_dir.long_name) - 3))
        {
            break;
        }
    }

    file = fat_open_file(fs, &file_dir);
}

void sd_get_data(uint8_t* buffer, uintptr_t amount)
{
    if (file)   fat_read_file(file, buffer, amount);
}
