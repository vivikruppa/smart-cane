#include "wav_player.h"

#include <cstring>

#include "pdm_output.h"

namespace
{
constexpr uint16_t WAV_PCM_FORMAT = 1;
constexpr uint16_t WAV_EXPECTED_CHANNELS = 1;
constexpr uint16_t WAV_EXPECTED_BITS = 16;
constexpr size_t WAV_STREAM_SAMPLES = 256;

uint16_t readLe16(const uint8_t *data)
{
    return static_cast<uint16_t>(data[0]) |
           (static_cast<uint16_t>(data[1]) << 8);
}

uint32_t readLe32(const uint8_t *data)
{
    return static_cast<uint32_t>(data[0]) |
           (static_cast<uint32_t>(data[1]) << 8) |
           (static_cast<uint32_t>(data[2]) << 16) |
           (static_cast<uint32_t>(data[3]) << 24);
}

bool readExact(
    File &file,
    uint8_t *buffer,
    size_t size
)
{
    return file.read(buffer, size) == size;
}
}

bool wav_player_play_file(
    fs::FS &file_system,
    const char *path
)
{
    File file = file_system.open(path, FILE_READ);

    if (!file) {
        Serial.printf(
            "Nao foi possivel abrir o arquivo: %s\n",
            path
        );
        return false;
    }

    Serial.printf(
        "Arquivo aberto: %s, tamanho: %u bytes\n",
        path,
        static_cast<unsigned>(file.size())
    );

    uint8_t riff_header[12];

    if (!readExact(file, riff_header, sizeof(riff_header))) {
        Serial.println("Falha ao ler cabecalho RIFF.");
        file.close();
        return false;
    }

    if (
        std::memcmp(riff_header, "RIFF", 4) != 0 ||
        std::memcmp(riff_header + 8, "WAVE", 4) != 0
    ) {
        Serial.println("Arquivo nao possui cabecalho RIFF/WAVE.");
        file.close();
        return false;
    }

    bool found_fmt = false;
    bool found_data = false;

    uint16_t audio_format = 0;
    uint16_t channels = 0;
    uint32_t sample_rate = 0;
    uint16_t bits_per_sample = 0;
    uint32_t data_size = 0;

    while (
        file.available() &&
        (!found_fmt || !found_data)
    ) {
        uint8_t chunk_header[8];

        if (!readExact(
            file,
            chunk_header,
            sizeof(chunk_header)
        )) {
            Serial.println("Falha ao ler cabecalho de chunk.");
            file.close();
            return false;
        }

        const uint32_t chunk_size =
            readLe32(chunk_header + 4);

        if (std::memcmp(
            chunk_header,
            "fmt ",
            4
        ) == 0) {
            if (chunk_size < 16) {
                Serial.println("Chunk fmt invalido.");
                file.close();
                return false;
            }

            uint8_t fmt_data[16];

            if (!readExact(
                file,
                fmt_data,
                sizeof(fmt_data)
            )) {
                Serial.println("Falha ao ler chunk fmt.");
                file.close();
                return false;
            }

            audio_format =
                readLe16(fmt_data);

            channels =
                readLe16(fmt_data + 2);

            sample_rate =
                readLe32(fmt_data + 4);

            bits_per_sample =
                readLe16(fmt_data + 14);

            found_fmt = true;

            const uint32_t remaining =
                chunk_size - sizeof(fmt_data);

            if (remaining > 0) {
                file.seek(
                    file.position() + remaining
                );
            }
        }
        else if (std::memcmp(
            chunk_header,
            "data",
            4
        ) == 0) {
            data_size = chunk_size;
            found_data = true;

            /*
             * Não avançamos aqui:
             * a posição atual já está no início do PCM.
             */
        }
        else {
            file.seek(
                file.position() + chunk_size
            );
        }

        /*
         * Chunks RIFF são alinhados em 2 bytes.
         */
        if (
            !found_data &&
            (chunk_size & 1U)
        ) {
            file.seek(file.position() + 1);
        }
    }

    if (!found_fmt || !found_data) {
        Serial.println("Chunks fmt ou data nao encontrados.");
        file.close();
        return false;
    }

    Serial.printf(
        "WAV: formato=%u, canais=%u, taxa=%lu Hz, bits=%u, PCM=%lu bytes\n",
        audio_format,
        channels,
        static_cast<unsigned long>(sample_rate),
        bits_per_sample,
        static_cast<unsigned long>(data_size)
    );

    if (audio_format != WAV_PCM_FORMAT) {
        Serial.println("O WAV nao usa PCM sem compressao.");
        file.close();
        return false;
    }

    if (channels != WAV_EXPECTED_CHANNELS) {
        Serial.println("O WAV nao e mono.");
        file.close();
        return false;
    }

    if (bits_per_sample != WAV_EXPECTED_BITS) {
        Serial.println("O WAV nao usa PCM de 16 bits.");
        file.close();
        return false;
    }

    if (sample_rate != PDM_SAMPLE_RATE_HZ) {
        Serial.printf(
            "Taxa incompatível: WAV=%lu Hz, PDM=%u Hz\n",
            static_cast<unsigned long>(sample_rate),
            PDM_SAMPLE_RATE_HZ
        );

        file.close();
        return false;
    }

    if ((data_size % sizeof(int16_t)) != 0) {
        Serial.println("Quantidade de bytes PCM invalida.");
        file.close();
        return false;
    }

    int16_t pcm_buffer[WAV_STREAM_SAMPLES];

    uint32_t bytes_left = data_size;

    while (bytes_left > 0) {
        const size_t bytes_to_read =
            bytes_left > sizeof(pcm_buffer)
                ? sizeof(pcm_buffer)
                : static_cast<size_t>(bytes_left);

        const size_t bytes_read =
            file.read(
                reinterpret_cast<uint8_t *>(pcm_buffer),
                bytes_to_read
            );

        if (bytes_read == 0) {
            Serial.println("Falha ao ler dados PCM.");
            file.close();
            return false;
        }

        if ((bytes_read % sizeof(int16_t)) != 0) {
            Serial.println("Bloco PCM com tamanho invalido.");
            file.close();
            return false;
        }

        const size_t sample_count =
            bytes_read / sizeof(int16_t);

        const esp_err_t err =
            pdm_output_write(
                pcm_buffer,
                sample_count
            );

        if (err != ESP_OK) {
            Serial.printf(
                "Falha ao enviar PCM: %s\n",
                esp_err_to_name(err)
            );

            file.close();
            return false;
        }

        bytes_left -= bytes_read;
    }

    file.close();

    Serial.println("Reproducao WAV concluida.");

    return true;
}