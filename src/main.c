#include <stdio.h>
#include "stm32f103xb.h"

#include "../include/drivers/rcc.h"
#include "../include/drivers/timer.h"
#include "../include/drivers/systick.h"
#include "../include/drivers/dht11.h"
#include "../include/drivers/rgb.h"
#include "../include/drivers/buzzer.h"
#include "../include/drivers/usart.h"
#include "../include/drivers/lcd.h"

void RCC_Config(void);

int main(void)
{
    uint8_t humidity = 0;
    uint8_t temperature = 0;

    uint8_t temp_offset = 10;
    uint8_t hum_offset  = 67;

    RCC_Config();

    SysTick_Init();
    TIM2_Init();

    DHT11_Init();
    RGB_Init();
    Buzzer_Init();   // nếu có
    USART1_Init(112500);
    LCD_Init();

    
    while (1)
    {
        extern uint8_t dht11_fail_step;
        uint8_t dht_ok;
        uint8_t lcd_ok = 1;
        uint8_t rgb_ok = 1;
        uint8_t buzzer_ok = 1;

        if (DHT11_ReadData(&temperature, &humidity))
        {
            dht_ok = 1;
            /* ===== APPLY OFFSET DEBUG ===== */
            int temp_dbg = (int)temperature + temp_offset;
            int hum_dbg  = (int)humidity - hum_offset;

            char line[17];

            LCD_Clear();

            LCD_SetCursor(0, 0);
            snprintf(line, sizeof(line), "Temp:%2d C", temp_dbg);
            LCD_SendString(line);

            LCD_SetCursor(1, 0);
            snprintf(line, sizeof(line), "Humi:%2d %%", hum_dbg);
            LCD_SendString(line);
            printf("Temp(raw=%d, dbg=%d) | Hum(raw=%d, dbg=%d)\r\n",
                   temperature, temp_dbg,
                   humidity, hum_dbg);

            /* ===== RGB theo nhiệt độ debug ===== */
            RGB_UpdateByTemp((uint8_t)temp_dbg);
            
            /* ===== BUZZER ===== */
            if (temp_dbg > 35)  Buzzer_On();
            else   Buzzer_Off();

            USART1_SendStatus(
            temp_dbg,
            hum_dbg,
            dht_ok,
            lcd_ok,
            rgb_ok,
            buzzer_ok
        );
        }
        else
        {
            printf("DHT11 Error, step = %d\r\n", dht11_fail_step);
            dht_ok = 0;
            /* Error state */
            // RGB_Set(0, 1, 0);  // đỏ báo lỗi
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
    RCC->APB2ENR |= (1u << 0);   // AFIO
    RCC->APB2ENR |= (1u << 14);
}