#include "stm32f103xb.h"
#include "../include/drivers/timer.h"


void TIM2_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->CR1  &= ~TIM_CR1_CEN;

    TIM2->PSC   = 72 - 1;
    TIM2->ARR   = 0xFFFFFFFF;
    TIM2->CNT   = 0;

    TIM2->EGR   = TIM_EGR_UG;  
    TIM2->SR    = 0;

    TIM2->CR1  |= TIM_CR1_CEN;
}


void TIM2_DelayUs(uint32_t us)
{
    uint32_t start = TIM2->CNT;
    while ((TIM2->CNT - start) < us);
}