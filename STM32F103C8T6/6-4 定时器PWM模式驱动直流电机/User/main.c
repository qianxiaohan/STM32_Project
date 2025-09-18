#include "stm32f10x.h"                  // Device header
#include "oled.h"
#include "delay.h"
#include "tb6612fng.h"
#include "key.h"


int main(void)
{
	int isStart = 0;		//1�����������0�����ֹͣ
	int speed = 400;		//����TIM_CCR2�Ĵ����ıȽ�ֵ�����Ϊ1000
	int cnt = 0;			//cntΪż������ת��cntΪ��������ת
	int index = 0;			//����strings[]������
	uint8_t keyNum;			//������ţ�1��key1����; 2: key2��/��ת; 3: key3ֹͣ
	//OLEDҪ��ʾ���ַ�
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
		if((keyNum == 1) && isStart){	//�������
			speed += 100;
			if(speed > 1000)
				speed = 500;
			Motor_SetSpeed(speed);
			OLED_Clear();			//����
			OLED_ShowString(1, 1, strings[index]);	//��ʾģʽ
			OLED_ShowString(2, 1, "Speed: ");
			OLED_ShowNum(2, 7, speed, 4);
			
		}else if(keyNum == 2){		//�������ת
			if(cnt %2 == 0){
				Motor_SetMode(CW);	//�����ת
				OLED_Clear();
				OLED_ShowString(1, 1, strings[1]);
				index = 1;
			}
				
			else{
				Motor_SetMode(CCW);	//�����ת
				OLED_Clear();
				OLED_ShowString(1, 1, strings[2]);
				index = 2;
			}
			OLED_ShowString(2, 1, "Speed: ");
			OLED_ShowNum(2, 7, speed, 4);
			isStart = 1;
			cnt++;
		}else if((keyNum == 3) && isStart){	//���ֹͣ
			Motor_SetMode(STOP);
			OLED_Clear();
			OLED_ShowString(1, 1, strings[0]);
			OLED_ShowString(2, 1, strings[3]);
			isStart = 0;
			cnt = 0;
		}
	}
}
