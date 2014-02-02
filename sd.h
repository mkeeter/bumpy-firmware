#ifndef SD_H
#define SD_H

#include <stdint.h>

// Initializes the SD filesystem.
// Returns 1 if success, 0 otherwise.
int sd_init();
void sd_next_song();

void sd_get_data(uint8_t* buffer, uintptr_t amount);

#endif
