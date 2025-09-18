/**
 * ******************************************************************************
  * @file    delay.c
  * @author  
  * @version 
  * @date    
  * @brief   ʵ����ʱ������ʹ��Cortex-M3�ں˵Ķ�ʱ��SysTickʵ�ֶ�ʱ         
 * ******************************************************************************
 * 
 */

#include "delay.h"

void delay_us(uint32_t us)
{
    SysTick->LOAD = SystemCoreClock / us;           //������װֵ
    SysTick->VAL = 0;                               //�趨��ǰֵ
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |    //SysTick_CTRL_CLKSOURCE_Msk��ʱ��Դѡ��HCLK
                    SysTick_CTRL_ENABLE_Msk;        //������ʱ��
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));          //�ж�COUNTFLAGλ�Ƿ�Ϊ1
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      //�رն�ʱ��
}

void delay_ms(uint32_t ms)
{
    while(ms--) delay_us(1000);
}

void delay_s(uint32_t s)
{
    while(s--) delay_ms(1000);
}
