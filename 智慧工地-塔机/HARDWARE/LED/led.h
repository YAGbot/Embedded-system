#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PAout(8)	// PA8
#define LED1 PDout(2)	// PD2	

#define FIFO1 PBout(5)	// PB5		//继电器控制
#define FIFO2 PBout(6)	// PB6
#define FIFO3 PBout(7)	// PB7
#define FIFO4 PBout(8)	// PB8

void LED_Init(void);//初始化
void control_init(void);
		 				    
#endif
