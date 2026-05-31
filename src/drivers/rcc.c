#include "drivers/rcc.h"
#include "stm32f103xb.h"

void RCC_Init(void)
{
    /* Enable HSE */
    RCC->CR |= RCC_CR_HSEON;

    /* Wait HSE ready */
    while ((RCC->CR & RCC_CR_HSERDY) == 0);

    /* Flash latency for 72MHz */
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    /* AHB = SYSCLK */
    RCC->CFGR &= ~RCC_CFGR_HPRE;

    /* APB1 = HCLK / 2 */
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    /* APB2 = HCLK */
    RCC->CFGR &= ~RCC_CFGR_PPRE2;

    /* PLL source = HSE */
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLSRC;

    /* PLL x9 -> 72MHz */
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait PLL ready */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0);

    /* Switch SYSCLK to PLL */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait switch complete */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

}



void RCC_EnableTim2(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}
