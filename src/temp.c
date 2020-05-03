#include <stdint.h>
#include <stddef.h>
#include "temp.h"
#include "nrf52.h"

#define TEMP_SENS_BASE      (0x4000C000U)
#define TEMP_TASK_START     *((uint32_t *) (TEMP_SENS_BASE + 0x000))
#define TEMP_TASK_STOP      *((uint32_t *) (TEMP_SENS_BASE + 0x004))
#define TEMP_EVENT_DATARDY  *((uint32_t *) (TEMP_SENS_BASE + 0x100))
#define TEMP_INTENSET       *((uint32_t *) (TEMP_SENS_BASE + 0x304))
#define TEMP_INTENCLR       *((uint32_t *) (TEMP_SENS_BASE + 0x308))
#define TEMP_DATA_REG       *((uint32_t *) (TEMP_SENS_BASE + 0x508))

static void tempEnableIsr(void);
static void tempDisableIsr(void);

tempSensorCallback callback = NULL;

void tempSensorStart(tempSensorCallback cb)
{
    if (cb) {
        callback = cb;
        tempEnableIsr();
    } else {
        tempDisableIsr();
    }
    TEMP_TASK_START = 0x01;
}

uint32_t tempSensorGetData(void)
{
    if (callback)
        return 0;

    while(!TEMP_EVENT_DATARDY);
    uint32_t temp = TEMP_DATA_REG * 25 / 100;
    TEMP_EVENT_DATARDY = 0;
    TEMP_TASK_START = 0x01;
    return temp;
}

void tempSensorStop(void)
{
    TEMP_EVENT_DATARDY = 0;
    callback = NULL;
    tempDisableIsr();
}

static void tempEnableIsr(void)
{
    TEMP_INTENSET = (1 << 0);
    NVIC_ClearPendingIRQ(TEMP_IRQn);
    NVIC_EnableIRQ(TEMP_IRQn);
}

static void tempDisableIsr(void)
{
    NVIC_DisableIRQ(TEMP_IRQn);
    TEMP_INTENCLR = (1 << 0);
}

void TEMP_IRQHandler(void)
{
    TEMP_EVENT_DATARDY = 0;
    if (callback != NULL) {
        callback((TEMP_DATA_REG * 25) / 100);
    }
    TEMP_TASK_START = 0x01;
}
