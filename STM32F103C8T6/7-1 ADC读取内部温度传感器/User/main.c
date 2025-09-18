#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "usart.h"
#include <stdio.h>

void showTemperature(void);	//显示温度
void ADC_Configuration(void);

int main(void)
{
	COM_Init();	
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
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//PCLK2进行6分频：72 / 6 = 12MHz  = 0.083us 
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

	/* 开启ADC */
	ADC_Cmd(ADC1, ENABLE);

	/* 重置ADC校准寄存器 */
	ADC_ResetCalibration(ADC1);

	/* 检查校准是否结束 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* 打开校准寄存器 */
	ADC_StartCalibration(ADC1);

	/* 检查校准是否结束 */
	while(ADC_GetCalibrationStatus(ADC1));

	/* 开始ADC软件转换 */
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

void showTemperature(void)
{
	/*Returns the ADC1 Master data value of the last converted channel*/ 
	uint16_t DataValue = 0;
	DataValue = ADC_GetConversionValue(ADC1);	
//	for(int i = 0; i < 20; i++)
//	{
//		DataValue = ADC_GetConversionValue(ADC1);
//	}
	
	// Temperature (in °C) = {(V25 - VSENSE) / Avg_Slope} + 25.
	// v25典型值1.43V  Avg_Slope 4.3mv/°c VSENSE=DataValue
	//精度Vdda/4096
	DataValue = ADC_GetConversionValue(ADC1);
	double temperature;
	double VSENSE = (double)DataValue*(3.3/4096);
	temperature = (1.43 - VSENSE) / (0.0043) + 25;
	printf("VSENSE: %.2f, temperature: %.2f",VSENSE, temperature);
	//printf("DataValue: %d ",DataValue);
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t)ch);
	/* 硬件会自动置1 */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)	
	{
		// USART_ClearFlag(USART1, USART_FLAG_TXE);
	}

	return ch;
}
