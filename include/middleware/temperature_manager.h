#ifndef TEMPERATURE_MANAGER_H
#define TEMPERATURE_MANAGER_H

#include <stdint.h>

typedef struct {
	uint8_t temperature;
	uint8_t humidity;
} TemperatureData_t;

void TemperatureManager_Init(void);
void TemperatureManager_Update(uint8_t temperature, uint8_t humidity);
uint8_t TemperatureManager_GetTemperature(void);
uint8_t TemperatureManager_GetHumidity(void);
TemperatureData_t TemperatureManager_GetData(void);

#endif // TEMPERATURE_MANAGER_H
