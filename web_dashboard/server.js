const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');
const os = require('os');

// Hàm tự động lấy địa chỉ IP của máy tính trong mạng LAN
function getLocalIP() {
    const interfaces = os.networkInterfaces();
    for (const name of Object.keys(interfaces)) {
        for (const iface of interfaces[name]) {
            // Chỉ lấy địa chỉ IPv4 không phải là localhost (127.0.0.1)
            if (iface.family === 'IPv4' && !iface.internal) {
                return iface.address;
            }
        }
    }
    return '127.0.0.1';
}

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const PORT = process.env.PORT || 3000;

// Middleware
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// Mảng lưu trữ tối đa 100 bản ghi đo gần nhất để hiển thị biểu đồ lịch sử lúc load trang
let telemetryHistory = [];
const MAX_HISTORY_LEN = 100;

// API nhận dữ liệu từ ESP32 gửi lên
app.post('/api/telemetry', (req, res) => {
    const { temp, humi, dht, lcd, rgb, buzzer } = req.body;

    // Kiểm tra định dạng dữ liệu cơ bản
    if (temp === undefined || humi === undefined) {
        return res.status(400).json({ error: 'Thiếu dữ liệu nhiệt độ hoặc độ ẩm!' });
    }

    const dataPoint = {
        temp: Number(temp),
        humi: Number(humi),
        dht: dht !== undefined ? Number(dht) : 1,
        lcd: lcd !== undefined ? Number(lcd) : 1,
        rgb: rgb !== undefined ? Number(rgb) : 1,
        buzzer: buzzer !== undefined ? Number(buzzer) : 0,
        timestamp: new Date().toLocaleTimeString('vi-VN', { hour: '2-digit', minute: '2-digit', second: '2-digit' })
    };

    // Thêm vào lịch sử
    telemetryHistory.push(dataPoint);
    if (telemetryHistory.length > MAX_HISTORY_LEN) {
        telemetryHistory.shift();
    }

    // Phát (Broadcast) dữ liệu mới tới tất cả các client đang kết nối trang web qua WebSocket
    const msg = JSON.stringify({ type: 'NEW_DATA', data: dataPoint });
    wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(msg);
        }
    });

    console.log(`[ESP32 -> Server] Nhận dữ liệu: Temp=${temp}°C, Humi=${humi}%, Buzzer=${buzzer}`);
    res.status(202).json({ status: 'success', message: 'Đã nhận và phân phối dữ liệu.' });
});

// API lấy toàn bộ lịch sử (cho trang web load biểu đồ lúc đầu)
app.get('/api/telemetry', (req, res) => {
    res.json(telemetryHistory);
});

// Sự kiện kết nối WebSocket từ trang web
wss.on('connection', (ws) => {
    console.log('[Web Client] Kết nối WebSocket mới được thiết lập.');

    // Gửi lịch sử dữ liệu hiện tại ngay khi client kết nối
    ws.send(JSON.stringify({ type: 'HISTORY', data: telemetryHistory }));

    ws.on('close', () => {
        console.log('[Web Client] Ngắt kết nối WebSocket.');
    });
});

// Khởi động Server
server.listen(PORT, '0.0.0.0', () => {
    const localIP = getLocalIP();
    console.log('==================================================');
    console.log(`  MÁY CHỦ BẮT ĐẦU CHẠY THÀNH CÔNG!`);
    console.log(`  Giao diện Dashboard: http://localhost:${PORT}`);
    console.log(`  IP Local để ESP32 gửi dữ liệu: http://${localIP}:${PORT}/api/telemetry`);
    console.log('==================================================');
});
