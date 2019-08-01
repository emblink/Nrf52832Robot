#include <stdint.h>
#include <stdbool.h>
#include "clock.h"

#define GPIO_BASE       (0x50000000U)
#define GPIO_OUT        *((uint32_t *) (GPIO_BASE + 0x504))
#define GPIO_OUT_SET    *((uint32_t *) (GPIO_BASE + 0x508))
#define GPIO_OUT_CLR    *((uint32_t *) (GPIO_BASE + 0x50C))
#define GPIO_DIR        *((uint32_t *) (GPIO_BASE + 0x514))
#define GPIO_DIR_SET    *((uint32_t *) (GPIO_BASE + 0x518))
#define GPIO_DIR_CLR    *((uint32_t *) (GPIO_BASE + 0x51C))
#define GPIO_PIN_CNF4   *((uint32_t *) (GPIO_BASE + 0x710))


#define LED_PIN (4U)
void gpioSetPinState(uint32_t pin, bool state)
{
    if (state) {
        GPIO_OUT_SET |= (1 << pin);
    } else {
        GPIO_OUT_CLR |= (1 << pin);
    }
}

void gpioConfig(uint32_t pin)
{
    GPIO_PIN_CNF4 |= (1 << 0); // output
    GPIO_PIN_CNF4 |= (1 << 1); // diconnect input buffer
    GPIO_PIN_CNF4 |= ((1 << 2) | (1 << 3)); // pull up
    GPIO_PIN_CNF4 |= (1 << 1);
}

static void delay(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++) {
        for (uint32_t j = 0; j < 3200; j++) {
            __asm("nop");
        }
    }
}

int main(void)
{
    clockSetHfClk();
    gpioConfig(LED_PIN);
	while (1) {
        gpioSetPinState(LED_PIN, true);
        delay(500);
        gpioSetPinState(LED_PIN, false);
        delay(500);
	}
}
