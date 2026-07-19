# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目性质

本仓库是 **FreeRTOS 源码学习 + 注释工程**，目标板为 STM32F103C8T6。两类产物并存：

- **代码产物**：`C8T6工程-写注释/` 下的可编译工程，含 FreeRTOS V9.0.0 内核源码（带中文注释）和外设驱动示例。
- **学习笔记**（仓库根目录）：
  - `函数定义.md` — FreeRTOS API 参考（按模块整理：任务/队列/信号量/软件定时器/事件组/协程）
  - `数据结构定义.md` — `TCB_t` / `List_t` / `ListItem_t` / `Queue_t` 等核心结构
  - `各种初始化的流程.md` — 从源码层面追踪执行流（如 `xTaskCreate → prvInitialiseNewTask → prvAddNewTaskToReadyList`）

**约定**：每次新增 FreeRTOS 源码注释或流程梳理，需同步更新对应的根目录笔记；改 `FreeRTOSConfig.h` 配置项时也要检查笔记里的描述是否仍然成立。

---

## 构建与烧录

工程同时支持 Keil MDK 和 VS Code + EIDE 两种工作流。

### VS Code + EIDE（推荐）

打开 `C8T6工程-写注释/Project.code-workspace`，EIDE 插件会接管构建。`.vscode/tasks.json` 已配置以下任务，可通过 `Ctrl+Shift+B` 选择：

| 任务 | 说明 |
| --- | --- |
| `build` | 编译 |
| `flash` | 烧录（依赖上一次构建产物） |
| `build and flash` | 一键编译并烧录 |
| `rebuild` | 清理后重新编译 |
| `clean` | 仅清理 |

烧录使用 **STLink / SWD**（见 `.eide/eide.yml` 中 `uploader: STLink`）。

### Keil MDK

直接打开 `C8T6工程-写注释/Project.uvprojx`。注意：`.gitignore` 已忽略 `*.uvguix.*` / `*.uvoptx` 等用户级窗口布局，不要把这些提交进仓库。

### 构建工具链

- 编译器：ARMCC 5（`toolchain: AC5`），开启 `c99-mode`、`--diag_suppress=1 --diag_suppress=1295`、`use-microLIB`
- 全局宏：`USE_STDPERIPH_DRIVER`、`STM32F10X_HD`（定义设备为 HD 类）
- 包含路径见 `.eide/eide.yml` 的 `incList`
- 格式化：根目录的 `.clang-format`（Microsoft 风格，4 空格缩进，禁止 Tab，`ColumnLimit: 0`）。手动执行 `clang-format -i <file>` 即可

---

## 工程结构（按职责，非完整文件树）

| 目录 | 职责 |
| --- | --- |
| `Start/` | CMSIS 内核支持 + `system_stm32f10x.c`；启动文件用 `startup_stm32f10x_md.s`（C8T6 实际是 MD 容量，但代码按 HD 类编译，需要注意 `STM32F10X_HD` 与实际芯片容量不匹配——若改了外设假设要回到这里核对） |
| `Library/` | STM32 标准外设库，未使用到的外设仍保留在编译列表里；新增依赖时核对 `User/stm32f10x_conf.h` 是否打开对应模块 |
| `FreeRTOS/` | V9.0.0 内核源码；**只编译** `croutine.c` / `event_groups.c` / `list.c` / `queue.c` / `tasks.c` / `timers.c`（见 `.eide/eide.yml` 的 `FreeRTOS_core`）。其中 `croutine.c` 和 `timers.c` 受 `configUSE_CO_ROUTINES` / `configUSE_TIMERS` 开关控制 |
| `FreeRTOS/portable/RVDS/ARM_CM3/` | 移植层：`port.c`（上下文切换/PendSV/SysTick）+ `portmacro.h`（基础类型定义） |
| `FreeRTOS/portable/MemMang/` | 内存分配算法；工程只链入 `heap_4.c`，其他实现保留供切换对照 |
| `Public/` | 通用基础：`SysTick.c`（72 MHz 计时，`delay_ms/us`）、`usart.c`（USART1 重定向 printf）、`system.h`（位带别名宏 `PAout/PAin` 等） |
| `Applications/` | 应用层：`led.c`（PA11/PA12）、`KEY/key.c`（PB 口三按键扫描） |
| `User/` | `main.c`（入口：创建 `start_task` 后 `vTaskStartScheduler()`，在 `start_task` 中再创建 `led1_task` 并删除自己）+ `stm32f10x_it.c`（中断向量，挂接 `PendSV_Handler` / `SVC_Handler` / `SysTick_Handler`） |
| `.cmsis/include/` | CMSIS 头文件；新增 CMSIS 依赖时检查此目录 |

