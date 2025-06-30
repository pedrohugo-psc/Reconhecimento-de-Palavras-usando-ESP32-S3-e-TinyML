#ifndef AFE_H
#define AFE_H

#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"

#define GAIN_BITS 6

void afe_setup();
esp_afe_sr_iface_t *afe_get_handle();
esp_afe_sr_data_t *afe_get_data();

#endif // AFE_H