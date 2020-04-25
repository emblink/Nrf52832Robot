#include <stdint.h>
#include <stdbool.h>
#include "nrf52.h"
#include "gpio.h"

#define GPIO_BASE       (0x50000000U)

#define GPIO            ((NRF_GPIO_Type *)(GPIO_BASE))

void gpioConfig(uint32_t pin, GpioConfig cfg)
{
    GPIO->PIN_CNF[pin] = 0;
    GPIO->PIN_CNF[pin] = cfg.config;
}

void gpioSetPin(uint32_t pin, bool state)
{
    if (state) {
        GPIO->OUTSET = (1 << pin);
    } else {
        GPIO->OUTCLR = (1 << pin);
    }
}

bool gpioReadPin(uint32_t pin)
{
    return (((GPIO->IN) & (1 << pin)) == 1);
}