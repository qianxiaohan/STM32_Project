//FreeRTOS实现两个灯同时闪烁

#include "stm32f10x.h"                  // Device header

#include "FreeRTOS.h"
#include "task.h"

//两个任务
void vTask1(void * pvParameters);
void vTask2(void * pvParameters);

//硬件初始化
void Hardware_Init(void);

int main(void)
{

	Hardware_Init();

	/* 创建任务 */
	xTaskCreate(vTask1, 	/* 实现该任务的指针 */
				"LED1", 	/* 任务的名字，无实际意义 */
				512, 		/* 堆栈深度（以字为单位） */
				NULL, 		/* 不使用任务参数 */
				1, 			/* 优先级等级为1 */
				NULL);		/* 不使用任务句柄 */
	xTaskCreate(vTask2, "LED2", 512, NULL, 1, NULL);

	/* 启动调度器，以便任务开始执行 */
	vTaskStartScheduler();
}

void vTask1(void *pvParameters)
{
	while(1)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		vTaskDelay(1000);
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
		vTaskDelay(1000);
	}
	
}

void vTask2(void *pvParameters)
{
	while (1)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		vTaskDelay(1000);
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
		vTaskDelay(1000);
	}
	
	
}

void Hardware_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1);
}
