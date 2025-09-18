/**
 * ******************************************************************************
  * @file    delay.h
  * @author  
  * @version 
  * @date    
  * @brief   延时函数头文件         
 * ******************************************************************************
 * 
 */



#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void delay_s(uint32_t s);

#endif
