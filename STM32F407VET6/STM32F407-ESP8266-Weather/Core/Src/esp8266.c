/**
 * @file esp8266.c
 * @author lyx 
 * @brief ESP8266驱动文件
 * @version 0.1
 * @date 2025-10-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//单片机头文件
#include "stm32f4xx_hal.h"

//网络设备驱动
#include "esp8266.h"

//硬件驱动
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>

#include "weather.h"	//解析JSON

/* ESP8266指令 */
const char *test_at = "AT\r\n";	//测试AT指令
const char *set_sta_mode = "AT+CWMODE=1\r\n";	//设置STA模式
const char *set_dhcp = "AT+CWDHCP=1,1\r\n";	//设置DHCP
const char *connect_wifi = "AT+CWJAP=\"Redmi K70\",\"qq1804068646\"\r\n";			//连接wifi指令
const char *connect_server = "AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n";	//连接TCP服务器指令
const char *set_cipmode = "AT+CIPMODE=1\r\n"; //透传模式，发送数据时不用指定数据的大小
const char *send_data_cmd = "AT+CIPSEND\r\n";	//透传模式下发送数据
const char *exit_send_cmd = "+++";	//退出透传模式
const char *close_tcp_cmd = "AT+CIPCLOSE\r\n";	//关闭TCP连接

/* API可配置的参数 */
const char *key = "SigzO8oaCNRAS96L8";	//API私钥
const char *location = "chengdu";		//地区
const char *days = "1";					//查询1~3天的天气

/* 测试用 */
// const uint8_t *GETstring = "HEAD /upload/test.bin HTTP/1.1\r\n"
// 					  		"Host: www.funiot.xyz\r\n"
// 					  "\r\n\r\n";
// #define CIPSTART_TEST			"AT+CIPSTART=\"TCP\",\"www.funiot.xyz\",80\r\n"

/**
 * @brief 清空缓冲区
 * 
 */
void ESP8266_Clear(void)
{
	memset(debugBuffer, 0, sizeof(debugBuffer));
	debug_data_len = 0;
}


/**
 * @brief ESP8266发送指令
 * 
 * @param cmd 要发送的指令
 * @param res 期望收到的指令
 * @return unsigned char 
 */
unsigned char ESP8266_SendCmd(const char *cmd, char *res)
{
	uint16_t timeOut = 500;	//超时时间2S

	/* 向ESP8266发送指令 */
	if(uart_transmit_str(huart1, (uint8_t*)cmd) != HAL_OK)
	{
		/* 发送失败 */
	}
	
	/* 等待ESP8266发来的数据 */
	while(timeOut--)
	{
		if(rx_data_ready == 1)
		{
			rx_data_ready = 0;
			if(strstr((const char *)debugBuffer, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				return 0;
			}
		}
		
		HAL_Delay(10);
	}

	/* 如果匹配失败也要清空缓存 */
	ESP8266_Clear();	

	return 1;
}

/**
 * @brief ESP8266发送指定长度的数据
 * 
 * @param data 要发送的数据
 * @param len 数据长度
 */
void ESP8266_SendLenData(unsigned char *data, unsigned short len)
{
	
}

/**
 * @brief 透传模式下发送数据 
 * 
 * 
 * @param data 要发送的数据
 */
void ESP8266_SendData(unsigned char *data)
{

	char buffer2[128];
	char *ret;
	uint16_t timeOut = 500;	//等待5S
	future_weather_t wt;
	ESP8266_Clear();								//清空接收缓存
	if(!ESP8266_SendCmd(send_data_cmd, ">"))				//收到‘>’时可以发送数据
	{
		/* 向ESP8266发送数据 */
		uart_transmit_str(huart1, data);

		/* 等待ESP8266发来的数据 */
		while(timeOut--)
		{
			if(rx_data_ready == 1)	//接收完成
			{
				rx_data_ready = 0; //标志位置0，准备下次DMA接收

				/* GET请求返回来的响应包括请求头，这里是为了找到json格式的数据以便解析 */
				ret = strchr((char*)debugBuffer, '{');

				/* 解析天气 */
				wt = parse_future_weather_data(ret);

				/* DMA的速度过快，直接将缓冲区的数据发送可能会出问题？将解析后的数据写入缓冲区 */
				sprintf(buffer2, "%s, %s, %s, %s, %s, %s\r\n", wt.city_name, wt.date, wt.weather_day, wt.weather_night, wt.high_temp, wt.low_temp);

				/* 使用DMA发送至串口助手 */
  				HAL_UART_Transmit_DMA(&huart3, (uint8_t*)buffer2, strlen(buffer2));
				break;
			}
			
			HAL_Delay(10);
		}

		/* 关闭透传 */
		uart_transmit_str(huart1, (uint8_t*)exit_send_cmd);

		/* 发送透传指令后需等待至少1s */
		HAL_Delay(2000);

		/* 关闭TCP连接 */
		if(!ESP8266_SendCmd(close_tcp_cmd, "OK"))
		{
			uart_transmit_str(huart3, (uint8_t*)"Connect Closed Succeed!\r\n");
		}
		else
		{
			uart_transmit_str(huart3, (uint8_t*)"Connect Closed Failed\r\n");
		}
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t*)"Send data to server error!\r\n");
	}
}

