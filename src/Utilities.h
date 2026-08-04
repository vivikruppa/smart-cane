//
// Created by wenoysd on 01/06/2026.
//

#ifndef NOT_ESPECIAL_VISION_UTILITIES_H
#define NOT_ESPECIAL_VISION_UTILITIES_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Utilities
{
public:
    char* getCharInput();

    Utilities();


    String nightPrompt; // prompt modo noturno
    String shortPrompt; // prompt "macro"
    String landscapePrompt; // prompt paisagem


    static String extractCleanText(String rawJsonResponse);








};







#endif //NOT_ESPECIAL_VISION_UTILITIES_H
