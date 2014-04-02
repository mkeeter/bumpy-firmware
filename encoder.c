#include <avr/io.h>
#include <avr/interrupt.h>

#include "encoder.h"
#include "macros.h"

volatile int encoder=0;

void encoder_bootloader_check()
{
    PORTB |= (1 << PB6); // encoder switch pull-up
    if (encoder_switch)     __asm("jmp 0x7000");
}

void encoder_init()
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
}

void encoder_clear()
{
    cli();
    encoder = 0;
    sei();
}

ISR(PCINT0_vect)
{
    // Only count a tick on a rising edge of ENCODER_B
    if (PINB & (1 << PB5)) {
        // Determine direction from ENCODER_A
        if (PINC & (1 << PC6))  encoder++;
        else                    encoder--;
    }
}
