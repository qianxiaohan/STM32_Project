// FreeRTOS消息缓冲区示例

#include "stm32f10x.h" // Device header

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" /* 信号量所在头文件 */
#include "queue.h"
#include "message_buffer.h" //消息缓冲区所需头文件
#include "oled.h"
#include "usart.h"
#include "oled_image.h"
#include "string.h"
#include "stdio.h"

#define MESSAGE_SIZE 300

/* 消息缓冲区 */
MessageBufferHandle_t messageBuffer;

/* 任务 */
void vKeyTask(void *pvParameters);
void vOLEDTask(void *pvParameters);

/* 结构体 */
typedef struct MyMessage_t{
    uint8_t head;    //帧头
    uint8_t message[sizeof(smile_emoji)];  //数据
}MyMessage_t;

// 硬件初始化
void Hardware_Init(void);

int main(void)
{
    /* 初始化硬件 */
    Hardware_Init();

    /* 创建大小为300*2个字节的消息缓冲区 */
    messageBuffer = xMessageBufferCreate(MESSAGE_SIZE * 2);

    xTaskCreate(vOLEDTask, "OLED", 128, NULL, 1, NULL);
    xTaskCreate(vKeyTask, "Key", 128, NULL, 2, NULL);

    /* 启动调度器，以便任务开始执行 */
    vTaskStartScheduler();

    while (1){}
}

/* OLED显示任务 */
void vOLEDTask(void *pvParameters)
{
    const TickType_t xTicksToWait = pdMS_TO_TICKS(200);

    /* 接收到缓冲区的大小 */
    size_t xReceivedBytes;

    MyMessage_t ReciveMessage; /* 接收原始数据的缓冲区 */

    for(;;)
    {
        /* 阻塞等待200ms 消息缓冲区 */
        xReceivedBytes = xMessageBufferReceive(  messageBuffer, 
                                                (void *)&ReciveMessage,
                                                sizeof(ReciveMessage),
                                                xTicksToWait );
        /* 收到数据，刷新OLED */
        if (xReceivedBytes > 0)
        {
            /* 对接收到的原始数据进行解包 */
           
            if(ReciveMessage.head == 0x00)
            {
                OLED_Clear();
                OLED_ShowString(1, 8, "smile");
                OLED_DrawImage(0, 2, 48, 48, ReciveMessage.message);
                OLED_Update();
            }
            else if(ReciveMessage.head == 0x01)
            {
                OLED_Clear();
                OLED_ShowString(1, 10, "cry");
                OLED_DrawImage(16, 2, 48, 48, ReciveMessage.message);
                OLED_Update();
            }

            /* 清空消息缓冲区数据 */
            xMessageBufferReset(messageBuffer);
        }
        vTaskDelay(xTicksToWait); //延时200ms
    }   
}


/* 按键扫描任务 */
void vKeyTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    /* 获取当前tick数 */
    xLastWakeTime = xTaskGetTickCount();

    MyMessage_t message_t;
    
    /* 20ms转换为对应的tick数 */
    const TickType_t xDelay20ms = pdMS_TO_TICKS(20);

    const TickType_t xTicksToWait = pdMS_TO_TICKS(200);

    /* 判断按键是否被释放，0：释放，1：按下 */
    uint16_t key1State = 0, key2State = 0;
    // 发送的字节数
    size_t xBytesSent;

    for(;;)
    {
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == RESET){
            vTaskDelayUntil(&xLastWakeTime, xDelay20ms); //延时20ms，消除按键抖动
            if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == RESET) && (key1State == 0)){
                key1State = 1;
                message_t.head = 0x00;
                memcpy(message_t.message, cry_emoji, sizeof(cry_emoji));
                xBytesSent = xMessageBufferSend(messageBuffer, 
                                                &message_t, 
                                                sizeof(message_t), 
                                                xTicksToWait);

                /* 调试用，发送的字节数不是源数据的大小则打印0，否则为1 */
                if(xBytesSent != sizeof(message_t))
                {
                    USART_SendData(USART1, 0);
                }
                else{
                    USART_SendData(USART1, 1);

                }
                /*--------------------------*/
            }
        } else{
            key1State = 0;  //按键释放
        }
        
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == RESET){
            vTaskDelayUntil(&xLastWakeTime, xDelay20ms); //延时20ms，消除按键抖动
            if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == RESET) && (key2State == 0)){
                key2State = 1;
                message_t.head = 0x01;
                memcpy(message_t.message, smile_emoji, sizeof(smile_emoji));
                xBytesSent = xMessageBufferSend(messageBuffer, 
                                                &message_t, 
                                                sizeof(message_t), 
                                                xTicksToWait);

                if(xBytesSent != sizeof(message_t))
                {
                    USART_SendData(USART1, 0);
                }
                else
                {
                    USART_SendData(USART1, 1);
                }
            }
        } else{
            key2State = 0;  //按键释放
        }
        vTaskDelay(50);
    }   
}

void Hardware_Init(void)
{
    /* 初始化2个按键：PB0、PB11 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 初始化OLED */
    OLED_Init();

    /* 显示字符 */
    OLED_ShowString(1, 1, "please press ");
    OLED_ShowString(2, 1, "key1 or key2");
    OLED_Update();

    /* 初始化串口 */
    MyUSART_Init();

}


