#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#include "ws2812.pio.h"

const int PIN_TX = 4;

#define LED_MAXNUM	50

static int led_count = LED_MAXNUM;
static uint32_t led_state[LED_MAXNUM] = {};

static inline uint32_t rgb2u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 16) | ((uint32_t)(g) << 24) | ((uint32_t)(b) << 8);
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

enum {
    DMA_CHANNEL_MAIN = 0,
};

void dma_init(PIO pio, uint sm) {
    dma_claim_mask(DMA_CHANNEL_MAIN);
    dma_channel_config c = dma_channel_get_default_config(DMA_CHANNEL_MAIN);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, true));
    dma_channel_configure(DMA_CHANNEL_MAIN, &c, &pio->txf[sm], NULL,
            count_of(led_state), false);
}

void dma_transfer() {
    dma_channel_set_read_addr(DMA_CHANNEL_MAIN, led_state, true);
}

void setup_rainbow(int t) {
    for (int i = 0; i < led_count; i++) {
        uint8_t r = 0, g = 0, b = 0;
        float h = (float)((i + t) % led_count)/ (float)led_count * 6;
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
        led_state[i] = rgb2u32(r>>4, g>>4, b>>4);
    }
}

int main() {
    stdio_init_all();
    puts("LED array");

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, PIN_TX, 800000);

    dma_init(pio, sm);
    //setup_rainbow();
    //dma_transfer();

    uint t = 0;
    while (1) {
        //led_state_update(t);
        //led_state_send();
        setup_rainbow(t);
        dma_transfer();
        gpio_put(LED_PIN, t % 2);
        t++;
        sleep_ms(100);
    }

    return 0;
}
