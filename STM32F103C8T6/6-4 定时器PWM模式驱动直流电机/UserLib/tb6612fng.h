#ifndef __TB6612FNG_H
#define __TB6612FNG_H

#include "stm32f10x.h"

/* ���õ������оƬAIN1��AIN2 */
#define     AIN1_Pin            GPIO_Pin_2
#define     AIN2_Pin            GPIO_Pin_3
#define     AIN1_SetBit(X)      GPIO_WriteBit(GPIOA, AIN1_Pin, (BitAction)(X))
#define     AIN2_SetBit(X)      GPIO_WriteBit(GPIOA, AIN2_Pin, (BitAction)(X))

/**
 *  SHORT_BRAKE���ƶ�
 *  CCW(CounterClockWise)����ת
 *  CCW(CounterClockWise)����ת
 * 
 *  STOP: ֹͣ
 * 
*/
typedef enum {
    SHORT_BRAKE,
    CCW,
    CW,
    STOP
} MODE;

void Motor_Driver_Init(void);   //�������оƬ��ʼ��
void Motor_SetMode(MODE mode);          //���õ��ģʽ
void Motor_SetSpeed(int speed);          //���õ���ٶ�

#endif
