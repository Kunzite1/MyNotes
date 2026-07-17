#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"

#define KEY_PORT_RCC RCC_APB2Periph_GPIOB
#define KEY_PORT     GPIOB
#define KEY1_PIN     GPIO_Pin_12
#define KEY2_PIN     GPIO_Pin_13
#define KEY3_PIN     GPIO_Pin_14

void KEY_Init(void);
uint8_t Key_GetNum(void);

#endif
