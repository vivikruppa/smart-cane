#ifndef WAV_PLAYER_H
#define WAV_PLAYER_H

#include <Arduino.h>
#include <FS.h>

bool wav_player_play_file(
    fs::FS &file_system,
    const char *path
);

#endif