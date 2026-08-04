#include <Arduino.h>
#include <ostream>
#include <iostream>

#include "Camera.h"
#include "Led.h"
#include "Utilities.h"
#include "WifiManager.h"
#include "SignInference.h"
#include "secrets.h"
#include <LittleFS.h>

char* ssid =
    const_cast<char*>(WIFI_SSID);

char* password =
    const_cast<char*>(WIFI_PASSWORD);


Wifi wifizito{ssid, password};

void executarModoNuvem(
    VisionMode visionMode,
    const String& prompt,
    int modeNumber
) {
    const uint32_t totalStart = millis();

    Serial.printf(
        "\n========== INICIO DO TESTE - MODO %d ==========\n",
        modeNumber
    );

    Camera camerazita;

    /*
     * Configuração da câmera.
     */
    const uint32_t cameraConfigStart = millis();

    camerazita.applyVisionMode(
        visionMode
    );

    const uint32_t cameraConfigTime =
        millis() - cameraConfigStart;

    Serial.printf(
        "[TEMPO] Configuracao da camera: %lu ms\n",
        static_cast<unsigned long>(
            cameraConfigTime
        )
    );

    /*
     * Captura e conversão da imagem para Base64.
     */
    const uint32_t photoStart = millis();

    String photoBase64 =
        camerazita.takePhoto64();

    const uint32_t photoTime =
        millis() - photoStart;

    Serial.printf(
        "[TEMPO] Captura e Base64: %lu ms\n",
        static_cast<unsigned long>(
            photoTime
        )
    );

    if (photoBase64.isEmpty()) {
        Serial.println(
            "[RESULTADO] Falha na captura ou conversao da imagem."
        );

        Serial.printf(
            "[TEMPO] Fluxo interrompido do modo %d: %lu ms\n",
            modeNumber,
            static_cast<unsigned long>(
                millis() - totalStart
            )
        );

        return;
    }

    /*
     * Envio da imagem e obtenção da resposta do Gemini.
     */
    const uint32_t geminiStart = millis();

    String geminiResponse =
        wifizito.sendBase64ToGemini(
            photoBase64,
            prompt
        );

    const uint32_t geminiTime =
        millis() - geminiStart;

    Serial.printf(
        "[TEMPO] Requisicao Gemini: %lu ms\n",
        static_cast<unsigned long>(
            geminiTime
        )
    );

    /*
     * Liberação da String Base64 antes do TTS.
     */
    photoBase64 = "";

    if (
        geminiResponse.isEmpty() ||
        geminiResponse.startsWith("Error:")
    ) {
        Serial.println(
            "[RESULTADO] Falha na requisicao ao Gemini."
        );

        Serial.printf(
            "[TEMPO] Fluxo interrompido do modo %d: %lu ms\n",
            modeNumber,
            static_cast<unsigned long>(
                millis() - totalStart
            )
        );

        return;
    }

    /*
     * Azure TTS e reprodução.
     *
     * As medições específicas de Azure e reprodução
     * já são exibidas dentro de sendStringToTTS().
     */
    const uint32_t ttsStart = millis();

    wifizito.sendStringToTTS(
        geminiResponse
    );

    const uint32_t ttsTime =
        millis() - ttsStart;

    Serial.printf(
        "[TEMPO] TTS e reproducao: %lu ms\n",
        static_cast<unsigned long>(
            ttsTime
        )
    );

    const uint32_t totalTime =
        millis() - totalStart;

    Serial.printf(
        "[TEMPO] Fluxo completo do modo %d: %lu ms\n",
        modeNumber,
        static_cast<unsigned long>(
            totalTime
        )
    );

    Serial.printf(
        "========== FIM DO TESTE - MODO %d ==========\n\n",
        modeNumber
    );
}


void setup() {
    Serial.begin(115200);

    delay(1000);

if (!LittleFS.begin(false)) {
    Serial.println(
        "Falha ao montar LittleFS."
    );

    while (true) {
        delay(1000);
    }
}

Serial.println(
    "LittleFS montado."
);

    Serial.println("System Booting... Serial routed successfully!");

    wifizito.scanList();
    wifizito.connectWifi();

    Serial.println(
        "Sistema pronto. Digite um modo."
    );
}

void loop()
{
    Utilities utilities;

    if (Serial.available() > 0)
    {
        int receivedNumber =
            Serial.parseInt();

        if (receivedNumber != 0)
        {
            Serial.printf(
                "Command received: Switching to Mode %d\n",
                receivedNumber
            );

            if (receivedNumber == 1)
            {
                executarModoNuvem(
                    MODE_NIGHT_SCAN,
                    utilities.nightPrompt,
                    1
                );
            }

            else if (receivedNumber == 2)
            {
                executarModoNuvem(
                    MODE_CLOSE_READING,
                    utilities.shortPrompt,
                    2
                );
            }

            else if (receivedNumber == 3)
            {
                executarModoNuvem(
                    MODE_LANDSCAPE_AMBIENT,
                    utilities.landscapePrompt,
                    3
                );
            }

            else if (receivedNumber == 4)
            {
                SignInference detector;

                Serial.println(
                    "Inicializando camera para inferencia..."
                );

                if (!detector.begin())
                {
                    Serial.println(
                        "Falha ao inicializar camera do modo 4."
                    );

                    return;
                }

                detector.run();
                detector.end();

                Serial.println(
                    "Inferencia concluida."
                );
            }

            else
            {
                Serial.println(
                    "Invalid mode."
                );
            }
        }
    }

    delay(10);
}