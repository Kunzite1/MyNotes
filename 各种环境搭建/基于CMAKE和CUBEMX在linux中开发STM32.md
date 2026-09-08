# 基于 CMake 和 CubeMX 在 Linux 中开发 STM32

本文介绍一套可复用于多数 STM32 裸机或 HAL 工程的 Linux 开发流程：使用 STM32CubeMX 生成初始化代码，CMake 组织构建，GNU Arm Embedded Toolchain 编译，OpenOCD 配合 ST-Link 烧录和调试。不同型号不能直接共用 CPU/FPU 参数、芯片宏、启动文件、链接脚本或 OpenOCD target 配置。

## 1. 先理解工具链

```text
CubeMX 生成初始化代码
        ↓
CMake 读取源码、头文件、编译选项和链接脚本
        ↓
Make/Ninja 调用 arm-none-eabi-gcc
        ↓
ELF（可调试）→ objcopy → BIN/HEX（可烧录）
        ↓
OpenOCD + ST-Link 烧录，并为 GDB 提供调试接口
```

CMake 不是编译器；它生成构建规则。`arm-none-eabi-gcc` 才负责为无操作系统的 Arm MCU 交叉编译。STM32 无法运行 Linux 本机程序，因此 CMake 还需要工具链文件描述目标平台。

## 2. 安装开发工具

Ubuntu/Debian 可直接安装发行版软件包：

```sh
sudo apt update
sudo apt install cmake make ninja-build gcc-arm-none-eabi \
  binutils-arm-none-eabi libnewlib-arm-none-eabi openocd \
  gdb-multiarch git
```

逐项确认安装结果：

```sh
cmake --version
arm-none-eabi-gcc --version
arm-none-eabi-objcopy --version
arm-none-eabi-gcc -print-file-name=nano.specs
openocd --version
gdb-multiarch --version
```

具体 CMake 最低版本以工程顶层 `cmake_minimum_required()` 为准。若发行版提供的 GCC 太旧，可从 [Arm GNU Toolchain 官方发布页](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) 下载与 Linux 主机架构匹配的 **AArch32 bare-metal (`arm-none-eabi`)** 工具链，校验下载文件后解压，并把其 `bin/` 加入 `PATH`。不要混用多套工具链；用下面的命令确认实际生效路径：

```sh
command -v arm-none-eabi-gcc
arm-none-eabi-gcc -print-sysroot
```

## 3. 用 CubeMX 建立 CMake 工程

