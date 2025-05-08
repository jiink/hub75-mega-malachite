#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define WIDTH 64
#define HEIGHT 32

#define ENCODER_CLK_PIN GPIO_NUM_36
#define ENCODER_DT_PIN  GPIO_NUM_39
#define ENCODER_SW_PIN  GPIO_NUM_34 // switch (button)

MatrixPanel_I2S_DMA *matrix = nullptr;

static void drawNum(uint8_t n)
{
    for (int i = 0; i < 8; i++) {
        if ((n >> i) % 2 == 0) {
            matrix->drawPixelRGB888(i, i, 255, 255, 255);
        }
    }
    matrix->drawPixelRGB888(n%WIDTH, HEIGHT-1, 255, 255, 255);
}

extern "C" void app_main(void)
{
    printf("Hello world!\n");

    HUB75_I2S_CFG mxconfig(WIDTH, HEIGHT, 1);
    matrix = new MatrixPanel_I2S_DMA(mxconfig);
    matrix->begin();
    matrix->setBrightness8(80);
    matrix->clearScreen();
    matrix->fillScreenRGB888(100, 0, 200);

    gpio_config_t knob_conf = {};
    // Configure CLK pin for input and falling edge interrupt
    knob_conf.pin_bit_mask = (1ULL << ENCODER_CLK_PIN)
        | (1ULL << ENCODER_DT_PIN)
        | (1ULL << ENCODER_SW_PIN);
    knob_conf.mode = GPIO_MODE_INPUT;
    knob_conf.pull_up_en = GPIO_PULLUP_DISABLE; // external pull-ups
    knob_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    knob_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&knob_conf);

    static int t = 0;
    bool btn = false;
    uint8_t knobVal = 0;
    bool knobClkPrev = false;
    while (true) {
        t++;
        btn = !gpio_get_level(ENCODER_SW_PIN);
        bool knobClk = gpio_get_level(ENCODER_CLK_PIN);
        bool knobClkRising = knobClk && !knobClkPrev;
        if (knobClkRising)
        {
            if (gpio_get_level(ENCODER_DT_PIN)) {
                knobVal++;
            } else {
                knobVal--;
            }
        }
        knobClkPrev = knobClk;
        matrix->fillScreenRGB888(0, 5, 20);
        drawNum(knobVal);
        if (btn)
        {
            matrix->drawPixelRGB888(32, 4, 0, 255, 0);
        }
        if (!gpio_get_level(ENCODER_CLK_PIN))
        {
            matrix->drawPixelRGB888(32, 6, 255, 255, 0);
        }
        if (!gpio_get_level(ENCODER_DT_PIN))
        {
            matrix->drawPixelRGB888(32, 7, 0, 255, 255);
        }
        vTaskDelay(1);
    }
}
