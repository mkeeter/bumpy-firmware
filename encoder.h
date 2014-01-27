#ifndef ENCODER_H
#define ENCODER_H

#include <avr/io.h>

// The encoder is defined by two values:
//      encoder is an int representing ticks in either direction
//      encoder_switch is a macro to check the desired pin
extern volatile int encoder;
#define encoder_switch (!(PINB & (1 << PB6)))

void encoder_init();

#endif
