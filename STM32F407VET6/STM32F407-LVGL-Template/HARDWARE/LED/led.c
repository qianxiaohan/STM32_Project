/**
 ****************************************************************************************************
 * @file        led.c
 * @brief       LED�Ƴ�ʼ������(����ʱ������/�жϹ���/GPIO���õ�)
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
#include "led.h" 
	 
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//LED0��LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
	
	GPIO_SetBits(GPIOA,GPIO_Pin_6 | GPIO_Pin_7);//���øߣ�����

}






