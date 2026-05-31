#ifndef TIMER_H
#define TIMER_H


#include <stdio.h>
#include "stm32f103xb.h"

void TIM2_Init(void);
void TIM2_DelayUs(uint32_t us);

#endif /* TIMER */