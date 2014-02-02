#include <avr/io.h>
#include <avr/interrupt.h>

#include "leds.h"
#include "macros.h"

uint8_t brightness[8];      // Values from 0 to 7, with 7 being brightest
volatile uint8_t tick = 0;  // Goes from 0 to 7.
uint8_t levels[8] = {
    0, 10, 20, 35, 55, 80, 110, 145
};

void LEDs_init()
{
    OUTPUT(DDRD, PD4);  // SHCP (shift register clock input)
    OUTPUT(DDRD, PD6);  // STCP (storage register clock input)
    OUTPUT(DDRD, PD7);  // OE (output enable)
    OUTPUT(DDRB, PB4);  // DS (serial data input)

    CLEAR(PORTD, PD7);   // Set output enable to 0 (enables outputs)

    SET(TIMSK0, OCIE0A); // output compare interrupt
    OCR0A = levels[0];  // output compare register

    SET(TCCR0B, CS02); // 256x prescalar

    for (int i=0; i < 8; ++i)
    {
        brightness[i] = (i > 6) ? 6 : i;
    }
}

void update_LEDs()
{
    for (int i=0; i < 8; ++i)
    {
        if (brightness[i] > tick) {
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


ISR(TIMER0_COMPA_vect)
{
    update_LEDs();
    tick = (tick + 1) % 8;
    OCR0A = levels[tick];
}
