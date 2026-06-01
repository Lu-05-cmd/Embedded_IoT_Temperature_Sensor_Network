#include <WiFi.h>
#include <ArduinoJson.h>

#include "config.h"

// Liên kết code C-style của MQTT ESP-IDF vào chương trình Arduino C++
extern "C" {
#include "mqtt.h"
}

// Cấu hình phần cứng ESP32
#define STM32_RX_PIN 16 // Kết nối tới chân PA9 (TX) của STM32
#define STM32_TX_PIN 17 // Kết nối tới chân PA10 (RX) của STM32
#define STM32_BAUDRATE 112500 // Khớp với tốc độ USART1 trên STM32 (112500)

void setup() {
    Serial.begin(115200);
    
    // Khởi chạy UART2 nhận dữ liệu từ STM32
    Serial2.begin(STM32_BAUDRATE, SERIAL_8N1, STM32_RX_PIN, STM32_TX_PIN);
    Serial.println("ESP32 Local Gateway (ESP-IDF MQTT Mode) đã khởi động.");

    // Kết nối Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Đang kết nối Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nĐã kết nối Wi-Fi thành công!");
    Serial.print("Địa chỉ IP ESP32: ");
    Serial.println(WiFi.localIP());

    // Khởi động tiến trình MQTT Client của ESP-IDF
    mqtt_app_start();
}

void loop() {
    // Đọc dữ liệu từ STM32 khi có tín hiệu UART
    if (Serial2.available() > 0) {
        String jsonStr = Serial2.readStringUntil('\n');
        jsonStr.trim();

        if (jsonStr.length() > 0) {
            Serial.print("Nhận từ STM32: ");
            Serial.println(jsonStr);

            // Kiểm tra tính hợp lệ của chuỗi JSON trước khi gửi lên MQTT
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, jsonStr);

            if (!error) {
                // Kiểm tra xem MQTT đã kết nối chưa
                if (mqtt_is_connected()) {
                    Serial.println("Đang chuyển tiếp dữ liệu lên MQTT Broker...");
                    mqtt_publish_sensor(jsonStr.c_str());
                } else {
                    Serial.println("Cảnh báo: MQTT chưa sẵn sàng kết nối!");
                }
            } else {
                Serial.print("Lỗi định dạng JSON nhận được: ");
                Serial.println(error.c_str());
            }
        }
    }
}
