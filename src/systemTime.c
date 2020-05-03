#include <stdint.h>
#include <stdlib.h>
#include "nrf52.h"
#include "systemTime.h"

static volatile uint32_t timeTick = 0;
static sysTickCallback sysTickcb = NULL;

void sysTickInit(SysTickPeriod period, sysTickCallback callback)
{
    NVIC_DisableIRQ(SysTick_IRQn);
    timeTick = 0;
    sysTickcb = callback;
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // disable sys tick
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; // set processor clock
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // enable interrupt
    SysTick->LOAD = (SystemCoreClock / period) & SysTick_LOAD_RELOAD_Msk; // 1 ms tick
    SysTick->VAL = 0; // clear current value
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // enable sys tick
    NVIC_ClearPendingIRQ(SysTick_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
}

void sysTickStop(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    SysTick->VAL = 0; // clear current value
}

void sysTickStart(void)
{
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void delay(uint32_t period)
{
    uint32_t currentTick = getCurrentTick();
    while(getCurrentTick() - currentTick < period) {
        // blocking delay
    }
}

uint32_t getCurrentTick(void)
{
    NVIC_DisableIRQ(SysTick_IRQn);
    uint32_t tick = timeTick;
    NVIC_EnableIRQ(SysTick_IRQn);
    return tick;
}

void SysTick_Handler(void)
{
    timeTick++;
    if (sysTickcb)
        sysTickcb();
}