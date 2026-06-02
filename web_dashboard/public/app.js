// Custom Dashboard Application Logic
let chart;
let socket;

const TEMP_WARNING_C = 32;
const TEMP_DANGER_C = 38;
const HUMI_WARNING_PERCENT = 75;
const HUMI_DANGER_PERCENT = 85;

const ENV_STATUS_META = {
    normal: {
        label: 'Bình thường',
        className: 'status-ok',
        pillClass: 'status-pill-success',
        buzzerText: 'Tắt'
    },
    warning: {
        label: 'Cảnh báo',
        className: 'status-warning',
        pillClass: 'status-pill-warning',
        buzzerText: 'Cảnh báo'
    },
    danger: {
        label: 'Nguy hiểm',
        className: 'status-active',
        pillClass: 'status-pill-error',
        buzzerText: 'Kêu nhanh'
    },
    sensor_error: {
        label: 'Lỗi DHT11',
        className: 'status-error',
        pillClass: 'status-pill-error',
        buzzerText: 'Lỗi sensor'
    }
};

function getEnvStatus(temp, humi) {
    const temperature = Number(temp);
    const humidity = Number(humi);

    if (temperature >= TEMP_DANGER_C || humidity >= HUMI_DANGER_PERCENT) {
        return 'danger';
    }

    if (temperature >= TEMP_WARNING_C || humidity >= HUMI_WARNING_PERCENT) {
        return 'warning';
    }

    return 'normal';
}

function normalizeEnvStatus(data) {
    if (data.env_status) {
        return String(data.env_status).toLowerCase();
    }

    if (data.env_level !== undefined && data.env_level !== null) {
        const level = Number(data.env_level);
        if (level === 3) return 'sensor_error';
        if (level === 2) return 'danger';
        if (level === 1) return 'warning';
    }

    return getEnvStatus(data.temp, data.humi);
}

function formatStatValue(value) {
    return value === undefined || value === null || Number(value) < 0 ? '--' : value;
}

function getEnvStatusInfo(data) {
    const key = normalizeEnvStatus(data);
    return ENV_STATUS_META[key] ? { key, ...ENV_STATUS_META[key] } : { key: 'normal', ...ENV_STATUS_META.normal };
}

function getAlertReason(data, statusKey) {
    if (statusKey === 'sensor_error') {
        return 'DHT11 lỗi hoặc mất tín hiệu';
    }

    const temp = Number(data.temp);
    const humi = Number(data.humi);
    const reasons = [];

    if (temp >= TEMP_DANGER_C) reasons.push('Nhiệt độ vượt ngưỡng nguy hiểm');
    else if (temp >= TEMP_WARNING_C) reasons.push('Nhiệt độ vượt ngưỡng cảnh báo');

    if (humi >= HUMI_DANGER_PERCENT) reasons.push('Độ ẩm vượt ngưỡng nguy hiểm');
    else if (humi >= HUMI_WARNING_PERCENT) reasons.push('Độ ẩm vượt ngưỡng cảnh báo');

    return reasons.length > 0 ? reasons.join(' + ') : 'Thông số trong ngưỡng an toàn';
}

function getChartValue(data, field) {
    const statusKey = normalizeEnvStatus(data);
    const value = Number(data[field]);
    return statusKey === 'sensor_error' || value < 0 || Number.isNaN(value) ? null : value;
}

function renderChartHistory(history) {
    chart.data.labels = history.map(d => d.timestamp);
    chart.data.datasets[0].data = history.map(d => getChartValue(d, 'temp'));
    chart.data.datasets[1].data = history.map(d => getChartValue(d, 'humi'));
    chart.update();
}

function appendChartPoint(dataPoint) {
    chart.data.labels.push(dataPoint.timestamp);
    chart.data.datasets[0].data.push(getChartValue(dataPoint, 'temp'));
    chart.data.datasets[1].data.push(getChartValue(dataPoint, 'humi'));

    if (chart.data.labels.length > 50) {
        chart.data.labels.shift();
        chart.data.datasets[0].data.shift();
        chart.data.datasets[1].data.shift();
    }

    chart.update();
}

