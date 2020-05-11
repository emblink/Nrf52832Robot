#ifndef __TWIM
#define __TWIM
#include <stdint.h>
#include <stdbool.h>

#define TWIM_WRITE_BUFF_SIZE 40
#define TWIM_READ_BUFF_SIZE 40

typedef enum {
    TwimErrorOk = 0U,
    TwimErrorOverrun = 1U,
    TwimErrorANACK = 2U,
    TwimErrorDNACK = 4U,
} TwimError;

typedef void (* twimCb)(TwimError error);

bool twimInit(uint32_t sdaPin, uint32_t sclPin, twimCb callback);
bool twimWrite(uint8_t addr, void *txBuff, uint32_t len);
bool twimRead(uint8_t addr, void *rxBuff, uint32_t len);
#endif // __TWIM