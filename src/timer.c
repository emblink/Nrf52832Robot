#include <stdint.h>
#include <stddef.h>
#include "timer.h"
#include "nrf52.h"

#define TIMER_0_BASE (0x40008000U)
#define TIMER_1_BASE (0x40009000U)
#define TIMER_2_BASE (0x4000A000U)
#define TIMER_3_BASE (0x4001A000U)
#define TIMER_4_BASE (0x4001B000U)

#define REGISTER(base, offset) (*((uint32_t *)(base + offset)))

#define TIMER_START_TASK    (0x000)
#define TIMER_STOP_TASK     (0x004)
#define TIMER_COUNT         (0x008)
#define TIMER_CLEAR_TASK    (0x00C)
#define TIMER_SHUTDOWN_TASK (0x010)
#define TIMER_SHORTS        (0x200)
#define TIMER_INTSET        (0x304)
#define TIMER_INTCLR        (0x308)
#define TIMER_MODE          (0x504)
#define TIMER_BITMODE       (0x508)
#define TIMER_PRESCALER     (0x510)
#define TIMER_CAPTCOMP0     (0x540)
#define TIMER_CAPTCOMP1     (0x544)
#define TIMER_CAPTCOMP2     (0x548)
#define TIMER_CAPTCOMP3     (0x54C)
#define TIMER_CAPTCOMP4     (0x550)
#define TIMER_CAPTCOMP5     (0x554)
#define EVENT_CAPTCOMP0     (0x140)
#define EVENT_CAPTCOMP1     (0x144)
#define EVENT_CAPTCOMP2     (0x148)
#define EVENT_CAPTCOMP3     (0x14C)
#define EVENT_CAPTCOMP4     (0x150)
#define EVENT_CAPTCOMP5     (0x154)

static timerCallback callback[5] = {NULL};
static uint32_t base = 0x00;

void timerStart(Timer timer, uint32_t periodUs, timerCallback cb)
{
    base = 0x00;
    switch (timer) {
    case Timer0:
        base = TIMER_0_BASE;
        NVIC_ClearPendingIRQ(TIMER0_IRQn);
        NVIC_EnableIRQ(TIMER0_IRQn);
        break;
    case Timer1:
        base = TIMER_1_BASE;
        NVIC_ClearPendingIRQ(TIMER1_IRQn);
        NVIC_EnableIRQ(TIMER1_IRQn);
        break; 
    case Timer2:
        base = TIMER_2_BASE;
        NVIC_ClearPendingIRQ(TIMER2_IRQn);
        NVIC_EnableIRQ(TIMER2_IRQn);
        break; 
    case Timer3:
        base = TIMER_3_BASE;
        NVIC_ClearPendingIRQ(TIMER3_IRQn);
        NVIC_EnableIRQ(TIMER3_IRQn);
        break; 
    case Timer4:
        base = TIMER_4_BASE;
        NVIC_ClearPendingIRQ(TIMER4_IRQn);
        NVIC_EnableIRQ(TIMER4_IRQn);
        break;
    default:
        return;
    }

    REGISTER(base, TIMER_STOP_TASK) |= (1 << 0); // stop
    REGISTER(base, TIMER_CLEAR_TASK) |= (1 << 0); // clear
    REGISTER(base, TIMER_SHORTS) |= (1 << 0); // compare clear enable
    REGISTER(base, TIMER_MODE) &= (~3); // timer mode
    REGISTER(base, TIMER_BITMODE) |= (0x03); // 32 bits
    REGISTER(base, TIMER_PRESCALER) |= (0x04); // divider 16 // fTIMER = 16 MHz / (2 in PRESCALER)
    uint32_t ccrValue = periodUs; // 1 sec == 1 000 000 counts
    REGISTER(base, TIMER_CAPTCOMP0) = ccrValue; // set compare 0 value
    REGISTER(base, TIMER_INTSET) = (1 << 16); // compare 0 isr enable
    callback[timer] = cb;
    REGISTER(base, TIMER_START_TASK) |= 0x01; // start timer
}

void timerStop(Timer timer)
{
    base = 0x00;
    switch (timer) {
    case Timer0:
        base = TIMER_0_BASE;
        REGISTER(base, TIMER_INTCLR) = (1 << 16);
        NVIC_DisableIRQ(TIMER0_IRQn);
        break;
    case Timer1:
        base = TIMER_1_BASE;
        REGISTER(base, TIMER_INTCLR) = (1 << 16);
        NVIC_DisableIRQ(TIMER1_IRQn);
        break; 
    case Timer2:
        base = TIMER_2_BASE;
        REGISTER(base, TIMER_INTCLR) = (1 << 16);
        NVIC_DisableIRQ(TIMER2_IRQn);
        break; 
    case Timer3:
        base = TIMER_3_BASE;
        REGISTER(base, TIMER_INTCLR) = (1 << 16);
        NVIC_DisableIRQ(TIMER3_IRQn);
        break; 
    case Timer4:
        base = TIMER_4_BASE;
        REGISTER(base, TIMER_INTCLR) = (1 << 16);
        NVIC_DisableIRQ(TIMER4_IRQn);
        break;
    default:
        return;
    }

    REGISTER(base, TIMER_STOP_TASK) |= (1 << 0);
}

void TIMER0_IRQHandler(void)
{
    REGISTER(TIMER_0_BASE, EVENT_CAPTCOMP0) = 0;
    if (callback[0] != NULL) {
        callback[0]();
    }
}

void TIMER1_IRQHandler(void)
{
    REGISTER(TIMER_1_BASE, EVENT_CAPTCOMP0) = 0;
    if (callback[1] != NULL) {
        callback[1]();
    }
}

void TIMER2_IRQHandler(void)
{
    REGISTER(TIMER_2_BASE, EVENT_CAPTCOMP0) = 0;
    if (callback[2] != NULL) {
        callback[2]();
    }
}

void TIMER3_IRQHandler(void)
{
    REGISTER(TIMER_3_BASE, EVENT_CAPTCOMP0) = 0;
    if (callback[3] != NULL) {
        callback[3]();
    }
}

void TIMER4_IRQHandler(void)
{
    REGISTER(TIMER_4_BASE, EVENT_CAPTCOMP0) = 0;
    if (callback[4] != NULL) {
        callback[4]();
    }
}