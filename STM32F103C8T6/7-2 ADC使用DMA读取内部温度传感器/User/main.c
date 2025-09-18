#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "usart.h"
#include <stdio.h>

void showTemperature(void);	//显示温度
void ADC_Configuration(void);
void DMA_Configuration(void);	//配置DMA

__IO uint16_t ADCConvertedValue; //DMA将读取到的数据存放到其中

int main(void)
{
	COM_Init();
	DMA_Configuration();	
	ADC_Configuration();

	

	while (1)
	{
		showTemperature();
		
		delay_ms(1000);	//delay 1s
	}
}

void ADC_Configuration(void)
{
	/* 开启GPIO、ADC1时钟 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);	//PCLK2进行2分频：28 / 2 = 14MHz  = 0.083us 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* 配置ADC */
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立工作模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启持续转换模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//关闭多通道扫描模式，只有1个通道
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//数据对齐方式：右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//无外部触发，由软件触发
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//1个ADC通道
	ADC_Init(ADC1, &ADC_InitStructure);

	/* Configures ADC1 Channel16 as: first converted channel with an 239 cycles sample time */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5); //采样时间0.083us * 239.5 = 19.88us

	/* Enable the temperature sensor and vref internal channel */ 
	ADC_TempSensorVrefintCmd(ENABLE); 

	/* 开始ADC1的DMA模式 */
	ADC_DMACmd(ADC1, ENABLE);

	/* 开启ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* 重置ADC校准寄存器 */
	ADC_ResetCalibration(ADC1);

	/* 检查复位ADC校准是否结束 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* 开启ADC校准 */
	ADC_StartCalibration(ADC1);

	/* 检查校准是否结束 */
	while(ADC_GetCalibrationStatus(ADC1));

	/* 开始ADC软件转换 */
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

void DMA_Configuration(void)
{
	/* 开启DMA时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* 反初始化DMA1通道1 */
	DMA_DeInit(DMA1_Channel1);

	/* 配置DMA初始化结构体 */
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);		//配置外设基地址，这里是ADC1数据寄存器的地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据大小，16位
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;	//配置内存基地址，这里是数据存放的内存地址
	DMA_InitStructure.DMA_BufferSize = 1;		//缓存大小，以数据量为一个单位
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//外设作为数据传输的来源 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//关闭DMA的内存到内存模式
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//内存数据大小，16位
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;	//内存地址不递增
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//循环缓存模式，即多次读取数据到缓冲区
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;		//优先级高
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址不递增
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	//初始化DMA
	DMA_Cmd(DMA1_Channel1, ENABLE);	//使能DMA
}

void showTemperature(void)
{
	/*Returns the ADC1 Master data value of the last converted channel*/ 
	// uint16_t DataValue = 0; 
	// Temperature (in °C) = {(V25 - VSENSE) / Avg_Slope} + 25.
	// v25典型值1.43V  Avg_Slope 4.3mv/°c VSENSE=DataValue
	//精度Vdda/4096
	double temperature;
	double VSENSE = (double)ADCConvertedValue*(3.3/4096);
	
	temperature = (1.43 - VSENSE) / (0.0043) + 25;
	printf("VSENSE: %.2f, temperature: %.2f", VSENSE, temperature);
	// printf("ADCConvertedValue: %d", ADCConvertedValue);
}


