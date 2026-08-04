#include "azure_tts_client.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "secrets.h"

namespace
{
String escapeXml(const String &text)
{
    String escaped;
    escaped.reserve(text.length() + 64);

    for (size_t i = 0; i < text.length(); ++i) {
        const char character = text[i];

        switch (character) {
            case '&':
                escaped += "&amp;";
                break;

            case '<':
                escaped += "&lt;";
                break;

            case '>':
                escaped += "&gt;";
                break;

            case '"':
                escaped += "&quot;";
                break;

            case '\'':
                escaped += "&apos;";
                break;

            default:
                escaped += character;
                break;
        }
    }

    return escaped;
}

bool isValidWav(
    fs::FS &fileSystem,
    const char *path
)
{
    File file =
        fileSystem.open(
            path,
            FILE_READ
        );

    if (!file) {
        return false;
    }

    uint8_t header[12];

    const size_t bytesRead =
        file.read(
            header,
            sizeof(header)
        );

    file.close();

    if (bytesRead != sizeof(header)) {
        return false;
    }

    return
        memcmp(header, "RIFF", 4) == 0 &&
        memcmp(header + 8, "WAVE", 4) == 0;
}
}

bool azure_tts_generate_wav(
    fs::FS &fileSystem,
    const char *outputPath,
    const String &text
)
{
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi nao conectado.");
        return false;
    }

    if (text.isEmpty()) {
        Serial.println("Texto do TTS esta vazio.");
        return false;
    }

    WiFiClientSecure secureClient;

    /*
     * Somente para o primeiro teste.
     * Depois substituiremos por validação TLS.
     */
    secureClient.setInsecure();

    HTTPClient http;

    http.setConnectTimeout(15000);
    http.setTimeout(60000);

    if (!http.begin(
            secureClient,
            AZURE_TTS_URL
        )) {
        Serial.println(
            "Falha ao iniciar conexao com Azure."
        );

        return false;
    }

    http.addHeader(
        "Ocp-Apim-Subscription-Key",
        AZURE_SPEECH_KEY
    );

    http.addHeader(
        "Content-Type",
        "application/ssml+xml"
    );

    http.addHeader(
        "X-Microsoft-OutputFormat",
        "riff-24khz-16bit-mono-pcm"
    );

    http.addHeader(
        "User-Agent",
        "CFA-XIAO-ESP32S3"
    );

    const String escapedText =
        escapeXml(text);

    String ssml;

    ssml.reserve(
        escapedText.length() + 256
    );

    ssml =
        "<speak version=\"1.0\" "
        "xmlns=\"http://www.w3.org/2001/10/synthesis\" "
        "xml:lang=\"pt-BR\">"
        "<voice name=\"pt-BR-FranciscaNeural\">";

    ssml += escapedText;

    ssml +=
        "</voice>"
        "</speak>";

    Serial.println(
        "Enviando solicitacao ao Azure TTS..."
    );

    const int httpCode =
    http.POST(ssml);

    Serial.printf(
        "Codigo HTTP Azure: %d\n",
        httpCode
    );

    if (httpCode != HTTP_CODE_OK) {
        const String errorBody =
            http.getString();

        Serial.println(
            "Erro retornado pelo Azure:"
        );

        Serial.println(errorBody);

        http.end();
        return false;
    }

    if (fileSystem.exists(outputPath)) {
    fileSystem.remove(outputPath);
}

    File output =
        fileSystem.open(
            outputPath,
            FILE_WRITE
        );

    if (!output) {
        Serial.printf(
            "Nao foi possivel criar %s\n",
            outputPath
        );

        http.end();
        return false;
    }

    Serial.printf(
    "Tamanho informado pelo Azure: %d bytes\n",
    http.getSize()
);

    const int bytesWritten =
    http.writeToStream(&output);

output.close();
http.end();

if (bytesWritten < 0) {
    Serial.printf(
        "Falha ao receber audio do Azure: %s\n",
        HTTPClient::errorToString(
            bytesWritten
        ).c_str()
    );

    fileSystem.remove(outputPath);
    return false;
}

const size_t totalBytesWritten =
    static_cast<size_t>(bytesWritten);

Serial.printf(
    "Arquivo recebido: %u bytes\n",
    static_cast<unsigned>(
        totalBytesWritten
    )
);

    if (totalBytesWritten == 0) {
        Serial.println(
            "Azure retornou arquivo vazio."
        );

        fileSystem.remove(outputPath);
        return false;
    }

    if (!isValidWav(
            fileSystem,
            outputPath
        )) {
        Serial.println(
            "Resposta recebida nao possui "
            "cabecalho WAV valido."
        );

        fileSystem.remove(outputPath);
        return false;
    }

    Serial.printf(
        "WAV Azure salvo com sucesso em %s\n",
        outputPath
    );

    return true;
}