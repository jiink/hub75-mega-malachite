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
#include "IApplet.h"
#include "SimpleClock.h"
#include "Raymarch.h"
#include "Simplex.h"
#include "JaDraw.h"
#include "system.h"
#include "Raytrace.h"

MatrixPanel_I2S_DMA *matrix = nullptr;

std::array<std::unique_ptr<IApplet>, 4> applets = {
    std::make_unique<Raytrace>(),
    std::make_unique<Simplex>(),
    std::make_unique<Raymarch>(),
    std::make_unique<SimpleClock>()
};

static void drawNum(uint8_t n)
{
    for (int i = 0; i < 8; i++) {
        if ((n >> i) % 2 == 0) {
            matrix->drawPixelRGB888(i, i, 255, 255, 255);
        }
    }
    matrix->drawPixelRGB888(n%WIDTH, HEIGHT-1, 255, 255, 255);
}

static void rotaryEncoderSetup() {
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
}

extern "C" void app_main(void)
{
    printf("**** Lapillos v0 - the ESP32 LED board ****\n");
    HUB75_I2S_CFG mxconfig(WIDTH, HEIGHT, 1);
    mxconfig.double_buff = true;
    matrix = new MatrixPanel_I2S_DMA(mxconfig);
    matrix->begin();
    matrix->setBrightness8(80);

    rotaryEncoderSetup();

    //static int t = 0;
    bool btn = false;
    uint8_t knobVal = 0;
    bool knobClkPrev = false;
    size_t appletIdx = 0;
    applets[appletIdx]->setup();
    int64_t last_loop_time_us = esp_timer_get_time();
    float delta_time_sec = 0.0f;
    static JaDraw<WIDTH, HEIGHT> canvas;
    while (true) {
        int64_t current_loop_time_us = esp_timer_get_time();
        delta_time_sec = (float)(current_loop_time_us - last_loop_time_us) / 1000000.0f;
        last_loop_time_us = current_loop_time_us;
        btn = !gpio_get_level(ENCODER_SW_PIN);
        bool knobClk = gpio_get_level(ENCODER_CLK_PIN);
        bool knobClkRising = knobClk && !knobClkPrev;
        if (knobClkRising)
        {
            if (gpio_get_level(ENCODER_DT_PIN)) {
                knobVal++;
                //appletIdx++;
            } else {
                knobVal--;
                //appletIdx--;
            }
        }
        knobClkPrev = knobClk;
        matrix->flipDMABuffer();
        vTaskDelay(pdMS_TO_TICKS(1000/matrix->calculated_refresh_rate));
        //matrix->fillScreenRGB888(0, 5, 20);
        //drawNum(knobVal);
        if (btn)
        {
            //matrix->drawPixelRGB888(32, 4, 0, 255, 0);
        }
        if (!gpio_get_level(ENCODER_CLK_PIN))
        {
            //matrix->drawPixelRGB888(32, 6, 255, 255, 0);
        }
        if (!gpio_get_level(ENCODER_DT_PIN))
        {
            //matrix->drawPixelRGB888(32, 7, 0, 255, 255);
        }
        InputData nothing;
        applets[appletIdx]->loop(canvas, delta_time_sec, nothing);
        // transfer canvas to matrix
        for (size_t i = 0; i < canvas.canvas.size(); i++)
        {
            int16_t x = i % WIDTH;
            int16_t y = i / WIDTH;
            auto color = canvas.canvas[i];
            matrix->drawPixelRGB888(x, y,
                JADRAW_RED(color),
                JADRAW_GREEN(color),
                JADRAW_BLUE(color));
        }
    }
}
