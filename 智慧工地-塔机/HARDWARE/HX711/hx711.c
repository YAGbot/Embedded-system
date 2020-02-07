/*******************************************************

本文件中，是操作称重传感器专用AD芯片 HX711 的程序。

********************************************************/
#include "hx711.h"
#include "delay.h"


#define ADIO GPIOA        //定义AD芯片使用的I/O端口组。 
#define DATA GPIO_Pin_12	  //定义AD芯片的数据线使用的引脚。
#define CLK  GPIO_Pin_11  //定义AD芯片的时钟线使用的引脚。
#define ADCLK RCC_APB2Periph_GPIOA  //定义AD芯片所使用的I/O端口的时钟。

#define N 12	   //定义滤波时，存放采样值的队列的长度。使用循环队列。
unsigned long BUF[N];  //定义作为队列的全局数组。
unsigned long SUM=0;	//定义存放采样值之和的全局变量。
int i=0;	//定义指向队列当前位置的数组下标变量。

void ADInit(void) //初始化AD芯片使用的I/O端口。
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(ADCLK,ENABLE);

	GPIO_InitStructure.GPIO_Pin = CLK; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //CLK引脚定义为输出。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADIO,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = DATA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//DATA引脚定义为输入。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADIO,&GPIO_InitStructure);		
}

unsigned long ReadAD(void)  //读AD芯片输出的数据。
{
	unsigned long val = 0;
	unsigned char i = 0;
	
	GPIO_SetBits(ADIO,DATA);
	GPIO_ResetBits(ADIO,CLK);
	while(GPIO_ReadInputDataBit(ADIO,DATA));
	delay_us(1);
	for(i=0;i<24;i++)
	{
		GPIO_SetBits(ADIO,CLK);
		val=val<<1;
		delay_us(1); 
		GPIO_ResetBits(ADIO,CLK);
		if(GPIO_ReadInputDataBit(ADIO,DATA))
		val++;
		delay_us(1);
	}
	GPIO_SetBits(ADIO,CLK);
	val = val^0x800000;
	delay_us(1);
	GPIO_ResetBits(ADIO,CLK);
	delay_us(1); 
	return val; 		
}