// Khởi tạo đồ thị Chart.js
function initChart() {
    const ctx = document.getElementById('liveChart').getContext('2d');

    // Tạo gradient màu nền cho biểu đồ nhiệt độ
    const tempGradient = ctx.createLinearGradient(0, 0, 0, 350);
    tempGradient.addColorStop(0, 'rgba(37, 99, 235, 0.2)');
    tempGradient.addColorStop(1, 'rgba(37, 99, 235, 0.0)');

    // Tạo gradient màu nền cho biểu đồ độ ẩm
    const humiGradient = ctx.createLinearGradient(0, 0, 0, 350);
    humiGradient.addColorStop(0, 'rgba(14, 165, 233, 0.2)');
    humiGradient.addColorStop(1, 'rgba(14, 165, 233, 0.0)');

    chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                {
                    label: 'Nhiệt độ (°C)',
                    data: [],
                    borderColor: '#2563eb',
                    backgroundColor: tempGradient,
                    borderWidth: 3,
                    fill: true,
                    tension: 0.4,
                    pointRadius: 3,
                    pointHoverRadius: 6,
                    yAxisID: 'y-temp'
                },
                {
                    label: 'Độ ẩm (%)',
                    data: [],
                    borderColor: '#0ea5e9',
                    backgroundColor: humiGradient,
                    borderWidth: 3,
                    fill: true,
                    tension: 0.4,
                    pointRadius: 3,
                    pointHoverRadius: 6,
                    yAxisID: 'y-humi'
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            layout: {
                padding: {
                    left: 15,
                    right: 15,
                    top: 0,
                    bottom: 0
                }
            },
            interaction: {
                mode: 'index',
                intersect: false
            },
            plugins: {
                legend: {
                    display: true,
                    position: 'top',
                    labels: {
                        color: '#64748b',
                        font: {
                            family: 'Plus Jakarta Sans',
                            size: 12,
                            weight: '600'
                        },
                        padding: 20
                    }
                },
                tooltip: {
                    backgroundColor: '#ffffff',
                    titleColor: '#0f172a',
                    bodyColor: '#334155',
                    borderColor: 'rgba(148, 163, 184, 0.2)',
                    borderWidth: 1,
                    padding: 12,
                    boxPadding: 6,
                    titleFont: {
                        family: 'Plus Jakarta Sans',
                        weight: '700'
                    },
                    bodyFont: {
                        family: 'Plus Jakarta Sans'
                    }
                }
            },
            scales: {
                x: {
                    grid: {
                        color: 'rgba(148, 163, 184, 0.08)'
                    },
                    ticks: {
                        color: '#64748b',
                        font: {
                            family: 'Plus Jakarta Sans'
                        }
                    }
                },
                'y-temp': {
                    type: 'linear',
                    display: true,
                    position: 'left',
                    min: 0,
                    max: 80,
                    grid: {
                        color: 'rgba(148, 163, 184, 0.08)'
                    },
                    ticks: {
                        color: '#2563eb',
                        font: {
                            family: 'Plus Jakarta Sans',
                            weight: '600'
                        },
                        callback: function (value) { return value + ' °C'; }
                    }
                },
                'y-humi': {
                    type: 'linear',
                    display: true,
                    position: 'right',
                    min: 0,
                    max: 100,
                    grid: {
                        drawOnChartArea: false // Ẩn đường lưới để tránh rối mắt
                    },
                    ticks: {
                        color: '#0ea5e9',
                        font: {
                            family: 'Plus Jakarta Sans',
                            weight: '600'
                        },
                        callback: function (value) { return value + ' %'; }
                    }
                }
            }
        }
    });
}

// Cập nhật các thẻ giá trị trên Dashboard
function updateDashboardUI(data) {
    const { temp, humi, dht, lcd, rgb, buzzer } = data;
    const envStatusInfo = getEnvStatusInfo(data);
    const envStatus = envStatusInfo.key;
    const envReason = getAlertReason(data, envStatus);

    // 1. Cập nhật Nhiệt độ
    const tempVal = document.getElementById('val-temp');
    tempVal.innerText = envStatus === 'sensor_error' ? '--' : temp;
    const tempBar = document.getElementById('bar-temp');
    // Tính phần trăm thanh đo dựa trên khoảng nhiệt độ 0-50°C
    const tempPercent = envStatus === 'sensor_error'
        ? 0
        : Math.min(Math.max((Number(temp) / 50) * 100, 0), 100);
    tempBar.style.setProperty('--bar-width', `${tempPercent}%`);
    // Sử dụng JS để đổi chiều dài thanh trạng thái
    tempBar.style.width = `${tempPercent}%`;
    document.getElementById('val-temp-min').innerText = envStatus === 'sensor_error' ? '--' : formatStatValue(data.temp_min);
    document.getElementById('val-temp-max').innerText = envStatus === 'sensor_error' ? '--' : formatStatValue(data.temp_max);

    // 2. Cập nhật Độ ẩm
    const humiVal = document.getElementById('val-humi');
    humiVal.innerText = envStatus === 'sensor_error' ? '--' : humi;
    const humiBar = document.getElementById('bar-humi');
    const humiPercent = envStatus === 'sensor_error' ? 0 : Math.min(Math.max(Number(humi), 0), 100);
    humiBar.style.width = `${humiPercent}%`;
    document.getElementById('val-humi-min').innerText = envStatus === 'sensor_error' ? '--' : formatStatValue(data.humi_min);
    document.getElementById('val-humi-max').innerText = envStatus === 'sensor_error' ? '--' : formatStatValue(data.humi_max);

    const envCard = document.getElementById('card-env');
    document.getElementById('val-env-status').innerText = envStatusInfo.label;
    document.getElementById('val-env-reason').innerText = envReason;
    envCard.className = `stat-card env-card ${envStatusInfo.className}`;

    // 3. Cập nhật Còi Buzzer
    const buzzerCard = document.getElementById('card-buzzer');
    const buzzerVal = document.getElementById('val-buzzer');
    buzzerVal.innerText = envStatusInfo.buzzerText;
    buzzerCard.className = `stat-card buzzer-card ${envStatusInfo.className}`;

    // 4. Cập nhật Cảm biến DHT11
    const sensorCard = document.getElementById('card-sensor');
    const sensorVal = document.getElementById('val-sensor');
    if (dht === 1) {
        sensorVal.innerText = "Hoạt động";
        sensorCard.className = "stat-card sensor-card status-ok";
    } else {
        sensorVal.innerText = "LỖI ĐỌC SENSOR";
        sensorCard.className = "stat-card sensor-card status-error";
    }

    // 5. Cập nhật bảng chẩn đoán hệ thống (Diagnostics)
    updateEnvStatusPill(envStatusInfo);
    updateStatusPill('diag-dht', dht === 1);
    updateStatusPill('diag-lcd', lcd === 1);
    updateStatusPill('diag-rgb', rgb === 1);
    updateStatusPill('diag-buzzer', buzzer === 1, false, "OK");
}

