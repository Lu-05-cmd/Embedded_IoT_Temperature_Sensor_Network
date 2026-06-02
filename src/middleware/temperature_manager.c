#include "middleware/temperature_manager.h"
#include <string.h>

static TemperatureData_t g_tempData;

void TemperatureManager_Init(void) {
    memset(&g_tempData, 0, sizeof(g_tempData));
}

void TemperatureManager_Update(int temperature, int humidity) {
    g_tempData.temperature = temperature;
    g_tempData.humidity = humidity;
}

int TemperatureManager_GetTemperature(void) {
    return g_tempData.temperature;
}

int TemperatureManager_GetHumidity(void) {
    return g_tempData.humidity;
}

TemperatureData_t TemperatureManager_GetData(void) {
    return g_tempData;
}

EnvStatus_t TemperatureManager_EvaluateStatus(int temperature, int humidity) {
    if (temperature >= TEMP_DANGER_C || humidity >= HUM_DANGER_PERCENT) {
        return ENV_STATUS_DANGER;
    }

    if (temperature >= TEMP_WARNING_C || humidity >= HUM_WARNING_PERCENT) {
        return ENV_STATUS_WARNING;
    }

    return ENV_STATUS_NORMAL;
}

const char *TemperatureManager_GetStatusText(EnvStatus_t status) {
    switch (status) {
    case ENV_STATUS_SENSOR_ERROR:
        return "SENSOR_ERROR";
    case ENV_STATUS_DANGER:
        return "DANGER";
    case ENV_STATUS_WARNING:
        return "WARNING";
    default:
        return "NORMAL";
    }
}
