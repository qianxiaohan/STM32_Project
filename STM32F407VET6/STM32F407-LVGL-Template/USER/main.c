#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "touch.h"
#include "timer.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "events_init.h"
#include "gui_guider.h"
lv_ui guider_ui;

int main(void)
{ 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);        //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	
	LED_Init();				//初始化LED 
	KEY_Init(); 			//按键初始化  
	
	TIM3_Int_Init(1000 - 1, 84 - 1);	//定时1ms

	lv_init();                          // lvgl初始化
    lv_port_disp_init();                // 显示设备初始化
    lv_port_indev_init();               // 输入设备初始化
   
	/* 使用GUI GUIDIER创建的UI */
	setup_ui(&guider_ui);
	events_init(&guider_ui);
	/*-----------------------*/

	while (1)
	{
		if(flag_5ms)
		{
			flag_5ms = 0;
			lv_timer_handler();
		}
	}
	
}
