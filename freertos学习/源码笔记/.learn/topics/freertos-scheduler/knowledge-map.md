# FreeRTOS 调度器

> 0/16 mastered · 0% complete

## 调度基础结构

- ⚪ **任务状态与状态机** (unexplored)
  - Ready / Running / Blocked / Suspended 四种状态
  - 状态切换触发点：创建、延时、挂起、就绪、被调度
  - list.h 中 tskTaskControlBlock 与 ulTaskState 的关系
- ⚪ **就绪链表与优先级位图** (unexplored)
  - pxReadyTasksLists[configMAX\_PRIORITIES] 数组结构
  - uxTopReadyPriority 与优先级位图
  - configUSE\_PORT\_OPTIMISED\_TASK\_SELECTION 下的位图差异
- ⚪ **任务优先级语义** (unexplored)
  - 数值越大优先级越高（与部分 RTOS 相反）
  - configMAX\_PRIORITIES = 32 与 CLZ 指令配合
  - vTaskPrioritySet / uxTaskPriorityGet

## 调度启动与任务就绪

- 🔵 **vTaskStartScheduler 启动流程** (in progress)
  - 创建空闲任务与可选定时器任务的顺序
  - prvInitialiseTaskLists 链表初始化
  - xPortStartScheduler → prvStartFirstTask 移交控制权
- ⚪ **任务创建与就绪插入** (unexplored)
  - xTaskCreate → prvInitialiseNewTask → prvAddNewTaskToReadyList 链路
  - prvAddTaskToReadyList 宏与位图更新
  - 对照根目录『各种初始化的流程.md』
- ⚪ **prvInitialiseNewTask 栈初始化** (unexplored)
  - pxTopOfStack 初值设定（模拟异常压栈）
  - 栈增长方向与 ARM\_CM3 约定
  - 任务入口 prvPortStartTask/任务包装

## 上下文切换

- ⚪ **首次启动切换 (SVC)** (unexplored)
  - prvStartFirstTask 汇编流程
  - vPortSVCHandler / SVC\_Handler 挂接点
  - User/stm32f10x\_it.c 中断向量核对
- ⚪ **PendSV 上下文切换** (unexplored)
  - xPortPendSVHandler / PendSV\_Handler 汇编
  - 现场保存与恢复（R4-R11、LR、PSP）
  - pxCurrentTCB 切换
- ⚪ **vTaskSwitchContext 任务选择** (unexplored)
  - taskSELECT\_HIGHEST\_PRIORITY\_TASK 宏
  - pxCurrentTCB 更新与就绪链表取头
  - 同优先级任务轮转的链表尾部插入
- ⚪ **CLZ 硬件优化选任务** (unexplored)
  - configUSE\_PORT\_OPTIMISED\_TASK\_SELECTION = 1
  - uxTopReadyPriority 位图 + \_\_clz 找最高优先级
  - 与普通循环遍历算法的对比

## 时间片与延时

- ⚪ **SysTick 与时间片轮转** (unexplored)
  - xPortSysTickHandler / SysTick\_Handler
  - configUSE\_TIME\_SLICING 同优先级轮转
  - configTICK\_RATE\_HZ = 1000 → 1ms 一 tick
- ⚪ **vTaskDelay 与延时链表** (unexplored)
  - prvAddCurrentTaskToDelayedList
  - xDelayedTaskList1/2 与溢出延时链表
  - pdMS\_TO\_TICKS 换算
- ⚪ **xTaskIncrementTick 调度心跳** (unexplored)
  - xTaskIncrementTick 内部逻辑
  - 延时到期任务迁回就绪链表
  - 调度器挂起时如何推迟处理

## 调度控制

- ⚪ **临界区保护** (unexplored)
  - taskENTER\_CRITICAL / taskEXIT\_CRITICAL 宏
  - port.c 中 vPortEnterCritical / vPortExitCritical
  - configMAX\_SYSCALL\_INTERRUPT\_PRIORITY 阈值
- ⚪ **调度器挂起与恢复** (unexplored)
  - vTaskSuspendAll / xTaskResumeAll
  - uxSchedulerSuspended 计数器
  - xTaskResumeAll 中的挂起任务处理与任务切换
- ⚪ **任务挂起与恢复** (unexplored)
  - vTaskSuspend / vTaskResume
  - xSuspendedTaskList 挂起链表
  - 与 vTaskSuspendAll 的区别
