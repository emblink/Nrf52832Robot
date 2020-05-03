#include <stdint.h>
#include "nrf52.h"
#include "systemTime.h"

static volatile uint32_t timeTick = 0;

void sysTickInit(void)
{
    NVIC_DisableIRQ(SysTick_IRQn);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // disable sys tick
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; // set processor clock
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // enable interrupt
    SysTick->LOAD = (SystemCoreClock / 1000) & SysTick_LOAD_RELOAD_Msk; // 1 ms tick
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

void delay(uint32_t ms)
{
    uint32_t currentTick = getCurrentTick();
    while(getCurrentTick() - currentTick < ms) {
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
}