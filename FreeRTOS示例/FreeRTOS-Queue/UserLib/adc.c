#include "adc.h"
#include "stm32f10x.h"

void ADC_Configuration(void)
{
	/* 开启GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ADC_Pin;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ADC1时钟 */
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

	/* Configures ADC1 Channel1 as: first converted channel with an 239 cycles sample time */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_239Cycles5); //采样时间0.083us * 239.5 = 19.88us

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
