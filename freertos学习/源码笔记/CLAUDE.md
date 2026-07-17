# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

这是一个 **STM32F103C8T6** 微控制器的 FreeRTOS 学习项目，用于研究 FreeRTOS 源码和嵌入式系统开发。

## 项目结构

```
C8T6工程-写注释/
├── Start/           # CMSIS 启动文件 和 system_stm32f10x.c
├── Library/         # STM32 标准外设库 (stm32f10x_gpio.c, stm32f10x_rcc.c 等)
├── FreeRTOS/        # FreeRTOS 内核源码
│   ├── tasks.c      # 任务管理核心
│   ├── queue.c      # 队列实现
│   ├── list.c       # 列表数据结构
│   ├── portable/    # 处理器相关移植代码 (ARM_CM3)
│   └── include/     # FreeRTOS 头文件
├── User/            # 用户代码 (main.c, stm32f10x_it.c)
├── Applications/    # 应用层代码 (led.c, led.h)
└── build/           # 构建输出目录
```

## 构建方法

**使用 EIDE (VS Code 插件)**:
- 打开 `C8T6工程-写注释/Project.code-workspace`
- EIDE 会自动识别 `.uvprojx` 工程文件
- 构建: `EIDE: Build` 或 `Ctrl+Shift+B`

**使用 Keil MDK**:
- 直接打开 `Project.uvprojx`

## 代码规范

- 格式化: `.clang-format` (BasedOnStyle: Microsoft, 4 空格缩进, 不使用 Tab)
- 中文注释风格
- 主要语言: C

## 关键文件说明

- `User/main.c` - FreeRTOS 任务创建示例 (start_task → led1_task)
- `FreeRTOS/tasks.c` - 任务创建 `xTaskCreate()` 实现位于此文件
- `FreeRTOS/portable/RVDS/ARM_CM3/` - ARM Cortex-M3 架构移植层

## 常见操作

- **编译**: EIDE Build 或 Keil MDK Build
- **格式化代码**: `clang-format -i <file>`
- **烧录**: 通过 EIDE 的下载功能或 Keil MDK 的 Flash 下载
