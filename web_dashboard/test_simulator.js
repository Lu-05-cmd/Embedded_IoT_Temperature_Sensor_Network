const http = require('http');

console.log("==================================================");
console.log("   BẮT ĐẦU GIẢ LẬP ESP32 GATEWAY (SIMULATOR)");
console.log("   Gửi dữ liệu tới: http://localhost:3000/api/telemetry");
console.log("   Bấm Ctrl + C để dừng giả lập.");
console.log("==================================================");

let temp = 25.0;
let humi = 60.0;
let buzzer = 0;

setInterval(() => {
    // Tạo dữ liệu nhiệt độ & độ ẩm thay đổi ngẫu nhiên theo thời gian
    temp += (Math.random() - 0.5) * 1.5;
    humi += (Math.random() - 0.5) * 2.0;

    // Giới hạn giá trị thực tế
    if (temp < 15) temp = 15;
    if (temp > 40) temp = 40;
    if (humi < 30) humi = 30;
    if (humi > 90) humi = 90;

    // Còi kêu nếu nhiệt độ vượt quá 35°C (khớp logic code STM32)
    buzzer = temp > 35.0 ? 1 : 0;

    const payload = JSON.stringify({
        temp: Number(temp.toFixed(1)),
        humi: Number(humi.toFixed(1)),
        dht: 1,
        lcd: 1,
        rgb: 1,
        buzzer: buzzer
    });

    const options = {
        hostname: 'localhost',
        port: 3000,
        path: '/api/telemetry',
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Content-Length': Buffer.byteLength(payload)
        }
    };

    const req = http.request(options, (res) => {
        res.on('data', () => {});
    });

    req.on('error', (e) => {
        console.error(`[LỖI SIMULATOR] Không gửi được dữ liệu: ${e.message}`);
    });

    req.write(payload);
    req.end();

    console.log(`[SIMULATOR] Gửi thành công: Temp=${temp.toFixed(1)}°C, Humi=${humi.toFixed(1)}%, Buzzer=${buzzer}`);
}, 2000); // Gửi mỗi 2 giây
