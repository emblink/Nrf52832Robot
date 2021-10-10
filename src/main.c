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
#include "mpu9250.h"
#include "SEGGER_RTT.h"

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

}

/* Button Callbacks */
static void stepModeCallback(void)
{
    StepMotorDriverMode mode = (stepMotorDriverGetMode() + 1) % StepMotorModeCount;
    char *modeMsg[] = {
        "StepMotorFullMode",
        "StepMotorHalfMode",
        "StepMotorQuaterMode",
        "StepMotorEighthMode",
        "StepMotorModeCount",
    };
    SEGGER_RTT_printf(0, "%s\n", modeMsg[mode]);
}

static uint32_t getStepCount(void)
{
    switch (stepMotorDriverGetMode()) {
    case StepMotorFullMode:
        return 4;

    case StepMotorHalfMode:
        return 8;

    case StepMotorQuaterMode:
        return 16;

    case StepMotorEighthMode:
        return 32;
    
    default:
        return 0;
    }
}

static void onOffCallback(void)
{
    static bool enable = false;
    stepMotorDriverEnable(enable);
    enable = !enable;
    gpioTogglePin(APP_LED);
    SEGGER_RTT_printf(0, "Driver %s\n", enable ? "enabled" : "disabled");
}

static void directionCallback(void)
{
    StepMotorDirection dir = stepMotorDriverGetDir();

    if (dir == StepMotorDirectionForward) {
        stepMotorDriverSetDir(StepMotorDirectionBackward);
        gpioSetPin(FORWARD_LED, false);
        gpioSetPin(BACKWARD_LED, true);
        SEGGER_RTT_printf(0, "Direction backward\n");
    } else {
        stepMotorDriverSetDir(StepMotorDirectionForward);
        gpioSetPin(FORWARD_LED, true);
        gpioSetPin(BACKWARD_LED, false);
        SEGGER_RTT_printf(0, "Direction forward\n");
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

int main(void)
{
    clockSetHfClk();
    sysTickInit(SysTickPeriodMilisecond, NULL);
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
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
    stepMotorDriverInit(driverPins);
    stepMotorDriverSetMode(StepMotorFullMode);
    stepMotorDriverSetDir(StepMotorDirectionForward);
    gpioSetPin(FORWARD_LED, true);
    stepMotorDriverEnable(true);

    // timerStart(Timer1, 1000, onTimerCallback); // 700 mks is minimum for StepMotorFullMode
    // mpu9250Init();

    #define STEPS_PER_ROTATION (4076U) // 
    AllSensorsData data = {0};
	while (1) {
        // __WFI();
        // if (mpu9250DataReady()) {
        //     mpu9250ReadAllSensors(&data);
        //     SEGGER_RTT_printf(0, "\nAccel: x = %d y = %d z = %d\nTemp: t = %d\nGyro: x = %d y = %d z = %d\n",
        //     data.accel.x, data.accel.y, data.accel.z, data.temp.t,
        //     data.gyro.x, data.gyro.y, data.gyro.z
        //     );
        // }

        /* exact gear ratio is in fact 63.68395:1, which results in approximately 4076
        steps per full revolution (in half step mode). */

        /* To turn driver for 5.625°,  phase periods need to be done.
        Each phase period reqiers few steps depending on driver step mode aka StepMotorFullMode 4, StepMotorHalfMode 8 etc.
        So to make an full turn is StepMotorFullMode driver need to generate 64 periods * 8 steps = 512 steps*/

        // uint32_t tickMs = getCurrentTick();
        for (uint32_t i = 0; i < 512; i++) {
            uint32_t steps = getStepCount();
            for (uint32_t i = 0; i < steps; i++) {
                gpioSetPin(DRIVER_STEP_PIN, true);
                delayUs(900);
                gpioSetPin(DRIVER_STEP_PIN, false);
                delayUs(900);
            }
        }
        // SEGGER_RTT_printf(0, "Full turn, rpm = %u\n", (60U * 1000) / (getCurrentTick() - tickMs));
        stepMotorDriverSetDir(stepMotorDriverGetDir() == StepMotorDirectionForward ?
                              StepMotorDirectionBackward : StepMotorDirectionForward);
        delayMs(1000);
	}
}