#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

/* 从机地址，这里是OLED模块的地址 */
#define SLAVE_ADDRESS 0x78

void MyI2C_init(void);
uint8_t MyI2C_SendData(uint8_t *data, uint8_t len);
uint8_t MyI2C_ReadData(uint8_t *data);

#endif
