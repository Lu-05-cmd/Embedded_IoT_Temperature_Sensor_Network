#include "../include/drivers/usart.h"
#include "stm32f103xb.h"
#include <stdio.h>

void USART1_Init(uint32_t baudrate)
{
    AFIO->MAPR &= ~AFIO_MAPR_USART1_REMAP;

    // PA9 TX
    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |=  (0xB << 4);

    // PA10 RX
    GPIOA->CRH &= ~(0xF << 8);
    GPIOA->CRH |=  (0x4 << 8);

    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;

    USART1->SR = 0;

    USART1->BRR = 72000000 / baudrate;

    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
}

void USART1_SendChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE));

    USART1->DR = c;
}

void USART1_SendString(const char *str)
{
    while (*str)
    {
        USART1_SendChar(*str++);
    }
}

void USART1_SendStatus(int temp,
                       int humi,
                       int temp_min,
                       int temp_max,
                       int humi_min,
                       int humi_max,
                       const char *env_status,
                       uint8_t env_level,
                       uint8_t dht_ok,
                       uint8_t lcd_ok,
                       uint8_t rgb_ok,
                       uint8_t buzzer_ok)
{
    char buffer[192];

    sprintf(buffer,
            "{\"device\":\"stm32_01\","
            "\"temp\":%d,"
            "\"humi\":%d,"
            "\"temp_min\":%d,"
            "\"temp_max\":%d,"
            "\"humi_min\":%d,"
            "\"humi_max\":%d,"
            "\"env_status\":\"%s\","
            "\"env_level\":%d,"
            "\"dht\":%d,"
            "\"lcd\":%d,"
            "\"rgb\":%d,"
            "\"buzzer\":%d}\r\n",
            temp,
            humi,
            temp_min,
            temp_max,
            humi_min,
            humi_max,
            env_status,
            env_level,
            dht_ok,
            lcd_ok,
            rgb_ok,
            buzzer_ok);

    USART1_SendString(buffer);
}
