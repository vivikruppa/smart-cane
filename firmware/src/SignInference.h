/*
* SignInference
 * ------------------------------------------------------------------
 * Wraps the Edge Impulse camera + classifier pipeline (originally the
 * esp32_camera.ino example) as a reusable module. Drop this folder
 * into lib/ of any PlatformIO project, then from your own main.cpp:
 *
 *   #include "SignInference.h"
 *   SignInference detector;
 *
 *   void setup() {
 *       Serial.begin(115200);
 *       detector.begin();
 *   }
 *
 *   void loop() {
 *       detector.run();
 *   }
 *
 * Adapted from the Edge Impulse Arduino camera example
 * (c) 2022 EdgeImpulse Inc. — MIT licensed.
 * ------------------------------------------------------------------
 */

#pragma once

#include <Arduino.h>
#include "esp_camera.h"

// Select the camera board wiring you're actually using.
// (Original example defaulted to ESP_EYE — verify against your hardware.)
#define CAMERA_MODEL_XIAO_ESP32S3_SENSE
//#define CAMERA_MODEL_AI_THINKER

class SignInference {
public:
    // Initializes serial-independent camera hardware. Call once from setup().
    bool begin();

    // Captures one frame, runs the classifier, and prints results to Serial.
    // Call repeatedly from loop().
    void run();

    // Stops the camera sensor.
    void end();

private:
    bool ei_camera_init();
    void ei_camera_deinit();
    bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
    static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);

    bool _is_initialised = false;
    bool _debug_nn = false;
    uint8_t *_snapshot_buf = nullptr;

    static SignInference *_instance; // needed so the static C-style callback can reach instance data
};