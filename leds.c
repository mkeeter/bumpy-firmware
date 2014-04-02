#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "leds.h"
#include "macros.h"

uint8_t LEDs[8];      // Values from 0 to 7, with 7 being brightest

void LEDs_init()
{
    OUTPUT(DDRD, PD4);  // SHCP (shift register clock input)
    OUTPUT(DDRD, PD6);  // STCP (storage register clock input)
    OUTPUT(DDRD, PD7);  // OE (output enable)
    OUTPUT(DDRB, PB4);  // DS (serial data input)

    CLEAR(PORTD, PD7);   // Set output enable to 0 (enables outputs)

    SET(TIMSK0, OCIE0A); // output compare interrupt

    SET(TCCR0B, CS02); // 256x prescalar

    for (int i=0; i < 8; ++i)   LEDs[i] = 0;
}

void LEDs_volume(bool bright, uint8_t volume)
{
    for (uint8_t i=0; i < 8; ++i)
    {
        if (i+1 <= volume)  LEDs[i] = bright ? 4 : 2;
        else                LEDs[i] = 0;
    }
}

void LEDs_sleep()
{
    for (int i=0; i < 7; ++i)   LEDs[i] = 0;
    LEDs[7] = 1;
}

void LEDs_next()
{
    uint8_t buffer[24];
    for (int i=0; i < 8; ++i)
    {
        buffer[i] = LEDs[i];
        buffer[i + 8] = 0;
        buffer[i + 16] = LEDs[i];
    }

    for (int i=0; i < 16; ++i)
    {
        for (int j=0; j < 8; ++j)   LEDs[j] = buffer[i+j];
        _delay_ms(20);
    }
}

void LEDs_prev()
{
    uint8_t buffer[24];
    for (int i=0; i < 8; ++i)
    {
        buffer[i] = LEDs[i];
        buffer[i + 8] = 0;
        buffer[i + 16] = LEDs[i];
    }

    for (int i=15; i >= 0; --i)
    {
        for (int j=0; j < 8; ++j)   LEDs[j] = buffer[i+j];
        _delay_ms(20);
    }
}

////////////////////////////////////////////////////////////////////////////////

volatile uint8_t tick = 0;  // Goes from 0 to 7.
uint8_t levels[8] = {
    0, 10, 20, 35, 55, 80, 110, 145
};

void update_LEDs(void)
{
    for (int i=0; i < 8; ++i)
    {
        if (LEDs[i] > tick) {
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
