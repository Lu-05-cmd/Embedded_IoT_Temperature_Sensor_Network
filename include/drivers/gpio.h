#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f103xb.h"

/* GPIO Pin Definitions */
#define GPIO_PIN_0  (1U << 0)   // Thêm chân 0
#define GPIO_PIN_1  (1U << 1)   
#define GPIO_PIN_2  (1U << 2)
#define GPIO_PIN_3  (1U << 3)
#define GPIO_PIN_4  (1U << 4)
#define GPIO_PIN_5  (1U << 5)
#define GPIO_PIN_6  (1U << 6)
#define GPIO_PIN_7  (1U << 7)
#define GPIO_PIN_8  (1U << 8)
#define GPIO_PIN_9  (1U << 9)
#define GPIO_PIN_10 (1U << 10)
#define GPIO_PIN_11 (1U << 11)
#define GPIO_PIN_12 (1U << 12)
#define GPIO_PIN_13 (1U << 13)
#define GPIO_PIN_14 (1U << 14)
#define GPIO_PIN_15 (1U << 15)
 

/* GPIO Pull-up/Pull-down Definitions */
#define PULL_NONE 0
#define PULL_UP   1
#define PULL_DOWN 2

#define GPIO_PIN_RESET 0
#define GPIO_PIN_SET   1

/* GPIO Mode Definitions */
typedef enum
{
    GPIO_MODE_INPUT = 0x00,
    GPIO_MODE_OUTPUT_PP = 0x01,
    GPIO_MODE_OUTPUT_OD = 0x11,
    GPIO_MODE_AF_PP = 0x02,
    GPIO_MODE_AF_OD = 0x12,
    GPIO_MODE_ANALOG = 0x03
}GPIO_ModeTypeDef;


/***************************************************************************************************
 * GPIO CONFIG MODE
 */
typedef struct 
{
    uint16_t Pin;
    GPIO_ModeTypeDef Mode;
    uint32_t Pull;
    uint32_t Speed;
}GPIO_InitTypeDef;



/* GPIO Functions */
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);
void GPIO_SetPin(uint16_t pin);
void GPIO_ResetPin(uint16_t pin);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t state);
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t pin);
#endif // GPIO_H
