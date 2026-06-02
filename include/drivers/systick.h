#ifndef SYSTICK_H
#define SYSTICK_H
#include <stdint.h>
void SysTick_Init(void);
void SysTick_DelayMs(uint32_t ms);
uint32_t SysTick_GetMs(void);
#endif // SYSTICK_H
