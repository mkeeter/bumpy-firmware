#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "encoder.h"
#include "serial.h"
#include "leds.h"
#include "mp3.h"
#include "sd.h"
#include "tenths.h"

////////////////////////////////////////////////////////////////////////////////

void show_volume(bool bright)
{
    for (int i=0; i < 8; ++i)
    {
        if (i + 1 <= mp3_volume)    LEDs[i] = bright ? 4 : 2;
        else                        LEDs[i] = 0;
    }
}

void clear_volume()
{
    for (int i=0; i < 7; ++i)   LEDs[i] = 0;
    LEDs[7] = 1;
}

////////////////////////////////////////////////////////////////////////////////

void animate_next()
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
    sd_next_song();
}

void animate_prev()
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
    sd_prev_song();
}

////////////////////////////////////////////////////////////////////////////////

int main()
{
    LEDs_init();
    encoder_init();
    serial_init();
    tenths_init();
    sei();

    // If the encoder switch is pressed on startup, jump to DFU bootloader.
    if (encoder_switch)
    {
        printf("Jumping to bootloader!\n");
        for (int i=0; i < 4; ++i)
        {
            for (int j=0; j < 8; ++j)   LEDs[j] = 8;
            _delay_ms(200);
            for (int j=0; j < 8; ++j)   LEDs[j] = 0;
            _delay_ms(200);
        }
        cli();
        __asm("jmp 0x3800");
    }


    printf("Booting up...\n");

    if (sd_init())  { printf("SD card initialized!\n"); }
    if (mp3_init()) { printf("VS1003  initialized!\n"); }

    uint8_t buffer[MP3_BUFFER_SIZE];
    bool buffer_empty = true;
    bool playing = false;
    bool scrolled = false;

    bool sleeping = false;
    bool just_slept = false;

    show_volume(playing);

    bool switch_status = encoder_switch;
    unsigned time = tenths;

    while (1)
    {
        if (buffer_empty) {
            // If we're at the end of a song, skip to a new one
            // and get a new buffer's worth of data.
            while (!sd_get_data(buffer, MP3_BUFFER_SIZE))
            {
                sd_next_song();
            }
            buffer_empty = false;
        }

        if (mp3_wants_data() && playing)
        {
            mp3_send_data(buffer);
            buffer_empty = true;
        }

        // Handle wheel + button interactions
        const int e = encoder;
        const bool s = encoder_switch;
        bool refresh = false;

        // Start or stop playing based on button press.
        if (s != switch_status) {
            if (s)
            {
                // Reset the variable that tracks whether we've scrolled
                scrolled = false;

                // Keep track of when this button was pressed
                // (because we go into sleep mode after 5 seconds)
                if (!sleeping)      time = tenths;
            }

            // Handle button release with no scrolling.
            else if (!scrolled)
            {
                // Look for a double-tap within 0.5 seconds
                // to wake up from sleep mode.
                if (sleeping)
                {
                    if (tenths - time < 5)      sleeping = false;
                    else if (!just_slept)       time = tenths;
                    just_slept = false;
                }

                // Otherwise, a single tap toggles play/pause
                else
                {
                    playing = !playing;
                }
            }

            switch_status = s;
            refresh = true;
        }

        // If the button is held for 5 seconds, switch to sleep mode.
        if (switch_status && !sleeping && !scrolled && tenths - time >= 50)
        {
            sleeping = true;
            refresh = true;
            just_slept = true;
        }


        // Adjust volume or go back and forward in track list.
        if (e)
        {
            // Set encoder's recorded value to 0
            cli();
            encoder = 0;
            sei();

            if (e > 0 && !sleeping)
            {
                if (s)  animate_next();
                else    mp3_volume_up();
            }
            else if (e < 0 && !sleeping)
            {
                if (s)  animate_prev();
                else    mp3_volume_down();
            }
            refresh = true;
            scrolled = true;
        }

        if (refresh)
        {
            if (sleeping)   clear_volume();
            else            show_volume(playing);
        }
    }
}
