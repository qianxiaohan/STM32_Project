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

//LVGLʹ��
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "events_init.h"
#include "gui_guider.h"

//FreeRTOSʹ��
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ȫ�ֱ��������ڷ������ */
lv_ui guider_ui;

/* ���о�� */
QueueHandle_t   ui_update_queue;

typedef struct 
{
	lv_obj_t *target;	//Ҫִ�и��µ�Ŀ��ؼ�
	void (*update_func)(lv_obj_t *, void *);	//Ҫִ�еĺ���
	void *data;	//Ҫ���µ�����
}UI_Update_Message;

// FreeRTOS����
void vLVGLTask(void *pvParameters);
void vTHTask(void *pvParameters);
void vKeyTask(void *pvParameters);

/* �������� */
void process_ui_messages(void);	//ִ��UIˢ��
void safe_ui_update(lv_obj_t* target, 
                     void (*func)(lv_obj_t*, void*),
                     void* data, 
                     size_t data_size);	//ģ�庯��������ִ��ˢ�µľ������
void update_label_text(lv_obj_t* label, const char* text);	//ˢ��label

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	TIM3_Int_Init(0xffff - 1, 84 - 1);	//��ʼ������������������1us������delay����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	
	LED_Init();				//��ʼ��LED 
	KEY_Init(); 			//������ʼ�� 

	lv_init();                          // lvgl��ʼ��
    lv_port_disp_init();                // ��ʾ�豸��ʼ��
    lv_port_indev_init();               // �����豸��ʼ��
   
	/* ʹ��GUI GUIDIER������UI */
	setup_ui(&guider_ui);
	events_init(&guider_ui);
	/*-----------------------*/

	/* ��������Ϊ20�Ķ��� */
	ui_update_queue = xQueueCreate(20, sizeof(UI_Update_Message));

	 /* �������� */
    xTaskCreate(vLVGLTask,        /* ʵ�ָ������ָ�� */
                "LED1",           /* ��������֣���ʵ������ */
                512,              /* ��ջ��ȣ�����Ϊ��λ�� */
                NULL,             /* ��ʹ��������� */
                4,                /* ���ȼ��ȼ�Ϊ4 */
                NULL); 			  /* ��ʹ�������� */

	xTaskCreate(vKeyTask, "key", 128, NULL, 3, NULL);

	xTaskCreate(vTHTask, "THTask", 128, NULL, 1, NULL);
	
	 /* �������������Ա�����ʼִ�� */
    vTaskStartScheduler();

	while (1)
	{
		
	}
}

/* LVGL���� */
void vLVGLTask(void *pvParameters)
{
	const TickType_t waitTicks = pdMS_TO_TICKS(5);
	for(;;)
	{
		lv_timer_handler();
		process_ui_messages(); //ִ��Delay
		vTaskDelay(waitTicks);
	}
}

/* �������ݣ�ִ�����ݸ��²��� */
void process_ui_messages(void) {
      UI_Update_Message msg;
      if(xQueueReceive(ui_update_queue, &msg, 0) == pdTRUE) {
          if(msg.target && msg.update_func) {
              msg.update_func(msg.target, msg.data);
          }
          // �ͷŶ�̬��������ݣ������Ҫ���ͷţ�
          if(msg.data) 
          {
              free(msg.data);
          }
      }
}

/* ����UI�ؼ�ģ�� */
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

//���±�ǩ�ı�
void update_label_text(lv_obj_t *label, const char *text)
{
	safe_ui_update(	label, 
					(void (*)(lv_obj_t*, void*))lv_label_set_text,
					(void*)text, 
                    strlen(text)+1);
}

/* ģ���ȡ��ʪ�ȣ���ִ��ˢ��UI�Ĳ��� */
void vTHTask(void *pvParameters)
{
	const TickType_t waitTicks = pdMS_TO_TICKS(10000);
	for(;;)
	{
		uint8_t humi = (rand() % 61) + 30;	//ʪ�ȣ�30~90
		uint8_t temp = (rand() % 16) + 20;	//�¶ȣ�20~35
		char buffer1[10];
		char buffer2[10];
		snprintf(buffer1, sizeof(buffer1), "T: %dC", temp);
		snprintf(buffer2, sizeof(buffer2), "H: %d%%", humi);
		update_label_text(guider_ui.screen_label_temp, buffer1);
		update_label_text(guider_ui.screen_label_humi, buffer2);
		vTaskDelay(waitTicks);	//delay 10s
	}
}

/* �������񣬰���KEY0/KEY1��ˢ�±�ǩ */
void vKeyTask(void *pvParameters)
{    
    /* 20msת��Ϊ��Ӧ��tick�� */
    const TickType_t xDelay20ms = pdMS_TO_TICKS(20);

    /* �жϰ����Ƿ��ͷţ�0���ͷţ�1������ */
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
            key1State = 0;  //�����ͷ�
        }
        
        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == RESET){
            vTaskDelay(xDelay20ms);	//��ʱ20ms��������������
            if((GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == RESET) && (key2State == 0)){
                key2State = 1;
                char buffer1[] = "Normal";
				update_label_text(guider_ui.screen_label_status, buffer1);
            }
        } else{
            key2State = 0;  //�����ͷ�
        }
        vTaskDelay(50);
    }
}

/* ʵ��Tick���Ӻ�����FreeRTOS��ÿ��Tick��1Tick=1ms���������һ�θú��� */
void vApplicationTickHook( void )
{
	lv_tick_inc(1);	/* LVGL��Ҫ���������ڸ���LVGLʱ�����1ms */
}
