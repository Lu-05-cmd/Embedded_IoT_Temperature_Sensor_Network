#include "stm32f103xb.h"
#include "../include/drivers/dht11.h"
#include "../include/drivers/systick.h"
#include "../include/drivers/timer.h"
#include "../include/drivers/gpio.h"

#define DHT11_PORT  GPIOB
#define DHT11_PIN   (1UL << 12)

uint8_t dht11_fail_step = 0;

/* =========================================================
 * GPIO
 * ========================================================= */

static inline void DHT11_SetOutput(void)
{
    DHT11_PORT->CRH &= ~(0xFUL << 16);
    DHT11_PORT->CRH |=  (0x7UL << 16);
}

static inline void DHT11_SetInput(void)
{
    DHT11_PORT->CRH &= ~(0xFUL << 16);
    DHT11_PORT->CRH |=  (0x8UL << 16);
    DHT11_PORT->BSRR  =  DHT11_PIN;
}

static inline void DHT11_Low(void)
{
    DHT11_PORT->BRR = DHT11_PIN;
}

static inline uint8_t DHT11_Read(void)
{
    return (DHT11_PORT->IDR & DHT11_PIN) ? 1u : 0u;
}

/* =========================================================
 * WAIT HELPERS
 * ========================================================= */

static uint8_t WaitLow(uint32_t timeout_us)
{
    while (DHT11_Read())
    {
        TIM2_DelayUs(1);
        if (--timeout_us == 0) return 0;
    }
    return 1;
}

static uint8_t WaitHigh(uint32_t timeout_us)
{
    while (!DHT11_Read())
    {
        TIM2_DelayUs(1);
        if (--timeout_us == 0) return 0;
    }
    return 1;
}

/* =========================================================
 * INIT
 * ========================================================= */

void DHT11_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    DHT11_SetInput();
    SysTick_DelayMs(1000);
}

static uint8_t DHT11_Start(void)
{
    DHT11_SetOutput();
    DHT11_Low();
    SysTick_DelayMs(20);
    DHT11_SetInput();


    if (DHT11_Read())
    {

        if (!WaitLow(300)) { dht11_fail_step = 1; return 0; }
    }
    else
    {
        if (!WaitHigh(200)) { dht11_fail_step = 2; return 0; }
        if (!WaitLow(200))  { dht11_fail_step = 3; return 0; }
    }

    return 1;
}

/* =========================================================
 * READ BIT
 * ========================================================= */
uint8_t DHT11_ReadBit(void)
{
    uint32_t timeout = 5000;

    while (DHT11_Read() == 0)
    {
        if (--timeout == 0) return 0;
        TIM2_DelayUs(1);
    }

    uint32_t count = 0;

    while (DHT11_Read())
    {
        TIM2_DelayUs(1);
        count++;
        if (count > 100) break; // tránh treo
    }

    // 3. phân loại bit
    // threshold ~40us
    return (count > 25) ? 1 : 0;
}

/* =========================================================
 * READ BYTE (MSB first)
 * ========================================================= */

static uint8_t DHT11_ReadByte(void)
{
    uint8_t data = 0;
    uint8_t i;

    for (i = 0; i < 8u; i++)
    {
        data <<= 1U;
        data  |= DHT11_ReadBit();
    }

    return data;
}

/* =========================================================
 * READ DATA
 * ========================================================= */

uint8_t DHT11_ReadData(uint8_t *temperature, uint8_t *humidity)
{
    uint8_t hum_int, hum_dec, temp_int, temp_dec, checksum;

    if (!temperature || !humidity) return 0;

    dht11_fail_step = 0;

    if (!DHT11_Start()) return 0;

    hum_int  = DHT11_ReadByte();
    hum_dec  = DHT11_ReadByte();
    temp_int = DHT11_ReadByte();
    temp_dec = DHT11_ReadByte();
    checksum = DHT11_ReadByte();

    DHT11_SetInput();

    // if ((uint8_t)(hum_int + hum_dec + temp_int + temp_dec) != checksum)
    // {
    //     dht11_fail_step = 0;
    //     return 0;
    // }

    *humidity    = hum_int;
    *temperature = temp_int;

    return 1;
}