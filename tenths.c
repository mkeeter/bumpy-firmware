#include <avr/io.h>
#include <avr/interrupt.h>

#include "tenths.h"
#include "macros.h"

volatile unsigned tenths = 0;

void tenths_init(void)
{
    // Hit output compare about once every tenth of a second
    OCR1AH = 0x07;
    OCR1AL = 0x35;

    SET(TCCR1B, CS12); // 256x prescalar
    SET(TIMSK1, OCIE1A);    // trigger interrupt
}

// After each tenth of a second, reset the timer.
ISR(TIMER1_COMPA_vect)
{
    tenths++;
    TCNT1H = 0;
    TCNT1L = 0;
}
