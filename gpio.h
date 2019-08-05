#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GpioInput = 0U,
    GpioOutput = 1U,
} GpioDir;

typedef enum {
    GpioInputConnect = 0U,
    GpioOutputDisconnect = 1U,
} GpioInputState;

typedef enum {
    GpioPullDisabled = 0U,
    GpioPullDown = 1U,
    GpioPullUp = 3U,
} GpioPull;

typedef enum {
    GpioSenseDisabled = 0U,
    GpioSenseHigh = 2U,
    GpioSenseLow = 3U,
} GpioSense;

typedef union GpioConfig {
    struct {
        uint8_t dir : 1;
        uint8_t input : 1;
        uint8_t pull : 2;
        uint8_t reserved : 4;
        uint8_t drive : 3;
        uint8_t reserved1 : 5;
        uint8_t sence : 2;
    };
    uint32_t config;
} GpioConfig;

void gpioSetPin(uint32_t pin, bool state);
bool gpioReadPin(uint32_t pin);
void gpioConfig(uint32_t gpioNumber, GpioConfig config);