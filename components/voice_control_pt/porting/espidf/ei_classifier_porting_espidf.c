#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// malloc / free / calloc
void *ei_malloc(size_t size) {
  return malloc(size);
}
void ei_free(void *ptr) {
  free(ptr);
}
void *ei_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

// printf genérico
void ei_printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}
// imprime um float (usado por alguns logs internos)
void ei_printf_float(float f) {
  // basta usar printf com %f
  printf("%f", f);
}

// TEMPERATURA de tempo em ms/us — **uint64_t** conforme o header
uint64_t ei_read_timer_ms(void) {
  // esp_timer_get_time() → micros desde boot
  return esp_timer_get_time() / 1000ULL;
}
uint64_t ei_read_timer_us(void) {
  return esp_timer_get_time();
}

// sleep em ms — **EI_IMPULSE_ERROR** conforme o header!
EI_IMPULSE_ERROR ei_sleep(int32_t time_ms) {
  vTaskDelay(pdMS_TO_TICKS(time_ms));
  return EI_IMPULSE_OK;
}

// cancelamento não suportado neste exemplo
EI_IMPULSE_ERROR ei_run_impulse_check_canceled(void) {
  return EI_IMPULSE_OK;
}
