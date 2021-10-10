#include <stdint.h>
#include <stdbool.h>

#define DIRECTION_INVERT (0)

typedef enum driverMode {
    StepMotorFullMode = 0U,
    StepMotorHalfMode,
    StepMotorQuaterMode,
    StepMotorEighthMode,
    StepMotorModeCount,
} StepMotorDriverMode;

typedef enum motorDirection {
    StepMotorDirectionForward = 0U,
    StepMotorDirectionBackward,
} StepMotorDirection;

typedef enum driverPins {
    StepMotorStepPin = 0UL,
    StepMotorDirectionPin,
    StepMotorResetPin,
    StepMotorSleepPin,
    StepMotorEnablePin,
    StepMotorMs1Pin,
    StepMotorMs2Pin,
    StepMotorPfdPin,
    StepMotorPinCount,
} StepMotorDriverPin;

void stepMotorDriverInit(StepMotorDriverPin pins[StepMotorPinCount]);
void stepMotorDriverEnable(bool enable);
void stepMotorDriverReset(void);
void stepMotorDriverSleep(bool sleep);
void stepMotorDriverSetDir(StepMotorDirection direction);
StepMotorDirection stepMotorDriverGetDir(void);
void stepMotorDriverSetMode(StepMotorDriverMode mode);
StepMotorDriverMode stepMotorDriverGetMode(void);
void stepMotorDriverStep(bool state);