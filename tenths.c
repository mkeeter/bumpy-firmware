#include <avr/io.h>
#include <avr/interrupt.h>

#include "tenths.h"

volatile unsigned tenths = 0;

void tenths_init(void)
{
    // Hit output compare about once every tenth of a second
    OCR1AH = 0x0c;
    OCR1AL = 0x35;

    TCCR1B |= (1 << CS12); // 256x prescalar
    TIMSK1 |= (1 << OCIE1A); // interrupt on output compare A
}

// After each tenth of a second, reset the timer.
ISR(TIMER1_COMPA_vect)
{
    tenths++;
    TCNT1H = 0;
    TCNT1L = 0;
}
