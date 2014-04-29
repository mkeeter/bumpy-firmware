#include "player.h"
#include "sd.h"
#include "encoder.h"
#include "LEDs.h"
#include "mp3.h"
#include "tenths.h"

struct Buffer buffer;
struct PlayerState player_state;

void player_manage_buffer(void)
{
    if (buffer.empty)
    {
        sd_mount_filesystem();

        // Copy data from the SD card into our local the buffer.
        // If we're out of data, skip to the next song.
        while (!sd_get_data(buffer.data, MP3_BUFFER_SIZE))
        {
            sd_next_song();
        }
    }
    buffer.empty = false;

    // Move data from our buffer to the mp3 decoder's buffer.
    if (mp3_wants_data() && player_state.playing)
    {
        mp3_send_data(buffer.data);
        buffer.empty = true;
    }
}

////////////////////////////////////////////////////////////////////////////////

// Redraws LEDs based on player state
void player_redraw(void)
{
    if (player_state.sleeping)
        LEDs_sleep();
    else
        LEDs_volume(player_state.playing, mp3_volume);
}

////////////////////////////////////////////////////////////////////////////////

// Check if we should wake from sleep mode by detecting double-tap
static void player_check_wake(void)
{
    // Look for a double-tap to detect wake-up.
    if (player_state.sleeping && tenths - player_state.button_time < 5) {
        player_state.sleeping = false;
        player_state.scrolled = true; // dummy scroll to prevent play-pause
        player_redraw();
    }
}

// On button press, check if we should wake up from sleep mode.
static void player_button_pressed(void)
{
    // Reset the variable that tracks whether we've scrolled
    player_state.scrolled = false;

    // Look for a double-tap within 0.5 seconds
    // to wake up from sleep mode.
    player_check_wake();

    // Keep track of when this button was pressed
    // (because we go into sleep mode after 5 seconds)
    player_state.button_time = tenths;
}


// On button release, switch play/pause unless we've scrolled
static void player_button_released(void)
{
    // If we've scrolled, then button release doesn't matter.
    // Otherwise, a single tap toggles play/pause
    if (!player_state.sleeping && !player_state.scrolled)
    {
         player_state.playing = !player_state.playing;
         player_redraw();
    }
}


// Check to see if we should enter sleep mode.
static void player_check_sleep(void)
{
    if (player_state.button_down && !player_state.sleeping &&
       !player_state.scrolled && tenths - player_state.button_time >= 50)
    {
        player_state.sleeping = true;
        player_redraw();
    }
}

////////////////////////////////////////////////////////////////////////////////

static void player_animate_next(void)
{
    LEDs_next();
    sd_next_song();
    player_redraw();
}

static void player_animate_prev(void)
{
    LEDs_prev();
    sd_prev_song();
    player_redraw();
}

static void player_volume_up(void)
{
    mp3_volume_up();
    player_redraw();
}

static void player_volume_down(void)
{
    mp3_volume_down();
    player_redraw();
}
////////////////////////////////////////////////////////////////////////////////

void player_update_state(void)
{
    bool button_down = encoder_switch;

    if (button_down && !player_state.button_down)
        player_button_pressed();
    else if (player_state.button_down && !button_down)
        player_button_released();

    player_state.button_down = button_down;

    int scroll = encoder;
    encoder_clear();

    if (scroll && !player_state.sleeping)
    {
        player_state.scrolled = true;
        if (scroll > 0) {
            if (button_down)    player_animate_next();
            else                player_volume_up();
        } else {
            if (button_down)    player_animate_prev();
            else                player_volume_down();
        }
    }

    // Go to sleep if necessary
    player_check_sleep();
}

////////////////////////////////////////////////////////////////////////////////

void player_init(void)
{
    buffer.empty = true;
    player_state.playing = false;
    player_state.sleeping = false;
    player_state.button_down = false;
    player_redraw();
}