从 [STM32CubeMX 官方页面](https://www.st.com/content/st_com/en/stm32cubemx.html) 安装 Linux 版，然后：

1. 选择准确的 MCU 或开发板，配置时钟、引脚和外设。
2. 在 **Project Manager** 中把 Toolchain/IDE 设为 **CMake**。
3. 生成代码并确认工程包含以下关键文件：

```text
project/
├── CMakeLists.txt                  # 顶层目标、用户源码和产物
├── cmake/gcc-arm-none-eabi.cmake  # 交叉编译器及 CPU/FPU 参数
├── cmake/stm32cubemx/CMakeLists.txt
├── Core/ 和 Drivers/              # HAL、CMSIS 和初始化代码
├── startup_stm32xxxx.s            # 中断向量与复位入口
└── STM32xxxx_FLASH.ld             # Flash/RAM 布局
```

CubeMX 再生成时，只保证保留 `/* USER CODE BEGIN */` 与 `/* USER CODE END */` 之间的修改。更稳妥的做法是把业务代码放到独立目录，再从顶层 `CMakeLists.txt` 引入。

## 4. 工具链文件为什么重要

CubeMX 生成的 `cmake/gcc-arm-none-eabi.cmake` 通常完成四件事：

- `CMAKE_SYSTEM_NAME=Generic` 表示目标没有操作系统；
- 指定 `arm-none-eabi-gcc/g++/objcopy/size`；
- `CMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY` 避免 CMake 探测时尝试运行或链接主机程序；
- 指定目标 Cortex-M 内核、浮点 ABI 和链接选项。

核心结构通常如下，CPU 参数应以 CubeMX 为具体芯片生成的内容为准：

```cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TARGET_FLAGS "<该芯片对应的 -mcpu/-mfpu/-mfloat-abi 参数>")
set(CMAKE_EXE_LINKER_FLAGS
    "${TARGET_FLAGS} -T ${CMAKE_SOURCE_DIR}/<芯片对应的链接脚本>.ld")
```

这些参数必须与芯片及所链接库的 ABI 一致。例如 Cortex-M0/M3 通常使用软件浮点，不能照搬 M4F 的 `-mfpu` 和 `-mfloat-abi=hard`。STM32F103（Cortex-M3）的常见参数是 `-mcpu=cortex-m3 -mthumb -mfloat-abi=soft`。链接脚本决定代码、数据、堆和栈所在的 Flash/RAM 地址及容量，并且必须匹配具体料号和封装。

顶层 `CMakeLists.txt` 负责：

- 用 `project()` 和 `enable_language(C ASM)` 启用 C 与汇编；
- 用 `target_sources()`、`target_include_directories()` 和 `target_compile_definitions()` 组织源码；
- 链接 CubeMX 生成的 HAL、CMSIS，以及工程实际启用的中间件；
- 构建后用 `objcopy` 生成 `.bin`、`.hex`，并用 `size` 打印占用量。

新增手写固件模块时，应使用工程已有的 `target_sources()` 或源码列表变量将 `.c/.cpp` 文件加入最终目标，并同步添加头文件目录。

## 5. 配置与构建

### 方式 A：显式配置

```sh
cd <project>
cmake -S . -B build/Debug -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug --parallel
```

成功后得到：

```text
build/Debug/<target>.elf   # 含符号，优先用于烧录和调试
build/Debug/<target>.bin   # 纯二进制镜像
build/Debug/<target>.hex   # 带地址信息的 Intel HEX
build/Debug/<target>.map   # 链接映射，分析内存布局
```

检查产物并清理：

```sh
arm-none-eabi-size build/Debug/<target>.elf
arm-none-eabi-objdump -h build/Debug/<target>.elf
cmake --build build/Debug --target clean
```

工具链、生成器或关键选项改变后，不要复用旧的 `CMakeCache.txt`；改用新的构建目录重新配置，例如 `-B build-gcc`。

### 方式 B：使用工程预设

若 CubeMX 生成了 `CMakePresets.json`，通常可直接使用 `Debug` 或 `Release` 预设：

```sh
cd <project>
cmake --preset Debug
cmake --build --preset Debug
```

产物目录由预设中的 `binaryDir` 决定。IDE 任务、烧录脚本和调试配置必须使用同一套目录布局。

## 6. 连接 ST-Link 并烧录

连接 `SWDIO`、`SWCLK`、`GND`、目标电压参考 `VTref/3.3V`，建议同时连接 `NRST`；确认目标板供电与接线后再操作。先用 `lsusb` 确认 Linux 能看到 ST-Link。

Ubuntu 的 OpenOCD 软件包通常会安装 udev 规则。若普通用户无 USB 权限，可把当前用户加入 `plugdev`，重新加载规则后注销并重新登录，再重新插拔 ST-Link：

```sh
sudo usermod -aG plugdev "$USER"
sudo udevadm control --reload-rules
sudo udevadm trigger
```

先检查本机提供的 STM32 target 配置：

```sh
find /usr/share/openocd/scripts/target -name 'stm32*.cfg'
```

按芯片系列选择配置，例如 STM32F1 常用 `stm32f1x.cfg`，STM32F4 常用 `stm32f4x.cfg`，STM32G0 常用 `stm32g0x.cfg`。不要只根据 Cortex 内核猜测配置。通用烧录命令为：

```sh
cd <project>
openocd -f interface/stlink.cfg -f target/<stm32-family>.cfg \
  -c "adapter speed 1000" \
  -c "program {build/Debug/<target>.elf} verify reset exit"
```

`verify` 回读校验，`reset` 让 MCU 重新运行，`exit` 在完成后退出。烧录前必须检查目标型号、供电、接线和产物路径。连接不稳定时可降低 `adapter speed`。

## 7. 使用 GDB 调试

终端一启动 OpenOCD，不加烧录命令：

```sh
openocd -f interface/stlink.cfg -f target/<stm32-family>.cfg
```

终端二连接默认的 3333 端口：

```sh
gdb-multiarch build/Debug/<target>.elf
```

```gdb
target extended-remote :3333
monitor reset halt
load
break main
continue
```

VS Code 可安装 Cortex-Debug。配置时至少核对 ELF 路径、OpenOCD target、GDB 路径和工作目录。若扩展找不到 `arm-none-eabi-gdb`，可将 `gdbPath` 指向 `/usr/bin/gdb-multiarch`。建议在 CMake 中启用 `CMAKE_EXPORT_COMPILE_COMMANDS`，供 clangd 或 C/C++ 插件解析宏和头文件。

## 8. 编写可复用脚本

构建和烧录脚本不应写死用户名、工程目录、目标文件名或某个芯片族。建议从脚本自身位置计算工程根目录，并允许通过参数或环境变量覆盖：

- 构建类型，如 `Debug` 或 `Release`；
- ELF 路径与 CMake target 名称；
- OpenOCD interface、target 和 SWD 速度；
- 构建目录或 CMake preset。

典型项目可提供 `build.sh [Debug|Release]` 和 `flash.sh [Debug|Release]`。`flash.sh` 应在 ELF 不存在时先构建，并在任一步失败时以非零状态退出。

## 9. 常见问题

| 现象 | 原因与处理 |
| --- | --- |
| 找不到 `arm-none-eabi-gcc` | 工具链未安装或 `PATH` 未生效；检查 `command -v`。 |
| 找不到 `nano.specs` | Newlib 未安装，或 GCC 与库来自不同工具链；安装 `libnewlib-arm-none-eabi` 并统一路径。 |
| CMake 报生成器不一致 | 旧构建目录缓存了 Make/Ninja；换一个全新的 `-B` 目录。 |
| `undefined reference` | 检查源码是否加入 `target_sources()`、函数签名是否一致，以及 HAL/启动文件是否完整。 |
| ELF 超出 Flash/RAM | 核对具体料号容量和链接脚本，并查看 `.map` 文件。 |
| 程序能链接但上板异常 | 检查芯片宏、CPU/FPU ABI、链接脚本、系统时钟与启动文件是否对应同一型号。 |
| OpenOCD 找不到 ST-Link | 检查 USB 线、udev 权限、SWD 接线、目标供电；重新插拔后再试。 |
| `target not halted` 或连接不稳 | 接上 `NRST`，降低 SWD 速度，并确认 BOOT0、电源和地线。 |

## 10. 完成标准

环境搭建完成应同时满足：全新构建目录可配置；编译无错误并生成 ELF/BIN/HEX；`size` 能显示 Flash/RAM 占用；OpenOCD 能识别芯片并完成 `verify`；复位后板上行为符合预期。编译成功只能证明软件构建链正确，不能替代实板验证。

## 官方参考

- [CMake：Cross Compiling With CMake](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Cross%20Compiling%20With%20CMake.html)
- [Arm GNU Toolchain 下载](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
- [OpenOCD：Flash Programming](https://openocd.org/doc/html/Flash-Programming.html)
