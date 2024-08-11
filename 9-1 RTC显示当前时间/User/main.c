#include "stm32f10x.h"                  
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include <stdio.h>

int main(void)
{
	COM_Init();			//串口初始化，采样数据
	MyRTC_Init();		//初始化RTC
    Time_Show();		//在无限循环中显示时间
}





