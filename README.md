# LED Array

using WS2812 driver (using compatible YS-SK6812MINI-E devices).

## How to build

Require `PICO_SDK_PATH` env var.

```console
$ cmake -B build
$ make -j4 -C build
```

## Timinig

Name | Desc                   | Min | Standard | Max  | Unit 
-----|------------------------|----:|---------:|-----:|------
T    |Code period             |  1.2|(N/A)     |(N/A) |us
T0H  |0 code, high level time |  0.2|      0.32|   0.4|us
T0L  |0 code, low level time  |  0.8|(N/A)     |(N/A) |us
T1H  |1 code, high level time | 0.58|      0.64|   1.0|us
T1L  |1 code, low level time  |  0.2|(N/A)     |(N/A) |us
Trst |Reset code, low level   |  >80|(N/A)     |(N/A) |us
