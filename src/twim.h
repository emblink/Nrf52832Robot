#ifndef __TWIM
#define __TWIM
#include <stdint.h>
#include <stdbool.h>

// pins 9 and 10 by default used by nfc, define CONFIG_NFCT_PINS_AS_GPIOS set them as gpios.
#define TWIM_SDA_PIN            9U
#define TWIM_SCL_PIN            10U

typedef enum {
    TwimErrorOk = 0U,
    TwimErrorOverrun = 1U,
    TwimErrorANACK = 2U,
    TwimErrorDNACK = 4U,
} TwimError;

typedef void (* twimCb)(TwimError error);

bool twimInit(uint32_t sdaPin, uint32_t sclPin, twimCb callback);
bool twimWrite(uint8_t i2cAddr, void *txBuff, uint32_t len);
bool twimRead(uint8_t i2cAddr, uint8_t registerAddr, void *rxBuff, uint32_t len);
#endif // __TWIM
