#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "leds.h"
#include "tenths.h"

uint8_t LEDs[8];      // Values from 0 to 7, with 7 being brightest

void LEDs_init()
{
    // Shift register clock, storage register clock, output enable
    // are all outputs.
    DDRD |= (1 << PD4) | (1 << PD6) | (1 << PD7);

    // Serial data input is also an output.
    DDRB |= (1 << PB4);

    // Set output enable to 0 (enables outputs on shift register)
    PORTD &= ~(1 << PD7);

    // Enable output compare interrupt
    TIMSK0 |= (1 << OCIE0A);

    // Turn on timer0 with 256x prescalar
    TCCR0B |= (1 << CS02);

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

void LEDs_usb()
{
    for (int i=0; i < 8; ++i)
        LEDs[i] = 0;

    unsigned t = tenths % 14;
    if (t < 8)  LEDs[t] = 5;
    else        LEDs[14-t] = 5;
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
            PORTB |= (1 << PB4);
        } else {
            PORTB &= ~(1 << PB4);
        }

        // Pulse the clock (SHCP) to send this bit of data
        PORTD |=  (1 << PD4);
        PORTD &= ~(1 << PD4);
    }

    // Toggle STCP high and low to shift data to output stage
    PORTD |=  (1 << PD6);
    PORTD &= ~(1 << PD6);
}


ISR(TIMER0_COMPA_vect)
{
    update_LEDs();
    tick = (tick + 1) % 8;
    OCR0A = levels[tick];
}
