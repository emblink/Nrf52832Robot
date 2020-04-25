#include <stdint.h>
#include <stdbool.h>

typedef enum driverMode {
    StepMotorFullMode = 0U,
    StepMotorHalfMode = 2U,
    StepMotorQuaterMode = 1U,
    StepMotorEighthMode = 3U,
    StepMotorEighthModeCount = 4U,
} StepMotorDriverMode;

typedef enum motorDirection {
    StepMotorDirectionForward,
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

bool stepMotorDriverInit(StepMotorDriverPin pins[StepMotorPinCount], StepMotorDriverMode mode);
bool stepMotorDriverEnable(bool enable);
bool stepMotorDriverReset(void);
bool stepMotorDriverSleep(bool sleep);
bool stepMotorDriverSetDir(StepMotorDirection direction);
bool stepMotorDriverSetMode(StepMotorDriverMode mode);
bool stepMotorDriverStep(void);