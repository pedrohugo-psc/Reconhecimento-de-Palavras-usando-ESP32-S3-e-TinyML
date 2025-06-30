#ifdef __cplusplus
extern "C" void app_main(void);
#endif

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

// ─── begin C‐linkage for the audio drivers ─────────────────────────────────────
#ifdef __cplusplus
extern "C"
{
#endif

#include "inmp441.h"
#include "afe.h"
#include "ring_buffer.h"
#include "led_rgb.h"

#ifdef __cplusplus
}
#endif
// ─── end C‐linkage ─────────────────────────────────────────────────────────────

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "model-parameters/model_metadata.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"

#define STACK_TASK_SIZE 1024
#define STACK_TASK_SIZE_AUDIO_CAPTURE STACK_TASK_SIZE * 8
#define STACK_TASK_SIZE_AUDIO_FORMAT STACK_TASK_SIZE * 5
#define STACK_TASK_SIZE_AUDIO_INTERPRETER STACK_TASK_SIZE * 16

static const char *TAG = "main";

TaskHandle_t audio_capture_handle = NULL;
TaskHandle_t audio_format_handle = NULL;
TaskHandle_t audio_interpreter_handle = NULL;

SemaphoreHandle_t buffer_full_handle = NULL;

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

static int16_t input_buffer[BUFFER_SIZE];
static ring_buffer_t ring_buffer;

int16_t parse_sample(int32_t raw_sample)
{
    return (int16_t)(raw_sample >> (15 - GAIN_BITS));
}

void audio_capture_task(void *pvParameters)
{

    ESP_LOGI(TAG, "Recording audio");

    while (1)
    {

        void *buffer = inmp441_read_data();

        inmp441_datachunk_t *datachunk = inmp441_get_datachunk();
        int32_t *source = (int32_t *)buffer;
        int32_t samples = datachunk->bytes_read / sizeof(int32_t);

        for (int i = 0; i < samples; i += 1)
        {
            datachunk->feed_buffer[datachunk->feed_buffer_offset++] = parse_sample(source[i]); // Left mic.
            datachunk->feed_buffer[datachunk->feed_buffer_offset++] = 0;                       // Playback/reference channel.

            if (datachunk->feed_buffer_offset * sizeof(int16_t) >= datachunk->feed_buffer_len)
            {
                afe_get_handle()->feed(afe_get_data(), datachunk->feed_buffer);
                datachunk->feed_buffer_offset = 0;
            }
        }
    }
}

void audio_format_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Formatting audio");

    while (1)
    {
        afe_fetch_result_t *res = afe_get_handle()->fetch_with_delay(afe_get_data(), portMAX_DELAY);
        ESP_ERROR_CHECK(res->ret_value);

        int16_t *in_buf = (int16_t *)res->data;

        for (size_t i = 0; i < (res->data_size) / (sizeof(int16_t)); i++)
        {
            rb_insert(&ring_buffer, in_buf[i]);
        }

        while (rb_get_size(&ring_buffer) >= BUFFER_SIZE)
        {

            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                int idx = (ring_buffer.start + i) % BUFFER_SIZE;
                input_buffer[i] = ring_buffer.buffer[idx];
            }

            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                rb_remove(&ring_buffer);
            }

            xSemaphoreGive(buffer_full_handle);
        }
    }
}

void audio_interpreter_task(void *pvParameters)
{

    ESP_LOGI(TAG, "Interpreting audio");

    signal_t signal;
    ei_impulse_result_t result;
    EI_IMPULSE_ERROR ret;

    size_t buf_len = sizeof(input_buffer) / sizeof(input_buffer[0]);

    if (buf_len != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE)
    {
        ESP_LOGE(TAG, "ERROR: The size of the input buffer is not correct.");
        ESP_LOGE(TAG, "Expected %d items, but got %d",
                 EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE,
                 (int)buf_len);
        esp_restart();
    }

    // // Assign callback function to fill buffer used for preprocessing/inference
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &get_signal_data;

    while (1)
    {
        if (xSemaphoreTake(buffer_full_handle, portMAX_DELAY) == pdPASS)
        {

            ret = run_classifier_continuous(&signal, &result, false);

            // Print return code and how long it took to perform inference
            ESP_LOGI(TAG, "run_classifier returned: %d", ret);
            ESP_LOGI(TAG, "Timing: DSP %d ms, inference %d ms, anomaly %d ms",
                     result.timing.dsp,
                     result.timing.classification,
                     result.timing.anomaly);

            // Print the prediction results (object detection)
            ESP_LOGI(TAG, "Predictions:");
            for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++)
            {
                ESP_LOGI(TAG, "%s: %.5f", ei_classifier_inferencing_categories[i], result.classification[i].value);
            }

            for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++)
            {
                led_rgb_command(ei_classifier_inferencing_categories[i], result.classification[i].value);
            }
            
        }
    }
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr)
{

    numpy::int16_to_float(&input_buffer[offset], out_ptr, length);

    return EIDSP_OK;
}

extern "C" void app_main(void)
{
    inmp441_setup();
    afe_setup();
    led_rgb_setup();
    inmp441_enable();

    rb_init(&ring_buffer);

    run_classifier_init();

    buffer_full_handle = xSemaphoreCreateBinary();
    if (buffer_full_handle == NULL)
    {
        ESP_LOGE(TAG, "Failed to create semaphore");
        esp_restart();
    }

    xTaskCreatePinnedToCore(audio_capture_task, "Audio_Capture", STACK_TASK_SIZE_AUDIO_CAPTURE, NULL, 5, &audio_capture_handle, 1);
    xTaskCreatePinnedToCore(audio_format_task, "Audio_Format", STACK_TASK_SIZE_AUDIO_FORMAT, NULL, 5, &audio_format_handle, 1);
    xTaskCreatePinnedToCore(audio_interpreter_task, "Audio_Interpreter", STACK_TASK_SIZE_AUDIO_INTERPRETER, NULL, 5, &audio_interpreter_handle, 0);

    if ((audio_capture_handle == NULL) || (audio_format_handle == NULL) || (audio_interpreter_handle == NULL))
    {
        ESP_LOGE(TAG, "Failed to create task");
        esp_restart();
    }

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
