#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define     KEY1    GPIO_Pin_1
#define     KEY2    GPIO_Pin_6
#define     KEY3    GPIO_Pin_11

void Key_Init(void);
uint8_t getKeyNum(void);    //获取按键序号，判断是哪个按键被按下

#endif

