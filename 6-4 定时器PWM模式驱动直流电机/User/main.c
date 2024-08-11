#include "stm32f10x.h"                  // Device header
#include "oled.h"
#include "delay.h"
#include "tb6612fng.h"
#include "key.h"


int main(void)
{
	int isStart = 0;		//1：电机启动，0：电机停止
	int speed = 400;		//用于TIM_CCR2寄存器的比较值，最大为1000
	int cnt = 0;			//cnt为偶数：正转，cnt为奇数：反转
	int index = 0;			//用于strings[]的索引
	uint8_t keyNum;			//按键序号，1：key1加速; 2: key2正/反转; 3: key3停止
	//OLED要显示的字符
	const uint8_t *strings[4] = { 
		"Mode: STOP", 
		"Mode: CW",
		"Mode: CCW",
		"Speed: 0000"  
	};
	Key_Init();
	Motor_Driver_Init();
	OLED_Init();
	OLED_ShowString(1, 1, strings[index]);
	OLED_ShowString(2, 1, strings[3]);
	while (1)
	{	
		keyNum = getKeyNum();
		if((keyNum == 1) && isStart){	//电机调速
			speed += 100;
			if(speed > 1000)
				speed = 500;
			Motor_SetSpeed(speed);
			OLED_Clear();			//清屏
			OLED_ShowString(1, 1, strings[index]);	//显示模式
			OLED_ShowString(2, 1, "Speed: ");
			OLED_ShowNum(2, 7, speed, 4);
			
		}else if(keyNum == 2){		//电机正反转
			if(cnt %2 == 0){
				Motor_SetMode(CW);	//电机正转
				OLED_Clear();
				OLED_ShowString(1, 1, strings[1]);
				index = 1;
			}
				
			else{
				Motor_SetMode(CCW);	//电机反转
				OLED_Clear();
				OLED_ShowString(1, 1, strings[2]);
				index = 2;
			}
			OLED_ShowString(2, 1, "Speed: ");
			OLED_ShowNum(2, 7, speed, 4);
			isStart = 1;
			cnt++;
		}else if((keyNum == 3) && isStart){	//电机停止
			Motor_SetMode(STOP);
			OLED_Clear();
			OLED_ShowString(1, 1, strings[0]);
			OLED_ShowString(2, 1, strings[3]);
			isStart = 0;
			cnt = 0;
		}
	}
}
