#include "pdm_output.h"

#include <stdbool.h>
#include <string.h>

#include "driver/i2s.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#define PDM_I2S_PORT I2S_NUM_0

#define PDM_DATA_GPIO 1
#define PDM_CLK_GPIO  2

#define PDM_DMA_BUFFER_COUNT    4
#define PDM_DMA_BUFFER_SAMPLES  256
#define SILENCE_BUFFER_SAMPLES  256

static const char *TAG = "pdm_output";

static bool pdm_initialized = false;

esp_err_t pdm_output_init(void)
{
    if (pdm_initialized) {
        ESP_LOGW(TAG, "PDM ja estava inicializado.");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Inicio de pdm_output_init()");

    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(
            I2S_MODE_MASTER |
            I2S_MODE_TX |
            I2S_MODE_PDM
        ),

        .sample_rate = PDM_SAMPLE_RATE_HZ,

        .bits_per_sample =
            I2S_BITS_PER_SAMPLE_16BIT,

        .channel_format =
            I2S_CHANNEL_FMT_ONLY_LEFT,

        .communication_format =
            I2S_COMM_FORMAT_STAND_I2S,

        .intr_alloc_flags = 0,

        .dma_buf_count =
            PDM_DMA_BUFFER_COUNT,

        .dma_buf_len =
            PDM_DMA_BUFFER_SAMPLES,

        .use_apll = false,

        .tx_desc_auto_clear = true,

        .fixed_mclk = 0
    };

    ESP_LOGI(
        TAG,
        "Etapa 1: chamando i2s_driver_install()"
    );

    esp_err_t err = i2s_driver_install(
        PDM_I2S_PORT,
        &i2s_config,
        0,
        NULL
    );

    ESP_LOGI(
        TAG,
        "Etapa 1 retornou: %s (%d)",
        esp_err_to_name(err),
        (int)err
    );

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Falha ao instalar driver I2S"
        );

        return err;
    }

    const i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_PIN_NO_CHANGE,
        .bck_io_num = I2S_PIN_NO_CHANGE,
        .ws_io_num = PDM_CLK_GPIO,
        .data_out_num = PDM_DATA_GPIO,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    ESP_LOGI(
        TAG,
        "Etapa 2: chamando i2s_set_pin()"
    );

    err = i2s_set_pin(
        PDM_I2S_PORT,
        &pin_config
    );

    ESP_LOGI(
        TAG,
        "Etapa 2 retornou: %s (%d)",
        esp_err_to_name(err),
        (int)err
    );

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Falha ao configurar pinos PDM"
        );

        const esp_err_t uninstall_err =
            i2s_driver_uninstall(PDM_I2S_PORT);

        ESP_LOGI(
            TAG,
            "Remocao do driver retornou: %s (%d)",
            esp_err_to_name(uninstall_err),
            (int)uninstall_err
        );

        return err;
    }

    ESP_LOGI(
        TAG,
        "Etapa 3: chamando i2s_zero_dma_buffer()"
    );

    err = i2s_zero_dma_buffer(PDM_I2S_PORT);

    ESP_LOGI(
        TAG,
        "Etapa 3 retornou: %s (%d)",
        esp_err_to_name(err),
        (int)err
    );

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Falha ao zerar buffers DMA"
        );

        const esp_err_t uninstall_err =
            i2s_driver_uninstall(PDM_I2S_PORT);

        ESP_LOGI(
            TAG,
            "Remocao do driver retornou: %s (%d)",
            esp_err_to_name(uninstall_err),
            (int)uninstall_err
        );

        return err;
    }

    pdm_initialized = true;

    ESP_LOGI(
        TAG,
        "PDM inicializado: DATA=GPIO%d, CLK=GPIO%d, taxa=%d Hz",
        PDM_DATA_GPIO,
        PDM_CLK_GPIO,
        PDM_SAMPLE_RATE_HZ
    );

    ESP_LOGI(TAG, "Fim de pdm_output_init()");

    return ESP_OK;
}

esp_err_t pdm_output_write(
    const int16_t *samples,
    size_t sample_count
)
{
    if (!pdm_initialized) {
        ESP_LOGE(TAG, "PDM nao foi inicializado.");
        return ESP_ERR_INVALID_STATE;
    }

    if (samples == NULL || sample_count == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    const size_t requested_bytes =
        sample_count * sizeof(int16_t);

    size_t bytes_written = 0;

    const esp_err_t err = i2s_write(
        PDM_I2S_PORT,
        samples,
        requested_bytes,
        &bytes_written,
        portMAX_DELAY
    );

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Falha ao escrever PCM: %s",
            esp_err_to_name(err)
        );

        return err;
    }

    if (bytes_written != requested_bytes) {
        ESP_LOGE(
            TAG,
            "Escrita incompleta: esperado=%u, escrito=%u",
            (unsigned)requested_bytes,
            (unsigned)bytes_written
        );

        return ESP_ERR_INVALID_SIZE;
    }

    return ESP_OK;
}

esp_err_t pdm_output_silence(uint32_t duration_ms)
{
    static int16_t silence[
        SILENCE_BUFFER_SAMPLES
    ];

    memset(
        silence,
        0,
        sizeof(silence)
    );

    uint32_t samples_left =
        ((uint32_t)PDM_SAMPLE_RATE_HZ * duration_ms)
        / 1000U;

    while (samples_left > 0) {
        const size_t sample_count =
            samples_left > SILENCE_BUFFER_SAMPLES
                ? SILENCE_BUFFER_SAMPLES
                : (size_t)samples_left;

        const esp_err_t err = pdm_output_write(
            silence,
            sample_count
        );

        if (err != ESP_OK) {
            return err;
        }

        samples_left -= sample_count;
    }

    return ESP_OK;
}

esp_err_t pdm_output_deinit(void)
{
    if (!pdm_initialized) {
        return ESP_OK;
    }

    /*
     * Preenche o DMA com silêncio antes de remover
     * o driver, reduzindo ruídos no desligamento.
     */
    i2s_zero_dma_buffer(PDM_I2S_PORT);

    const esp_err_t err =
        i2s_driver_uninstall(PDM_I2S_PORT);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Falha ao remover driver I2S: %s",
            esp_err_to_name(err)
        );

        return err;
    }

    pdm_initialized = false;

    ESP_LOGI(TAG, "PDM desinicializado.");

    return ESP_OK;
}