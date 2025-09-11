#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

#define SDA_WriteBit(x) GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)(x))
#define SCL_WriteBit(x) GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)(x))

void OLED_Init(void);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
void OLED_I2C_Init(void);
void OLED_WriteCmd(uint8_t cmd);
void OLED_WriteData(uint8_t data);
void OLED_SendByte(uint8_t byte);
void OLED_ShowChar(uint8_t row, uint8_t col, uint8_t ch);
void OLED_ShowString(uint8_t row, uint8_t col, uint8_t *str);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void);
uint32_t OLED_Pow(uint32_t X, uint32_t Y);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

#endif
