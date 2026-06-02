const http = require('http');

const API_HOST = '127.0.0.1';
const API_PORT = 3000;
const API_PATH = '/api/telemetry';

const scenarios = [
    { temp: 25, humi: 60, env_status: 'NORMAL', env_level: 0, dht: 1 },
    { temp: 25, humi: 79, env_status: 'WARNING', env_level: 1, dht: 1 },
    { temp: 39, humi: 70, env_status: 'DANGER', env_level: 2, dht: 1 },
    { temp: -1, humi: -1, env_status: 'SENSOR_ERROR', env_level: 3, dht: 0 }
];

let tempMin = 25;
let tempMax = 25;
let humiMin = 60;
let humiMax = 60;
let index = 0;

function updateStats(payload) {
    if (payload.dht !== 1) return payload;

    tempMin = Math.min(tempMin, payload.temp);
    tempMax = Math.max(tempMax, payload.temp);
    humiMin = Math.min(humiMin, payload.humi);
    humiMax = Math.max(humiMax, payload.humi);

    return {
        ...payload,
        temp_min: tempMin,
        temp_max: tempMax,
        humi_min: humiMin,
        humi_max: humiMax
    };
}

function postTelemetry(payload) {
    const body = JSON.stringify({
        device: 'local_sim',
        lcd: 1,
        rgb: 1,
        buzzer: payload.env_level === 0 ? 0 : 1,
        temp_min: tempMin,
        temp_max: tempMax,
        humi_min: humiMin,
        humi_max: humiMax,
        ...payload
    });

    const req = http.request({
        hostname: API_HOST,
        port: API_PORT,
        path: API_PATH,
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Content-Length': Buffer.byteLength(body)
        }
    }, (res) => {
        res.resume();
        console.log(`[LOCAL SIM] ${payload.env_status} temp=${payload.temp} humi=${payload.humi} status=${res.statusCode}`);
    });

    req.on('error', (err) => {
        console.error(`[LOCAL SIM] Cannot reach http://${API_HOST}:${API_PORT}${API_PATH}: ${err.message}`);
    });

    req.write(body);
    req.end();
}

console.log(`Local telemetry simulator posting to http://${API_HOST}:${API_PORT}${API_PATH}`);
console.log('Run `npm start` in another terminal first.');

setInterval(() => {
    const payload = updateStats({ ...scenarios[index] });
    postTelemetry(payload);
    index = (index + 1) % scenarios.length;
}, 2000);
