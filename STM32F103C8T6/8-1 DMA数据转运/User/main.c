#include "stm32f10x.h"                  
#include "delay.h"
#include "usart.h"
#include <stdio.h>
#define BufferSize 8

void DMA_Configuration(void);			
void RCC_Configuration(void);			
void NVIC_Configuration(void);			
uint8_t BufferCompare(const uint32_t *buffer1, uint32_t *buffer2, uint32_t buffersize);

const uint32_t SRC_Buffer[BufferSize] = {
				0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
				0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20}; //DMA要搬运的数据

uint32_t DST_Buffer[BufferSize];	//DMA搬运的数据的目的内存地址
	
uint32_t CurrDataCounterEnd = 0x01;		//DMA剩余要搬运的数据单元个数

int main(void)
{
	COM_Init();				//串口初始化，采样数据
	RCC_Configuration();	//配置DMA
	NVIC_Configuration();	//配置DMA时钟
	DMA_Configuration();	//配置NVIC

	/* 等待数据传输完成 */
	while(CurrDataCounterEnd != 0)
	{
	}

	/* 判断数据传输是否丢失，返回1就向串口发送数据 */
	if(BufferCompare(SRC_Buffer, DST_Buffer, BufferSize))
	{
		for(int i = 0; i < BufferSize; i++)
		{
			printf("%lX ", DST_Buffer[i]);	//以16进制形式打印
			delay_ms(20);
		}
	}

	while (1)
	{
	}
}


void RCC_Configuration(void)
{
	/* 开启DMA时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void NVIC_Configuration(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;	//使能DMA1通道1中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

/* 比较两个内存地址的数据是否相等 */
uint8_t BufferCompare(const uint32_t *buffer1, uint32_t *buffer2, uint32_t buffersize)
{
	while(buffersize--)
	{
		if(*buffer1 != *buffer2) 
			return 0;
		buffer1++;
		buffer2++;
	}
    return 1;
}


void DMA_Configuration(void)
{
	/* 反初始化DMA1通道1 */
	DMA_DeInit(DMA1_Channel1);

	/* 配置DMA初始化结构体 */
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SRC_Buffer;		//配置外设基地址，这里是ADC1数据寄存器的地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DST_Buffer;		//配置内存基地址，这里是数据存放的内存地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//外设数据大小，32位, 双ADC模式
	DMA_InitStructure.DMA_BufferSize = BufferSize;		//缓存大小，以数据单元为一个单位，比如数据是16位，BufferSize = 8表示有8x16位的数据
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//外设作为数据传输的来源 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;	//开启DMA的内存到内存模式
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;	//内存数据大小，32位
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//内存地址递增
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;		//常规模式，即只读取1次数据到缓冲区
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;		//优先级高
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;	//外设地址递增
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	//初始化DMA

	/* 使能DMA1传输完成中断 */
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

	DMA_Cmd(DMA1_Channel1, ENABLE);	//使能DMA
	

}

/* DMA1通道1中断处理函数 */
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
		CurrDataCounterEnd = DMA_GetCurrDataCounter(DMA1_Channel1); //DMA还需传输的数据单元个数
		DMA_ClearITPendingBit(DMA1_IT_TC1);	//清除TC标志位
	}
}




