#include <stdio.h>

#include "pico/stdlib.h"

#include "config.h"
#include "ledarray.h"

static void update_rainbow(uint t) {
    uint level = 3;
    //uint level = (t / LEDARRAY_NUM) % 7;
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
        ledarray_set_rgb(i, r >> level, g >> level, b >> level);
    }
}

static void update_snake(uint t) {
    for (int i = 0; i < LEDARRAY_NUM; i++) {
        uint32_t c = 0;
        uint x = (i + t) % LEDARRAY_NUM;
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

typedef void (*pattern)(uint t);

static pattern patterns[] = {
    update_snake,
    update_rainbow,
};

const uint pattern_choice = 1;

static void update_ledarray(uint t) {
    patterns[pattern_choice % count_of(patterns)](t);
}

const uint ONBOARD_LED_PIN = 25;

static volatile bool nled_lock = false;

void ledarray_resetdelay_completed() {
    nled_lock = false;
}

int main() {
    stdio_init_all();
    puts("LED array");

    gpio_init(ONBOARD_LED_PIN);
    gpio_set_dir(ONBOARD_LED_PIN, GPIO_OUT);
    gpio_put(ONBOARD_LED_PIN, 1);

    uint64_t last_blink = time_us_64();
    bool blink_state = false;

    ledarray_init();
    uint64_t last_led = 0;
    uint32_t led_state = 0;

    while (1) {
        uint64_t now = time_us_64();

        // blink led on the board 500ms interval.
        if (now - last_blink >= 500 * 1000) {
            gpio_put(ONBOARD_LED_PIN, blink_state ? 1 : 0);
            blink_state = !blink_state;
            last_blink = now;
        }

        if (now - last_led >= 33 * 1000) {
            nled_lock = true;
            update_ledarray(led_state);
            ledarray_task();
            led_state++;
            last_led = now;
        }
        sleep_ms(10);
    }

    return 0;
}
