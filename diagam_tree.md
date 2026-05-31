STM32_DHT11_RGB_MONITOR/
│
├── platformio.ini
│
├── include/
│   │
│   ├── stm32f103xx.h
│   │
│   ├── config/
│   │   ├── system_config.h
│   │   ├── pin_config.h
│   │   └── clock_config.h
│   │
│   ├── drivers/
│   │   ├── gpio.h
│   │   ├── systick.h
│   │   ├── dht11.h
│   │   └── rgb_led.h
│   │
│   ├── middleware/
│   │   ├── logger.h
│   │   └── temperature_manager.h
│   │
│   └── app/
│       └── app.h
│
├── src/
│   │
│   ├── main.c
│   │
│   ├── system/
│   │   ├── system_stm32f1xx.c
│   │   ├── clock_config.c
│   │   └── startup_stm32f103xb.s
│   │
│   ├── drivers/
│   │   ├── gpio.c
│   │   ├── systick.c
│   │   ├── dht11.c
│   │   └── rgb_led.c
│   │
│   ├── middleware/
│   │   ├── logger.c
│   │   └── temperature_manager.c
│   │
│   └── app/
│       └── app.c
│
├── lib/
│
├── test/
│
└── docs/
    ├── schematic.pdf
    ├── pinout.png
    └── project_report.md