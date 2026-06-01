#include "../include/drivers/lcd.h"
#include "../include/drivers/systick.h"
#include "stm32f103xb.h"


#define LCD_RS_PIN    (1U << 0)
#define LCD_EN_PIN    (1U << 1)

#define LCD_D4_PIN    (1U << 4)
#define LCD_D5_PIN    (1U << 5)
#define LCD_D6_PIN    (1U << 6)
#define LCD_D7_PIN    (1U << 7)

static void LCD_DelayShort(void)
{
    for(volatile uint32_t i = 0; i < 5000; i++);
}

static void LCD_EnablePulse(void)
{
    GPIOB->BSRR = LCD_EN_PIN;

    LCD_DelayShort();
    LCD_DelayShort();

    GPIOB->BRR = LCD_EN_PIN;

    LCD_DelayShort();
    LCD_DelayShort();
}

static void LCD_Write4Bit(uint8_t data)
{
    GPIOA->BRR =
        LCD_D4_PIN |
        LCD_D5_PIN |
        LCD_D6_PIN |
        LCD_D7_PIN;

    if(data & 0x01) GPIOA->BSRR = LCD_D4_PIN;
    if(data & 0x02) GPIOA->BSRR = LCD_D5_PIN;
    if(data & 0x04) GPIOA->BSRR = LCD_D6_PIN;
    if(data & 0x08) GPIOA->BSRR = LCD_D7_PIN;

    LCD_EnablePulse();
}

static void LCD_SendCommand(uint8_t cmd)
{
    GPIOB->BRR = LCD_RS_PIN;

    LCD_Write4Bit(cmd >> 4);
    LCD_Write4Bit(cmd & 0x0F);

    SysTick_DelayMs(5);
}

static void LCD_SendData(uint8_t data)
{
    GPIOB->BSRR = LCD_RS_PIN;

    LCD_Write4Bit(data >> 4);
    LCD_Write4Bit(data & 0x0F);

    SysTick_DelayMs(2);
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    SysTick_DelayMs(5);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr;

    if(row == 0)
        addr = 0x80 + col;
    else
        addr = 0xC0 + col;

    LCD_SendCommand(addr);
}

void LCD_SendChar(char c)
{
    LCD_SendData((uint8_t)c);
}

void LCD_SendString(const char *str)
{
    while(*str)
    {
        LCD_SendChar(*str++);
    }
}

void LCD_Init(void)
{
    /* Config RCC & GPIO*/
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    GPIOA->CRL &= ~(0xFFFFUL << 16);
    GPIOA->CRL |=  (0x3333UL << 16);

    /* PB0-PB1 Output Push Pull 50MHz */
    GPIOB->CRL &= ~(0xFFUL);
    GPIOB->CRL |=  (0x33UL);

    GPIOB->BRR = LCD_RS_PIN;
    GPIOB->BRR = LCD_EN_PIN;

    SysTick_DelayMs(50);

    /* HD44780 reset sequence */
    LCD_Write4Bit(0x03);
    SysTick_DelayMs(20);

    LCD_Write4Bit(0x03);
    SysTick_DelayMs(20);

    LCD_Write4Bit(0x03);
    SysTick_DelayMs(20);

    LCD_Write4Bit(0x02);
    SysTick_DelayMs(20);

    /* Function Set */
    LCD_SendCommand(0x28);
    /* Display OFF */
    LCD_SendCommand(0x08);
    /* Clear */
    LCD_SendCommand(0x01);
    /* Entry Mode */
    LCD_SendCommand(0x06);
    /* Display ON */
    LCD_SendCommand(0x0C);
    SysTick_DelayMs(20);
}