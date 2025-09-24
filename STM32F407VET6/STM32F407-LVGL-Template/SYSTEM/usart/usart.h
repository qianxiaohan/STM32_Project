/**
 ****************************************************************************************************
 * @file        usart.h
 * @brief       串口初始化代码(一般是串口1)，支持printf
 ****************************************************************************************************
 * @attention
 * @note:     在使用此头文件时，需确保相关的依赖项已正确配置，
 * @contact:  微信公众号 - [GenBotter]，分享技术内容
 *            QQ群号 - [366182133]，在线技术支持
 *            淘宝店铺 - [https://genbotter.taobao.com]，提供配套硬件产品
 *
 * 修改说明
 *	加入了对printf的支持
 *	增加了串口接收命令功能.
 *  修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
 *	增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
 *	修改了EN_USART1_RX的使能方式
 ****************************************************************************************************
 */
#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
#endif