function updateEnvStatusPill(statusInfo) {
    const el = document.getElementById('diag-env');
    if (!el) return;

    el.innerText = statusInfo.label;
    el.className = `diag-status ${statusInfo.pillClass}`;
}

function updateStatusPill(elementId, isOk, activeAsWarning = false, okText = "OK") {
    const el = document.getElementById(elementId);
    if (!el) return;

    if (activeAsWarning) {
        if (isOk) {
            el.innerText = "ĐANG CẢNH BÁO";
            el.className = "diag-status status-pill-error";
        } else {
            el.innerText = "Tắt";
            el.className = "diag-status status-pill-success";
        }
    } else {
        if (isOk) {
            el.innerText = okText;
            el.className = "diag-status status-pill-success";
        } else {
            el.innerText = "LỖI";
            el.className = "diag-status status-pill-error";
        }
    }
}

// Thiết lập kết nối WebSocket
function connectWebSocket() {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${window.location.host}`;

    const connStatus = document.getElementById('conn-status');
    const connText = document.getElementById('conn-text');

    socket = new WebSocket(wsUrl);

    socket.onopen = () => {
        console.log('Đã kết nối thành công tới WebSocket server.');
        connStatus.className = 'status-indicator online';
        connText.innerText = 'Đã kết nối';
    };

    socket.onmessage = (event) => {
        const message = JSON.parse(event.data);

        if (message.type === 'HISTORY') {
            // Nhận dữ liệu lịch sử khi vừa tải trang
            const history = message.data;
            renderChartHistory(history);

            if (history.length > 0) {
                updateDashboardUI(history[history.length - 1]);
            }
        }
        else if (message.type === 'NEW_DATA') {
            // Nhận dữ liệu mới thời gian thực
            const dataPoint = message.data;

            // Thêm vào biểu đồ
            appendChartPoint(dataPoint);

            // Giới hạn hiển thị tối đa 50 điểm trên biểu đồ
            updateDashboardUI(dataPoint);
        }
    };

    socket.onclose = () => {
        console.log('Mất kết nối WebSocket. Đang kết nối lại sau 3 giây...');
        connStatus.className = 'status-indicator offline';
        connText.innerText = 'Đang kết nối lại...';
        setTimeout(connectWebSocket, 3000);
    };

    socket.onerror = (error) => {
        console.error('Lỗi WebSocket:', error);
        socket.close();
    };
}

// Tải lịch sử ban đầu qua API GET HTTP phòng hờ
async function loadInitialHistory() {
    try {
        const response = await fetch('/api/telemetry');
        const history = await response.json();
        if (history && history.length > 0) {
            renderChartHistory(history);
            updateDashboardUI(history[history.length - 1]);
        }
    } catch (err) {
        console.log('Chưa lấy được lịch sử qua HTTP API, chờ WebSocket...');
    }
}

// Khởi chạy khi tải trang xong
window.addEventListener('DOMContentLoaded', () => {
    const buzzerLabel = document.querySelector('#card-buzzer .card-label');
    if (buzzerLabel) buzzerLabel.innerText = 'Còi cảnh báo';

    initChart();
    loadInitialHistory();
    connectWebSocket();
});
