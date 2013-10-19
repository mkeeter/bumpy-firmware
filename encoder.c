#include <avr/io.h>
#include <avr/interrupt.h>

#include "encoder.h"
#include "macros.h"

volatile int encoder=0;

void encoder_init()
{
    // Set internal pull-ups to on
    PORTC |= (1 << PC2); // encoder B
    PORTD |= (1 << PD0); // encoder switch
    PORTD |= (1 << PD1); // encoder A

    // Encoder B is on the pin corresponding to PCINT11,
    // so we'll enable pin change interrupt 1 with the correct
    // mask register value.
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT11);
}

ISR(PCINT1_vect)
{
    // Only count a tick on a rising edge
    if (PINC & (1 << PC2)) {
        // Determine direction from ENCODER_A
        if (PIND & (1 << PD1))  encoder++;
        else                    encoder--;
    }
}