---

## 内存布局（来自 `.eide/eide.yml`）

- ROM：`0x08000000`，大小 `0x10000`（64 KB）— 启动地址即此
- RAM：`0x20000000`，大小 `0x5000`（20 KB）— `configTOTAL_HEAP_SIZE = 14*1024` 留给 FreeRTOS 堆，余量极小，**新建任务/队列时务必核算栈与堆**

---

## FreeRTOS 关键配置（`C8T6工程-写注释/FreeRTOS/include/FreeRTOSConfig.h`）

阅读或修改源码前先看这里，因为大量条件编译依赖这些宏：

| 宏 | 当前值 | 影响 |
| --- | --- | --- |
| `configCPU_CLOCK_HZ` | `SystemCoreClock` | SysTick 与调度相关延时计算 |
| `configTICK_RATE_HZ` | `1000` | 1 ms 一个 tick；`vTaskDelay` / `pdMS_TO_TICKS` 都基于此 |
| `configMAX_PRIORITIES` | `32` | 配合 `configUSE_PORT_OPTIMISED_TASK_SELECTION=1` 使用 CM3 的 CLZ 指令 |
| `configUSE_PREEMPTION` | `1` | 抢占式调度 |
| `configUSE_TIME_SLICING` | `1` | 同优先级时间片轮转 |
| `configMINIMAL_STACK_SIZE` | `128` | 空闲任务栈，单位 `StackType_t`（4 字节） |
| `configTOTAL_HEAP_SIZE` | `14*1024` | 见上文内存布局 |
| `configSUPPORT_DYNAMIC_ALLOCATION` | `1` / `configSUPPORT_STATIC_ALLOCATION` | `0` — 只支持动态分配（`xTaskCreate`，不能 `xTaskCreateStatic`） |
| `configUSE_MUTEXES` / `configUSE_RECURSIVE_MUTEXES` / `configUSE_COUNTING_SEMAPHORES` | 全 `0` | 互斥/计数信号量**未启用**，相关源码不会被实际编译进可执行文件 |
| `configUSE_TIMERS` / `configUSE_CO_ROUTINES` | `0` / `0` | 软件定时器与协程 API 不可用 |
| `configUSE_TICKLESS_IDLE` | `0` | 低功耗 tickless 关闭；如需开启需先解决 SWD 锁死问题（见头文件注释） |
| `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` | `5` | 中断安全 API 的优先级阈值；中断优先级 ≥ 5 时不能调用 `FromISR` 接口 |
| `xPortPendSVHandler` / `vPortSVCHandler` | `PendSV_Handler` / `SVC_Handler` | 移植层中断入口映射，确保 `User/stm32f10x_it.c` 中同名中断向量未被改写 |

`configASSERT` 被重定向到 `printf("Error:%s,%d\r\n", __FILE__, __LINE__)`——触发断言时会通过串口打印文件与行号，配合 USART1（115200 bps）即可定位。

---

## 阅读源码时的几条捷径

1. **API → 实现**：根目录 `函数定义.md` 给出每个 API 的语义与参数约束，比直接啃 `tasks.c` / `queue.c` 高效。
2. **结构体 → 字段**：`数据结构定义.md` 已拆解 `TCB_t` / `List_t` 等。
3. **流程追踪**：参考 `各种初始化的流程.md` 中已有的 `xTaskCreate` 链路；新写流程时保持相同的章节结构（函数名作为二级标题、调用顺序 + 关键判断）。
4. **类型与句柄**：`portmacro.h` 定义了 `BaseType_t` / `UBaseType_t` / `TickType_t` / `StackType_t`；`task.h` 等头文件把它们进一步封装成 `TaskHandle_t` / `QueueHandle_t` 等不透明句柄，**应用层禁止强转访问内部成员**。