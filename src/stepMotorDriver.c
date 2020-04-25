#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "gpio.h"
#include "stepMotorDriver.h"

static stepMotorDriverConfig driver = {0};

void stepMotorDriverInit(stepMotorDriverConfig config)
{
    driver = config;
    GpioConfig pinConfig = {
        .dir = GpioOutput,
        .input = GpioInputConnect,
        .pull = GpioPullDisabled,
        .drive = 0,
        .sence = GpioSenseDisabled
    };
    for (uint32_t i = 0; i < 5; i++) {
        gpioConfig(config.pins[i], pinConfig);
    }
		stepMotorDriverSleep(false);
    stepMotorDriverReset();
    gpioSetPin(driver.stepPin, false);
}

void stepMotorDriverEnable(bool enable)
{
    gpioSetPin(driver.enablePin, !enable);
}

void stepMotorDriverReset(void)
{
    gpioSetPin(driver.resetPin, false);
    gpioSetPin(driver.resetPin, true);
}

void stepMotorDriverSleep(bool enable)
{
    gpioSetPin(driver.sleepPin, !enable);
}

void stepMotorDriverSetDir(bool direction)
{
    gpioSetPin(driver.directionPin, direction);
}

void stepMotorDriverStep(void)
{
    gpioSetPin(driver.stepPin, true);
    for (uint32_t i = 0; i < 50; i++);
    gpioSetPin(driver.stepPin, false);
}



