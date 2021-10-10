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
void delayMs(uint32_t periodMs);
void delayUs(uint32_t periodUs);
uint32_t getCurrentTick(void);