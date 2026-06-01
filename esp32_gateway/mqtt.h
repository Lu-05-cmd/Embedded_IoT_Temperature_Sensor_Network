#ifndef MQTT_H
#define MQTT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool mqtt_is_connected(void);
void mqtt_app_start(void);
void mqtt_publish_sensor(const char *payload);

#ifdef __cplusplus
}
#endif

#endif // MQTT_H
