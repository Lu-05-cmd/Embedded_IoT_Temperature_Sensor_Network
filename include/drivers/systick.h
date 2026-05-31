#ifndef SYSTICK_H
#define SYSTICK_H
#include <stdint.h>
void SysTick_Init(void);
void SysTick_DelayMs(uint32_t ms);
#endif // SYSTICK_H
