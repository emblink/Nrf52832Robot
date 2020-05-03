#include <stdint.h>

typedef void (*tempSensorCallback)(uint32_t temperature);

void tempSensorStart(tempSensorCallback cb); // for polling method cb must be NULL
uint32_t tempSensorGetData(void); // polling
void tempSensorStop(void);


