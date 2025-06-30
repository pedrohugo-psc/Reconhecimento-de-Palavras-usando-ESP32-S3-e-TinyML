file(GLOB_RECURSE SRC_C
    "${CMAKE_CURRENT_LIST_DIR}/porting/espidf/ei_classifier_porting_espidf.c"
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/CMSIS/DSP/Source/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/c/common.c"
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/porting/**/*.c"
)
file(GLOB_RECURSE SRC_CPP
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/dsp/**/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/classifier/*.cc"
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/**/*.cc"
    "${CMAKE_CURRENT_LIST_DIR}/tflite-model/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/porting/**/*.cpp"
)

set(INCLUDE_DIRS
    "."
    "porting/espidf"
    "edge-impulse-sdk"
    "model-parameters"
    "tflite-model"
)

idf_component_register(
    SRCS         ${SRC_C} ${SRC_CPP}
    INCLUDE_DIRS ${INCLUDE_DIRS}
    REQUIRES     freertos esp_timer
)

idf_build_get_property(comp_target TARGET)

idf_build_set_property(comp_target COMPILE_OPTIONS
    -Wno-deprecated-declarations
    -Wno-missing-field-initializers
    -Wno-maybe-uninitialized
)
