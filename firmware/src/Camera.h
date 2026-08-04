//
// Created by wenoysd on 16/06/2026.
//

#ifndef NOT_ESPECIAL_VISION_CAMERA_H
#define NOT_ESPECIAL_VISION_CAMERA_H
#include "esp_camera.h"
#include "mbedtls/base64.h"
#include <Arduino.h>



#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39
#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

enum VisionMode {
    MODE_NIGHT_SCAN,
    MODE_CLOSE_READING,
    MODE_LANDSCAPE_AMBIENT
};


class Camera
{
private:
    sensor_t * s;
    camera_config_t config;

public:
    Camera();

    String takePhoto64();
    void applyVisionMode(VisionMode mode);


};


#endif //NOT_ESPECIAL_VISION_CAMERA_H
