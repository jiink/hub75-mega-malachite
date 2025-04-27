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

const uint64_t KNOB_DEBOUNCE_US = 15000; // for turning the knob. in microseconds
volatile uint64_t prevKnobIsrTime = 0;

MatrixPanel_I2S_DMA *matrix = nullptr;
volatile uint8_t knobVal = 0;

static void IRAM_ATTR knob_clk_falling_handler(void* arg)
{
    uint64_t currTime = esp_timer_get_time();
    if ((currTime - prevKnobIsrTime) < KNOB_DEBOUNCE_US)
    {
        return;
    }
    bool dtState = gpio_get_level(ENCODER_DT_PIN);
    if (dtState) {
        knobVal = knobVal + 1;
    } else {
        knobVal = knobVal - 1;
    }
    prevKnobIsrTime = currTime;
}

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
    knob_conf.pin_bit_mask = (1ULL << ENCODER_CLK_PIN);
    knob_conf.mode = GPIO_MODE_INPUT;
    knob_conf.pull_up_en = GPIO_PULLUP_DISABLE; // external pull-ups
    knob_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    knob_conf.intr_type = GPIO_INTR_NEGEDGE; // Trigger on falling edge
    gpio_config(&knob_conf);
    // DT knob pin
    knob_conf.pin_bit_mask = (1ULL << ENCODER_DT_PIN);
    knob_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&knob_conf);
    // button
    knob_conf.pin_bit_mask = (1ULL << ENCODER_SW_PIN);
    knob_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&knob_conf);

    // Interrupts
    esp_err_t isr_err = gpio_install_isr_service(0);
    if (isr_err != ESP_OK)
    {
        printf("Couldn't initialize ISR service: %d\n", isr_err);
    }
    esp_err_t aisr_err = gpio_isr_handler_add(ENCODER_CLK_PIN, knob_clk_falling_handler, NULL);
    if (aisr_err != ESP_OK)
    {
        printf("Couldn't add ISR handler: %d\n", aisr_err);
    }

    static int t = 0;
    bool btn = false;
    while (true) {
        t++;
        btn = !gpio_get_level(ENCODER_SW_PIN);

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
