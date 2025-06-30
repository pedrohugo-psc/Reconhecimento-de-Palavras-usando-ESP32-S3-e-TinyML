#include "driver/i2s_std.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "afe.h"
#include "inmp441.h"

#define DATA_PIN 10
#define SCK_PIN 12
#define WS_PIN 11

#define WS_WIDTH_BIT 32
#define SAMPLE_RATE 16000

static const char *TAG = "INMP441";

static i2s_chan_handle_t chan;
static inmp441_datachunk_t datachunk;

void inmp441_setup(){
    
    ESP_LOGI(TAG, "Setting up I2S recording");

    i2s_chan_config_t chan_config = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_config, NULL, &chan));

    i2s_std_config_t rx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg =
            {
                .data_bit_width = I2S_DATA_BIT_WIDTH_32BIT,
                .slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT,
                .slot_mode = I2S_SLOT_MODE_MONO,
                .slot_mask = I2S_STD_SLOT_LEFT,
                .ws_width = WS_WIDTH_BIT,
                .ws_pol = false,
                .bit_shift = false,
                .left_align = true,
                .big_endian = false,
                .bit_order_lsb = false,
            },
        .gpio_cfg =
            {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = (gpio_num_t)SCK_PIN,
                .ws = (gpio_num_t)WS_PIN,
                .dout = I2S_GPIO_UNUSED,
                .din = (gpio_num_t)DATA_PIN,
                .invert_flags =
                    {
                        .mclk_inv = false,
                        .bclk_inv = false,
                        .ws_inv = false,
                    },
            },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(chan, &rx_std_cfg));
}

static void inmp441_init_audio_buffers(){
    
    datachunk.feed_chunksize = afe_get_handle()->get_feed_chunksize(afe_get_data());
    datachunk.feed_nch = afe_get_handle()->get_feed_channel_num((afe_get_data())); 

    assert(datachunk.feed_nch == 2);
    datachunk.feed_buffer_len = datachunk.feed_chunksize * datachunk.feed_nch * sizeof(int16_t);
    datachunk.feed_buffer = (int16_t *)malloc(datachunk.feed_buffer_len);
    assert(datachunk.feed_buffer);
    datachunk.feed_buffer_offset = 0;

    datachunk.buffer_len = (datachunk.feed_chunksize * 1 /* mono */ * sizeof(int32_t));
    datachunk.buffer = malloc(datachunk.buffer_len);
    assert(datachunk.buffer);

    ESP_LOGI(TAG, "buffer_len %d feed_chunksize %d feed_nch %d feed_buffer_len %d", (int)datachunk.buffer_len,
             (int)datachunk.feed_chunksize, (int)datachunk.feed_nch, (int)datachunk.feed_buffer_len);
    
}

void inmp441_enable(){

    inmp441_init_audio_buffers();
    ESP_ERROR_CHECK(i2s_channel_enable(chan));

}

void* inmp441_read_data(){
    
    ESP_ERROR_CHECK(i2s_channel_read(chan, datachunk.buffer, (datachunk.feed_chunksize * sizeof(int32_t)), &datachunk.bytes_read, portMAX_DELAY));

    return datachunk.buffer;
}

inmp441_datachunk_t *inmp441_get_datachunk(){
    
    return &datachunk;
}
