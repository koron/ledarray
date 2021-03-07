#pragma once

#ifndef LEDARRAY_NUM
# error LEDARRAY_NUM is unavailable, please check your config.h
#endif

#ifndef LEDARRAY_PIN
# error LED_PIN is unavailable, please check your config.h
#endif

extern uint32_t led_state[LEDARRAY_NUM];

#ifdef __cplusplus
extern "C" {
#endif

// ledarray_init() initializes LED array.
// it requires a GPIO, a SM of a PIO, and a DMA channel to work.
void ledarray_init();

// ledarray_task transfers bit data to NeoPixel LED.
// it will do nothign when previos transfer doesn't end.
void ledarray_task();

// ledarray_num is number of LED in the array.
inline int ledarray_num() {
    return LEDARRAY_NUM;
}

// ledarray_set_rgb set color of a LED with RGB.
inline void ledarray_set_rgb(int i, uint8_t r, uint8_t g, uint8_t b) {
    led_state[i] =
        ((uint32_t)(r) << 16) |
        ((uint32_t)(g) << 24) |
        ((uint32_t)(b) << 8);
}

#ifdef __cplusplus
}
#endif
