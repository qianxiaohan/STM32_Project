#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"

void MyRTC_Init(void);
void Time_Adjust(void);
void RTC_Configuration(void);
uint32_t Time_Regulate(void);
void Time_Show(void);
void Time_Display(uint32_t value);

#endif
