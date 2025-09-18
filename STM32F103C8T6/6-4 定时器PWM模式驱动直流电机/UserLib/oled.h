#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

#define  OLED_SCL   GPIO_Pin_6
#define  OLED_SDA   GPIO_Pin_7

#define     SDA_WriteBit(x)     GPIO_WriteBit(GPIOB, OLED_SDA, (BitAction)(x))    
#define     SCL_WriteBit(x)     GPIO_WriteBit(GPIOB, OLED_SCL, (BitAction)(x))     

void OLED_Init(void);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
void OLED_I2C_Init(void);
void OLED_WriteCmd(uint8_t cmd);
void OLED_WriteData(uint8_t data);
void OLED_SendByte(uint8_t byte);
void OLED_ShowChar(uint8_t row, uint8_t col, uint8_t ch);
void OLED_ShowString(uint8_t row, uint8_t col, const uint8_t *str);
void OLED_ShowNum(uint8_t row, uint8_t col, uint32_t num, uint8_t length);
uint32_t OLED_Pow(uint32_t X, uint32_t Y);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void);

#endif
