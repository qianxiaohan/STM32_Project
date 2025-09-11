// FreeRTOS实现两个灯同时闪烁

#include "stm32f10x.h" // Device header

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" /* 信号量所在头文件 */
#include "queue.h"
#include "oled.h"
#include <stdio.h>
#include "usart.h"
#include "adc.h"

// 两个任务
void vLedTask1(void *pvParameters);
void vLedTask2(void *pvParameters);
void vLight_SensorTask(void *pvParameters);
void vOLEDTask(void *pvParameters);
void vSenderTask(void *pvParameters);
void vReceiverTask(void *pvParameters);

/* 队列 */
QueueHandle_t xLightQueue, xDisplayQueue;

/* 互斥锁 */
SemaphoreHandle_t xMutex;

/* 二值信号量 */
SemaphoreHandle_t xSemaphore1, xSemaphore2;

TaskHandle_t LED1TaskHandle = NULL, LED2TaskHandle = NULL;

// 硬件初始化
void Hardware_Init(void);




int main(void)
{

    Hardware_Init();

    /* 创建互斥量 */
    xMutex = xSemaphoreCreateMutex();
    xLightQueue = xQueueCreate(10, sizeof(uint16_t));
    xDisplayQueue = xQueueCreate(10, sizeof(uint16_t));

    if(xLightQueue == pdFAIL){
        OLED_Clear();
        OLED_ShowString(1, 1, "queue error"); 
    }

    // /* 创建二值信号量 */
    // // xSemaphore1 = xSemaphoreCreateBinary();

    /* 创建任务 */
    xTaskCreate(vLedTask1,        /* 实现该任务的指针 */
                "LED1",           /* 任务的名字，无实际意义 */
                1000,              /* 堆栈深度（以字为单位） */
                NULL,             /* 不使用任务参数 */
                2,                /* 优先级等级为1 */
                &LED1TaskHandle); /* 不使用任务句柄 */
    // xTaskCreate(vLedTask2, "LED2", 128, NULL, 1, &LED2TaskHandle);

    xTaskCreate(vOLEDTask, "OLED", 128, NULL, 1, NULL);

    xTaskCreate(vLight_SensorTask, "Light_Sensor", 128, NULL, 3, NULL);

    

    // /* 启动调度器，以便任务开始执行 */
    vTaskStartScheduler();

    // OLED_ShowString(2, 1, "OLED OK");
    while (1){}
}

void vLedTask1(void *pvParameters)
{
    uint16_t lReceivedValue;
    for(;;)
    {        
        if(xQueueReceive(xLightQueue, &lReceivedValue, portMAX_DELAY) == pdPASS) //阻塞等待队列的数据
        {
            if (lReceivedValue < 1000){
                GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2); //灯全亮
            }else if ( lReceivedValue < 2000 ){
                GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1);
                GPIO_SetBits(GPIOA, GPIO_Pin_2);
            }else if (lReceivedValue < 3000){
                GPIO_ResetBits(GPIOA, GPIO_Pin_0);
                GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);
            }else{
                GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);  //灯全灭
            }
            xQueueSend(xDisplayQueue, &lReceivedValue, 0);
        }
    }
}

void vLedTask2(void *pvParameters)
{
    while (1) {
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        vTaskDelay(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        vTaskDelay(500);
    }
}

/* 光照数据采集任务 */
void vLight_SensorTask(void *pvParameters)
{
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    uint16_t light_value;
    for(;;)
    {
        light_value = ADC_GetConversionValue(ADC1);
        if(xQueueSend(xLightQueue, &light_value, xTicksToWait) != pdPASS)
        {

        }
        vTaskDelay(100);    //100ms的采样周期
    }
}

/* OLED显示任务 */
void vOLEDTask(void *pvParameters)
{
    uint16_t light_value;
    for(;;)
    {
        if(xQueueReceive(xDisplayQueue, &light_value, portMAX_DELAY) == pdPASS)
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);  //进入临界区
            OLED_Clear();
            OLED_ShowNum(1 , 1, light_value, 5);    
            xSemaphoreGive(xMutex); //退出临界区
        }
    }   
}

void Hardware_Init(void)
{
    /* 初始化三个LED灯 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);

    /* 初始化OLED */
    OLED_Init();

    OLED_ShowString(1, 1, "OLED Init...");

    /* 初始化串口1 */
    // MyUSART_Init();

    /* 配置ADC */
    ADC_Configuration();

}
