/**
 * 
 * 	ADC在双ADC模式下，使用DMA进行多通道采样：
 *  
 *  ADC通道和引脚对应关系：
 * 	ADC1_IN0   ---->  PA0 
 *  ADC1_IN16  ---->  内部温度传感器，无需外接
 *  ADC2_IN7   ---->  PA7
 *  
 * 
 */


#include "stm32f10x.h"                  
#include "delay.h"
#include "usart.h"
#include <stdio.h>

void parseData(void);					//解析ADC采样到的数据
void ADC_Configuration(void);			//配置ADC
void DMA_Configuration(void);			//配置DMA
void GPIO_Configuration(void);			//配置GPIO
void RCC_Configuration(void);			//配置ADCCLK、GPIO、ADC时钟

__IO uint32_t ADCConvertedValue[2]; //缓冲区，DMA将读取到的数据存放到其中

int main(void)
{
	COM_Init();				//串口初始化，用于输出采样数据
	RCC_Configuration();	
	GPIO_Configuration();
	DMA_Configuration();	
	ADC_Configuration();
	
	while (1)
	{
		parseData();
		delay_ms(3000);	//delay 3s
	}
}


void RCC_Configuration(void)
{
    /* 对PCLK2进行二分频 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);	//PCLK2进行2分频：28 / 2 = 14MHz  = 0.083us 

	/* 开启GPIOB、ADC1时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC2, ENABLE);

	/* 开启DMA时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void ADC_Configuration(void)
{
	
	/* 配置ADC1 */
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;	//常规同步模式Regular simultaneous mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启持续转换模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//开启扫描转换模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//数据对齐方式：右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//无外部触发
	ADC_InitStructure.ADC_NbrOfChannel = 2;	//2个ADC通道
	ADC_Init(ADC1, &ADC_InitStructure);

	/* 配置ADC1通道0、通道16的采样顺序和采样时间 */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5); //采样时间0.083us * 239.5 = 19.88us
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 2, ADC_SampleTime_239Cycles5); //采样时间0.083us * 239.5 = 19.88us

	/* Enable the temperature sensor and vref internal channel */ 
	ADC_TempSensorVrefintCmd(ENABLE); 
	
	/* 开始ADC1的DMA模式 */
	ADC_DMACmd(ADC1, ENABLE);

	/* 配置ADC2 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;	//常规同步模式Regular simultaneous mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启持续转换模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//开启扫描转换模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//数据对齐方式：右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//无外部触发
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//1个ADC通道
	ADC_Init(ADC2, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC2, ADC_Channel_7, 1, ADC_SampleTime_239Cycles5); //采样时间0.083us * 239.5 = 19.88us

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


	/* 开启ADC2 */
	ADC_Cmd(ADC2, ENABLE);

	/* 重置ADC校准寄存器 */
	ADC_ResetCalibration(ADC2);

	/* 检查复位ADC校准是否结束 */
	while(ADC_GetResetCalibrationStatus(ADC2));

	/* 开启ADC校准 */
	ADC_StartCalibration(ADC2);

	/* 检查校准是否结束 */
	while(ADC_GetCalibrationStatus(ADC2));

	/* 开始ADC软件转换 */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);

	/* Test on DMA1 channel1 transfer complete flag */
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));

	/* Clear DMA1 channel1 transfer complete flag */
	DMA_ClearFlag(DMA1_FLAG_TC1);	
}



void DMA_Configuration(void)
{
	/* 反初始化DMA1通道1 */
	DMA_DeInit(DMA1_Channel1);

	/* 配置DMA初始化结构体 */
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);		//配置外设基地址，这里是ADC1数据寄存器的地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//外设数据大小，32位, 双ADC模式
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;		//配置内存基地址，这里是数据存放的内存地址
	DMA_InitStructure.DMA_BufferSize = 2;		//缓存大小，以数据量为一个单位
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//外设作为数据传输的来源 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//关闭DMA的内存到内存模式
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;	//内存数据大小，32位
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//内存地址递增
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//循环缓存模式，即多次读取数据到缓冲区
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;		//优先级高
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址不递增
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	//初始化DMA

	DMA_Cmd(DMA1_Channel1, ENABLE);	//使能DMA
}


void GPIO_Configuration(void)
{
	/* 配置GPIO */    
    GPIO_InitTypeDef GPIOInitStructure;
    GPIOInitStructure.GPIO_Mode = GPIO_Mode_AIN;	//模拟输入
    GPIOInitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_7;
    GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIOInitStructure);
}

void parseData(void)
{
	// Temperature (in °C) = {(V25 - VSENSE) / Avg_Slope} + 25.
	// v25典型值1.43V  Avg_Slope 4.3mv/°c VSENSE=DataValue
	//精度Vdda/4096

	/**
	 *   adc1_data采样内部温度传感器的数据
	 *   adc1_data2采样PA0的数据
	 *   adc2_data采样PA7的数据	
	 * 
	 */

	double temperature;
	uint16_t adc1_data, adc2_data, adc1_data2;
	/* 低16位是ADC1的数据，高16位是ADC2的数据 */
	adc1_data = ADCConvertedValue[0] & 0xffff;
	adc2_data = (ADCConvertedValue[0] >> 16);
	adc1_data2 = ADCConvertedValue[1] & 0xffff;

	/* 使用参考手册所给出的公式计算出温度 */
	double VSENSE = (double)adc1_data*(3.3/4096);
	temperature = (1.43 - VSENSE) / (0.0043) + 25;
	
	/* 串口输出数据 */
	printf("t: %.2f, d1: %d, d2: %d", temperature, adc2_data, adc1_data2);
}





