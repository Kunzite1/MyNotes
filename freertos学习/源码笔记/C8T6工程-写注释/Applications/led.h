#ifndef __LED_H__
#define __LED_H__
#include "stm32f10x.h"
#include "system.h"

#define LED1_PORT_RCC   RCC_APB2Periph_GPIOA
#define LED1_PORT       GPIOA
#define LED1_PIN        GPIO_Pin_11

#define LED2_PORT_RCC   RCC_APB2Periph_GPIOA
#define LED2_PORT       GPIOA
#define LED2_PIN        GPIO_Pin_12

typedef enum{LIGHT_ON = 0, LIGHT_OFF = 1}Light_State;

void LED_Init(void);
void LED_Ctrl(uint8_t LED_Num, Light_State state);

#endif