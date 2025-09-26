/**
 ****************************************************************************************************
 * @file        led.h
 * @brief       LED灯初始化代码
 ****************************************************************************************************
 * @attention
 * @note:     在使用此头文件时，需确保相关的依赖项已正确配置，
 * @contact:  微信公众号 - [GenBotter]，分享技术内容
 *            QQ群号 - [366182133]，在线技术支持
 *            淘宝店铺 - [https://genbotter.taobao.com]，提供配套硬件产品
 * 初始化PA6和PA7为输出口.并使能这两个口的时钟		    
 * LED IO初始化
 ****************************************************************************************************
 */
#ifndef __LED_H
#define __LED_H
#include "sys.h"

	
//LED端口定义
#define LED0 PAout(6)	// D2
#define LED1 PAout(7)	// D3

#define OPEN_LED1()   GPIO_ResetBits(GPIOA, GPIO_Pin_6)	 
#define CLOSE_LED1()  GPIO_SetBits(GPIOA, GPIO_Pin_6)	 
#define OPEN_LED2()   GPIO_ResetBits(GPIOA, GPIO_Pin_7)	 
#define CLOSE_LED2()  GPIO_SetBits(GPIOA, GPIO_Pin_7)	 

void LED_Init(void);//初始化		 				    
#endif
