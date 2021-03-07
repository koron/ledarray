#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#include "config.h"
#include "ledarray.h"

static void update_rainbow(int t) {
    for (int i = 0; i < LEDARRAY_NUM; i++) {
        uint8_t r = 0, g = 0, b = 0;
        float h = (float)((i + t) % LEDARRAY_NUM)/ (float)LEDARRAY_NUM * 6;
        int phase = (int)h;
        uint8_t f = (uint8_t)(255 * (h - (float)phase));
        switch (phase) {
            default:
            case 0:
                r = 255;
                g = f;
                b = 0;
                break;
            case 1:
                r = 255 - f;
                g = 255;
                b = 0;
                break;
            case 2:
                r = 0;
                g = 255;
                b = f;
                break;
            case 3:
                r = 0;
                g = 255 - f;
                b = 255;
                break;
            case 4:
                r = f;
                g = 0;
                b = 255;
                break;
            case 5:
                r = 255;
                g = 0;
                b = 255 - f;
                break;
        }
        ledarray_set_rgb(i, r >> 3, g >> 3, b >> 3);
    }
}

static void update_snake(int t) {
    for (int i = 0; i < LEDARRAY_NUM; i++) {
        uint32_t c = 0;
        int x = (i + t) % LEDARRAY_NUM;
        if (x < 5) {
            ledarray_set_rgb(i, 255, 0, 0);
        } else if (x < 10) {
            ledarray_set_rgb(i, 0, 255, 0);
        } else if (x < 15) {
            ledarray_set_rgb(i, 0, 0, 255);
        } else {
            ledarray_set_rgb(i, 0, 0, 0);
        }
    }
}

typedef void (*pattern)(int t);

static pattern patterns[] = {
    update_snake,
    update_rainbow,
};

const uint pattern_choice = 1;

static void update_ledarray(int t) {
    patterns[pattern_choice % count_of(patterns)](t);
}

const uint ONBOARD_LED_PIN = 25;

int main() {
    stdio_init_all();
    puts("LED array");

    gpio_init(ONBOARD_LED_PIN);
    gpio_set_dir(ONBOARD_LED_PIN, GPIO_OUT);
    gpio_put(ONBOARD_LED_PIN, 1);

    ledarray_init();

    uint t = 0;
    while (1) {
        gpio_put(ONBOARD_LED_PIN, t % 2);
        update_ledarray(t);
        ledarray_task();
        t++;
        sleep_ms(100);
    }

    return 0;
}
