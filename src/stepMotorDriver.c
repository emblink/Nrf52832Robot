#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "gpio.h"
#include "stepMotorDriver.h"

typedef union {
    struct {
        StepMotorDriverPin step;
        StepMotorDriverPin direction;
        StepMotorDriverPin reset;
        StepMotorDriverPin sleep;
        StepMotorDriverPin enable;
        StepMotorDriverPin ms1;
        StepMotorDriverPin ms2;
        StepMotorDriverPin pfd;
    };
    StepMotorDriverPin pin[StepMotorPinCount];
} driverPins;

static driverPins driver = {0};
bool isDriverInited = false;

bool stepMotorDriverInit(StepMotorDriverPin pins[StepMotorPinCount], StepMotorDriverMode mode)
{
    GpioConfig pinConfig =  {
        .dir = GpioOutput,
        .input = GpioInputDisconnect,
        .pull = GpioPullDisabled,
        .drive = 0,
        .sence = GpioSenseDisabled
    };

    for (uint32_t i = 0; i < StepMotorPinCount; i++) {
        driver.pin[i] = pins[i];
        gpioConfig(driver.pin[i], pinConfig);
    }

    isDriverInited = true;

    gpioSetPin(driver.reset, false); // hold reset
    gpioSetPin(driver.step, false); // hold step low for proper transition
    stepMotorDriverSleep(false); // disable sleep
    stepMotorDriverEnable(false); // disable outputs
    gpioSetPin(driver.pfd, true); // have no idea what this doing
    stepMotorDriverReset();
    stepMotorDriverSetMode(mode);
    return stepMotorDriverStep();
}

bool stepMotorDriverEnable(bool enable)
{
    if (!isDriverInited)
        return false;

    gpioSetPin(driver.enable, !enable);
    return true;
}

bool stepMotorDriverReset(void)
{
    if (!isDriverInited)
        return false;

    gpioSetPin(driver.reset, false);
    gpioSetPin(driver.reset, true);
    return true;
}

bool stepMotorDriverSleep(bool enable)
{
    if (!isDriverInited)
        return false;

    gpioSetPin(driver.sleep, !enable);
    return true;
}

bool stepMotorDriverSetDir(StepMotorDirection direction)
{
    if (!isDriverInited)
        return false;

    gpioSetPin(driver.direction, direction);
    return true;
}

bool stepMotorDriverSetMode(StepMotorDriverMode mode)
{
    if (!isDriverInited)
        return false;

    gpioSetPin(driver.ms1, mode & 0x02);
    gpioSetPin(driver.ms2, mode & 0x01);
    return true;
}

bool stepMotorDriverStep(void)
{
    if (!isDriverInited)
        return false;

    static bool state = false;
    gpioSetPin(driver.step, state);
    state = !state;
    return true;
}



