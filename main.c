#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "encoder.h"
#include "serial.h"
#include "leds.h"
#include "mp3.h"

#include "sd-reader/sd_raw.h"
#include "sd-reader/fat.h"
#include "sd-reader/partition.h"

int main()
{
    LEDs_init();
    encoder_init();
    serial_init();
    mp3_init();

    sei(); // enable interrupts (used for encoder and serial comms)

    if (sd_raw_init()) {
        printf("SD card init successful\n");
    }

    struct partition_struct* partition = partition_open(
            sd_raw_read,  sd_raw_read_interval,
            sd_raw_write, sd_raw_write_interval,
            0);
    if (partition) {
        printf("Opened partition\n");
    }

    struct fat_fs_struct* fs = fat_open(partition);
    if (fs) {
        printf("Opened filesystem\n");
    }

    const uint16_t status = mp3_read(0x1);
    printf("VS1063ds status = %x\n", status);

    uint8_t c=0;
    while (1) {
        LEDs(++c);
        _delay_ms(100);
    }
}
