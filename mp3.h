#ifndef MP3_H
#define MP3_H

#include <stdbool.h>
#include <stdint.h>

#define MP3_BUFFER_SIZE 32

extern int mp3_volume;

int mp3_init(void);

uint16_t mp3_read(const uint8_t addr);
bool mp3_wants_data(void);
void mp3_send_data(uint8_t* buffer);

void mp3_volume_up(void);
void mp3_volume_down(void);
#endif
