// FreeRTOS信号量示例

#include "stm32f10x.h" // Device header

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" /* 信号量所在头文件 */
#include "queue.h"
#include "oled.h"
#include "adc.h"
#include "usart.h"

/* 待送的串口数据 */
const uint8_t size = 12;
uint16_t cnt = 0;   //计数
uint8_t TxBuffer[size] = "Hello USART";

// 四个任务
void vLedTask(void *pvParameters);
void vKeyTask(void *pvParameters);
void vLight_SensorTask(void *pvParameters);
void vOLEDTask(void *pvParameters);
void vUartTask(void *pvParameters);

/* 计数、二值信号量 */
SemaphoreHandle_t countingSemaphore, binarySemaphore;
SemaphoreHandle_t xTxSemaphore;


// 硬件初始化
void Hardware_Init(void);

void UART_low_level_send(USART_TypeDef *USARTx, uint8_t *pucDataSource, uint8_t uxLength );

BaseType_t xUART_Send(USART_TypeDef *USARTx, uint8_t *pucDataSource, uint8_t uxLength );


int main(void)
{

    Hardware_Init();

    /* 创建计数信号量 */
    countingSemaphore = xSemaphoreCreateCounting(10, 0);

    /* 创建二值信号量 */
    binarySemaphore = xSemaphoreCreateBinary();
    xTxSemaphore = xSemaphoreCreateBinary();

    /* 创建任务 */
    xTaskCreate(vLedTask,        /* 实现该任务的指针 */
                "LED",           /* 任务的名字，无实际意义 */
                128,              /* 堆栈深度（以字为单位） */
                NULL,             /* 不使用任务参数 */
                1,                /* 优先级等级为1 */
                NULL);             /* 不使用任务句柄 */

    xTaskCreate(vOLEDTask, "OLED", 128, NULL, 4, NULL);

    xTaskCreate(vLight_SensorTask, "Light_Sensor", 128, NULL, 3, NULL);

    xTaskCreate(vKeyTask, "Key", 128, NULL, 5, NULL);

    xTaskCreate(vUartTask, "UART", 128, NULL, 1, NULL);

     /* 启动调度器，以便任务开始执行 */
    vTaskStartScheduler();

    while (1){}
}

/* LED任务 */
void vLedTask(void *pvParameters)
{
    const TickType_t xTicksToWait = pdMS_TO_TICKS(50);
    for(;;)
    {    
        // 每50ms尝试获取一次信号    
        if(xSemaphoreTake(binarySemaphore, xTicksToWait) == pdTRUE){
            //如果获取到了信号量则点亮LED灯
            GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 );    
            xSemaphoreGive(binarySemaphore); //释放信号量
        }else{
            //没有获取到二值信号量，则熄灭所有灯
            GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 );
        }
    }
}

/* 光照数据采集任务 */
void vLight_SensorTask(void *pvParameters)
{
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    uint16_t light_value;
    for(;;)
    {
        light_value = Get_LightValue(10);
        OLED_ShowNum(3, 7, light_value, 3);
        if(light_value < 15){ //光照强度小于15%
            xSemaphoreGive(binarySemaphore);  /* 释放二值信号量，点亮LED灯 */
        }else{
            xSemaphoreTake(binarySemaphore, 0);  /* 获取二值信号量 */
        }
        vTaskDelay(xTicksToWait);    //100ms的采样周期
    }
}

/* OLED显示任务 */
void vOLEDTask(void *pvParameters)
{
    const TickType_t xTicksToWait = pdMS_TO_TICKS(200);
    UBaseType_t current_cnt;
    for(;;)
    {
        current_cnt = uxSemaphoreGetCount(countingSemaphore);
        OLED_ShowNum(1, 10, (uint32_t)current_cnt, 2);
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2) == RESET)
        {
            OLED_ClearArea(2, 5, 3);
            OLED_ShowString(2, 5, "ON");
        }
        else
        {
            OLED_ClearArea(2, 5, 3);
            OLED_ShowString(2, 5, "OFF");
        }
        vTaskDelay(xTicksToWait); //延时200ms
    }   
}

