#include "led.h"

void LED_Init(void)
{
    /* 开启外设时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* GPIO配置 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LED1_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED1_GPIO, &GPIO_InitStructure);

    /* 熄灭LED */
    // GPIO_WriteBit(LED1_GPIO, LED1_Pin, 0);

}
