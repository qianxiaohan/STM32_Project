#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

#define ADC_Channel ADC_Channel_9
#define ADC_Pin GPIO_Pin_1 

void ADC_Configuration(void);
uint16_t Get_ADC_Value(uint16_t cnt);
uint16_t Get_LightValue(uint16_t cnt);

#endif
