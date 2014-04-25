#ifndef SD_H
#define SD_H

#include <stdint.h>
#include <stdbool.h>

#include "sd-reader/fat.h"

// Initializes the SD filesystem.
// Returns 1 if success, 0 otherwise.
int sd_init(void);
void sd_mount_filesystem(void);
void sd_unmount_filesystem(void);

struct fat_dir_entry_struct sd_next_song(void);
struct fat_dir_entry_struct sd_prev_song(void);

bool sd_get_data(uint8_t* buffer, uintptr_t amount);

/** Checks to see if the SD card is properly initialized.
 */
bool sd_check(void);

#endif
