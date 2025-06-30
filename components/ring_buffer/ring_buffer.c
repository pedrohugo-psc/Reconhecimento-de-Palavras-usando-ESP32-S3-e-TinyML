#include <stdio.h>
#include <string.h>
#include <esp_log.h>

#include "ring_buffer.h"

static const char *TAG = "RING_BUFFER";

void rb_init(ring_buffer_t *cb)
{
    cb->start = 0;
    cb->end = 0;
    cb->length = 0;
    memset(cb->buffer, 0, sizeof(cb->buffer)); 
}

void rb_insert(ring_buffer_t *cb, int16_t data)
{

    cb->buffer[cb->end] = data;
    cb->end = (cb->end + 1) % BUFFER_SIZE;

    if (cb->length < BUFFER_SIZE)
    {
        cb->length++;
    }
    else
    {
        cb->start = (cb->start + 1) % BUFFER_SIZE;
    }
    
}

void rb_remove(ring_buffer_t *cb){

    if (cb->length > 0)
    {
        cb->start = (cb->start + 1) % BUFFER_SIZE;
        cb->length--;
    }
    else
    {
        ESP_LOGE(TAG, "Buffer is empty, cannot remove data");
    }

}

int rb_get_size(ring_buffer_t *cb)
{
    return cb->length;
}