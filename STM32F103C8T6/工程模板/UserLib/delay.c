/**
 * ******************************************************************************
  * @file    delay.c
  * @author  
  * @version 
  * @date    
  * @brief   实现延时函数，使用Cortex-M3内核的定时器SysTick实现定时         
 * ******************************************************************************
 * 
 */

#include "delay.h"

void delay_us(uint32_t us)
{
    SysTick->LOAD = SystemCoreClock / us;           //设置重装值
    SysTick->VAL = 0;                               //设定当前值
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |    //SysTick_CTRL_CLKSOURCE_Msk：时钟源选择HCLK
                    SysTick_CTRL_ENABLE_Msk;        //启动定时器
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));          //判断COUNTFLAG位是否为1
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      //关闭定时器
}

void delay_ms(uint32_t ms)
{
    while(ms--) delay_us(1000);
}

void delay_s(uint32_t s)
{
    while(s--) delay_ms(1000);
}
