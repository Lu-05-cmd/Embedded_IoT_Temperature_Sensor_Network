#include "../include/drivers/buzzer.h"
#include "stm32f103xb.h"

#define BUZZER_PIN (1U << 14)

static uint8_t alert_output_on = 0;

/* ===== GPIO INIT ===== */
void Buzzer_Init(void)
{
    /* Enable clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    /* PB14 output push-pull 2MHz */
    GPIOB->CRH &= ~(0xF << 24);
    GPIOB->CRH |=  (0x2 << 24);

    GPIOB->BRR = BUZZER_PIN;
    alert_output_on = 0;
}

/* ===== ON/OFF ===== */
void Buzzer_On(void)
{
    GPIOB->BSRR = BUZZER_PIN;
    alert_output_on = 1;
}

void Buzzer_Off(void)
{
    GPIOB->BRR = BUZZER_PIN;
    alert_output_on = 0;
}

void Buzzer_UpdateAlert(BuzzerAlertMode_t mode, uint32_t now_ms)
{
    uint32_t period_ms;
    uint32_t on_ms;
    uint8_t should_be_on;

    if (mode == BUZZER_ALERT_OFF)
    {
        if (alert_output_on) {
            Buzzer_Off();
        }
        return;
    }

    if (mode == BUZZER_ALERT_ERROR)
    {
        period_ms = 1200;
        on_ms = 0;
        should_be_on = (now_ms % period_ms) < 120 ||
                       ((now_ms + 1000) % period_ms) < 120 ||
                       ((now_ms + 800) % period_ms) < 120;
    }
    else if (mode == BUZZER_ALERT_FAST)
    {
        period_ms = 700;
        on_ms = 250;
        should_be_on = (now_ms % period_ms) < on_ms;
    }
    else
    {
        period_ms = 3000;
        on_ms = 200;
        should_be_on = (now_ms % period_ms) < on_ms;
    }

    if (should_be_on && !alert_output_on)
    {
        Buzzer_On();
    }
    else if (!should_be_on && alert_output_on)
    {
        Buzzer_Off();
    }
}
