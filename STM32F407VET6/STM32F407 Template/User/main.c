#include "delay.h"
#include "led.h"

int main(void)
{ 
 
	delay_init(168);		 //延时函数初始化，168是主频
	LED_Init();		        //初始化LED
	
	
	/* 以500ms的周期闪烁两个LED灯 */
	while(1)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);  
		GPIO_SetBits(GPIOA,GPIO_Pin_7);   
		delay_ms(500);  		   
		GPIO_SetBits(GPIOA,GPIO_Pin_6);	   
		GPIO_ResetBits(GPIOA,GPIO_Pin_7); 
		delay_ms(500);                     
	}
}



 



