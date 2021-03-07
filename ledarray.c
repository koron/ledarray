#include <stdio.h>

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#include "config.h"
#include "ws2812.pio.h"

uint32_t led_state[LEDARRAY_NUM] = {};

static int ledarray_chan0;

void ledarray_init() {
    // TODO: claim a PIO and SM
    PIO pio = pio0;
    int sm = 0;

    // setup PIO/SM with ws2812 program.
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, LEDARRAY_PIN, 800000);

    // setup DMA to send LED data.
    int chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, true));
    dma_channel_configure(chan, &c, &pio->txf[sm], NULL,
            count_of(led_state), false);
    ledarray_chan0 = chan;
}

bool ledarray_task() {
    if (dma_channel_is_busy(ledarray_chan0)) {
        return false;
    }
    // TODO: wait more than 80us for next data.
    dma_channel_set_read_addr(ledarray_chan0, led_state, true);
    return true;
}