/**
 * @brief 从DMA缓冲区缓冲区读数据
 * 
 * @param buffer 
 * @param max_len 
 * @return uint16_t 
 */
unsigned short ESP8266_ReadData(unsigned char *buffer, unsigned short max_len)
{
	uint16_t bytes_to_read = 0;
	uint16_t write_snapshot = 0;

	write_snapshot = Write_index; // 获取当前写入位置快照
	// 计算有多少数据可以读
	if (write_snapshot >= PreWrite_index)
	{
		bytes_to_read = write_snapshot - PreWrite_index;
	}
	else
	{
		// 环形缓冲区绕回的情况
		bytes_to_read = RX_BUFFER_SIZE - PreWrite_index + write_snapshot;
	}

	// 限制读取长度，避免溢出
	if (bytes_to_read > max_len)
	{
		bytes_to_read = max_len;
	}

	// 如果有数据可读
	if (bytes_to_read > 0)
	{
		// 处理环形缓冲区绕回的情况
		if (PreWrite_index + bytes_to_read > RX_BUFFER_SIZE)
		{
			// 数据分成两段
			uint16_t len1 = RX_BUFFER_SIZE - PreWrite_index;
			memcpy(buffer, &rxBuffer[PreWrite_index], len1);
			memcpy(buffer + len1, rxBuffer, bytes_to_read - len1);
			PreWrite_index = bytes_to_read - len1;
		}
		else
		{
			// 数据在一段连续内存中
			memcpy(buffer, &rxBuffer[PreWrite_index], bytes_to_read);
			PreWrite_index += bytes_to_read;
		}
	}
	return bytes_to_read; // 返回实际读取的字节数
}

/**
 * @brief 向ESP8266发送GET请求
 * 
 */
void ESP8266_GetResponse(void)
{
	//开启透传模式
	if (!ESP8266_SendCmd(set_cipmode, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t*)"AT CIPMODE OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t*)"AT CIPMODE ERROR\r\n");
		return;
	}
	
	HAL_Delay(100);

	if (!ESP8266_SendCmd(connect_server, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t*)"Connect Server Successed!\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t*)"Connect Server Failed!\r\n");
		return;
	}

	HAL_Delay(500);

	//向服务器发送GET请求
	char get_info[512];
	sprintf(get_info, "GET /v3/weather/daily.json?key=%s&location=%s&language=en&unit=c&start=0&days=%s HTTP/1.1\r\nHost: api.seniverse.com\r\n\r\n\r\n", key, location, days);
	ESP8266_SendData((uint8_t*)get_info);
}

/**
 * @brief 初始化ESP8266
 * 
 * @return uint8_t 0：成功，1：失败
 */
uint8_t ESP8266_Init(void)
{
	ESP8266_Clear();
	
	uint8_t initSuccess = 1; // 初始化成功标志
	uint8_t times = 5;	//wifi重连次数

	/* 测试AT指令 */
	if (!ESP8266_SendCmd(test_at, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t*)"AT TEST OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t*)"AT TEST ERROR\r\n");
		initSuccess = 0;
		goto INIT_END;
	}

	HAL_Delay(500);

	/* 设置STA模式 */
	if (!ESP8266_SendCmd(set_sta_mode, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t*)"AT CWMODE OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t*)"AT CWMODE ERROR\r\n");
		initSuccess = 0;
		goto INIT_END;
	}

	HAL_Delay(500);

	/* 设置DHCP */
	if (!ESP8266_SendCmd(set_dhcp, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t*)"AT CWDHCP OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t*)"AT CWDHCP ERROR\r\n");
		initSuccess = 0;
		goto INIT_END;
	}

	HAL_Delay(500);

	/* 连接WIFI，最多重试5次 */
	while (ESP8266_SendCmd(connect_wifi, "GOT IP") && (times > 0))
	{
		HAL_Delay(1000);
		times--;
	}
	
	if (times != 0)
	{
		uart_transmit_str(huart3, (uint8_t*)"WIFI Connect Success!\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t*)"WIFI Connect Failed!\r\n");
		initSuccess = 0;
		goto INIT_END;
	}

	HAL_Delay(500);

	uart_transmit_str(huart3, (uint8_t*)"ESP8266 Init OK!\r\n");

INIT_END:
    if(!initSuccess)
    {
        uart_transmit_str(huart3, (uint8_t*)"ESP8266 Init Failed!\r\n");
        // 这里可以添加错误处理代码，比如重启模块或者进入错误状态
    }
	return initSuccess;
}
