#ifndef AZURE_TTS_CLIENT_H
#define AZURE_TTS_CLIENT_H

#include <Arduino.h>
#include <FS.h>

bool azure_tts_generate_wav(
    fs::FS &fileSystem,
    const char *outputPath,
    const String &text
);

#endif