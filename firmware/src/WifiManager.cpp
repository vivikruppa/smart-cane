//
// Created by wenoysd on 01/06/2026.
//

#include "WifiManager.h"
#include <WiFi.h>

#include "Utilities.h"
#include "secrets.h"

#include <LittleFS.h>

#include "azure_tts_client.h"
#include "pdm_output.h"
#include "wav_player.h"


Wifi::Wifi(char* ssid, char* password)
{
   this->password = password;
    this->ssid = ssid;
    Serial.begin(115200);

};

bool Wifi::changePassword(char* password)
{
    this->password = password;
return true;
}

bool Wifi::changeSSID(char* ssid)
{
    this->ssid = ssid;
    return true;
}



bool Wifi::connectWifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    delay(1000);

    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    return true;
};

void Wifi::scanList()
{
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");

    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");

        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i)); // Network Name
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i)); // Signal Strength (dBm)
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
        }
    }
    Serial.println("");
}

// Função que pede como requisito uma imagem em base64 para enviar para o gemini

String Wifi::sendBase64ToGemini(const String &base64ImageString, const String &prompt) {
    String response;
    WiFiClientSecure client;
    client.setInsecure(); // pular certificado SSL

    HTTPClient https;

    // https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash-8b:generateContent?key= server supostamente não cheio

    // https://generativelanguage.googleapis.com/v1beta/models/gemini-flash-latest:generateContent?key= server cheio

    String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-flash-latest:generateContent?key=" + String(GEMINI_API_KEY);

    Serial.println("Conectando a API do gemini");

    if (https.begin(client, url)) {

        // 1. dizer para o server que estamos enviando dados json
        https.addHeader("Content-Type", "application/json");

        https.setTimeout(30000);

        // 2. construir a string json parte por parte
        String payload;

/*
 * Reserva espaço antes de montar o JSON para evitar várias
 * realocações de memória enquanto o Base64 é concatenado.
 */
const size_t estimatedPayloadSize =
    base64ImageString.length() +
    prompt.length() +
    256;

if (!payload.reserve(estimatedPayloadSize)) {
    Serial.println(
        "Falha ao reservar memoria para o JSON do Gemini."
    );

    https.end();
    return "";
}

payload +=
    "{\"contents\":[{\"parts\":[";

payload +=
    "{\"text\":\"";

payload += prompt;

payload +=
    "\"},";

payload +=
    "{\"inline_data\":{"
    "\"mime_type\":\"image/jpeg\","
    "\"data\":\"";

payload += base64ImageString;

payload +=
    "\"}}";

payload +=
    "]}]}";

    for (size_t i = 0; i < payload.length(); ++i) {
    if (payload[i] == '\0') {
        Serial.printf(
            "Byte nulo encontrado no payload na posicao %u\n",
            static_cast<unsigned>(i)
        );

        https.end();
        return "";
    }
}

        // 3. Send the data
        Serial.println("enviando dados da imagem...");

        int maxRetries = 3;
        int httpResponseCode = -1;

        for (int i = 0; i < maxRetries; i++) {
            Serial.print("Tentativa ");
            Serial.println(i + 1);

            Serial.printf(
    "Base64: %u caracteres\n",
    static_cast<unsigned>(
        base64ImageString.length()
    )
);

Serial.printf(
    "Payload Gemini: %u caracteres\n",
    static_cast<unsigned>(
        payload.length()
    )
);

Serial.printf(
    "Heap livre antes do POST: %u bytes\n",
    ESP.getFreeHeap()
);

            httpResponseCode = https.POST(payload);

            // If it's 200 (Success), break out of the retry loop!
            if (httpResponseCode == 200) {
                break;
            }
            // If it's 503 (Busy) or 429 (Rate Limit), wait and try again
            else if (httpResponseCode == 503 || httpResponseCode == 429) {
                Serial.println("Ocupado, espere alguns segundos");
                delay(5000);
            } else {
                // Some other fatal error, don't retry
                break;
            }
        }

        // Now handle the final result
        if (httpResponseCode == 200) {
            response = https.getString();
        } else {
            Serial.printf("Falha após todas as tentativas. Código de erro: %d\n", httpResponseCode);
            Serial.println(https.getString()); // Print the exact error message
        }


        // 5. Close the connection
        https.end();
    } else {
        Serial.println("Não foi possível se conectar ao server.");
    }

    return response;
}


