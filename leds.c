#include <avr/io.h>

#include "leds.h"
#include "macros.h"

void LEDs_init()
{
    OUTPUT(DDRD, PD4);  // SHCP (shift register clock input)
    OUTPUT(DDRD, PD5);  // STCP (storage register clock input)
    OUTPUT(DDRD, PD6);  // OE (output enable)
    OUTPUT(DDRD, PD7);  // DS (serial data input)

    CLEAR(PORTD, PD6);   // Set output enable to 0 (enables outputs)
    LEDs(0);
}

void LEDs(const char L)
{
    for (int i=0; i < 8; ++i) {
        // Set the data going into the shift register
        if (L & (1 << i)) {
            SET(PORTD, PD7);
        } else {
            CLEAR(PORTD, PD7);
        }

        // Pulse the clock
        SET(PORTD, PD4);
        CLEAR(PORTD, PD4);
    }

    SET(PORTD, PD5);
    CLEAR(PORTD, PD5);
}
