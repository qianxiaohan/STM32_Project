#include "stm32f10x.h"                  
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include <stdio.h>

int main(void)
{
	COM_Init();			//���ڳ�ʼ������������
	MyRTC_Init();		//��ʼ��RTC
    Time_Show();		//������ѭ������ʾʱ��
}





