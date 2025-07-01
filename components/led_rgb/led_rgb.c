#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BLINK_GPIO 48
#define RESOLUTION_100MHz 10 * 1000 * 1000

static const char *TAG = "LEG_RGB";

static uint8_t s_led_state = 0;
static led_strip_handle_t led_strip;
static TaskHandle_t led_blink_handle = NULL;

static uint8_t red;
static uint8_t green;
static uint8_t blue;

static void led_rgb_init_colors()
{
    red = 16;
    green = 16;
    blue = 16;
}

static void led_rgb_check_blink_handle()
{
    if (led_blink_handle != NULL)
    {
        vTaskDelete(led_blink_handle);
        led_blink_handle = NULL;
    }
}

void led_rgb_setup()
{

    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1,
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = RESOLUTION_100MHz,
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    led_strip_clear(led_strip);

    led_rgb_init_colors();
}

static void led_rgb_turn_off()
{
    led_strip_clear(led_strip);
}

static void led_rgb_turn_on()
{
    led_strip_set_pixel(led_strip, 0, red, green, blue);
    led_strip_refresh(led_strip);
}

static void led_rgb_change_color()
{

    srand(time(0));

    red = rand() % 256;
    green = rand() % 256;
    blue = rand() % 256;

    led_strip_set_pixel(led_strip, 0, red, green, blue);
    led_strip_refresh(led_strip);
}

static void led_rgb_blink_task(void *pvParameters)
{

    while (1)
    {
        if (s_led_state)
        {
            led_rgb_turn_on();
        }
        else
        {
            led_rgb_turn_off();
        }

        s_led_state = !s_led_state;

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void led_rgb_blink()
{

    xTaskCreatePinnedToCore(led_rgb_blink_task, "LED_RGB_BLINK", configMINIMAL_STACK_SIZE, NULL, 4, &led_blink_handle, 1);

    if (led_blink_handle == NULL)
    {
        ESP_LOGE(TAG, "Failed to create task");
    }
}

void led_rgb_command(const char *command, float value_detection)
{
    if (value_detection < 0.85)
    {
        return;
    }
    

    if ((strcmp(command, "zero") == 0) && (value_detection > 0.85))
    {
        led_rgb_check_blink_handle();

        ESP_LOGI(TAG, "Command 0: Turn off LED");
        s_led_state = 0;
        led_rgb_turn_off();
    }
    else if ((strcmp(command, "um") == 0) && (value_detection > 0.85))
    {
        led_rgb_check_blink_handle();

        ESP_LOGI(TAG, "Command 1: Turn on LED");
        s_led_state = 1;
        led_rgb_turn_on();
    }
    else if ((strcmp(command, "dois") == 0) && (value_detection > 0.85))
    {
        led_rgb_check_blink_handle();

        ESP_LOGI(TAG, "Command 2: Change color LED");
        led_rgb_change_color();
    }
    else if ((strcmp(command, "tres") == 0) && (value_detection > 0.85))
    {
        led_rgb_check_blink_handle();

        ESP_LOGI(TAG, "Command 3: Blink LED");
        led_rgb_blink();
    }
}