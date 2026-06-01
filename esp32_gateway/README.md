# Hướng dẫn nạp, cấu hình ESP32 và chạy Local Web Dashboard

Mục này hướng dẫn bạn cách chạy Website Dashboard tự lập trình trên máy tính (Localhost) để giám sát nhiệt độ, độ ẩm từ STM32 gửi qua ESP32.

---

## 1. Cài đặt thư viện trên ESP32

Bạn cần cài đặt thư viện giải mã JSON trên **Arduino IDE** (hoặc PlatformIO):
* Vào mục **Library Manager** (Quản lý thư viện).
* Tìm kiếm và cài đặt thư viện: **`ArduinoJson`** (bởi Benoit Blanchon) - khuyến nghị chọn phiên bản **6.x** hoặc **7.x**.

---

## 2. Cấu hình thông số trên ESP32

Mở file `esp32_gateway.ino`, tìm mục `CẤU HÌNH CỦA BẠN` ở đầu trang và thay đổi các giá trị sau:

```cpp
// 1. Nhập tên và mật khẩu Wi-Fi của bạn
const char* ssid = "TEN_WIFI_CUA_BAN";
const char* password = "MAT_KHAU_WIFI_CUA_BAN";

// 2. Điền đúng địa chỉ IP máy tính chạy server Node.js của bạn
// Bạn có thể xem IP máy tính bằng cách gõ `ipconfig` trong CMD/PowerShell (dòng IPv4 Address)
const char* server_ip = "192.168.1.X"; 
const int server_port = 3000;
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
   Bạn sẽ thấy terminal thông báo:
   ```text
   ==================================================
     MÁY CHỦ BẮT ĐẦU CHẠY THÀNH CÔNG!
     Giao diện Dashboard: http://localhost:3000
     IP Local để ESP32 gửi dữ liệu: http://<IP_MÁY_TÍNH>:3000/api/telemetry
   ==================================================
   ```
4. Mở trình duyệt web của bạn và truy cập địa chỉ: **`http://localhost:3000`** để xem giao diện Dashboard.
5. Bật nguồn cho STM32 và ESP32. Dữ liệu nhiệt độ và độ ẩm sẽ tự động cập nhật và vẽ biểu đồ thời gian thực trên trang web mà không cần tải lại trang.
