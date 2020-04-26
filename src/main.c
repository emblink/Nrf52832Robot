#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "nrf52.h"
#include "gpio.h"
#include "clock.h"
#include "temp.h"
#include "timer.h"
#include "stepMotorDriver.h"
#include "pinInterrupt.h"

#define DRIVER_STEP_PIN         25U
#define DRIVER_DIR_PIN          26U
#define DRIVER_RESET_PIN        27U
#define DRIVER_SLEEP_PIN        28U
#define DRIVER_EN_PIN           29U
#define DRIVER_MS1              30U
#define DRIVER_MS2              31U
#define DRIVER_PFD              2U

#define FORWARD_LED             4U
#define BACKWARD_LED            5U

#define BUTTON_STEP_MODE        6U
#define BUTTON_ON_OFF           7U
#define BUTTON_DIRECTION        8U

static StepMotorDirection direction = StepMotorDirectionForward;
static void tempCallback(uint32_t temp);
static void onTimerCallback(void);
static void initPins(void);

static uint32_t temperature = 0;

static void tempCallback(uint32_t temp)
{
    temperature = temp;
}

static void onTimerCallback(void)
{
    stepMotorDriverStep();
}

/* Button Callbacks */
static void stepModeCallback(void)
{
    StepMotorDriverMode mode = StepMotorEighthModeCount;
    stepMotorDriverGetMode(&mode);

    switch(mode) {
    case StepMotorFullMode:
        stepMotorDriverSetMode(StepMotorHalfMode);
        break;
    case StepMotorHalfMode:
        stepMotorDriverSetMode(StepMotorQuaterMode);
        break;
    case StepMotorQuaterMode:
        stepMotorDriverSetMode(StepMotorEighthMode);
        break;
    case StepMotorEighthMode:
        stepMotorDriverSetMode(StepMotorFullMode);
        break;
    case StepMotorEighthModeCount:
        // stop?
        break;
    default:
        break;
    }
}

static void onOffCallback(void)
{
    static bool enable = false;
    stepMotorDriverEnable(enable);
    enable = !enable;
}

static void directionCallback(void)
{
    StepMotorDirection dir;
    stepMotorDriverGetDir(&dir);

    if (dir == StepMotorDirectionForward) {
        stepMotorDriverSetDir(StepMotorDirectionBackward);
        gpioSetPin(FORWARD_LED, false);
        gpioSetPin(BACKWARD_LED, true);
    } else {
        stepMotorDriverSetDir(StepMotorDirectionForward);
        gpioSetPin(FORWARD_LED, true);
        gpioSetPin(BACKWARD_LED, false);
    }
}

static void initPins(void)
{
    /* Init Outputs */
    GpioConfig pinConfig =  {
        .dir = GpioOutput,
        .input = GpioInputDisconnect,
        .pull = GpioPullDisabled,
        .drive = 0,
        .sence = GpioSenseDisabled
    };

    gpioConfig(FORWARD_LED, pinConfig);
    gpioConfig(BACKWARD_LED, pinConfig);

    /* Init Inputs */
    pinConfig.dir = GpioInput;
    pinConfig.input = GpioInputConnect;
    pinConfig.pull = GpioPullUp;

    gpioConfig(BUTTON_STEP_MODE, pinConfig);
    gpioConfig(BUTTON_ON_OFF, pinConfig);
    gpioConfig(BUTTON_DIRECTION, pinConfig);
}

int main(void)
{
    clockSetHfClk();
    initPins();
    tempSensorStart(tempCallback);

    pinInterruptInit();
    pinInterruptEnable(BUTTON_STEP_MODE, stepModeCallback, FallingEdge);
    pinInterruptEnable(BUTTON_ON_OFF, onOffCallback, FallingEdge);
    pinInterruptEnable(BUTTON_DIRECTION, directionCallback, FallingEdge);

    StepMotorDriverPin driverPins[StepMotorPinCount] = {
        [StepMotorStepPin] = DRIVER_STEP_PIN,
        [StepMotorDirectionPin] = DRIVER_DIR_PIN,
        [StepMotorResetPin] = DRIVER_RESET_PIN,
        [StepMotorSleepPin] = DRIVER_SLEEP_PIN,
        [StepMotorEnablePin] = DRIVER_EN_PIN,
        [StepMotorMs1Pin] = DRIVER_MS1,
        [StepMotorMs2Pin] = DRIVER_MS2,
        [StepMotorPfdPin] = DRIVER_PFD,
    };
    stepMotorDriverInit(driverPins, StepMotorFullMode);
    stepMotorDriverSetDir(StepMotorDirectionForward);
    gpioSetPin(FORWARD_LED, true);
    stepMotorDriverEnable(true);

    timerStart(Timer1, 700, onTimerCallback); // 700 mks is minimum for StepMotorFullMode

	while (1) {
        temperature = tempSensorGetData();
        __WFI();
	}
}