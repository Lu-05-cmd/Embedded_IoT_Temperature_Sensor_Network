#include "middleware/temperature_manager.h"

#include "middleware/temperature_manager.h"
#include <string.h>

static TemperatureData_t g_tempData;

void TemperatureManager_Init(void) {
    memset(&g_tempData, 0, sizeof(g_tempData));
}

void TemperatureManager_Update(uint8_t temperature, uint8_t humidity) {
    g_tempData.temperature = temperature;
    g_tempData.humidity = humidity;
}

uint8_t TemperatureManager_GetTemperature(void) {
    return g_tempData.temperature;
}

uint8_t TemperatureManager_GetHumidity(void) {
    return g_tempData.humidity;
}

TemperatureData_t TemperatureManager_GetData(void) {
    return g_tempData;
}
