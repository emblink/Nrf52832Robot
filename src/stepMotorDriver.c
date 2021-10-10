#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
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
static bool isDriverInited = false;

void stepMotorDriverInit(StepMotorDriverPin pins[StepMotorPinCount])
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
}

void stepMotorDriverEnable(bool enable)
{
    assert(isDriverInited);

    gpioSetPin(driver.enable, !enable);
}

void stepMotorDriverReset(void)
{
    assert(isDriverInited);

    gpioSetPin(driver.reset, false);
    gpioSetPin(driver.reset, true);
}

void stepMotorDriverSleep(bool enable)
{
    assert(isDriverInited);

    gpioSetPin(driver.sleep, !enable);
}

void stepMotorDriverSetDir(StepMotorDirection direction)
{
    assert(isDriverInited);

    #if defined DIRECTION_INVERT && DIRECTION_INVERT > 0
        direction = direction == StepMotorDirectionForward ? 
                  StepMotorDirectionBackward : StepMotorDirectionForward;
    #endif // DIRECTION_INVERT

    gpioSetPin(driver.direction,
               direction == StepMotorDirectionForward ? 
               true : false);
}

StepMotorDirection stepMotorDriverGetDir(void)
{
    assert(isDriverInited);

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

    return direction;
}

void stepMotorDriverSetMode(StepMotorDriverMode mode)
{
    assert(isDriverInited);
    assert(mode < StepMotorModeCount);

    // MS1 MS2 Resolution
    //  L   L  Full step (2 phase)
    //  H   L  Half step
    //  L   H  Quarter step
    //  H   H  Eighth step

    switch (mode) {
    case StepMotorFullMode:
        gpioSetPin(driver.ms1, false);
        gpioSetPin(driver.ms2, false);
        break;

    case StepMotorHalfMode:
        gpioSetPin(driver.ms1, true);
        gpioSetPin(driver.ms2, false);
        break;
    
    case StepMotorQuaterMode:
        gpioSetPin(driver.ms1, false);
        gpioSetPin(driver.ms2, true);
        break;

    case StepMotorEighthMode:
        gpioSetPin(driver.ms1, true);
        gpioSetPin(driver.ms2, true);
        break;
    }
}

StepMotorDriverMode stepMotorDriverGetMode(void)
{
    assert(isDriverInited);

    uint8_t ms1 = gpioReadPin(driver.ms1);
    uint8_t ms2 = gpioReadPin(driver.ms2);
    uint8_t state = (ms1 | (ms2 << 1));

    // MS1 MS2 Resolution
    //  L   L  Full step (2 phase)
    //  H   L  Half step
    //  L   H  Quarter step
    //  H   H  Eighth step

    switch (state) {
    case 0x00:
        return StepMotorFullMode;

    case 0x01:
        return StepMotorHalfMode;
    
    case 0x02:
        return StepMotorQuaterMode;

    case 0x03:
        return StepMotorEighthMode;

    default:
        return StepMotorModeCount;
    }
}

void stepMotorDriverStep(bool state)
{
    assert(isDriverInited);

    gpioSetPin(driver.step, state);
}



