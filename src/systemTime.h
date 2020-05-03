#include <stdint.h>

typedef enum {
    SysTickPeriodSecond = 1U,
    SysTickPeriodMilisecond = 1000U,
    SysTickPeriodMicrosecond = 1000000U,
} SysTickPeriod;

typedef void (*sysTickCallback)(void);

void sysTickInit(SysTickPeriod period, sysTickCallback callback);
void sysTickStop(void);
void sysTickStart(void);
void delay(uint32_t period);
uint32_t getCurrentTick(void);