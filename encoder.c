#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "encoder.h"

volatile int encoder=0;

void encoder_bootloader_check(void)
{
    // Turn on encoder switch pull-up
    PORTB |= (1 << PB6);

    // Wait a little bit for values to settle
    _delay_ms(100);

    // Then jump to the bootloader if the switch is presed.
    if (encoder_switch)     __asm("jmp 0x7000");
}

void encoder_init(void)
{
    // Set internal pull-ups to on
    PORTB |= (1 << PB5); // encoder B
    PORTB |= (1 << PB6); // encoder switch
    PORTC |= (1 << PC6); // encoder A

    // Prepare timer3 for use in debouncing.
    // (running at 1 MHz and interrupting at 3906 KHz)
    OCR3AH = 0x00;
    OCR3AL = 0xff;
    TIMSK3 |= (1 << OCIE3A);
    TCCR3B |= (1 << CS31);
}

void encoder_clear(void)
{
    TIMSK3 &= ~(1 << OCIE3A);
    encoder = 0;
    TIMSK3 |=  (1 << OCIE3A);
}

static volatile uint8_t current_state = 0;
static volatile uint8_t next_state = 0;
static volatile uint8_t count = 0;

ISR(TIMER3_COMPA_vect)
{
    uint8_t state = ((PINB & (1 << PB5)) ? 1 : 0) |
                    ((PINC & (1 << PC6)) ? 2 : 0);

    if (state == current_state)
    {
        ;
    }
    else if (state == next_state)
    {
        if (++count == 10)
        {
            if (current_state == 0 && next_state == 2)
                encoder++;
            else if (current_state == 0 && next_state == 1)
                encoder--;
            current_state = next_state;
        }
    }
    else
    {
        next_state = state;
        count = 0;
    }
    TCNT3H = 0;
    TCNT3L = 0;
}
