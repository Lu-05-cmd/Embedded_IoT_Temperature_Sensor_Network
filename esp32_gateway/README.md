# Hướng dẫn nạp, cấu hình ESP32 và chạy Local Web Dashboard (ESP-IDF MQTT)

Mục này hướng dẫn bạn cách chạy Website Dashboard tự lập trình trên máy tính (Localhost) để giám sát nhiệt độ, độ ẩm từ STM32 gửi qua ESP32 thông qua giao thức **MQTT** sử dụng thư viện kết nối gốc **ESP-IDF MQTT Client**.

---

## 1. Cài đặt thư viện trên ESP32

Bạn chỉ cần cài đặt thư viện giải mã JSON trên **Arduino IDE** (hoặc PlatformIO):
* Vào mục **Library Manager** (Quản lý thư viện).
* Tìm kiếm và cài đặt thư viện: **`ArduinoJson`** (bởi Benoit Blanchon) - khuyến nghị chọn phiên bản **6.x** hoặc **7.x**.
* *Lưu ý: Không cần cài thêm thư viện MQTT ngoài vì code sử dụng trực tiếp driver `mqtt_client.h` tích hợp sẵn trong nhân Core của ESP32.*

---

## 2. Cấu hình thông số trên ESP32

Mở file `esp32_gateway/config.h` và thay đổi các giá trị sau:

```cpp
// 1. Nhập tên và mật khẩu Wi-Fi của bạn
const char* ssid = "TEN_WIFI_CUA_BAN";
const char* password = "MAT_KHAU_WIFI_CUA_BAN";

// 2. Thông tin MQTT Broker (Cấu hình bảo mật HiveMQ Cloud)
#define MQTT_BROKER    "mqtts://xxxxx.s1.eu.hivemq.cloud:8883" // Thay bằng địa chỉ Broker của bạn (Giữ nguyên "mqtts://" và cổng 8883)
#define MQTT_USERNAME  "YOUR_USERNAME"                         // Thay bằng Username của bạn
#define MQTT_PASSWORD  "YOUR_PASSWORD"                         // Thay bằng Password của bạn
#define MQTT_TOPIC     "embedded_iot/temp_sensor/8c377539"
```

---

## 3. Sơ đồ kết nối dây (Wiring)

| STM32F103 (Blue Pill) | ESP32 Board | Ghi chú |
| :--- | :--- | :--- |
| **PA9 (TX1)** | **GPIO16 (RX2)** | STM32 gửi dữ liệu $\rightarrow$ ESP32 nhận |
| **PA10 (RX1)** | **GPIO17 (TX2)** | ESP32 gửi dữ liệu $\rightarrow$ STM32 nhận |
| **GND** | **GND** | **Bắt buộc** nối chung Mass |

---

## 4. Cách khởi động Local Web Dashboard

Đảm bảo bạn đã cài đặt **Node.js** trên máy tính. Sau đó:

1. Mở terminal (CMD hoặc PowerShell) tại thư mục **`web_dashboard`**:
   ```bash
   cd web_dashboard
   ```
2. Cài đặt các thư viện cần thiết:
   ```bash
   npm install
   ```
3. Khởi động Web Server:
   ```bash
   npm start
   ```
4. Mở trình duyệt web của bạn và truy cập địa chỉ: **`http://localhost:3000`** để xem giao diện Dashboard.
5. Bật nguồn cho STM32 và ESP32. Dữ liệu nhiệt độ và độ ẩm sẽ tự động truyền tải qua MQTT Broker lên máy chủ cục bộ và cập nhật thời gian thực trên trang web mà không cần tải lại trang.
