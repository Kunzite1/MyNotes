# stm32f1新工程编译找不到core_cm3.h

## 问题：

新建工程编译，报错如图：

![](C:\Users\王占东\AppData\Roaming\marktext\images\2025-04-28-17-46-00-image.png)

```c
C:\Keil_v5\ARM\PACK\Keil\STM32F1xx_DFP\2.2.0\Device\Include\stm32f10x.h(483): 
error:  #5: cannot open source input file "core_cm3.h": No such file or directory
```

## 解决方法

1、~~在C:\Keil_v5\ARM\PACK\ARM\CMSIS\6.1.0\CMSIS\Core\Include文件~~夹~~里，可以找到core_cm3.h。~~

~~把他复制到C:\Keil_v5\ARM\PACK\Keil\STM32F1xx_DFP\2.2.0\Device\Include~~~

<mark>无效</mark>

<mark>2、</mark>新建工程要添加必要文件，直接用江科大的模板工程比较方便
