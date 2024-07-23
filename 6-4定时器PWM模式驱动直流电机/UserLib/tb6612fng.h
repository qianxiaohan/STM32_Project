#ifndef __TB6612FNG_H
#define __TB6612FNG_H

#include "stm32f10x.h"

/* 配置电机驱动芯片AIN1、AIN2 */
#define     AIN1_Pin            GPIO_Pin_2
#define     AIN2_Pin            GPIO_Pin_3
#define     AIN1_SetBit(X)      GPIO_WriteBit(GPIOA, AIN1_Pin, (BitAction)(X))
#define     AIN2_SetBit(X)      GPIO_WriteBit(GPIOA, AIN2_Pin, (BitAction)(X))

/**
 *  SHORT_BRAKE：制动
 *  CCW(CounterClockWise)：反转
 *  CCW(CounterClockWise)：反转
 * 
 *  STOP: 停止
 * 
*/
typedef enum {
    SHORT_BRAKE,
    CCW,
    CW,
    STOP
} MODE;

void Motor_Driver_Init(void);   //电机驱动芯片初始化
void Motor_SetMode(MODE mode);          //设置电机模式
void Motor_SetSpeed(int speed);          //设置电机速度

#endif
