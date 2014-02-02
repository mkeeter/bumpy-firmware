#ifndef SD_H
#define SD_H

#include <stdint.h>
#include <stdbool.h>

// Initializes the SD filesystem.
// Returns 1 if success, 0 otherwise.
int sd_init();
void sd_next_song();
void sd_prev_song();

bool sd_get_data(uint8_t* buffer, uintptr_t amount);

#endif
