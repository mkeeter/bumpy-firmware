#ifndef ENCODER_H
#define ENCODER_H

// The encoder is defined by two values:
//      encoder is an int representing ticks in either direction
//      encoder_switch is a macro to evaluates to true or false
extern volatile int encoder;
#define encoder_switch bool(!(PIND & (1 << PD0)))

void encoder_init();

#endif
