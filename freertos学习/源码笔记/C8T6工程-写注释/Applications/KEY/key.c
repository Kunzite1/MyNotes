#include "key.h"
#include "SysTick.h"

void KEY_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU; // 外部接GND，内部上拉
    GPIO_InitStructure.GPIO_Pin   = KEY1_PIN | KEY2_PIN | KEY3_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
    uint8_t KeyNum = 0; // 定义变量，默认键码值为0

    if (GPIO_ReadInputDataBit(KEY_PORT, KEY1_PIN) == 0) // 如果为0，则代表按键1按下
    {
        delay_ms(20);                                           // 延时消抖
        while (GPIO_ReadInputDataBit(KEY_PORT, KEY1_PIN) == 0); // 等待按键松手
        delay_ms(20);                                           // 延时消抖
        KeyNum = 1;                                             // 置键码为1
    }

    if (GPIO_ReadInputDataBit(KEY_PORT, KEY2_PIN) == 0) // 如果为0，则代表按键2按下
    {
        delay_ms(20);                                           // 延时消抖
        while (GPIO_ReadInputDataBit(KEY_PORT, KEY2_PIN) == 0); // 等待按键松手
        delay_ms(20);                                           // 延时消抖
        KeyNum = 2;                                             // 置键码为2
    }

    if (GPIO_ReadInputDataBit(KEY_PORT, KEY3_PIN) == 0) // 如果为0，则代表按键3按下
    {
        delay_ms(20);                                           // 延时消抖
        while (GPIO_ReadInputDataBit(KEY_PORT, KEY3_PIN) == 0); // 等待按键松手
        delay_ms(20);                                           // 延时消抖
        KeyNum = 3;                                             // 置键码为3
    }

    return KeyNum;
}
