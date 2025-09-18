#include "rtc.h"
#include "usart.h"
#include <stdio.h>

uint8_t USART_Scanf(uint32_t value);
void NVIC_Configuration(void);


uint8_t time_display = 0;

/**
 * 
 * @retval 
 * 
 */

void RTC_Configuration(void)
{
    /* 开启PWR、BKP时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* 允许访问Backup和RTC寄存器 */
    PWR_BackupAccessCmd(ENABLE);

    /* 复位BKP */
    BKP_DeInit();

    /* 使能LSE */
    RCC_LSEConfig(RCC_LSE_ON);

    /* 等待LSE准备好 */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* 选择LSE做为RTC时钟源 */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* 使能RTC时钟 */
    RCC_RTCCLKCmd(ENABLE);

    /* 固定操作，在读写之前，必须等待RTC寄存器同步 */
    RTC_WaitForSynchro();

    /* Waits until last write operation on RTC registers has finished. */
    RTC_WaitForLastTask();

    /* 每秒产生一次中断 */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Waits until last write operation on RTC registers has finished. */
    RTC_WaitForLastTask();

    /* 设置预分频 */
    RTC_SetPrescaler(32767);    //RTC时钟周期 = (32.768KHz) / (32767 + 1) = 1Hz = 1s

    /* Waits until last write operation on RTC registers has finished. */
    RTC_WaitForLastTask();
}

void MyRTC_Init(void)
{
    NVIC_Configuration();
    if(BKP_ReadBackupRegister(BKP_DR1) != 0x1234)
    {
        printf("\r\n\n RTC not yet configured....");

        RTC_Configuration();    //设置

        printf("\r\n RTC configured....");

        Time_Adjust();

        BKP_WriteBackupRegister(BKP_DR1, 0x1234);
    }
    else
    {
        /* 检查电源复位标志位是否被设置 */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            printf("\r\n\n Power On Reset occurred....");
        }
        /* 检查电源复位标志位是否被设置 */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            printf("\r\n\n External Reset occurred....");
        }

        printf("\r\n No need to configure RTC....");
        /* 等待RTC同步 */
        RTC_WaitForSynchro();

        /* 使能RTC秒中断 */
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
 
        /* 等待RTC寄存器上的最后一次写入操作完成 */
        RTC_WaitForLastTask();
    }

    /* 清除复位标志位 */
    RCC_ClearFlag();


}

void NVIC_Configuration(void)
{
    /* NVIC设置分组 */
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;	//使能RTC中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  调整时间
  * @param  none
  * @retval none
  */
void Time_Adjust(void)
{
    /* 等待上次对RTC寄存器的写操作完成 */
    RTC_WaitForLastTask();

    /* 设置RTC计数器的值 */
    RTC_SetCounter(Time_Regulate());

    /* 等待上次对RTC寄存器的写操作完成 */
    RTC_WaitForLastTask();
}

/**
  * @brief  通过串口设置时间
  * @param  none
  * @retval 用秒表示的时间
  */
uint32_t Time_Regulate(void)
{
    uint32_t Tmp_HH = 0xFF, Tmp_MM = 0xFF, Tmp_SS = 0xFF;

    printf("\r\n==============Time Settings=====================================");
    printf("\r\n  Please Set Hours");
    
    while(Tmp_HH == 0xFF)
    {
        Tmp_HH = USART_Scanf(23);
    }
    printf(":  %d", Tmp_HH);
    printf("\r\n  Please Set Minutes");
    while(Tmp_MM == 0xFF)
    {
        Tmp_MM = USART_Scanf(59);
    }
    printf(":  %d", Tmp_MM);
    printf("\r\n  Please Set Seconds");
    while(Tmp_SS == 0xFF)
    {
        Tmp_SS = USART_Scanf(59);
    }
    printf(":  %d", Tmp_SS);

    /* 将输入的值转换为Counter CNT */
    return (Tmp_HH * 60 * 60 + Tmp_MM * 60 + Tmp_SS);
}

/**
  * @brief  显示实时时间
  * @param  none
  * @retval none
  */
void Time_Show(void)
{
    while(1)
    {
        if(time_display == 1)
        {
            Time_Display(RTC_GetCounter());
            time_display = 0;   //通过RTC中断来置1
        }
    }
}


/**
  * @brief  将时间按照指定格式显示
  * @param  value RTC计数寄存器的值
  * @retval none
  */
void Time_Display(uint32_t value)
{
    /* 当前时间为23:59:59时，复位RTC */
    if(RTC_GetCounter() == 0x0001517F)
    {
        RTC_SetCounter(0x00);
         /* Waits until last write operation on RTC registers has finished. */
        RTC_WaitForLastTask();
    }
    uint32_t Tmp_HH = 0x00, Tmp_MM = 0x00, Tmp_SS = 0x00;
    Tmp_HH = value / 3600;  //小时
    Tmp_MM = (value % 3600) / 60;   //分钟
    Tmp_SS = (value % 3600) % 60; //秒

    printf("Time: %0.2d:%0.2d:%0.2d\r", Tmp_HH, Tmp_MM, Tmp_SS);
}


/**
  * @brief  获取来自串口的数据
  * @param  value 允许串口输入的最大值
  * @retval 转换后的串口的数据
  */
uint8_t USART_Scanf(uint32_t value)
{
    uint32_t index = 0;
    uint8_t rev[2] = {0, 0};

    /* 等待RXNE = 1 */
    while(index < 2)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        {
        }
        rev[index++] = USART_ReceiveData(USART1);
        if(rev[index - 1] < 0x30 || rev[index - 1] > 0x39)
        {
            printf("\n\rPlease enter valid number between 0 and 9");
            index--;
        }
    }
    
    index = ((rev[0] - 0x30) * 10) + (rev[1] - 0x30);     //获取输入的值

    /* 检验 */
    if(index > value)
    {
        printf("\n\rPlease enter valid number between 0 and %d", value);
        return 0xFF;
    }
 
    return index;
}
