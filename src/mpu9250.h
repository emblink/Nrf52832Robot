#include "stdint.h"
#include "stdbool.h"

typedef bool (* txDataCallback)(uint8_t data[], uint32_t len);
typedef bool (* rxDataCallback)(uint8_t data[], uint32_t len);

bool mpu9250Init(txDataCallback callback);
