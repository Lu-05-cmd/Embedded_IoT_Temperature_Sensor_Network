#ifndef USART_H
#define USART_H

#include "stm32f103xb.h"

void USART1_Init(uint32_t baudrate);

void USART1_SendChar(char c);
void USART1_SendString(const char *str);

void USART1_SendStatus(int temp,
                       int humi,
                       const char *env_status,
                       uint8_t env_level,
                       uint8_t dht_ok,
                       uint8_t lcd_ok,
                       uint8_t rgb_ok,
                       uint8_t buzzer_ok);

#endif
