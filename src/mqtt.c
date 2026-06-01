#include "mqtt.h"
#include "config.h"

#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"

static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t client = NULL;
static bool mqtt_connected = false;

/* ===================== */
bool mqtt_is_connected(void)
{
    return mqtt_connected;
}

/* ===================== */
static void mqtt_event_handler(
    void *handler_args,
    esp_event_base_t base,
    int32_t event_id,
    void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
            mqtt_connected = true;
            ESP_LOGI(TAG, "MQTT CONNECTED");
            break;

        case MQTT_EVENT_DISCONNECTED:
            mqtt_connected = false;
            ESP_LOGW(TAG, "MQTT DISCONNECTED");
            break;

        case MQTT_EVENT_ERROR:
            mqtt_connected = false;
            ESP_LOGE(TAG, "MQTT ERROR");
            break;

        default:
            break;
    }
}

/* ===================== */
void mqtt_app_start(void)
{
    ESP_LOGI(TAG, "Starting MQTT...");

    esp_mqtt_client_config_t cfg =
    {
        .broker.address.uri = MQTT_BROKER,

        .credentials.username = MQTT_USERNAME,
        .credentials.authentication.password = MQTT_PASSWORD,

        /* ===== TLS FIX CHUẨN HIVE MQTT CLOUD ===== */
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,

        .session.keepalive = 60,
        .network.reconnect_timeout_ms = 5000,
        .network.timeout_ms = 10000,
    };

    client = esp_mqtt_client_init(&cfg);

    if (!client)
    {
        ESP_LOGE(TAG, "MQTT init failed");
        return;
    }

    esp_mqtt_client_register_event(
        client,
        ESP_EVENT_ANY_ID,
        mqtt_event_handler,
        NULL);

    esp_mqtt_client_start(client);
}

/* ===================== */
void mqtt_publish_sensor(const char *payload)
{
    if (!client || !mqtt_connected)
    {
        ESP_LOGW(TAG, "MQTT not ready");
        return;
    }

    int msg_id = esp_mqtt_client_publish(
        client,
        MQTT_TOPIC,
        payload,
        0,
        1,
        0);

    ESP_LOGI(TAG, "Published msg_id=%d", msg_id);
}