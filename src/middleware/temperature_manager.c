#include "middleware/temperature_manager.h"
#include <string.h>

static TemperatureData_t g_tempData;
static TemperatureStats_t g_tempStats;

void TemperatureManager_Init(void) {
    memset(&g_tempData, 0, sizeof(g_tempData));
    memset(&g_tempStats, 0, sizeof(g_tempStats));
}

void TemperatureManager_Update(int temperature, int humidity) {
    g_tempData.temperature = temperature;
    g_tempData.humidity = humidity;

    if (!g_tempStats.has_sample) {
        g_tempStats.temp_min = temperature;
        g_tempStats.temp_max = temperature;
        g_tempStats.humi_min = humidity;
        g_tempStats.humi_max = humidity;
        g_tempStats.has_sample = 1;
        return;
    }

    if (temperature < g_tempStats.temp_min) {
        g_tempStats.temp_min = temperature;
    }
    if (temperature > g_tempStats.temp_max) {
        g_tempStats.temp_max = temperature;
    }
    if (humidity < g_tempStats.humi_min) {
        g_tempStats.humi_min = humidity;
    }
    if (humidity > g_tempStats.humi_max) {
        g_tempStats.humi_max = humidity;
    }
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

TemperatureStats_t TemperatureManager_GetStats(void) {
    return g_tempStats;
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
