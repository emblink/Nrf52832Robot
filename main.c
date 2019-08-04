#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "nrf52.h"
#include "clock.h"
#include "temp.h"
#include "timer.h"

#define GPIO_BASE       (0x50000000U)
#define GPIO_OUT        *((uint32_t *) (GPIO_BASE + 0x504))
#define GPIO_OUT_SET    *((uint32_t *) (GPIO_BASE + 0x508))
#define GPIO_OUT_CLR    *((uint32_t *) (GPIO_BASE + 0x50C))
#define GPIO_DIR        *((uint32_t *) (GPIO_BASE + 0x514))
#define GPIO_DIR_SET    *((uint32_t *) (GPIO_BASE + 0x518))
#define GPIO_DIR_CLR    *((uint32_t *) (GPIO_BASE + 0x51C))
#define GPIO_PIN_CNF4   *((uint32_t *) (GPIO_BASE + 0x710))

#define LED_PIN (4U)

static void gpioSetPinState(uint32_t pin, bool state);
static void gpioConfig(uint32_t pin);
static void tempCallback(uint32_t temp);
static void onTimerCallback(void);

static uint32_t temperature = 0;

static void gpioSetPinState(uint32_t pin, bool state)
{
    if (state) {
        GPIO_OUT_SET |= (1 << pin);
    } else {
        GPIO_OUT_CLR |= (1 << pin);
    }
}

static void gpioConfig(uint32_t pin)
{
    GPIO_PIN_CNF4 |= (1 << 0); // output
    GPIO_PIN_CNF4 |= (1 << 1); // diconnect input buffer
    GPIO_PIN_CNF4 |= ((1 << 2) | (1 << 3)); // pull up
    GPIO_PIN_CNF4 |= (1 << 1);
}

static void tempCallback(uint32_t temp)
{
    temperature = temp;
}

static void onTimerCallback(void)
{
    static uint32_t state = 0x01;
    state ^= 0x01;
    gpioSetPinState(LED_PIN, state == 0x01);
}

int main(void)
{
    clockSetHfClk();
    gpioConfig(LED_PIN);
    tempSensorStart(tempCallback);
    timerStart(Timer1, 500, onTimerCallback);
    
	while (1) {
        temperature = tempSensorGetData();
        __WFI();
	}
}
