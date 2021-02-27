#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "ws2812.pio.h"

const int PIN_TX = 4;

#define LED_MAXNUM	50

static int led_count = LED_MAXNUM;
static uint32_t led_state[LED_MAXNUM] = {};

static inline uint32_t rgb2u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

static inline void led_state_send() {
    for (int i = 0; i < (sizeof(led_state) / sizeof(led_state[0])); i++) {
        pio_sm_put_blocking(pio0, 0, led_state[i] << 8);
    }
}

static void led_state_update(int t) {
    for (int i = 0; i < led_count; i++) {
        uint32_t c = 0;
        int x = (i + t) % led_count;
        if (x < 5) {
            c = rgb2u32(0xff, 0, 0);
        } else if (x < 10) {
            c = rgb2u32(0, 0xff, 0);
        } else if (x < 15) {
            c = rgb2u32(0, 0, 0xff);
        }
        led_state[i] = c;
    }
}

const uint LED_PIN = 25;

int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    puts("LED array");

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, PIN_TX, 800000);

    uint t = 0;
    while (1) {
        led_state_update(t);
        led_state_send();
        gpio_put(LED_PIN, t % 2);
        t++;
        sleep_ms(100);
    }

    return 0;
}
