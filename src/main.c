#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "uart.h"
#include "wifi.h"
#include "mqtt.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    char line[128];

    ESP_ERROR_CHECK(nvs_flash_init());

    uart_init();

    wifi_init();

    mqtt_app_start();

    ESP_LOGI(TAG,
             "System Ready");

    while (1)
    {
        int len =
            uart_read_line(
                line,
                sizeof(line));

        if (len > 0)
        {
            ESP_LOGI(TAG,
                     "RX DATA: %s",
                     line);

            if (mqtt_is_connected())
            {
                mqtt_publish_sensor(line);

                ESP_LOGI(TAG,
                         "Published MQTT");
            }
            else
            {
                ESP_LOGW(TAG,
                         "MQTT not connected -> skip publish");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}