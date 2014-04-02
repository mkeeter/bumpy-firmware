#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>
#include <stdbool.h>

extern uint8_t LEDs[8];
void LEDs_init(void);
void LEDs_volume(bool bright, uint8_t volume);
void LEDs_sleep(void);
void LEDs_next(void);
void LEDs_prev(void);

#endif
