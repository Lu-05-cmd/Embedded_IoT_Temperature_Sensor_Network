#ifndef CONFIG_H
#define CONFIG_H

// ==================== CẤU HÌNH WIFI & MQTT ====================

// 1. Thông tin Wi-Fi (Cho file .ino kết nối mạng)
const char *ssid = "Trai Loan";
const char *password = "28081978";

// 2. Thông tin MQTT Broker (Chuẩn bảo mật HiveMQ Cloud)
#define MQTT_BROKER "mqtts://a4e09e11da3946148c48fb757ee3b8f5.s1.eu.hivemq.cloud:8883" // Thay bằng địa chỉ Broker của bạn (Giữ nguyên "mqtts://" và cổng 8883)
#define MQTT_USERNAME "hivemq.webclient.1776624757608"                                 // Thay bằng Username của bạn
#define MQTT_PASSWORD "3n9<ITaLk,z&oE7j?B2H"                                           // Thay bằng Password của bạn
#define MQTT_TOPIC "device/esp32_01/sensor"

#endif // CONFIG_H
