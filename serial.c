#include <LUFA/Drivers/Peripheral/Serial.h>

#include "serial.h"

#define BAUD 57600

void serial_init(void)
{
    Serial_Init(BAUD, true);
    Serial_CreateStream(NULL);
}
