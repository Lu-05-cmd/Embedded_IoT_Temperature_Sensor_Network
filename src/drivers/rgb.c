#include "../include/drivers/rgb.h"

/* PA1 = R, PA2 = G, PA3 = B */

#define R_PIN (1U << 1)
#define G_PIN (1U << 2)
#define B_PIN (1U << 0)

void RGB_Init(void)
{
    /* Enable clock GPIOA */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* Reset config PA1 PA2 PA3 */
    GPIOA->CRL &= ~(
        (0xF << (4 * 1)) |
        (0xF << (4 * 2)) |
        (0xF << (4 * 0))
    );

    /* Output push-pull 2MHz */
    GPIOA->CRL |= (
        (0x2 << (4 * 1)) |
        (0x2 << (4 * 2)) |
        (0x2 << (4 * 0))
    );

    RGB_Off();
}

void RGB_Set(uint8_t r, uint8_t g, uint8_t b)
{
    if (r) GPIOA->BSRR = R_PIN;
    else   GPIOA->BRR  = R_PIN;

    if (g) GPIOA->BSRR = G_PIN;
    else   GPIOA->BRR  = G_PIN;

    if (b) GPIOA->BSRR = B_PIN;
    else   GPIOA->BRR  = B_PIN;
}

void RGB_Off(void)
{
    GPIOA->BRR = R_PIN | G_PIN | B_PIN;
}

void RGB_UpdateByTemp(uint8_t temp)
{
    if (temp < 25)
    {
        RGB_Set(1, 0, 1);   // xanh
    }
    else if (temp >=25 && temp <= 30)
    {
        RGB_Set(1, 1, 0);   // vàng
    }
    else
    {
        RGB_Set(0, 1, 0);   // đỏ
    }
}