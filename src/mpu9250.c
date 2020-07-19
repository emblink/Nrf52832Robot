#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "twim.h"
#include "mpu9250.h"

// Registers addresses
#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02
#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F
#define XG_OFFSET_H 0x13
#define XG_OFFSET_L 0x14
#define YG_OFFSET_H 0x15
#define YG_OFFSET_L 0x16
#define ZG_OFFSET_H 0x17
#define ZG_OFFSET_L 0x18
#define SMPLRT_DIV 0x19
#define CONFIG 0x1A
#define ACCEL_CONFIG 0x1B
#define ACCEL_CONFIG_2 0x1C
#define LP_ACCEL_ODR 0x1E
#define WOM_THR 0x1F
#define FIFO_EN 0x23

#pragma pack(1)
typedef struct {
    uint8_t xAxis;
    uint8_t yAxis;
    uint8_t zAxis;
} SelfTest;

typedef union {
    struct {
        uint8_t reserved : 3;
        uint8_t accel_fs_sel : 2;
        uint8_t az_st_en : 1;
        uint8_t ay_st_en : 1;
        uint8_t ax_st_en : 1;
    };
    uint8_t config;
} AccelConfig;
#pragma pop()

static volatile bool busy = false;
static uint8_t txBuff[100] = {0};
static uint8_t rxBuff[100] = {0};
static SelfTest gyroTest = {0};
static SelfTest accelTest = {0};

static void twimCallback(TwimError err);

bool mpu9250Init(void)
{
    twimInit(TWIM_SDA_PIN, TWIM_SCL_PIN, twimCallback);
    AccelConfig accel = {
        .accel_fs_sel = 0x03, // ±2g (00), ±4g (01), ±8g (10), ±16g (11)
        .az_st_en = 0x01,
        .ay_st_en = 0x01,
        .ax_st_en = 0x01,
    };
    txBuff[0] = ACCEL_CONFIG;
    txBuff[1] = accel.config;
    busy = twimWrite(0x68, txBuff, 2);
    while(busy);
    busy = twimRead(0x68, SELF_TEST_X_ACCEL, &accelTest, sizeof(accelTest));
    while(busy);

    return true;
}

static void twimCallback(TwimError err)
{
    if (err != TwimErrorOk)
        __asm("BKPT #255");
    busy = false;
}