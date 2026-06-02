const mqtt = require('mqtt');

const MQTT_BROKER = 'mqtt://broker.hivemq.com';
const MQTT_TOPIC = 'embedded_iot/temp_sensor/8c377539';

console.log("==================================================");
console.log("   BẮT ĐẦU GIẢ LẬP ESP32 GATEWAY QUA MQTT");
console.log(`   Broker: ${MQTT_BROKER}`);
console.log(`   Topic: ${MQTT_TOPIC}`);
console.log("   Bấm Ctrl + C để dừng giả lập.");
console.log("==================================================");

const client = mqtt.connect(MQTT_BROKER);

let temp = 25.0;
let humi = 60.0;
let buzzer = 0;
let tempMin = temp;
let tempMax = temp;
let humiMin = humi;
let humiMax = humi;

function getEnvStatus(temp, humi) {
    if (temp >= 38 || humi >= 85) {
        return { text: 'DANGER', level: 2 };
    }

    if (temp >= 32 || humi >= 75) {
        return { text: 'WARNING', level: 1 };
    }

    return { text: 'NORMAL', level: 0 };
}

client.on('connect', () => {
    console.log("[SIMULATOR] Đã kết nối thành công tới MQTT Broker.");
    
    // Gửi dữ liệu định kỳ mỗi 2 giây
    setInterval(() => {
        const sensorError = Math.random() < 0.05;

        // Tạo dữ liệu nhiệt độ & độ ẩm thay đổi ngẫu nhiên theo thời gian
        temp += (Math.random() - 0.5) * 1.5;
        humi += (Math.random() - 0.5) * 2.0;

        // Giới hạn giá trị thực tế
        if (temp < 15) temp = 15;
        if (temp > 40) temp = 40;
        if (humi < 30) humi = 30;
        if (humi > 90) humi = 90;

        const envStatus = sensorError ? { text: 'SENSOR_ERROR', level: 3 } : getEnvStatus(temp, humi);
        if (!sensorError) {
            tempMin = Math.min(tempMin, temp);
            tempMax = Math.max(tempMax, temp);
            humiMin = Math.min(humiMin, humi);
            humiMax = Math.max(humiMax, humi);
        }
        buzzer = envStatus.level !== 0 ? 1 : 0;

        const payload = JSON.stringify({
            temp: sensorError ? -1 : Number(temp.toFixed(1)),
            humi: sensorError ? -1 : Number(humi.toFixed(1)),
            temp_min: Number(tempMin.toFixed(1)),
            temp_max: Number(tempMax.toFixed(1)),
            humi_min: Number(humiMin.toFixed(1)),
            humi_max: Number(humiMax.toFixed(1)),
            env_status: envStatus.text,
            env_level: envStatus.level,
            dht: sensorError ? 0 : 1,
            lcd: 1,
            rgb: 1,
            buzzer: buzzer
        });

        client.publish(MQTT_TOPIC, payload, (err) => {
            if (!err) {
                console.log(`[SIMULATOR] Publish thành công: Temp=${temp.toFixed(1)}°C, Humi=${humi.toFixed(1)}%, Buzzer=${buzzer}`);
            } else {
                console.error(`[LỖI SIMULATOR] Lỗi khi publish: ${err.message}`);
            }
        });

    }, 2000);
});

client.on('error', (err) => {
    console.error(`[LỖI SIMULATOR] Lỗi kết nối MQTT: ${err.message}`);
});
