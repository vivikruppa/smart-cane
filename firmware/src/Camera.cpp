//
// Created by wenoysd on 16/06/2026.
//

#include "Camera.h"



String Camera::takePhoto64()
{
    Serial.println("Capturando foto...");

    // Tirar a foto
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Captura de foto falha");
        return "";
    }

    Serial.printf("Foto tirada! Tamanho: %d bytes\n", fb->len);
    Serial.println("Convertendo para base64...");

    // Calcular quanta memória a string base64 vai utilizar
    size_t outputLength;
    mbedtls_base64_encode(nullptr, 0, &outputLength, fb->buf, fb->len);

    // alocar memória
    unsigned char* encodedBuffer = (unsigned char*)malloc(outputLength);
    if (encodedBuffer == nullptr) {
        Serial.println("Erro: sem memória o suficiente para converter para base64!");
        esp_camera_fb_return(fb); // limpar memória da camera
        return "";
    }

    // Converter
    mbedtls_base64_encode(encodedBuffer, outputLength, &outputLength, fb->buf, fb->len);

    // Converter para uma string
    String result = String((char*)encodedBuffer);

    // limpar a memoria
    free(encodedBuffer);
    esp_camera_fb_return(fb);

    Serial.println("Conversao completa.");
    return result;
}




Camera::Camera() {



    while(!Serial) { delay(10); }

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 10000000;



    // Formato JPEG para envio rapido
    config.pixel_format = PIXFORMAT_JPEG;

    // Framesize QXGA para ter o max de qualidade possivel
    config.frame_size = FRAMESIZE_QXGA;
    config.jpeg_quality = 10; // qualidade do jpeg
    config.fb_count = 2;

    // Initialize the Camera
    esp_err_t err = esp_camera_init(&config);





    s = esp_camera_sensor_get();
    if (s != NULL) {
        s->set_whitebal(s, 1);       // Habilitar Basic White Balance
        s->set_awb_gain(s, 1);       // Habilitar Auto White Balance Gain

        //
        s->set_wb_mode(s, 2);        // 0:Auto, 1:Sunny, 2:Office/Fluorescent, 3:Cloudy

        // Brilho e contraste
        s->set_brightness(s, 2);     // Varia de -2 to 2
        s->set_contrast(s, 1);       // Varia de  -2 to 2
    }




    for (int i = 0; i < 15; i++) {
        // Aquecer a camera?
    }

    Serial.println("Camera inicializada com sucesso.");
}

void Camera::applyVisionMode(VisionMode mode)
{



    // Diminuir o ganho para zero
    s->set_gain_ctrl(s, 0);  // Desabilitar Auto Gain Control
    s->set_agc_gain(s, 0);   // Ganho 0

    switch (mode) {
    case MODE_NIGHT_SCAN:
        s->set_exposure_ctrl(s, 0);  // Desabilitar auto-exposição
        s->set_aec_value(s, 1200);   // Maximizar stutter time
        s->set_whitebal(s, 0);       // Disabilitar auto-balançeamento
        s->set_wb_mode(s, 0);        // Colocar white-balance como zero

        Serial.println("Modo Noturno");
        break;

    case MODE_CLOSE_READING:
        s->set_exposure_ctrl(s, 0);  // Habilitar Auto Exposição
        s->set_aec_value(s, 400);    // Exposição média
        s->set_whitebal(s, 1);       // Habilitar auto balanceamento
        s->set_wb_mode(s, 3);        // White balance Modo 3 =

        Serial.println("Macro");
        break;

    case MODE_LANDSCAPE_AMBIENT:
        s->set_exposure_ctrl(s, 1);  // Habilitar auto Exposição
        s->set_whitebal(s, 1);       // Habilitar auto balanceamento
        s->set_wb_mode(s, 1);        // White balance modo 1

        Serial.println("Paisagem");
        break;
    }

    // Delay
    delay(250);

}

