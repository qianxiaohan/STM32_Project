/**
 ****************************************************************************************************
 * @file        key.h
 * @brief       按键初始化代码(包括时钟配置/中断管理/GPIO设置等)
 ****************************************************************************************************
 * @attention
 * @note:     在使用此头文件时，需确保相关的依赖项已正确配置，
 * @contact:  微信公众号 - [GenBotter]，分享技术内容
 *            QQ群号 - [366182133]，在线技术支持
 *            淘宝店铺 - [https://genbotter.taobao.com]，提供配套硬件产品
 ****************************************************************************************************
 */
#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
 	 

/*下面的方式是通过直接操作库函数方式读取IO*/
#define KEY0 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4
#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE3 
//#define KEY2 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE2
#define WK_UP 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0


/*下面方式是通过位带操作方式读取IO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/


#define KEY0_PRES 	1
#define KEY1_PRES	2
//#define KEY2_PRES	3
#define WKUP_PRES   4

void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数	

#endif
