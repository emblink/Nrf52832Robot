#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "nrf52.h"
#include "gpio.h"
#include "clock.h"
#include "temp.h"
#include "timer.h"
#include "stepMotorDriver.h"

#define LED_PIN             (4U)
#define DRIVER_STEP_PIN     (11U)
#define DRIVER_DIR_PIN      (12U)
#define DRIVER_RESET_PIN    (13U)
#define DRIVER_SLEEP_PIN    (14U)
#define DRIVER_EN_PIN       (15U)

static void tempCallback(uint32_t temp);
static void onTimerCallback(void);

static uint32_t temperature = 0;

static void tempCallback(uint32_t temp)
{
    temperature = temp;
}

static void onTimerCallback(void)
{
    static uint32_t state = 0x01;
    state ^= 0x01;
    gpioSetPin(LED_PIN, state == 0x01);
    stepMotorDriverStep();
}

static GpioConfig pinConfig =  {
    .dir = GpioOutput,
    .input = GpioInputConnect,
    .pull = GpioPullDisabled,
    .drive = 0,
    .sence = GpioSenseDisabled
};

int main(void)
{
    clockSetHfClk();
    gpioConfig(LED_PIN, pinConfig);
    tempSensorStart(tempCallback);
    timerStart(Timer1, 10, onTimerCallback);
    
    stepMotorDriverConfig motorDriverConfig = {
        .enablePin = DRIVER_EN_PIN,
        .directionPin = DRIVER_DIR_PIN,
        .resetPin = DRIVER_RESET_PIN,
        .stepPin = DRIVER_STEP_PIN,
        .sleepPin = DRIVER_SLEEP_PIN,
    };
    stepMotorDriverInit(motorDriverConfig);
    stepMotorDriverEnable(true);
    stepMotorDriverSetDir(true);
    
	while (1) {
        temperature = tempSensorGetData();
        __WFI();
	}
}
