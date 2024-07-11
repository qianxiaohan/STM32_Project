STM32F103C8T6提供了[USART](https://baike.baidu.com/item/USART?fromModule=lemma_search-box)外设实现设备间的串行通信。

USART框图：

 ![](D:\嵌入式学习\Stm32笔记\图片\USART框图.png)

RTS、CTS是硬件控制流的引脚，在实际中很少使用RTS、CTS。可以将此框图简化：

 <img src="D:\嵌入式学习\Stm32笔记\图片\USART简化框图.png" style="zoom: 67%;" />

### USART初始化流程图

 ![](D:\嵌入式学习\Stm32笔记\图片\USART初始化流程.png)

使用结构体初始化时，结构体成员的取值可以参照《STM32F103xx固件函数库用户手册》

### 串口初始化

根据手册对USART、以及对应的GPIO进行配置，全双工通信方式下USART_TX的GPIO设置为复用推挽输出，USART_RX的GPIO设置为浮空输入/上拉输入

 ![](D:\嵌入式学习\Stm32笔记\图片\USART配置1.png)

 ![](D:\嵌入式学习\Stm32笔记\图片\USART配置2.png)

```c
void COM_Init()
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    /* 配置GPIO */
    GPIO_InitTypeDef GPIO_InitStructure;

    /*	配置USARTx Tx的GPIO口 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*	配置USARTx Rx的GPIO口 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /**
	 *  串口参数配置：
	 * 	波特率 115200、无硬件控制流、无校验位、8位数据位、1位停止位、发送/接收模式
	 *  
	 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    /* 配置NVIC */
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* 打开USART接收中断 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    /* 打开USART1 */
    USART_Cmd(USART1, ENABLE);
}

```

### 发送数据

```c
void COM_SendData(uint8_t data)
{
    USART_SendData(USART1, data);

	/**
	 *  TXE
	 *   0: Data is not transferred to the shift register
     *   1: Data is transferred to the shift register
	 * 	
	 * 	TXE = 1：发送完成， TXE = 0：还未发送完成
	 * 
	 */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
}
```

### 中断方式接收数据

```c
/* USART1中断处理函数 */
void USART1_IRQHandler(void)
{
	/**
	 * 判断中断源是否是接收中断
	 * 返回SET则说明是接收中断
	 */
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* 读取数据寄存器中的数据， */
		uint8_t tempdata = USART_ReceiveData(USART1);
		receiveBuffer[receiveCounter++] = tempdata;
        
        /* 将USART1接收到的数据再发送出去 */
		COM_SendData(tempdata);
	}
}
```

### USART发送数据重定向到printf

在keil的设置中勾选上[MicroLib](https://www.keil.com/arm/microlib.asp)

 ![](D:\嵌入式学习\Stm32笔记\图片\microLib设置.png)

包含头文件`stdio.h`，并重写`int fputc(int ch, FILE *f)`，在该函数中实现串口的发送，调用printf函数，会自动调用fputc函数

```c
#include <stdio.h>

/* 重写fputc函数 */
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
```

### 演示结果

 ![](D:\嵌入式学习\Stm32笔记\图片\串口演示结果.png)

### 参考链接

https://www.bilibili.com/video/BV1th411z7sn?p=25&vd_source=834d5a8ee41e57e5e62a70c875800119

《STM32F103xx固件函数库用户手册》

《STM32F10xxx  Reference manual》