#include "esp_err.h"
#include "esp_log.h"

#include "afe.h"

static const char *TAG = "AFE";

static srmodel_list_t *models;
static esp_afe_sr_iface_t *afe_handle;
static esp_afe_sr_data_t *afe_data;

void afe_setup()
{

    models = esp_srmodel_init("model");

    afe_config_t *afe_config = afe_config_init("MR", models, AFE_TYPE_VC, AFE_MODE_HIGH_PERF);

    ESP_LOGI(TAG, "aec_init %d", (int)afe_config->aec_init);
    ESP_LOGI(TAG, "aec_mode %d", (int)afe_config->aec_mode);
    ESP_LOGI(TAG, "aec_filter_length %d", (int)afe_config->aec_filter_length);
    ESP_LOGI(TAG, "se_init %d", (int)afe_config->se_init);
    ESP_LOGI(TAG, "ns_init %d", (int)afe_config->ns_init);
    if (afe_config->ns_model_name)
    {
        ESP_LOGI(TAG, "ns_model_name %s", afe_config->ns_model_name);
    }
    ESP_LOGI(TAG, "afe_ns_mode %d", (int)afe_config->afe_ns_mode);
    ESP_LOGI(TAG, "vad_init %d", (int)afe_config->vad_init);
    ESP_LOGI(TAG, "vad_mode %d", (int)afe_config->vad_mode);
    if (afe_config->vad_model_name)
    {
        ESP_LOGI(TAG, "vad_model_name %s", afe_config->vad_model_name);
    }
    ESP_LOGI(TAG, "vad_min_speech_ms %d", (int)afe_config->vad_min_speech_ms);
    ESP_LOGI(TAG, "vad_min_noise_ms %d", (int)afe_config->vad_min_noise_ms);
    ESP_LOGI(TAG, "vad_delay_ms %d", (int)afe_config->vad_delay_ms);
    ESP_LOGI(TAG, "vad_mute_playback %d", (int)afe_config->vad_mute_playback);
    ESP_LOGI(TAG, "vad_enable_channel_trigger %d", (int)afe_config->vad_enable_channel_trigger);
    ESP_LOGI(TAG, "wakenet_init %d", (int)afe_config->wakenet_init);
    if (afe_config->wakenet_model_name)
    {
        ESP_LOGI(TAG, "wakenet_model_name %s", afe_config->wakenet_model_name);
    }
    if (afe_config->wakenet_model_name_2)
    {
        ESP_LOGI(TAG, "wakenet_model_name_2 %s", afe_config->wakenet_model_name_2);
    }
    ESP_LOGI(TAG, "wakenet_mode %d", (int)afe_config->wakenet_mode);
    ESP_LOGI(TAG, "agc_init %d", (int)afe_config->agc_init);
    ESP_LOGI(TAG, "agc_mode %d", (int)afe_config->agc_mode);
    ESP_LOGI(TAG, "agc_compression_gain_db %d", (int)afe_config->agc_compression_gain_db);
    ESP_LOGI(TAG, "agc_target_level_dbfs %d", (int)afe_config->agc_target_level_dbfs);
    ESP_LOGI(TAG, "afe_mode %d", (int)afe_config->afe_mode);
    ESP_LOGI(TAG, "afe_type %d", (int)afe_config->afe_type);
    ESP_LOGI(TAG, "afe_perferred_core %d", (int)afe_config->afe_perferred_core);
    ESP_LOGI(TAG, "afe_perferred_priority %d", (int)afe_config->afe_perferred_priority);
    ESP_LOGI(TAG, "afe_ringbuf_size %d", (int)afe_config->afe_ringbuf_size);
    ESP_LOGI(TAG, "memory_alloc_mode %d", (int)afe_config->memory_alloc_mode);
    ESP_LOGI(TAG, "afe_linear_gain %f", afe_config->afe_linear_gain);
    ESP_LOGI(TAG, "debug_init %d", (int)afe_config->debug_init);
    ESP_LOGI(TAG, "fixed_first_channel %d", (int)afe_config->fixed_first_channel);

    assert(afe_config->aec_init == true);
    assert(afe_config->se_init == false);
    assert(afe_config->ns_init == true);
    afe_config->vad_init = false;
    assert(afe_config->wakenet_init == false);
    afe_config->agc_init = true;
    assert(afe_config->agc_mode == AFE_AGC_MODE_WEBRTC);
    assert(afe_config->agc_compression_gain_db == 9);
    assert(afe_config->agc_target_level_dbfs == 3);

    afe_config->afe_perferred_core = 1;

    afe_handle = esp_afe_handle_from_config(afe_config);
    afe_data = afe_handle->create_from_config(afe_config);
    afe_config_free(afe_config);
}

esp_afe_sr_iface_t *afe_get_handle()
{

    if (afe_handle == NULL)
    {
        ESP_LOGE(TAG, "AFE handle is NULL");
        return NULL;
    }

    return afe_handle;
}

esp_afe_sr_data_t *afe_get_data()
{

    if (afe_data == NULL)
    {
        ESP_LOGE(TAG, "AFE data is NULL");
        return NULL;
    }

    return afe_data;
}
