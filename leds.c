#include <avr/io.h>

#include "leds.h"
#include "macros.h"

void LEDs_init()
{
    OUTPUT(DDRD, PD6); // SHCP (shift register clock input)
    OUTPUT(DDRD, PD7); // STCP (storage register clock input)
    OUTPUT(DDRB, PB4); // DS (serial data input)
    LEDs(0);
}

void LEDs(const char L)
{
    for (int i=0; i < 8; ++i) {
        if (L & (1 << i)) {
            SET(PORTB, PB4);
        } else {
            CLEAR(PORTB, PB4);
        }

        // Pulse the clock
        SET(PORTD, PD6);
        CLEAR(PORTD, PD6);
    }

    SET(PORTD, PD7);
    CLEAR(PORTD, PD7);
}
