const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');
const os = require('os');
const mqtt = require('mqtt');

// Load tệp cấu hình config.json
const config = require('./config.json');
const PORT = config.PORT || 3000;
const MAX_HISTORY_LEN = config.MAX_HISTORY_LEN || 100;
const MQTT_BROKER = config.MQTT_BROKER || 'mqtt://broker.hivemq.com';
const MQTT_TOPIC = config.MQTT_TOPIC || 'embedded_iot/temp_sensor/8c377539';

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

// Thiết lập cấu hình kết nối MQTT (Hỗ trợ Username/Password cho HiveMQ Cloud)
const mqttOptions = {};
if (config.MQTT_USERNAME) {
    mqttOptions.username = config.MQTT_USERNAME;
}
if (config.MQTT_PASSWORD) {
    mqttOptions.password = config.MQTT_PASSWORD;
}

const mqttClient = mqtt.connect(MQTT_BROKER, mqttOptions);

mqttClient.on('connect', () => {
    console.log(`[MQTT Client] Kết nối thành công tới Broker: ${MQTT_BROKER}`);
    mqttClient.subscribe(MQTT_TOPIC, (err) => {
        if (!err) {
            console.log(`[MQTT Client] Đã subscribe thành công topic: ${MQTT_TOPIC}`);
        } else {
            console.error('[MQTT Client] Lỗi khi subscribe topic:', err);
        }
    });
});

mqttClient.on('message', (topic, message) => {
    if (topic === MQTT_TOPIC) {
        try {
            const payload = JSON.parse(message.toString());
            const {
                temp,
                humi,
                temp_min,
                temp_max,
                humi_min,
                humi_max,
                env_status,
                env_level,
                dht,
                lcd,
                rgb,
                buzzer
            } = payload;
            
            if (temp === undefined || humi === undefined) return;

            const dataPoint = {
                temp: Number(temp),
                humi: Number(humi),
                temp_min: temp_min !== undefined ? Number(temp_min) : null,
                temp_max: temp_max !== undefined ? Number(temp_max) : null,
                humi_min: humi_min !== undefined ? Number(humi_min) : null,
                humi_max: humi_max !== undefined ? Number(humi_max) : null,
                env_status: env_status || null,
                env_level: env_level !== undefined ? Number(env_level) : null,
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

            // Phát tới các WebSocket client
            const msg = JSON.stringify({ type: 'NEW_DATA', data: dataPoint });
            wss.clients.forEach(client => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(msg);
                }
            });

            console.log(`[MQTT -> Server] Nhận dữ liệu: Temp=${temp}°C, Humi=${humi}%, Buzzer=${buzzer}`);
        } catch (e) {
            console.error('[MQTT Client] Lỗi khi xử lý tin nhắn:', e.message);
        }
    }
});

// Middleware
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// Mảng lưu trữ tối đa 100 bản ghi đo gần nhất để hiển thị biểu đồ lịch sử lúc load trang
let telemetryHistory = [];

// API nhận dữ liệu từ ESP32 gửi lên
app.post('/api/telemetry', (req, res) => {
    const { temp, humi, temp_min, temp_max, humi_min, humi_max, env_status, env_level, dht, lcd, rgb, buzzer } = req.body;

    // Kiểm tra định dạng dữ liệu cơ bản
    if (temp === undefined || humi === undefined) {
        return res.status(400).json({ error: 'Thiếu dữ liệu nhiệt độ hoặc độ ẩm!' });
    }

    const dataPoint = {
        temp: Number(temp),
        humi: Number(humi),
        temp_min: temp_min !== undefined ? Number(temp_min) : null,
        temp_max: temp_max !== undefined ? Number(temp_max) : null,
        humi_min: humi_min !== undefined ? Number(humi_min) : null,
        humi_max: humi_max !== undefined ? Number(humi_max) : null,
        env_status: env_status || null,
        env_level: env_level !== undefined ? Number(env_level) : null,
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
