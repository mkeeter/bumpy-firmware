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

bool has_songs = false;

////////////////////////////////////////////////////////////////////////////////

static inline bool sd_has_extension_mp3(struct fat_dir_entry_struct f)
{
    return !strcmp("mp3", f.long_name + strlen(f.long_name) - 3);
}

////////////////////////////////////////////////////////////////////////////////

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

    // Search through the memory card to make sure that there's at least
    // one file ending in "mp3".
    while(fat_read_dir(root, &file_dir))
    {
        if (sd_has_extension_mp3(file_dir))
        {
            has_songs = true;
            break;
        }
    }

    sd_next_song();

    return 1;
}

////////////////////////////////////////////////////////////////////////////////

void sd_next_song()
{
    if (!has_songs)     return;

    if (file)   fat_close_file(file);
    file = NULL;

    while (1)
    {
        printf("Current file: %s\n", file_dir.long_name);

        // If we've reached the last file, reset the directory
        if (!fat_read_dir(root, &file_dir))
        {
            fat_reset_dir(root);
        }

        // Check to see if this directory entry has mp3 as its extension
        else if (sd_has_extension_mp3(file_dir))
        {
            break;
        }
    }

    file = fat_open_file(fs, &file_dir);
}

////////////////////////////////////////////////////////////////////////////////

void sd_prev_song()
{
    if (!has_songs)     return;

    if (file)   fat_close_file(file);
    file = NULL;

    const struct fat_dir_entry_struct current_song = file_dir;

    // Loop until the next song is our current song.
    while (1)
    {
        const struct fat_dir_entry_struct prev = file_dir;
        sd_next_song();
        if (strcmp(file_dir.long_name, current_song.long_name) == 0 &&
            file_dir.file_size == current_song.file_size)
        {
            file_dir = prev;
            break;
        }
    }

    file = fat_open_file(fs, &file_dir);
}

////////////////////////////////////////////////////////////////////////////////

bool sd_get_data(uint8_t* buffer, uintptr_t amount)
{
    if (file)   return fat_read_file(file, buffer, amount) > 0;
    else        return true;
}
