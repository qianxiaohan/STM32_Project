#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "touch.h"
#include "timer.h"
#include <stdlib.h>
#include <time.h>
#include "stdio.h"

//LVGL使用
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "events_init.h"
#include "gui_guider.h"

//FreeRTOS使用
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* 全局变量，用于访问组件 */
lv_ui guider_ui;

/* 队列句柄 */
QueueHandle_t   ui_update_queue;

typedef struct 
{
	lv_obj_t *target;	//要执行更新的目标控件
	void (*update_func)(lv_obj_t *, void *);	//要执行的函数
	void *data;	//要更新的数据
}UI_Update_Message;

// FreeRTOS任务
void vLVGLTask(void *pvParameters);
void vTHTask(void *pvParameters);
void vKeyTask(void *pvParameters);

/* 声明函数 */
void process_ui_messages(void);	//执行UI刷新
void safe_ui_update(lv_obj_t* target, 
                     void (*func)(lv_obj_t*, void*),
                     void* data, 
                     size_t data_size);	//模板函数，决定执行刷新的具体操作
void update_label_text(lv_obj_t* label, const char* text);	//刷新label

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	TIM3_Int_Init(0xffff - 1, 84 - 1);	//初始化计数器，计数周期1us，用于delay函数
	uart_init(115200);		//初始化串口波特率为115200
	
	LED_Init();				//初始化LED 
	KEY_Init(); 			//按键初始化 

	lv_init();                          // lvgl初始化
    lv_port_disp_init();                // 显示设备初始化
    lv_port_indev_init();               // 输入设备初始化
   
	/* 使用GUI GUIDIER创建的UI */
	setup_ui(&guider_ui);
	events_init(&guider_ui);
	/*-----------------------*/

	/* 创建长度为20的队列 */
	ui_update_queue = xQueueCreate(20, sizeof(UI_Update_Message));

	 /* 创建任务 */
    xTaskCreate(vLVGLTask,        /* 实现该任务的指针 */
                "LED1",           /* 任务的名字，无实际意义 */
                512,              /* 堆栈深度（以字为单位） */
                NULL,             /* 不使用任务参数 */
                4,                /* 优先级等级为4 */
                NULL); 			  /* 不使用任务句柄 */

	xTaskCreate(vKeyTask, "key", 128, NULL, 3, NULL);

	xTaskCreate(vTHTask, "THTask", 128, NULL, 1, NULL);
	
	 /* 启动调度器，以便任务开始执行 */
    vTaskStartScheduler();

	while (1)
	{
		
	}
}

/* LVGL任务 */
void vLVGLTask(void *pvParameters)
{
	const TickType_t waitTicks = pdMS_TO_TICKS(5);
	for(;;)
	{
		lv_timer_handler();
		process_ui_messages(); //执行Delay
		vTaskDelay(waitTicks);
	}
}

/* 处理数据，执行数据更新操作 */
void process_ui_messages(void) {
      UI_Update_Message msg;
      if(xQueueReceive(ui_update_queue, &msg, 0) == pdTRUE) {
          if(msg.target && msg.update_func) {
              msg.update_func(msg.target, msg.data);
          }
          // 释放动态分配的数据（如果需要则不释放）
          if(msg.data) 
          {
              free(msg.data);
          }
      }
}

/* 更新UI控件模板 */
void safe_ui_update(lv_obj_t *target, void (*func)(lv_obj_t*, void*), void* data, size_t data_size)
{
	UI_Update_Message msg = {
		.target = target,
		.update_func = func,
		.data = NULL
	};

	if(data && data_size > 0) {
		msg.data = pvPortMalloc(data_size);
		if(msg.data) memcpy(msg.data, data, data_size);
	}

	xQueueSend(ui_update_queue, &msg, portMAX_DELAY);
}

//更新标签文本
void update_label_text(lv_obj_t *label, const char *text)
{
	safe_ui_update(	label, 
					(void (*)(lv_obj_t*, void*))lv_label_set_text,
					(void*)text, 
                    strlen(text)+1);
}

/* 模拟读取温湿度，并执行刷新UI的操作 */
void vTHTask(void *pvParameters)
{
	const TickType_t waitTicks = pdMS_TO_TICKS(10000);
	for(;;)
	{
		uint8_t humi = (rand() % 61) + 30;	//湿度：30~90
		uint8_t temp = (rand() % 16) + 20;	//温度：20~35
		char buffer1[10];
		char buffer2[10];
		snprintf(buffer1, sizeof(buffer1), "T: %dC", temp);
		snprintf(buffer2, sizeof(buffer2), "H: %d%%", humi);
		update_label_text(guider_ui.screen_label_temp, buffer1);
		update_label_text(guider_ui.screen_label_humi, buffer2);
		vTaskDelay(waitTicks);	//delay 10s
	}
}

/* 按键任务，按下KEY0/KEY1，刷新标签 */
void vKeyTask(void *pvParameters)
{    
    /* 20ms转换为对应的tick数 */
    const TickType_t xDelay20ms = pdMS_TO_TICKS(20);

    /* 判断按键是否被释放，0：释放，1：按下 */
    uint16_t key1State = 0, key2State = 0;

    for(;;)
    {
        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == RESET){
            vTaskDelay(xDelay20ms);
            if((GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == RESET) && (key1State == 0)){
                key1State = 1;
                char buffer1[] = "AbNormal";
				update_label_text(guider_ui.screen_label_status, buffer1);
				
            }
        } else{
            key1State = 0;  //按键释放
        }
        
        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == RESET){
            vTaskDelay(xDelay20ms);	//延时20ms，消除按键抖动
            if((GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == RESET) && (key2State == 0)){
                key2State = 1;
                char buffer1[] = "Normal";
				update_label_text(guider_ui.screen_label_status, buffer1);
            }
        } else{
            key2State = 0;  //按键释放
        }
        vTaskDelay(50);
    }
}

/* 实现Tick钩子函数，FreeRTOS在每个Tick（1Tick=1ms）都会调用一次该函数 */
void vApplicationTickHook( void )
{
	lv_tick_inc(1);	/* LVGL必要函数，用于告诉LVGL时间过了1ms */
}
