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

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);        //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	
	LED_Init();				//��ʼ��LED 
	KEY_Init(); 			//������ʼ��  
	
	TIM3_Int_Init(1000 - 1, 84 - 1);	//��ʱ1ms

	lv_init();                          // lvgl��ʼ��
    lv_port_disp_init();                // ��ʾ�豸��ʼ��
    lv_port_indev_init();               // �����豸��ʼ��
   
	/* ʹ��GUI GUIDIER������UI */
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