/* 串口发送任务 */
void vUartTask(void *pvParameters)
{
    for(;;)
    {
        xUART_Send(USART1, TxBuffer, size);
        vTaskDelay(2000);   //每2s串口发送数据
    }
}

/* 按键扫描任务 */
void vKeyTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    /* 获取当前tick数 */
    xLastWakeTime = xTaskGetTickCount();
    
    /* 20ms转换为对应的tick数 */
    const TickType_t xDelay20ms = pdMS_TO_TICKS(20);

    /* 判断按键是否被释放，0：释放，1：按下 */
    uint16_t key1State = 0, key2State = 0;

    for(;;)
    {
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == RESET){
            vTaskDelayUntil(&xLastWakeTime, xDelay20ms); //延时20ms，消除按键抖动
            if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == RESET) && (key1State == 0)){
                key1State = 1;
                xSemaphoreTake(countingSemaphore, 0);   //尝试获取信号量
            }
        } else{
            key1State = 0;  //按键释放
        }
        
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == RESET){
            vTaskDelayUntil(&xLastWakeTime, xDelay20ms); //延时20ms，消除按键抖动
            if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == RESET) && (key2State == 0)){
                key2State = 1;
                xSemaphoreGive(countingSemaphore);  //释放信号量
            }
        } else{
            key2State = 0;  //按键释放
        }
        vTaskDelay(50);
    }   
}

void Hardware_Init(void)
{
    /* 初始化三个LED灯：PA0、PA1、PA2 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 );

    /* 初始化2个按键：PB0、PB11 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 初始化OLED */
    OLED_Init();

    OLED_ShowString(1, 1, "Resource:");
    OLED_ShowNum(1, 10, 0, 2);
    OLED_ShowString(2, 1, "LED ON");
    OLED_ShowString(3, 1, "Light:");
    OLED_ShowNum(3, 7, 0, 3);
    OLED_ShowString(3, 10, "%");

    MyUSART_Init();

    /* 配置ADC */
    ADC_Configuration();
}

/* 串口发送函数 */
void UART_low_level_send(USART_TypeDef *USARTx, uint8_t *pucDataSource, uint8_t uxLength )
{
    /* 关闭USART发送中断 */
    USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);

    if(uxLength > 0)
    {
        /* 开启发送中断 */
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
}

/* 使用二值信号量的串口发送函数 */
BaseType_t xUART_Send(USART_TypeDef *USARTx, uint8_t *pucDataSource, uint8_t uxLength )
{
    BaseType_t xReturn;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(50);
    /* 确保通过无超时方式尝试获取信号量，用于检查UART发送信号量是否仍处于被占用的锁定状态。 */
    // 如果信号量可用（为1），则获取它并将其置0
    // 如果信号量不可用（为0），则立即返回失败
    // 确保信号量起始状态为"空"（0）
    xSemaphoreTake(xTxSemaphore, 0);

    UART_low_level_send(USARTx, pucDataSource, uxLength);

    /* 
        通过阻塞信号量以等待传输完成。若成功获取信号量，则xReturn将被设置为pdPASS；若获取信号量的操作超时，则xReturn将被设置为pdFAIL。需注意：若中断发生在调用UART_low_level_send()之后、调用xSemaphoreTake()之前，则事件会被二进制信号量锁存，此时调用xSemaphoreTake()将立即返回。
    */
    xReturn = xSemaphoreTake( xTxSemaphore, 
                              xTicksToWait);

    return xReturn;
}


/* USART1中断处理函数 */
void USART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /**
     * 发送中断
     * 
     */
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        if(cnt < size) /* 还有数据发送 */
        {
            USART_SendData(USART1, TxBuffer[cnt++]);
        }
        else
        {
            /* 发送完毕，关闭发送中断、开启发送完成中断 */
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            USART_ITConfig(USART1, USART_IT_TC, ENABLE);
        }
    }

    if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        cnt = 0;
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);   //关闭TC中断
        USART_ClearITPendingBit(USART1, USART_IT_TC);   //清除TC中断标志位
        xSemaphoreGiveFromISR(xTxSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //从ISR切换到最高优先级的其他任务
    }
}

