#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "usart.h"
#include <stdio.h>

void showTemperature(void);	//��ʾ�¶�
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
	/* ����GPIO��ADC1ʱ�� */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//PCLK2����6��Ƶ��72 / 6 = 12MHz  = 0.083us 
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

	/* ����ADC */
	ADC_Cmd(ADC1, ENABLE);

	/* ����ADCУ׼�Ĵ��� */
	ADC_ResetCalibration(ADC1);

	/* ���У׼�Ƿ���� */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* ��У׼�Ĵ��� */
	ADC_StartCalibration(ADC1);

	/* ���У׼�Ƿ���� */
	while(ADC_GetCalibrationStatus(ADC1));

	/* ��ʼADC���ת�� */
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
	
	// Temperature (in ��C) = {(V25 - VSENSE) / Avg_Slope} + 25.
	// v25����ֵ1.43V  Avg_Slope 4.3mv/��c VSENSE=DataValue
	//����Vdda/4096
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
	/* Ӳ�����Զ���1 */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)	
	{
		// USART_ClearFlag(USART1, USART_FLAG_TXE);
	}

	return ch;
}
