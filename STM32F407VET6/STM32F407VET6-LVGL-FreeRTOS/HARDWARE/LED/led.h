/**
 ****************************************************************************************************
 * @file        led.h
 * @brief       LED�Ƴ�ʼ������
 ****************************************************************************************************
 * @attention
 * @note:     ��ʹ�ô�ͷ�ļ�ʱ����ȷ����ص�����������ȷ���ã�
 * @contact:  ΢�Ź��ں� - [GenBotter]������������
 *            QQȺ�� - [366182133]�����߼���֧��
 *            �Ա����� - [https://genbotter.taobao.com]���ṩ����Ӳ����Ʒ
 * ��ʼ��PA6��PA7Ϊ�����.��ʹ���������ڵ�ʱ��		    
 * LED IO��ʼ��
 ****************************************************************************************************
 */
#ifndef __LED_H
#define __LED_H
#include "sys.h"

	
//LED�˿ڶ���
#define LED0 PAout(6)	// D2
#define LED1 PAout(7)	// D3

#define OPEN_LED1()   GPIO_ResetBits(GPIOA, GPIO_Pin_6)	 
#define CLOSE_LED1()  GPIO_SetBits(GPIOA, GPIO_Pin_6)	 
#define OPEN_LED2()   GPIO_ResetBits(GPIOA, GPIO_Pin_7)	 
#define CLOSE_LED2()  GPIO_SetBits(GPIOA, GPIO_Pin_7)	 

void LED_Init(void);//��ʼ��		 				    
#endif
