// FreeRTOS任务通知示例

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

/* 任务 */
void vLedTask(void *pvParameters);
void vKeyTask(void *pvParameters);
void vLight_SensorTask(void *pvParameters);
void vOLEDTask(void *pvParameters);
void vUartTask(void *pvParameters);

/* 计数、二值信号量 */
SemaphoreHandle_t countingSemaphore, binarySemaphore;
SemaphoreHandle_t xTxSemaphore;

/* 任务句柄用于任务通知 */
TaskHandle_t xTaskToNotify;

// 硬件初始化
void Hardware_Init(void);

/* 不带任务通知的串口发送任务 */
void UART_low_level_send(USART_TypeDef *USARTx, uint8_t *pucDataSource, uint8_t uxLength );

/* 串口发送任务 */
BaseType_t xUART_Send(USART_TypeDef *USARTx, uint8_t *pucDataSource, uint8_t uxLength );


int main(void)
{

    Hardware_Init();

    /* 创建计数信号量 */
    countingSemaphore = xSemaphoreCreateCounting(10, 0);

    /* 创建二值信号量 */
    binarySemaphore = xSemaphoreCreateBinary();

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
        // 每50ms尝试获取一次信号量
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

    /* 初始化串口 */
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

/* 使用任务通知的串口发送函数 */
BaseType_t xUART_Send(USART_TypeDef *USARTx, uint8_t *pucDataSource, uint8_t uxLength )
{
    BaseType_t xReturn;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(50);

    /* 获取当前任务的句柄 */
    xTaskToNotify = xTaskGetCurrentTaskHandle();

    /*  清除可能存在的旧通知，
    
        将通知值作为返回值后，再将通知值清0 
    */
    ulTaskNotifyTake(pdTRUE, 0);

    UART_low_level_send(USARTx, pucDataSource, uxLength);

    /* 
        阻塞（当前任务），直到收到传输完成的通知。如果收到了通知，则 xReturn 将被设置为 1 (pdTRUE)，因为 ISR（中断服务程序）会将此任务的通知值递增到 1。如果操作超时，则 xReturn 将为 0 (pdFALSE)，因为此任务的通知值自上次被清零为 0 后未曾改变。请注意，如果 ISR 在调用 UART_low_level_send() 之后、调用 ulTaskNotifyTake() 之前执行，那么该事件将被锁存（latched）在任务的通知值中，随后对 ulTaskNotifyTake() 的调用将立即返回。
    */
    xReturn = (BaseType_t)ulTaskNotifyTake( pdTRUE, 
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
        xSemaphoreGiveFromISR(xTaskToNotify, &xHigherPriorityTaskWoken);
        xTaskToNotify = NULL;   //没有任务等待被通知，因此设置为NULL
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //从ISR切换到最高优先级的其他任务
    }
}

