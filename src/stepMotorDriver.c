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
    return stepMotorDriverSetMode(mode);
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

    #if defined DIRECTION_INVERT && DIRECTION_INVERT > 0
        direction = direction == StepMotorDirectionForward ? 
                  StepMotorDirectionBackward : StepMotorDirectionForward;
    #endif // DIRECTION_INVERT

    gpioSetPin(driver.direction,
               direction == StepMotorDirectionForward ? 
               true : false);
    return true;
}

bool stepMotorDriverGetDir(StepMotorDirection *dir)
{
    if (!isDriverInited || dir == NULL)
        return false;

    StepMotorDirection direction;
    if (gpioReadPin(driver.direction)) {
        direction = StepMotorDirectionForward;
    } else {
        direction = StepMotorDirectionBackward;
    }

    #if defined DIRECTION_INVERT && DIRECTION_INVERT > 0
        direction = direction == StepMotorDirectionForward ? 
                  StepMotorDirectionBackward : StepMotorDirectionForward;
    #endif // DIRECTION_INVERT

    *dir = direction;
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

bool stepMotorDriverGetMode(StepMotorDriverMode *mode)
{
    if (!isDriverInited || mode == NULL)
        return false;

    uint8_t ms1 = gpioReadPin(driver.ms1);
    uint8_t ms2 = gpioReadPin(driver.ms2);
    *mode = (ms2 | (ms1 << 1));
    return true;
}

bool stepMotorDriverStep(void)
{
    if (!isDriverInited)
        return false;

    gpioTogglePin(driver.step);
    return true;
}



