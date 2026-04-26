# 外骨骼康复机器控制系统分析(Analysis of control system of exoskeleton rehabilitation machine)

## 一、卡尔曼滤波算法(Kalman filtering)

卡尔曼滤波器是一种以最小二乘法为基础的线性二次型估计算法，该算法可以估计过程的状态，使均方差最小，无偏性，可以实时得到与原状态相似度较高的状态估计结果!5，卡尔曼滤波将上一时刻估计得到的最优值与传感器获取的观测值通过卡尔曼增益得到下一时刻的最优值，并不断迭代与更新，实时获取系统的最优状态。

- 计算公式

![](assets\2025-02-06-13-08-43-image.png)

> [1]李鑫,郑铖,杨星,等.卡尔曼滤波与PID的两轮自平衡小车设计[J].单片机与嵌入式系统应用,2023,23(01):84-87.

采集到MPU6050的原始角度和角速度后，通过卡尔曼滤波处理，可以显著减少噪声的干扰影响，减少误差。

![](assets\2025-02-05-16-14-47-b62479e594e83f8c3e982a676912e2a.jpg)

<u>卡尔曼滤波流程图</u>

- 处理步骤

以结构体保存卡尔曼参数，在初始化时赋值

```c
//卡尔曼参数结构体
typedef struct {
    float Q_angle;   // 过程噪声协方差
    float Q_bias;    // 过程噪声偏差协方差
    float R_measure; // 测量噪声协方差。
    float angle;     // 估计角度
    float bias;      // 估计偏差
    float P[2][2];   // 协方差矩阵
} KalmanFilter;
//初始化函数
void Kalman_Init(KalmanFilter *kf) {
    kf->Q_angle = 0.001;
    kf->Q_bias = 0.003;
    kf->R_measure = 0.03;
    kf->angle = 0;
    kf->bias = 0;
    kf->P[0][0] = 0;
    kf->P[0][1] = 0;
    kf->P[1][0] = 0;
    kf->P[1][1] = 0;
}
```

用代码实现上述的公式，并封装成函数

```c
// 参数：
// kf：指向卡尔曼滤波器结构体的指针，包含滤波器的状态和参数
// newAngle：新测量得到的角度值
// newRate：新测量得到的角速度值
// dt：时间间隔，即从上一次更新到现在经过的时间
// 返回值：更新后的角度值
float Kalman_Update(KalmanFilter *kf, float newAngle, float newRate, float dt) {
    // 公式：angle = angle + dt * (newRate - bias)
    kf->angle += dt * (newRate - kf->bias);
    // 更新协方差矩阵 P
    // P 矩阵用于描述状态估计的不确定性
    kf->P[0][0] += dt * (dt * kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
    kf->P[0][1] -= dt * kf->P[1][1];
    kf->P[1][0] -= dt * kf->P[1][1];
    kf->P[1][1] += kf->Q_bias * dt;
    // 计算测量残差 y
    // 即新测量的角度值与预测角度值之间的差异
    float y = newAngle - kf->angle;
    // 计算测量残差的协方差 S
    // 用于衡量测量残差的不确定性
    float S = kf->P[0][0] + kf->R_measure;
    // 计算卡尔曼增益 K
    // 用于权衡预测值和测量值的权重
    float K[2];
    K[0] = kf->P[0][0] / S;
    K[1] = kf->P[1][0] / S;
    // 根据卡尔曼增益更新角度和偏差，结合预测值和测量值得到更准确的状态估计
    kf->angle += K[0] * y;
    kf->bias += K[1] * y;
    // 临时保存 P[0][0] 和 P[0][1] 的值，用于后续更新 P 矩阵
    float P00_temp = kf->P[0][0];
    float P01_temp = kf->P[0][1];
    // 更新协方差矩阵 P
    // 根据卡尔曼滤波的更新方程更新 P 矩阵的各个元素
    kf->P[0][0] -= K[0] * P00_temp;
    kf->P[0][1] -= K[0] * P01_temp;
    kf->P[1][0] -= K[1] * P00_temp;
    kf->P[1][1] -= K[1] * P01_temp;
    // 返回更新后的角度值
    return kf->angle;
}
```

## 二、读取关节角度

#### 1、采用I2C通信配置和唤醒MPU6050

主控板使用STM32F103C8T6，标准库函数可以直接且方便地配置I2C。主控板作为主机，地址设置为0x00。MPU6050作为从机，AD0若接低电平，7位从机地址就是0x11010000；若AD0接高电平，7位从机地址就是0x11010001。这里AD0接地，从机地址即为0xD0。

将标准库函数中I2C的收发函数和控制函数，封装成MPU6050专用的读写函数`MPU6050_ReadData(MPU6050_Data *data)`、`I2C_WriteByte(uint8_t devAddr, uint8_t regAddr, uint8_t data)`。

程序上电后，先通过I2C向MPU6050发送数据，以配置MPU6050的基本参数。

```c
void MPU6050_Init() {
  I2C_WriteByte(MPU6050_ADDR, 0x6B, 0x00);
  // 配置加速度计量程 ±2g
  I2C_WriteByte(MPU6050_ADDR, 0x1C, 0x00);
  // 配置陀螺仪量程 ±250°/s
  I2C_WriteByte(MPU6050_ADDR, 0x1B, 0x00);
}
```

#### 2、定时读取MPU6050数据

使用主控板的定时器外设，通过定时器中断实现定时采集数据。设定定时器的技术周期为1000，预分频系数为720，时钟频率为72MHz，则定时器频率为100Hz，每10ms读取一次MPU6050的数据。
