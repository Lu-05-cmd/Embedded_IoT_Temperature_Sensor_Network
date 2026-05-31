
#include "../include/drivers/gpio.h"


void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct)
{
    for (uint8_t pin = 0; pin < 16; pin++)
    {
        if (!(GPIO_InitStruct->Pin & (1U << pin))) continue;

        uint32_t pos = (pin < 8) ? (pin * 4) : ((pin - 8) * 4);
        volatile uint32_t *reg = (pin < 8) ? &GPIOx->CRL : &GPIOx->CRH;

        uint32_t mode = GPIO_InitStruct->Mode;
        uint32_t cfg = 0;

        if (mode == GPIO_MODE_OUTPUT_OD)
        {
            cfg = 0x07;  
        }
        else if (mode == GPIO_MODE_INPUT)
        {
            cfg = 0x04;  

            if (GPIO_InitStruct->Pull == PULL_UP)
                GPIOx->ODR |= (1U << pin);
            else
                GPIOx->ODR &= ~(1U << pin);
        }

        *reg &= ~(0xF << pos);
        *reg |=  (cfg << pos);
    }
}

void GPIO_SetPin(uint16_t pin) {
    GPIOA->BSRR = pin; 
}

void GPIO_ResetPin(uint16_t pin) {
    GPIOA->BRR = pin; 
}


//    GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t state)
{
    if (state)
        GPIOx->BSRR = pin;
    else
        GPIOx->BSRR = ((uint32_t)pin << 16);
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t pin)
{
    return (GPIOx->IDR & pin) ? 1U : 0U;
}