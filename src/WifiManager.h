//
// Created by wenoysd on 01/06/2026.
//

#ifndef NOT_ESPECIAL_VISION_WIFI_H
#define NOT_ESPECIAL_VISION_WIFI_H
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Arduino.h>

class Wifi
{



private:
    char* ssid;
    char* password;

public:
    Wifi(char* ssid, char* password);

    bool changePassword(char* password);

    bool changeSSID(char* ssid);

    bool connectWifi();

    void scanList();

    String sendBase64ToGemini(
    const String &base64ImageString,
    const String &prompt
);

    void sendStringToTTS(String response);
};


#endif //NOT_ESPECIAL_VISION_WIFI_H
