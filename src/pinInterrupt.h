#include <stdint.h>
#include <stdbool.h>

typedef enum {
    RisingEdge,
    FallingEdge,
    AnyEdge,
} PinInterruptTrigger;

typedef void (* isrCallback)(void);

void pinInterruptInit(void);
bool pinInterruptEnable(uint32_t pin, isrCallback cb, PinInterruptTrigger trigger);
bool pinInterruptDisable(uint32_t pin);