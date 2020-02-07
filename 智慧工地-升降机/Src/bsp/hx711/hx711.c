/* 包含头文件 ----------------------------------------------------------------*/
#include "hx711/hx711.h"


//#define ADIO GPIOB        //定义AD芯片使用的I/O端口组。 
//#define DATA GPIO_PIN_4	  //定义AD芯片的数据线使用的引脚。
//#define CLK  GPIO_PIN_5  //定义AD芯片的时钟线使用的引脚。
//#define ADCLK() __HAL_RCC_GPIOB_CLK_ENABLE()  //定义AD芯片所使用的I/O端口的时钟。

#define ADIO GPIOH        //定义AD芯片使用的I/O端口组。 
#define DATA GPIO_PIN_10	  //定义AD芯片的数据线使用的引脚。
#define CLK  GPIO_PIN_11  //定义AD芯片的时钟线使用的引脚。
#define ADCLK() __HAL_RCC_GPIOH_CLK_ENABLE()  //定义AD芯片所使用的I/O端口的时钟。

#define N 12	   //定义滤波时，存放采样值的队列的长度。使用循环队列。
unsigned long BUF[N];  //定义作为队列的全局数组。
unsigned long SUM=0;	//定义存放采样值之和的全局变量。
int i=0;	//定义指向队列当前位置的数组下标变量。

void hx711_delay(uint16_t time)
{
	uint8_t i;

  while(time)
  {    
    for (i = 0; i < 10; i++)
    {
      
    }
    time--;
  }
}

void ADInit(void) //初始化AD芯片使用的I/O端口。
{
	 /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;

	/*使能hx711对应的IO口时钟*/
	ADCLK();

	GPIO_InitStruct.Pin = CLK;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //CLK引脚定义为输出。
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ADIO,&GPIO_InitStruct);

	GPIO_InitStruct.Pin = DATA;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;//DATA引脚定义为输入。
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ADIO,&GPIO_InitStruct);		
}

unsigned long ReadAD(void)  //读AD芯片输出的数据。
{
	unsigned long val = 0;
	unsigned char i = 0;
	
	HAL_GPIO_WritePin(ADIO,DATA,GPIO_PIN_SET);
	HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_RESET);
	while(HAL_GPIO_ReadPin(ADIO,DATA));
	hx711_delay(1);
	for(i=0;i<24;i++)
	{
		HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_SET);
		val=val<<1;
		hx711_delay(1); 
		HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(ADIO,DATA))
		val++;
		hx711_delay(1);
	}
	HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_SET);
	val = val^0x800000;
	hx711_delay(1);
	HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_RESET);
	hx711_delay(1); 
	return val; 		
}

