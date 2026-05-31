#include "../include/drivers/buzzer.h"

#define BUZZER_PIN (1U << 14)

void Buzzer_Init(void)
{
    // Enable clock GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // PB14 output push-pull 2MHz
    GPIOB->CRH &= ~(0xF << 24);
    GPIOB->CRH |=  (0x2 << 24);

    // OFF mặc định
    Buzzer_Off();
}

void Buzzer_On(void)
{
    GPIOB->BRR = BUZZER_PIN;   // ACTIVE LOW
}

void Buzzer_Off(void)
{
    GPIOB->BSRR = BUZZER_PIN;
}