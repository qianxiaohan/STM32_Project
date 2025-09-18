#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "usart.h"
#include <stdio.h>

void showTemperature(void);	//��ʾ�¶�
void ADC_Configuration(void);
void DMA_Configuration(void);	//����DMA

__IO uint16_t ADCConvertedValue; //DMA����ȡ�������ݴ�ŵ�����

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
	/* ����GPIO��ADC1ʱ�� */
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);	//PCLK2����2��Ƶ��28 / 2 = 14MHz  = 0.083us 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* ����ADC */
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//��������ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//��������ת��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//�رն�ͨ��ɨ��ģʽ��ֻ��1��ͨ��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//���ݶ��뷽ʽ���Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//���ⲿ���������������
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//1��ADCͨ��
	ADC_Init(ADC1, &ADC_InitStructure);

	/* Configures ADC1 Channel16 as: first converted channel with an 239 cycles sample time */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5); //����ʱ��0.083us * 239.5 = 19.88us

	/* Enable the temperature sensor and vref internal channel */ 
	ADC_TempSensorVrefintCmd(ENABLE); 

	/* ��ʼADC1��DMAģʽ */
	ADC_DMACmd(ADC1, ENABLE);

	/* ����ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* ����ADCУ׼�Ĵ��� */
	ADC_ResetCalibration(ADC1);

	/* ��鸴λADCУ׼�Ƿ���� */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* ����ADCУ׼ */
	ADC_StartCalibration(ADC1);

	/* ���У׼�Ƿ���� */
	while(ADC_GetCalibrationStatus(ADC1));

	/* ��ʼADC���ת�� */
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

void DMA_Configuration(void)
{
	/* ����DMAʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* ����ʼ��DMA1ͨ��1 */
	DMA_DeInit(DMA1_Channel1);

	/* ����DMA��ʼ���ṹ�� */
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);		//�����������ַ��������ADC1���ݼĴ����ĵ�ַ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//�������ݴ�С��16λ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;	//�����ڴ����ַ�����������ݴ�ŵ��ڴ��ַ
	DMA_InitStructure.DMA_BufferSize = 1;		//�����С����������Ϊһ����λ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//������Ϊ���ݴ������Դ 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//�ر�DMA���ڴ浽�ڴ�ģʽ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//�ڴ����ݴ�С��16λ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;	//�ڴ��ַ������
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//ѭ������ģʽ������ζ�ȡ���ݵ�������
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;		//���ȼ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//�����ַ������
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	//��ʼ��DMA
	DMA_Cmd(DMA1_Channel1, ENABLE);	//ʹ��DMA
}

void showTemperature(void)
{
	/*Returns the ADC1 Master data value of the last converted channel*/ 
	// uint16_t DataValue = 0; 
	// Temperature (in ��C) = {(V25 - VSENSE) / Avg_Slope} + 25.
	// v25����ֵ1.43V  Avg_Slope 4.3mv/��c VSENSE=DataValue
	//����Vdda/4096
	double temperature;
	double VSENSE = (double)ADCConvertedValue*(3.3/4096);
	
	temperature = (1.43 - VSENSE) / (0.0043) + 25;
	printf("VSENSE: %.2f, temperature: %.2f", VSENSE, temperature);
	// printf("ADCConvertedValue: %d", ADCConvertedValue);
}


