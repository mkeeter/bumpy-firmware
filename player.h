#ifndef PLAYER_H
#define PLAYER_H

#include "mp3.h"

struct Buffer {
    uint8_t data[MP3_BUFFER_SIZE];
    bool empty;
};

extern struct Buffer buffer;

struct PlayerState {
    bool playing;
    bool sleeping;
    bool button_down;
    bool scrolled;
    unsigned button_time;
};

extern struct PlayerState player_state;

void player_manage_buffer();
void player_update_state();
void player_init();

#endif
