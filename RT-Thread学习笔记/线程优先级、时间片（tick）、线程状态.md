# 线程优先级、时间片（tick）、线程状态

## 一、创建线程

1. 首先定义线程（如下图），其实是一个结构体指针![](C:\Users\王占东\AppData\Roaming\marktext\images\2025-01-21-22-42-41-image.png)，指向rt_thread结构体。

```c
rt_thread_t top;
rt_thread_t thread1, thread2, thread3, thread4;    //定义线程
```

2. 创建线程，如下图
- 函数rt_thread_create("线程名", 入口函数名, 传入参数/形参, 栈空间(暂不了解), 优先级, tick长度)；  

```c
//创建线程
    top = rt_thread_create("top", top_entry, RT_NULL, 512, 14, 5);
    thread1 = rt_thread_create("threa1", thread1_entry, RT_NULL, 512, 15, 5);
    thread2 = rt_thread_create("threa2", thread2_entry, RT_NULL, 512, 15, 5);
    thread3 = rt_thread_create("threa3", thread3_entry, RT_NULL, 512, 15, 5);
    thread4 = rt_thread_create("threa4", thread4_entry, RT_NULL, 512, 15, 5);
```

3. 编写线程函数。
- 这里加上延时函数，当此线程中的工作结束后，此线程被挂起，不让它干扰其它线程的运行。

```c
//线程1函数
void thread1_entry(void *p)
{
    while(1)
    {
        rt_kprintf("thread1 running\n");
        rt_thread_mdelay(10);   //线程挂起/休眠
    }
}
```

4. 线程入口
- 调用函数rt_thread_startup(线程名)，即进入上面写的线程函数。
  
  ```c
      //进入线程
      if (top != RT_NULL)
          rt_thread_startup(top);
      else
          rt_kprintf("top failed \n");
  ```
5. 编译下载，打开终端查看效果：
- 右下角终端窗口可以看到，优先级较高的线程top先运行，随后优先级较低的四个线程thread1~4轮流执行。

- <mark>同优先级别的线程运行顺序遵循先进先出</mark>，具体我不太了解。  

## 二、线程优先级

在rtconfig.h里可以看到最多能设置几个优先级，优先级数字越低，优先级越高

![](C:\Users\王占东\AppData\Roaming\marktext\images\2025-01-21-22-43-50-image.png)  

比如示例工程中的线程top优先级为14，thread1~4优先级为1。

## 三、时间片

1. tick**：单片机中，一个节拍就是一个tick，有点像数电里触发器锁存器那章的cp信号。每来一个节拍，或者说每个tick，单片机执行一次动作。

2. 时间片就是一个线程一次运行持续的tick数，在rt_thread_creat的参数中可以调整。  

## 四、线程状态

线程有五个状态，初始状态、就绪状态、运行状态、挂起状态、关闭状态。

没怎么研究这些状态，暂时只作了解。

# 示例代码main.c

```c
#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_thread_t top;
rt_thread_t thread1, thread2, thread3, thread4;    //定义线程

//线程1函数
void thread1_entry(void *p)
{
    while(1)
    {
        rt_kprintf("thread1 running\n");
        rt_thread_mdelay(10);   //线程挂起/休眠
    }
}

void thread2_entry(void *p)
{
    while(1)
    {
        rt_kprintf("thread2 running\n");
        rt_thread_mdelay(10);
    }
}

void thread3_entry(void *p)
{
    while(1)
    {
        rt_kprintf("thread3 running\n");
        rt_thread_mdelay(10);
    }
}

void thread4_entry(void *p)
{
    while(1)
    {
        rt_kprintf("thread4 running\n");
        rt_thread_mdelay(10);
    }
}

void top_entry(void *p)
{
    while(1)
    {
        rt_kprintf("top running\n");
        rt_thread_mdelay(10);
    }
}

int main(void)
{
    //创建线程
    top = rt_thread_create("top", top_entry, RT_NULL, 512, 14, 5);
    thread1 = rt_thread_create("threa1", thread1_entry, RT_NULL, 512, 15, 5);
    thread2 = rt_thread_create("threa2", thread2_entry, RT_NULL, 512, 15, 5);
    thread3 = rt_thread_create("threa3", thread3_entry, RT_NULL, 512, 15, 5);
    thread4 = rt_thread_create("threa4", thread4_entry, RT_NULL, 512, 15, 5);

    //进入线程
        if (top != RT_NULL)
            rt_thread_startup(top);
        else
            rt_kprintf("top failed \n");
        if (thread1 != RT_NULL)
            rt_thread_startup(thread1);
        else
            rt_kprintf("thread1 failed \n");
        if (thread2 != RT_NULL)
            rt_thread_startup(thread2);
        else
            rt_kprintf("thread2 failed \n");
        if (thread3 != RT_NULL)
            rt_thread_startup(thread3);
        else
            rt_kprintf("thread3 failed \n");
        if (thread4 != RT_NULL)
            rt_thread_startup(thread4);
        else
            rt_kprintf("thread4 failed \n");

    return RT_EOK;
}
```
