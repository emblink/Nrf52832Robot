#include <stdint.h>
#include <stdbool.h>

typedef union config {
    struct {
        uint32_t enablePin;
        uint32_t directionPin;
        uint32_t resetPin;
        uint32_t sleepPin;
        uint32_t stepPin;
    };
    uint32_t pins[5];
} stepMotorDriverConfig;

void stepMotorDriverInit(stepMotorDriverConfig config);
void stepMotorDriverEnable(bool enable);
void stepMotorDriverReset(void);
void stepMotorDriverSleep(bool enable);
void stepMotorDriverSetDir(bool direction);
void stepMotorDriverStep(void);