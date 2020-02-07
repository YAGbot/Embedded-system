#ifndef __HX711_H__
#define	__HX711_H__

#include "sys.h"

void ADInit(void); //初始化AD芯片，和它使用的I/O端口。
unsigned long ReadAD(void);  //读AD芯片输出的数据。

/*串口与电脑通信，在串口调试助手中显示AD模块返回的数据，以调试AD模块的函数。*/

void UsartDebugAD(void);
void UsartDebugAD_F(void);
void BUFInit(void);   //初始化队列数组BUF和SUM，即，往里面先存入N个采样值，SUM为N个采用值的和。
long double filter(void);



/***********************************
以下是用到的一些串口操作函数的声明。
************************************/

void UsartConf(unsigned long baud); //串口配置，使能函数。形参baud是波特率，例如，9600。
void USendStr(u8* str);  //函数USendStr()通过串口发送一个字符串。

#endif
