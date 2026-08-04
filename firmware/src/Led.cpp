//
// Created by wenoysd on 29/05/2026.
//

#include "Led.h"


#include <esp32-hal-gpio.h>
#include <esp32-hal.h>

// Define the physical pin where the LED is connected




Led::Led(){
    pinMode(LED_BUILTIN, OUTPUT);

}

void Led::ledOn()
{
    digitalWrite(LED_BUILTIN, HIGH);
}

void Led::ledOff()
{
    digitalWrite(LED_BUILTIN, LOW);
}

void  Led::Blink (int interval, int N) {

    for (int i = 0; i < N; i++)
    {
        // acender o LED por 1 segundo
        Led::ledOn();
        delay(interval);

        // apagar o LED por 1 segundo
        Led::ledOff();
        delay(interval);
    };
}
