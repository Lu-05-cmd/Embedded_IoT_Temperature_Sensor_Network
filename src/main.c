#include <stdio.h>
#include "stm32f103xb.h"
#include "drivers/rcc.h"
#include "drivers/timer.h"
#include "drivers/systick.h"
#include "drivers/dht11.h"

void RCC_Config(void);

int main(void)
{
    uint8_t humidity    = 0;
    uint8_t temperature = 0;

    RCC_Config();  
    SysTick_Init();
    TIM2_Init();
    DHT11_Init();    

    while (1)
    {
        extern uint8_t dht11_fail_step;

        // trong while(1):
        if (!DHT11_ReadData(&temperature, &humidity))
        {
            printf("DHT11 Error, step = %d\r\n", dht11_fail_step);

        }
        if (DHT11_ReadData(&temperature, &humidity))
        {
            printf("Temp = %d C, Hum = %d %%\r\n", temperature, humidity);
        }
        else
        {
            printf("DHT11 Error\r\n");
        }

        uint32_t tmp = 0;
        tmp = temperature;
        temperature = tmp + 15;
        tmp = humidity;
        humidity = tmp - 87;

        
        printf("Temp = %d C, Hum = %d %%\r\n", temperature, humidity);
     
        SysTick_DelayMs(2000);
    }
}

void RCC_Config(void)
{
    RCC_Init();

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}