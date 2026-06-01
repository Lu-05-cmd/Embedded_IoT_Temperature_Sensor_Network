#ifndef LCD_H
#define LCD_H

#include "stm32f103xb.h"
#include <stdint.h>

void LCD_Init(void);
void LCD_Clear(void);

void LCD_SetCursor(uint8_t row, uint8_t col);

void LCD_SendChar(char c);
void LCD_SendString(const char *str);

#endif