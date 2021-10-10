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
#define GYRO_CONFIG  0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_CONFIG_2 0x1D
#define LP_ACCEL_ODR 0x1E
#define WOM_THR 0x1F
#define FIFO_EN 0x23
/*** External i2c slave registers ***/
#define INT_PIN_CFG 0x37
#define INT_ENABLE 0x38
#define INT_STATUS 0x3A
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
/*** External sensors data registers ***/
#define SIGNAL_PATH_RESET 0x68
#define MOT_DETECT_CTRL 0x69
#define USER_CTRL 0x6A
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
#define FIFO_COUNTH 0x72
#define FIFO_COUNTL 0x73
#define FIFO_R_W 0x74
#define WHO_AM_I 0x75
#define XA_OFFSET_H 0x77
#define XA_OFFSET_L 0x78
#define YA_OFFSET_H 0x7A
#define YA_OFFSET_L 0x7B
#define ZA_OFFSET_H 0x7D
#define ZA_OFFSET_L 0x7E

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

    // reset
    // mpu9250Reset();

    // write accel config
    AccelConfig accel = {
        .accel_fs_sel = 0x03, // ±2g (00), ±4g (01), ±8g (10), ±16g (11)
        .az_st_en = 0x00,
        .ay_st_en = 0x00,
        .ax_st_en = 0x00,
    };
    txBuff[0] = ACCEL_CONFIG;
    txBuff[1] = accel.config;
    busy = twimWrite(0x68, txBuff, 2);
    while(busy);

    txBuff[0] = GYRO_CONFIG;
    txBuff[1] = (1 << 3) | (1 << 4); // GYRO_FS_SEL[1:0] Gyro Full Scale Select: 11 = +2000 dps
    busy = twimWrite(0x68, txBuff, 2);
    while(busy);

    // enable bypass mode
    txBuff[0] = INT_PIN_CFG;
    txBuff[1] = 1 << 1; // BYPASS_EN
    busy = twimWrite(0x68, txBuff, 2);
    while(busy);

    return true;
}

bool mpu9250Reset(void)
{
    txBuff[0] = PWR_MGMT_1;
    txBuff[1] = 1 << 7; // H_RESET Reset the internal registers and restores the default settings.
    if (!busy && twimWrite(0x68, txBuff, 2)) {
        busy = true;
        while(busy);
        return true;
    }
    return false;
}

bool mpu9250DataReady(void)
{
    if (!busy && twimRead(0x68, INT_STATUS, rxBuff, 1)) {
        busy = true;
        while(busy);
        return (rxBuff[0] & 0x01) == 1;
    }
    return false;
}

bool mpu9250ReadAccel(AccelData *data)
{
    if (!busy && twimRead(0x68, ACCEL_XOUT_H, data, sizeof(AccelData))) {
        busy = true;
        while(busy);
        return true;
    }
    return false;
}

bool mpu9250ReadTemp(TempData *data)
{
    // TEMP_degC = ((TEMP_OUT – RoomTemp_Offset)/Temp_Sensitivity) + 21degC

    if (!busy && twimRead(0x68, TEMP_OUT_H, data, sizeof(TempData))) {
        busy = true;
        while(busy);
        return true;
    }
    return false;
}

bool mpu9250ReadGyro(GyroData *data)
{
// Low byte of the X-Axis gyroscope output
// GYRO_XOUT = Gyro_Sensitivity * X_angular_rate
// Nominal
// Conditions
// FS_SEL = 3
// Gyro_Sensitivity = 16.4 LSB/(º/s)

    if (!busy && twimRead(0x68, GYRO_XOUT_H, data, sizeof(GyroData))) {
        busy = true;
        while(busy);
        return true;
    }
    return false;
}

bool mpu9250ReadAllSensors(AllSensorsData *data)
{
    if (!busy && twimRead(0x68, ACCEL_XOUT_H, data, sizeof(AllSensorsData))) {
        return busy = true;
    }
    return false;
}

// bool mpu9250ReadMagn(MagnData *data)
// {
//     busy = twimRead(0x68, INT_STATUS, &data, sizeof(AccelData));
//     while(busy);
//     return (rxBuff[0] & 0x01) == 1;
// }

static void twimCallback(TwimError err)
{
    if (err != TwimErrorOk)
        __asm("BKPT #255");
    busy = false;
}