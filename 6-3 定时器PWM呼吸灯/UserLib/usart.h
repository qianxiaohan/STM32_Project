#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

void COM_Init(void);
void COM_SendData(uint8_t data);
void COM_ReceiveData(uint8_t *data);

#endif
