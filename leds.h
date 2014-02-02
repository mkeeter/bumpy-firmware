#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

extern uint8_t LEDs[8];
void LED_brightness(uint8_t index, uint8_t value);
void LEDs_init();

#endif
