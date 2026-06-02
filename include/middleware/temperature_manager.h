#ifndef TEMPERATURE_MANAGER_H
#define TEMPERATURE_MANAGER_H

#include <stdint.h>

typedef struct {
	int temperature;
	int humidity;
} TemperatureData_t;

#define TEMP_WARNING_C 32
#define TEMP_DANGER_C  38
#define HUM_WARNING_PERCENT 75
#define HUM_DANGER_PERCENT  85

typedef enum {
    ENV_STATUS_NORMAL = 0,
    ENV_STATUS_WARNING,
    ENV_STATUS_DANGER,
    ENV_STATUS_SENSOR_ERROR
} EnvStatus_t;

void TemperatureManager_Init(void);
void TemperatureManager_Update(int temperature, int humidity);
int TemperatureManager_GetTemperature(void);
int TemperatureManager_GetHumidity(void);
TemperatureData_t TemperatureManager_GetData(void);
EnvStatus_t TemperatureManager_EvaluateStatus(int temperature, int humidity);
const char *TemperatureManager_GetStatusText(EnvStatus_t status);

#endif // TEMPERATURE_MANAGER_H
