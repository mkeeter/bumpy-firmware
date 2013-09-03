#include <avr/io.h>

#include "encoder.h"
#include "macros.h"

void encoder_init()
{
    // Set internal pull-ups to on
    PORTF |= (1 << PF5) | (1 << PF4) | (1 << PF1);
}

uint8_t encoder_status()
{
    return PINF;
}
