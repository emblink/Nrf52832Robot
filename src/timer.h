#include <stdint.h>

typedef enum {
    Timer0,
    Timer1,
    Timer2,
    Timer3,
    Timer4,
    Timer5,
    TimerCount
} Timer;

typedef void (* timerCallback) (void);

void timerStart(Timer timer, uint32_t period, timerCallback cb);
void timerStop(Timer timer);