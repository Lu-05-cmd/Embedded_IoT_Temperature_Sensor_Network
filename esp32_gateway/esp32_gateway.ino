#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "config.h"

// Chuỗi API endpoint
String server_url;

// Cấu hình phần cứng ESP32
#define STM32_RX_PIN 16 // Kết nối tới chân PA9 (TX) của STM32
#define STM32_TX_PIN 17 // Kết nối tới chân PA10 (RX) của STM32
#define STM32_BAUDRATE 112500 // Khớp với tốc độ USART1 trên STM32 (112500)

void setup() {
    Serial.begin(115200);
    
    // Khởi chạy UART2 nhận dữ liệu từ STM32
    Serial2.begin(STM32_BAUDRATE, SERIAL_8N1, STM32_RX_PIN, STM32_TX_PIN);
    Serial.println("ESP32 Local Gateway đã khởi động.");

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

    // Cấu hình URL server nhận dữ liệu
    server_url = "http://" + String(server_ip) + ":" + String(server_port) + "/api/telemetry";
    Serial.print("Server URL: ");
    Serial.println(server_url);
}

void sendToLocalServer(int temp, int humi, int dht, int lcd, int rgb, int buzzer) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Lỗi: Mất kết nối Wi-Fi!");
        return;
    }

    WiFiClient client;
    HTTPClient http;
    http.begin(client, server_url);
    
    http.addHeader("Content-Type", "application/json");

    // Tạo payload JSON gửi lên server local
    StaticJsonDocument<256> doc;
    doc["temp"] = temp;
    doc["humi"] = humi;
    doc["dht"] = dht;
    doc["lcd"] = lcd;
    doc["rgb"] = rgb;
    doc["buzzer"] = buzzer;

    String requestBody;
    serializeJson(doc, requestBody);
    
    Serial.println("Đang gửi dữ liệu lên Local Server...");
    Serial.println(requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
        Serial.print("Gửi thành công, Server phản hồi code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println(response);
    } else {
        Serial.print("Lỗi khi gửi POST: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    http.end();
}

void loop() {
    // Đọc dữ liệu từ STM32 khi có tín hiệu UART
    if (Serial2.available() > 0) {
        String jsonStr = Serial2.readStringUntil('\n');
        jsonStr.trim();

        if (jsonStr.length() > 0) {
            Serial.print("Nhận từ STM32: ");
            Serial.println(jsonStr);

            // Phân tích chuỗi JSON nhận được từ STM32
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, jsonStr);

            if (!error) {
                int temp = doc["temp"];
                int humi = doc["humi"];
                int dht = doc["dht"];
                int lcd = doc["lcd"];
                int rgb = doc["rgb"];
                int buzzer = doc["buzzer"];

                // Gửi lên Local Server
                sendToLocalServer(temp, humi, dht, lcd, rgb, buzzer);
            } else {
                Serial.print("Lỗi giải mã JSON: ");
                Serial.println(error.c_str());
            }
        }
    }
}
