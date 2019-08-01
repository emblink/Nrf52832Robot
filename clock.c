#include <stdint.h>
#include <stdbool.h>

#define CLOCK_BASE              (0x40000000U)
#define TASKS_HFCLKSTART        *((uint32_t *) (CLOCK_BASE + 0x000))
#define TASKS_HFCLKSTOP         *((uint32_t *) (CLOCK_BASE + 0x004))
#define EVENTS_HFCLKSTARTED     *((uint32_t *) (CLOCK_BASE + 0x100))
#define HFCLKRUN                *((uint32_t *) (CLOCK_BASE + 0x408)) // start task has been triggered
#define HFCLKSTAT               *((uint32_t *) (CLOCK_BASE + 0x40C))

bool clockSetHfClk(void)
{
    TASKS_HFCLKSTART |= 1;
    if (HFCLKRUN & 0x01) {
        while(1) {
            if ((HFCLKSTAT & (1 << 0)) && (HFCLKSTAT & (1 << 16)))
                break;
        }
        return true;
    }
    return false;
}
