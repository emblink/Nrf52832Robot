#include <stdlib.h>
#include "pinInterrupt.h"
#include "nrf.h"

#define PIN_INTERRUPT_COUNT 8
#define PIN_MAX_NUMBER      32

typedef struct {
    uint32_t pin;
    isrCallback cb;
    PinInterruptTrigger edge;
} pinInterruptConfig;

static pinInterruptConfig pinInterruptCfg[PIN_INTERRUPT_COUNT] = {0};
static bool isGPIOTEInited = false;

void pinInterruptInit(void)
{
    for (uint32_t event = 0; event < PIN_INTERRUPT_COUNT; event++) {
        NRF_GPIOTE->CONFIG[event] &= ~GPIOTE_CONFIG_MODE_Msk;
        NRF_GPIOTE->CONFIG[event] &= ~GPIOTE_CONFIG_PSEL_Msk;
        NRF_GPIOTE->CONFIG[event] &= ~GPIOTE_CONFIG_POLARITY_Msk;
        NRF_GPIOTE->EVENTS_IN[event] = 0;
        NRF_GPIOTE->INTENCLR = 1 << event;
        pinInterruptCfg[event].pin = PIN_MAX_NUMBER;
        pinInterruptCfg[event].cb = NULL;
    }
    NVIC_ClearPendingIRQ(GPIOTE_IRQn);
    NVIC_EnableIRQ(GPIOTE_IRQn);
    isGPIOTEInited = true;
}

bool pinInterruptEnable(uint32_t pin, isrCallback cb, PinInterruptTrigger trigger)
{
    if (!isGPIOTEInited || pin > PIN_MAX_NUMBER) // 31 is max pin count for nrf52832, it has only one gpio port
        return false;
    
    for (uint32_t event = 0; event < PIN_INTERRUPT_COUNT; event++) {
        if ((NRF_GPIOTE->CONFIG[event] & GPIOTE_CONFIG_MODE_Msk) == GPIOTE_CONFIG_MODE_Disabled) {
            NRF_GPIOTE->CONFIG[event] |= (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);
            NRF_GPIOTE->CONFIG[event] |= (pin << GPIOTE_CONFIG_PSEL_Pos);
            uint32_t polarity = GPIOTE_CONFIG_POLARITY_None;
            if (trigger == RisingEdge) {
                polarity = GPIOTE_CONFIG_POLARITY_LoToHi;
            } else if (trigger == FallingEdge) {
                polarity = GPIOTE_CONFIG_POLARITY_HiToLo;
            } else if (trigger == AnyEdge) {
                polarity = GPIOTE_CONFIG_POLARITY_Toggle;
            } else {
                return false;
            }
            NRF_GPIOTE->CONFIG[event] |= (polarity << GPIOTE_CONFIG_POLARITY_Pos);
            pinInterruptCfg[event].pin = pin;
            pinInterruptCfg[event].cb = cb;
            NRF_GPIOTE->INTENSET = 1 << event;
            return true;
        }
    }
    return false;
}

bool pinInterruptDisable(uint32_t pin)
{
    if (!isGPIOTEInited)
        return false;

    for (uint32_t event = 0; event < PIN_INTERRUPT_COUNT; event++) {
        if (pinInterruptCfg[event].pin == pin) {
            NRF_GPIOTE->CONFIG[event] &= ~GPIOTE_CONFIG_MODE_Msk;
            NRF_GPIOTE->CONFIG[event] &= ~GPIOTE_CONFIG_PSEL_Msk;
            NRF_GPIOTE->CONFIG[event] &= ~GPIOTE_CONFIG_POLARITY_Msk;
            NRF_GPIOTE->EVENTS_IN[event] = 0;
            NRF_GPIOTE->INTENCLR = 1 << event;
            pinInterruptCfg[event].pin = PIN_MAX_NUMBER;
            pinInterruptCfg[event].cb = NULL;
            return true;
        }
    }
    return false;
}

void GPIOTE_IRQHandler(void)
{
    for (uint32_t event = 0; event < PIN_INTERRUPT_COUNT; event++) {
        if (NRF_GPIOTE->EVENTS_IN[event] != 0) {
            if (pinInterruptCfg[event].cb) {
                pinInterruptCfg[event].cb();
            }
            NRF_GPIOTE->EVENTS_IN[event] = 0;
        }
    }
}