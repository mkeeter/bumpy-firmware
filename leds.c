#include <avr/io.h>

#include "leds.h"
#include "macros.h"

void LEDs_init()
{
    OUTPUT(DDRD, PD4);  // SHCP (shift register clock input)
    OUTPUT(DDRD, PD6);  // STCP (storage register clock input)
    OUTPUT(DDRD, PD7);  // OE (output enable)
    OUTPUT(DDRB, PB4);  // DS (serial data input)

    CLEAR(PORTD, PD7);   // Set output enable to 0 (enables outputs)
    LEDs(0);
}

void LEDs(const char L)
{
    for (uint8_t i=1; i != 0; i <<= 1) {
        // Set the data going into the shift register (DS)
        if (L & i) {
            SET(PORTB, PB4);
        } else {
            CLEAR(PORTB, PB4);
        }

        // Pulse the clock (SHCP)
        SET(PORTD, PD4);
        CLEAR(PORTD, PD4);
    }

    // Toggle STCP high and low
    SET(PORTD, PD6);
    CLEAR(PORTD, PD6);
}
