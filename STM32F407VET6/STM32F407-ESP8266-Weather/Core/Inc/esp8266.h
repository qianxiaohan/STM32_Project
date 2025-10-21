#ifndef _ESP8266_H_
#define _ESP8266_H_

unsigned char ESP8266_Init(void);   //ESP8266��ʼ��

void ESP8266_Clear(void);   //���ESP8266������

unsigned char ESP8266_SendCmd(const char *cmd, char *res);    //����ATָ��

void ESP8266_SendLenData(unsigned char *data, unsigned short len);  //���ͳ���len������

void ESP8266_SendData(unsigned char *data); //͸��ģʽ�·�������

void ESP8266_GetResponse(void); //��ESP8266����GET���󣬲������Ӧ

unsigned short ESP8266_ReadData(unsigned char *buffer, unsigned short max_len); //��DMA��������ȡ����

#endif
