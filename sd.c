#include <stdio.h>

#include "sd-reader/sd_raw.h"
#include "sd-reader/fat.h"
#include "sd-reader/partition.h"

struct partition_struct* partition;
struct fat_fs_struct* fs;

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

    return 1;
}