void Wifi::sendStringToTTS(String response)
{
    const String cleanText =
        Utilities::extractCleanText(response);

    Serial.println();
    Serial.println("--- RESPOSTA DO GEMINI ---");
    Serial.println(cleanText);

    if (
        cleanText.isEmpty() ||
        cleanText.startsWith("Error:")
    ) {
        Serial.println(
            "Nao foi possivel obter texto valido do Gemini."
        );
        return;
    }

    Serial.printf(
        "Texto limpo recebido: %u caracteres\n",
        static_cast<unsigned>(
            cleanText.length()
        )
    );

    Serial.printf(
        "LittleFS livre antes do TTS: %u bytes\n",
        static_cast<unsigned>(
            LittleFS.totalBytes() -
            LittleFS.usedBytes()
        )
    );

    /*
     * Começa a medir o tempo do Azure:
     * geração, recebimento e gravação do WAV.
     */
    const uint32_t azureStart =
        millis();

    const bool ttsSuccess =
        azure_tts_generate_wav(
            LittleFS,
            "/tts.wav",
            cleanText
        );

    Serial.printf(
        "[TEMPO] Azure e download do WAV: %lu ms\n",
        static_cast<unsigned long>(
            millis() - azureStart
        )
    );

    if (!ttsSuccess) {
        Serial.println(
            "Falha ao gerar WAV pelo Azure TTS."
        );
        return;
    }

    /*
     * Começa a medir inicialização do PDM,
     * reprodução, silêncio e desinicialização.
     */
    const uint32_t playbackStart =
        millis();

    Serial.println(
        "Inicializando saida PDM em 24 kHz..."
    );

    const esp_err_t pdmInitResult =
        pdm_output_init();

    if (pdmInitResult != ESP_OK) {
        Serial.printf(
            "Falha ao inicializar PDM: %s\n",
            esp_err_to_name(pdmInitResult)
        );

        LittleFS.remove("/tts.wav");
        return;
    }

    Serial.println(
        "Reproduzindo resposta do Gemini..."
    );

    const bool playbackSuccess =
        wav_player_play_file(
            LittleFS,
            "/tts.wav"
        );

    if (!playbackSuccess) {
        Serial.println(
            "Falha na reproducao do WAV."
        );
    }

    const esp_err_t silenceResult =
        pdm_output_silence(200);

    if (silenceResult != ESP_OK) {
        Serial.printf(
            "Falha ao enviar silencio: %s\n",
            esp_err_to_name(silenceResult)
        );
    }

    const esp_err_t deinitResult =
        pdm_output_deinit();

    if (deinitResult != ESP_OK) {
        Serial.printf(
            "Falha ao desinicializar PDM: %s\n",
            esp_err_to_name(deinitResult)
        );
    }

    Serial.printf(
        "[TEMPO] Reproducao do audio: %lu ms\n",
        static_cast<unsigned long>(
            millis() - playbackStart
        )
    );

    if (LittleFS.exists("/tts.wav")) {
        if (LittleFS.remove("/tts.wav")) {
            Serial.println(
                "Arquivo temporario /tts.wav removido."
            );
        } else {
            Serial.println(
                "Falha ao remover /tts.wav."
            );
        }
    }

    Serial.printf(
        "LittleFS livre ao final: %u bytes\n",
        static_cast<unsigned>(
            LittleFS.totalBytes() -
            LittleFS.usedBytes()
        )
    );

    if (playbackSuccess) {
        Serial.println(
            "Resposta do Gemini reproduzida com sucesso."
        );
    }
}
