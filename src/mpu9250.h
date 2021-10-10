#include "stdint.h"
#include "stdbool.h"

#pragma pack(1)
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} AccelData;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} GyroData;

typedef struct {
    uint16_t t;
} TempData;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} MagnData;

typedef struct {
    AccelData accel;
    TempData temp;
    GyroData gyro;
} AllSensorsData;
#pragma pop

bool mpu9250Init(void);
bool mpu9250Reset(void);
bool mpu9250DataReady(void);
bool mpu9250ReadAccel(AccelData *data);
bool mpu9250ReadTemp(TempData *data);
bool mpu9250ReadGyro(GyroData *data);
bool mpu9250ReadAllSensors(AllSensorsData * data);
// bool mpu9250ReadMagn(MagnData *data);

