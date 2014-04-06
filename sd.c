#include <stdio.h>
#include <string.h>

#include <util/delay.h>

#include "sd.h"

#include "sd-reader/sd_raw.h"
#include "sd-reader/fat.h"
#include "sd-reader/partition.h"

static struct partition_struct* partition = NULL;
static struct fat_fs_struct* fs = NULL;
static struct fat_dir_struct* root = NULL;
static struct fat_file_struct* file = NULL;

static unsigned song_count = 0;

////////////////////////////////////////////////////////////////////////////////

static inline bool sd_has_extension_mp3(struct fat_dir_entry_struct f)
{
    return !strcmp("mp3", f.long_name + strlen(f.long_name) - 3);
}

////////////////////////////////////////////////////////////////////////////////

static void sd_count_songs(void)
{
    song_count = 0;

    // Search through the memory card to make sure that there's at least
    // one file ending in "mp3".
    fat_reset_dir(root);

    struct fat_dir_entry_struct file_dir;
    while(fat_read_dir(root, &file_dir))
    {
        if (sd_has_extension_mp3(file_dir))     song_count++;
    }
}

////////////////////////////////////////////////////////////////////////////////

int sd_init(void)
{
    if (!sd_raw_init()) {
        printf("Error: sd_raw_init failed\n");
        return 0;
    }
    return 1;
}


void sd_mount_filesystem(void)
{
    // Make the function idempotent.
    if (partition != NULL && fs != NULL && root != NULL && file != NULL)
        return;

    // Make sure that the file system isn't partially mounted.
    sd_unmount_filesystem();

    // Mount the partition
    partition = partition_open(
            sd_raw_read,  sd_raw_read_interval,
            sd_raw_write, sd_raw_write_interval,
            0);

    // Mount the filesystem
    fs = fat_open(partition);

    // Open the root directory
    struct fat_dir_entry_struct root_dir;
    fat_get_dir_entry_of_path(fs, "/", &root_dir);
    root = fat_open_dir(fs, &root_dir);

    sd_count_songs();
    sd_next_song();
}


void sd_unmount_filesystem(void)
{
    if (file != NULL)
        fat_close_file(file);
    file = NULL;

    if (root != NULL)
        fat_close_dir(root);
    root = NULL;

    if (fs != NULL)
        fat_close(fs);
    fs = NULL;

    if (partition != NULL)
        partition_close(partition);
    partition = NULL;
}

////////////////////////////////////////////////////////////////////////////////

struct fat_dir_entry_struct sd_next_song(void)
{
    struct fat_dir_entry_struct file_dir;
    file_dir.file_size = 0;

    if (!song_count)     return file_dir;

    if (file)   fat_close_file(file);
    file = NULL;

    while (1)
    {
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

    return file_dir;
}

////////////////////////////////////////////////////////////////////////////////

struct fat_dir_entry_struct sd_prev_song(void)
{
    struct fat_dir_entry_struct file_dir;
    file_dir.file_size = 0;

    if (!song_count)     return file_dir;

    for (int i=0; i < song_count - 1; ++i)
    {
        file_dir = sd_next_song();
    }
    return file_dir;
}

////////////////////////////////////////////////////////////////////////////////

bool sd_get_data(uint8_t* buffer, uintptr_t amount)
{
    if (file)   return fat_read_file(file, buffer, amount) > 0;
    else        return true;
}

////////////////////////////////////////////////////////////////////////////////

bool sd_check(void)
{
    return (partition != NULL) && (fs != NULL);
}
