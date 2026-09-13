# vTaskStartScheduler 启动流程

- 日期：2026-08-20
- 方式：跟读 `C8T6工程-写注释` 源码（FreeRTOS V9.0.0，Cortex-M3）
- 笔记位置：仓库根目录 `各种初始化的流程.md` → 「调度器启动过程」

## 覆盖要点

1. 链表初始化触发点：第一个 `xTaskCreate`（空闲任务）→ `prvAddNewTaskToReadyList` → `prvInitialiseTaskLists`（tasks.c:982）
2. 空闲任务优先级 0；`configUSE_TIMERS=0` 跳过定时器任务
3. `portDISABLE_INTERRUPTS` → `xSchedulerRunning=pdTRUE` → `xPortStartScheduler()`（不返回）
4. `xPortStartScheduler`：PendSV/SysTick 最低优先级 → `vPortSetupTimerInterrupt` 配 1ms SysTick → `prvStartFirstTask`
5. `prvStartFirstTask`：复位 MSP、开中断、`svc 0`
6. `vPortSVCHandler`：取 `pxTopOfStack`、弹 r4-r11、恢复 PSP、`basepri=0`、`bx r14` 伪返回
7. 核心技巧：`pxPortInitialiseStack` 预装伪栈帧 → 硬件自动弹栈直接进任务入口

## 下一步候选

- PendSV 上下文切换（`xPortPendSVHandler`）
- `vTaskSwitchContext` 任务选择
- SysTick 与时间片轮转
