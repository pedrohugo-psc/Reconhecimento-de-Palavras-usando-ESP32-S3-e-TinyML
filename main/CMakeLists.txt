idf_component_register(
    SRCS  "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES freertos esp_timer cxx voice_control_pt afe microphone_inmp441 ring_buffer led_rgb
)
