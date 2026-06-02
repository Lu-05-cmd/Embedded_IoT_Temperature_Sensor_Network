#include "drivers/systick.h"
#include "stm32f103xb.h"

volatile uint32_t g_msTicks = 0;

void SysTick_Handler(void)
{
    g_msTicks++;
}

void SysTick_Init(void)
{
    SystemCoreClockUpdate();

    SysTick->LOAD = (SystemCoreClock / 1000) - 1;  
    SysTick->VAL = 0;

    SysTick->CTRL =
        SysTick_CTRL_CLKSOURCE_Msk |
        SysTick_CTRL_TICKINT_Msk   |
        SysTick_CTRL_ENABLE_Msk;
}

void SysTick_DelayMs(uint32_t ms)
{
    uint32_t start = g_msTicks;
    while ((g_msTicks - start) < ms);
}

uint32_t SysTick_GetMs(void)
{
    return g_msTicks;
}
