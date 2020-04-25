#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "nrf52.h"
#include "gpio.h"
#include "clock.h"
#include "temp.h"
#include "timer.h"
#include "stepMotorDriver.h"

#define DRIVER_STEP_PIN         25U
#define DRIVER_DIR_PIN          26U
#define DRIVER_RESET_PIN        27U
#define DRIVER_SLEEP_PIN        28U
#define DRIVER_EN_PIN           29U
#define DRIVER_MS1              30U
#define DRIVER_MS2              31U
#define DRIVER_PFD              2U

#define CLOCKWISE_LED           4U
#define COUNTERCLOCKWISE_LED    5U

#define BUTTON_STEP_MODE        6U
#define BUTTON_STEP             7U
#define BUTTON_DIRECTION        8U

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
    static bool state = true;
    state = !state;
    gpioSetPin(COUNTERCLOCKWISE_LED, state);
    stepMotorDriverStep();
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

    gpioConfig(CLOCKWISE_LED, pinConfig);
    gpioConfig(COUNTERCLOCKWISE_LED, pinConfig);

    /* Init Inputs */
    pinConfig.dir = GpioInput;
    pinConfig.input = GpioInputConnect;
    pinConfig.pull = GpioPullUp;

    gpioConfig(BUTTON_STEP_MODE, pinConfig);
    gpioConfig(BUTTON_STEP, pinConfig);
    gpioConfig(BUTTON_DIRECTION, pinConfig);
}

int main(void)
{
    clockSetHfClk();
    initPins();
    tempSensorStart(tempCallback);
    timerStart(Timer1, 10, onTimerCallback);
    
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
    stepMotorDriverInit(driverPins, StepMotorHalfMode);
    stepMotorDriverEnable(true);
    gpioSetPin(CLOCKWISE_LED, true);

	while (1) {
        temperature = tempSensorGetData();
        __WFI();
	}
}