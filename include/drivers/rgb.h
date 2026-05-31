#ifndef RGB_H
#define RGB_H

#include "stm32f103xb.h"

void RGB_Init(void);
void RGB_Set(uint8_t r, uint8_t g, uint8_t b);
void RGB_Off(void);
void RGB_UpdateByTemp(uint8_t temp);

#endif