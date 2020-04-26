#include <stdint.h>
#include <stdbool.h>

#define DIRECTION_INVERT (0)

typedef enum driverMode {
    StepMotorFullMode = 0U,
    StepMotorHalfMode = 2U,
    StepMotorQuaterMode = 1U,
    StepMotorEighthMode = 3U,
    StepMotorEighthModeCount = 4U,
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

bool stepMotorDriverInit(StepMotorDriverPin pins[StepMotorPinCount], StepMotorDriverMode mode);
bool stepMotorDriverEnable(bool enable);
bool stepMotorDriverReset(void);
bool stepMotorDriverSleep(bool sleep);
bool stepMotorDriverSetDir(StepMotorDirection direction);
bool stepMotorDriverGetDir(StepMotorDirection *dir);
bool stepMotorDriverSetMode(StepMotorDriverMode mode);
bool stepMotorDriverGetMode(StepMotorDriverMode *mode);
bool stepMotorDriverStep(void);