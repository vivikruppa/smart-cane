#ifndef PDM_OUTPUT_H
#define PDM_OUTPUT_H

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PDM_SAMPLE_RATE_HZ 24000

/**
 * Inicializa o I2S0 em modo PDM TX.
 *
 * Entrada:
 * PCM mono, 16 bits, 24 kHz.
 *
 * Saída:
 * DATA no GPIO 1
 * CLK  no GPIO 2
 */
esp_err_t pdm_output_init(void);

/**
 * Envia um bloco de amostras PCM para a saída PDM.
 */
esp_err_t pdm_output_write(
    const int16_t *samples,
    size_t sample_count
);

/**
 * Envia silêncio durante o intervalo informado.
 */
esp_err_t pdm_output_silence(uint32_t duration_ms);

/**
 * Desativa e remove o canal I2S/PDM.
 */
esp_err_t pdm_output_deinit(void);

#ifdef __cplusplus
}
#endif

#endif