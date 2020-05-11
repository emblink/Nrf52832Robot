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
#include "systemTime.h"
#include "twim.h"

#define DRIVER_STEP_PIN         25U
#define DRIVER_DIR_PIN          26U
#define DRIVER_RESET_PIN        27U
#define DRIVER_SLEEP_PIN        28U
#define DRIVER_EN_PIN           29U
#define DRIVER_MS1              30U
#define DRIVER_MS2              31U
#define DRIVER_PFD              2U

#define APP_LED                 3U
#define FORWARD_LED             4U
#define BACKWARD_LED            5U

#define BUTTON_STEP_MODE        6U
#define BUTTON_ON_OFF           7U
#define BUTTON_DIRECTION        8U

// pins 9 and 10 by default used by nfc, define CONFIG_NFCT_PINS_AS_GPIOS set them as gpios.
#define TWIM_SDA_PIN            9U
#define TWIM_SCL_PIN            10U

static uint8_t twimTx[TWIM_WRITE_BUFF_SIZE] = {0};
static uint8_t twimRx[TWIM_READ_BUFF_SIZE] = {0};
static StepMotorDirection direction = StepMotorDirectionForward;
static void tempCallback(uint32_t temp);
static void onTimerCallback(void);
static void initPins(void);
static void twimCallback(TwimError err);

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
    gpioTogglePin(APP_LED);
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
        .drive = GpioDriveS0S1,
        .sence = GpioSenseDisabled
    };

    gpioConfig(FORWARD_LED, pinConfig);
    gpioConfig(BACKWARD_LED, pinConfig);
    gpioConfig(APP_LED, pinConfig);

    /* Init Inputs */
    pinConfig.dir = GpioInput;
    pinConfig.input = GpioInputConnect;
    pinConfig.pull = GpioPullUp;

    gpioConfig(BUTTON_STEP_MODE, pinConfig);
    gpioConfig(BUTTON_ON_OFF, pinConfig);
    gpioConfig(BUTTON_DIRECTION, pinConfig);
}

static void twimCallback(TwimError err)
{

}

int main(void)
{
    clockSetHfClk();
    sysTickInit(SysTickPeriodMilisecond, NULL);
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

    twimInit(TWIM_SDA_PIN, TWIM_SCL_PIN, twimCallback);
    static uint8_t tx[] = {0xAC, 0, 1, 2, 3, 4, 5, 6};
    twimWrite(0x68, tx, sizeof(tx));
    __asm("BKPT #255");
    timerStart(Timer1, 1000, onTimerCallback); // 700 mks is minimum for StepMotorFullMode

    /* exact gear ratio is in fact 63.68395:1, which results in approximately 4076
       steps per full revolution (in half step mode). */
    // #define STEPS_PER_ROTATION (4076U) // 
	while (1) {
        __WFI();
        // delay(500);
        // stepMotorDriverSetDir(StepMotorDirectionForward);
        // for (uint32_t i = 0; i < STEPS_PER_ROTATION; i++) {
        //     stepMotorDriverStep();
        //     delay(1);
        // }
        // stepMotorDriverSetDir(StepMotorDirectionBackward);
        // delay(500);
        // for (uint32_t i = 0; i < STEPS_PER_ROTATION; i++) {
        //     stepMotorDriverStep();
        //     delay(1);
        // }
	}
}