#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define     LED1_GPIO   GPIOA
#define     LED1_Pin    GPIO_Pin_0

void LED_Init(void);

#endif
