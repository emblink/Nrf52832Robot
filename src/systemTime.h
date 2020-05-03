#include <stdint.h>

void sysTickInit(void);
void sysTickStop(void);
void sysTickStart(void);
void delay(uint32_t ms);
uint32_t getCurrentTick(void);