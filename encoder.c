#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "encoder.h"
#include "macros.h"

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

    // Encoder B is on the pin corresponding to PCINT11,
    // so we'll enable pin change interrupt 1 with the correct
    // mask register value.
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT5);

    // Prepare timer3 for use in debouncing.
    OCR3AH = 0x00;
    OCR3AL = 0x05;
    TIMSK3 |= (1 << OCIE3A);
}

void encoder_clear(void)
{
    PCICR &= ~(1 << PCIE0);
    encoder = 0;
    PCICR |=  (1 << PCIE0);
}

ISR(PCINT0_vect)
{
    // Only count a tick on a rising edge of ENCODER_B
    if (PINB & (1 << PB5)) {
        // Debouce by waiting until timer3 overflows
        TCNT3H = 0;
        TCNT3L = 0;
        TCCR3B |= (1 << CS32);
    }
}

ISR(TIMER3_COMPA_vect)
{
    // Determine direction from ENCODER_A
    if (PINC & (1 << PC6))  encoder++;
    else                    encoder--;

    // Then turn off the timer
    TCCR3B &= ~(1 << CS32);
}
