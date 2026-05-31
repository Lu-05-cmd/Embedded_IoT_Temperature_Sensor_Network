#include <stdio.h>
#include "stm32f103xb.h"

#include "../include/drivers/rcc.h"
#include "../include/drivers/timer.h"
#include "../include/drivers/systick.h"
#include "../include/drivers/dht11.h"
#include "../include/drivers/rgb.h"
#include "../include/drivers/buzzer.h"
#include "../include/drivers/sensor.h"
#include "../include/drivers/usart.h"


void RCC_Config(void);

int main(void)
{
    uint8_t humidity = 0;
    uint8_t temperature = 0;

    uint8_t temp_offset = 10;
    uint8_t hum_offset  = 87;

    RCC_Config();

    SysTick_Init();
    TIM2_Init();

    DHT11_Init();
    RGB_Init();
    Buzzer_Init();   // nếu có

    while (1)
    {
        extern uint8_t dht11_fail_step;

        if (DHT11_ReadData(&temperature, &humidity))
        {
            /* ===== APPLY OFFSET DEBUG ===== */
            int temp_dbg = (int)temperature + temp_offset;
            int hum_dbg  = (int)humidity - hum_offset;

            printf("Temp(raw=%d, dbg=%d) | Hum(raw=%d, dbg=%d)\r\n",
                   temperature, temp_dbg,
                   humidity, hum_dbg);

            /* ===== RGB theo nhiệt độ debug ===== */
            RGB_UpdateByTemp((uint8_t)temp_dbg);

            /* ===== BUZZER ===== */
            if (temp_dbg > 35)
                Buzzer_On();
            else
                Buzzer_Off();
            // send data to esp32
            Sensor_SendData(temp_dbg, hum_dbg);
        }
        else
        {
            printf("DHT11 Error, step = %d\r\n", dht11_fail_step);

            /* Error state */
            RGB_Set(1, 0, 0);  // đỏ báo lỗi
            Buzzer_Off();
        }

        SysTick_DelayMs(1000);
    }
}



/* ===== RCC CONFIG ===== */
void RCC_Config(void)
{
    RCC_Init();

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   // TIM2
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;   // GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;   // AFIO
}