#include "led.h"

// LED初始化
void LED_Init(void)
{
    /*开启时钟*/
	RCC_APB2PeriphClockCmd(LED1_PORT_RCC | LED2_PORT_RCC, ENABLE);	//开启GPIOA的时钟
															//使用各个外设前必须开启时钟，否则对外设的操作无效
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;					//定义结构体变量
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//GPIO模式，推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;				//GPIO引脚
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_Pin = LED2_PIN;				//GPIO引脚
	GPIO_Init(LED2_PORT, &GPIO_InitStructure);					
}

// LED控制函数
// 参数：LED_Num：LED编号。
// state：点亮或熄灭，取值：LIGHT_ON、LIGHT_OFF
void LED_Ctrl(uint8_t LED_Num, Light_State state)
{
    switch (LED_Num)
    {
        case 1:
            if (state == LIGHT_ON) GPIO_ResetBits(LED1_PORT, LED1_PIN);
            else GPIO_SetBits(LED1_PORT, LED1_PIN);

        case 2:
            if (state == LIGHT_ON)  GPIO_ResetBits(LED2_PORT, LED2_PIN);
            else GPIO_SetBits(LED2_PORT, LED2_PIN);
    }
}


