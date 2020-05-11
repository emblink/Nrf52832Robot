#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "twim.h"
#include "gpio.h"
#include "nrf.h"

static uint8_t *tx = NULL;
static uint8_t *rx = NULL;
static twimCb twimCallback = NULL;

bool twimInit(uint32_t sdaPin, uint32_t sclPin, twimCb callback)
{
    if (sdaPin > 31 || sclPin > 31 || callback == NULL)
        return false;
    
    NVIC_DisableIRQ(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);
    //PSEL.SCL, PSEL.SDA must only be configured when the TWI master is disabled.
    NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
    NRF_TWIM0->INTEN = 0; // disble all interrupts

    GpioConfig pinConfig =  {
        .dir = GpioInput,
        .input = GpioInputConnect,
        .pull = GpioPullUp,
        .drive = GpioDriveS0D1,
        .sence = GpioSenseDisabled
    };

    gpioConfig(sdaPin, pinConfig);
    gpioConfig(sclPin, pinConfig);

    twimCallback = callback;

    NRF_TWIM0->PSEL.SDA = sdaPin;
    NRF_TWIM0->PSEL.SCL = sclPin;

    NRF_TWIM0->FREQUENCY = TWI_FREQUENCY_FREQUENCY_K100 << TWI_FREQUENCY_FREQUENCY_Pos;

    NRF_TWIM0->TXD.LIST = TWIM_TXD_LIST_LIST_Disabled;
    NRF_TWIM0->RXD.LIST = TWIM_TXD_LIST_LIST_Disabled;

    NRF_TWIM0->SHORTS = (TWIM_SHORTS_LASTTX_STOP_Enabled << TWIM_SHORTS_LASTTX_STOP_Pos) |
                        (TWIM_SHORTS_LASTRX_STOP_Enabled << TWIM_SHORTS_LASTRX_STOP_Pos);

    NRF_TWIM0->INTENSET = (TWIM_INTENSET_ERROR_Enabled << TWIM_INTENSET_ERROR_Pos) |
                          (TWIM_INTENSET_STOPPED_Enabled << TWIM_INTENSET_STOPPED_Pos);

    NVIC_ClearPendingIRQ(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);
    NVIC_EnableIRQ(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);
    
    NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;
    return true;
}

bool twimWrite(uint8_t addr, void *txBuff, uint32_t len)
{
    if (len == 0 || len > TWIM_WRITE_BUFF_SIZE)
        return false;

    NRF_TWIM0->ADDRESS = addr & TWI_ADDRESS_ADDRESS_Msk;
    NRF_TWIM0->TXD.PTR = (uint32_t) txBuff;
    NRF_TWIM0->TXD.MAXCNT = len;
    NRF_TWIM0->TASKS_STARTTX = 1;
    return true;
}

bool twimRead(uint8_t addr, void *rxBuff, uint32_t len)
{
    if (len == 0 || len > TWIM_READ_BUFF_SIZE)
        return false;

    NRF_TWIM0->ADDRESS = addr & TWI_ADDRESS_ADDRESS_Msk;
    NRF_TWIM0->RXD.PTR = (uint32_t) rxBuff;
    NRF_TWIM0->RXD.MAXCNT = len;
    NRF_TWIM0->TASKS_STARTRX = 1;
    return true;
}

void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void)
{
    twimCallback((TwimError) NRF_TWIM0->ERRORSRC);
}