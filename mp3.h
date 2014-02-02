#ifndef MP3_H
#define MP3_H

#include <stdbool.h>
#include <stdint.h>

#define MP3_BUFFER_SIZE 32

int mp3_init();
uint16_t mp3_read(const uint8_t addr);
bool mp3_wants_data();
void mp3_send_data(uint8_t* buffer);

#endif
