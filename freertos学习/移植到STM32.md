# 移植到STM32，用KEIL开发

## 一、代码移植

- 1、在工程目录里新建一个文件夹FreeRTOS用于存放freertos的源码。

- 2、将路径：freertos入门\FreeRTOSv9.0.0\FreeRTOS\Source下的所有文件复制到工程文件夹中的FreeRTOS中。

- 3、路径：工程\FreeRTOS\portable下有大量文件夹，只需保留Keil、MemMang、RVDS这三个。

- 4、打开KEIL工程，添加两个group，分别命名为：FreeRTOS_core、FreeRTOS_port。

- 5、FreeRTOS_core中添加”工程\FreeRTOS"下的所有.c文件；

- 6、FreeRTOS_port中添加①“工程\FreeRTOS\Portable\MemMang”下的heap_4.c   ②“工程\FreeRTOS\portable\RVDS\ARM_CM3”下的port.c

- 7、添加头文件路径：“工程\FreeRTOS\include” 和 “工程\FreeRTOS\portable\RVDS\ARM_CM3”

- 8、将官方源码文件："FreeRTOSv9.0.0\FreeRTOS\Demo\CORTEX_STM32F103_Keil"下的FreeRTOSConfig.h复制到”工程\FreeRTOS\include“中

<mark>水平不到家，中间有很多配置的细节不会搞，全是报错。</mark>

第8步的FreeRTOSConfig.h，可以先用教程例程里的。然后把stm32f10x_it.c里两个服务函数：SVC_Handler和PendSV_Handler给注释掉。因为FreeRTOS已经给我们写好这两个服务函数了。

<mark>注意：FreeRTOSConfig.h中要调整堆的大小</mark>

在FreeRTOSConfig.h第180行部分进行修改。公式：**堆的大小=RW-data + ZI-data + Heap_Size**。

（KEIL编译完成后底部Program Size可以查看RW-data和ZI-data；Heap_Size位于stm32<u>启动文件startup_stm32f10x_md.s</u>中）

## 二、编写SysTick中断函数

该函数位于stm32f10x_it.c，代码如下

```c
extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        xPortSysTickHandler(); // 函数定义在port.c
}
```

同时，要在此文件头部include几个头文件，如下：

```c
#include "stm32f10x_it.h"
#include "FreeRTOS.h"
#include "task.h"
```

注意，FreeRTOS.h必须要在task.h之前被include。

## 三、检验移植是否成功

这里使用C8T6移植freertos，用到的串口和LED驱动网上很多，这里不放了。

main.c代码：

```c
#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"


//任务优先级
#define START_TASK_PRIO        1
//任务堆栈大小    
#define START_STK_SIZE         128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

// LED1任务
#define LED1_TASK_PRIO        2
#define LED1_STK_SIZE         50  
TaskHandle_t LED1Task_Handler;
void led1_task(void *pvParameters);

int main()
{
    SysTick_Init(72);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    LED_Init();
    USART1_Init(115200);
    printf("Program starting!\n");

    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区

    //创建LED1任务
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler); 

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
} 

//LED1任务函数
void led1_task(void *pvParameters)
{
    while(1)
    {
        LED_Ctrl(1, LIGHT_ON);
        LED_Ctrl(2, LIGHT_ON);
        vTaskDelay(800);
        LED_Ctrl(1, LIGHT_OFF);
        LED_Ctrl(2, LIGHT_OFF);
        vTaskDelay(200);
    }
}
```

<mark>可能遇到的问题：</mark>

- 如果编译无问题，烧录成功，但是LED没反应，可以看看是不是KEIL魔法棒的Target下的UseMicroLIB没有勾选。（实测勾选后LED正常闪烁）（解释：移植模板用到了printf，需要勾选微库）
