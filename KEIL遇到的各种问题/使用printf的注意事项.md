# 使用printf的注意事项

## 一、使用方法

- 重定向printf函数：

一般printf都是用于usart通信的，所以在usart.c要加上重定向函数。

```c
//重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
/* 发送一个字节数据到USART1 */
       USART_SendData(USART1, (uint8_t) ch);

     /* 等待发送完毕 */
     while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
     return (ch);
}
```

- 增加头文件stdio

- 勾选KEIL魔法棒的Target下的Use MicroLIB

## 二、注意事项

- 
