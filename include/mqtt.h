#ifndef MQTT_H
#define MQTT_H

#include <stdbool.h>

void mqtt_app_start(void);
void mqtt_publish_sensor(const char *payload);

bool mqtt_is_connected(void);

#endif