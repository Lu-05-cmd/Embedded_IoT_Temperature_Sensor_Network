#include "../include/drivers/buzzer.h"
#include "stm32f103xb.h"

#define BUZZER_PIN (1U << 14)

/* ===== PWM CONFIG ===== */
static volatile uint16_t pwm_counter = 0;
static volatile uint16_t pwm_duty = 0;
static volatile uint16_t pwm_period = 0;
static volatile uint8_t buzzer_enable = 0;

/* ===== GPIO INIT ===== */
void Buzzer_Init(void)
{
    /* Enable clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* PB14 output push-pull 2MHz */
    GPIOB->CRH &= ~(0xF << 24);
    GPIOB->CRH |=  (0x2 << 24);

    GPIOB->BRR = BUZZER_PIN;

    /* TIM2: 1MHz tick (72MHz / 72) */
    TIM2->PSC = 72 - 1;
    TIM2->ARR = 1;

    /* Enable update interrupt */
    TIM2->DIER |= (1 << 0);

    /* Enable NVIC TIM2 */
    NVIC_EnableIRQ(TIM2_IRQn);

    /* Start timer */
    TIM2->CR1 |= (1 << 0);
}

/* ===== SET FREQUENCY ===== */
void Buzzer_SetFreq(uint16_t freq)
{
    if (freq == 0)
    {
        buzzer_enable = 0;
        GPIOB->BRR = BUZZER_PIN;
        return;
    }

    pwm_period = 1000000 / freq;   // 1MHz base
    pwm_duty   = pwm_period / 2;    // 50%

    pwm_counter = 0;
    buzzer_enable = 1;
}

/* ===== ON/OFF ===== */
void Buzzer_On(void)
{
    buzzer_enable = 1;
    Buzzer_SetFreq(2000); // 2kHz default
}

void Buzzer_Off(void)
{
    buzzer_enable = 0;
    GPIOB->BRR = BUZZER_PIN;
}

/* ===== TIMER ISR ===== */
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & (1 << 0))
    {
        TIM2->SR &= ~(1 << 0);

        if (!buzzer_enable)
        {
            GPIOB->BRR = BUZZER_PIN;
            return;
        }

        pwm_counter++;

        if (pwm_counter >= pwm_period)
            pwm_counter = 0;

        if (pwm_counter < pwm_duty)
            GPIOB->BSRR = BUZZER_PIN;
        else
            GPIOB->BRR = BUZZER_PIN;
    }
}