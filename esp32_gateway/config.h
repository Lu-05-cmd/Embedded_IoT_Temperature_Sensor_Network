#ifndef CONFIG_H
#define CONFIG_H

// ==================== CẤU HÌNH WIFI & MQTT ====================

// 1. Thông tin Wi-Fi (Cho file .ino kết nối mạng)
const char* ssid = "TEN_WIFI_CUA_BAN";
const char* password = "MAT_KHAU_WIFI_CUA_BAN";

// 2. Thông tin MQTT Broker (Chuẩn bảo mật HiveMQ Cloud)
#define MQTT_BROKER    "mqtts://xxxxx.s1.eu.hivemq.cloud:8883" // Thay bằng địa chỉ Broker của bạn (Giữ nguyên "mqtts://" và cổng 8883)
#define MQTT_USERNAME  "YOUR_USERNAME"                         // Thay bằng Username của bạn
#define MQTT_PASSWORD  "YOUR_PASSWORD"                         // Thay bằng Password của bạn
#define MQTT_TOPIC     "embedded_iot/temp_sensor/8c377539"

#endif // CONFIG_H
