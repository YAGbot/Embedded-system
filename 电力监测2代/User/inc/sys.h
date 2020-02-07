#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"

//0,不支持ucos
//1,支持ucos
#define SYSTEM_SUPPORT_OS		0		//定义系统文件夹是否支持UCOS
																	    
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
/*端口寄存器BSRR地址定义*/
#define GPIOA_BSRR_Addr   (GPIOA_BASE+0x10) 						 
#define GPIOB_BSRR_Addr   (GPIOB_BASE+0x10)
#define GPIOC_BSRR_Addr   (GPIOC_BASE+0x10)
#define GPIOD_BSRR_Addr   (GPIOD_BASE+0x10)
#define GPIOE_BSRR_Addr   (GPIOE_BASE+0x10)
#define GPIOF_BSRR_Addr   (GPIOF_BASE+0x10)
#define GPIOG_BSRR_Addr   (GPIOG_BASE+0x10)

/*端口寄存器BRR地址定义*/
#define GPIOA_BRR_Addr    (GPIOA_BASE+0x14)
#define GPIOB_BRR_Addr    (GPIOB_BASE+0x14)
#define GPIOC_BRR_Addr    (GPIOC_BASE+0x14)
#define GPIOD_BRR_Addr    (GPIOD_BASE+0x14)
#define GPIOE_BRR_Addr    (GPIOE_BASE+0x14)
#define GPIOF_BRR_Addr    (GPIOF_BASE+0x14)
#define GPIOG_BRR_Addr    (GPIOG_BASE+0x14)						

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

/*位带操作端口读*/ 
#define PA_RED(n)    BIT_ADDR(GPIOA_IDR_Addr,n)		                                                                             
#define PB_RED(n)    BIT_ADDR(GPIOB_IDR_Addr,n)
#define PC_RED(n)    BIT_ADDR(GPIOC_IDR_Addr,n)
#define PD_RED(n)    BIT_ADDR(GPIOD_IDR_Addr,n)
#define PE_RED(n)    BIT_ADDR(GPIOE_IDR_Addr,n)
#define PF_RED(n)    BIT_ADDR(GPIOF_IDR_Addr,n)
#define PG_RED(n)    BIT_ADDR(GPIOG_IDR_Addr,n)

/*位带操作端口输出*/ 
#define PA_OUT(n)    BIT_ADDR(GPIOA_ODR_Addr,n)		                                                                             
#define PB_OUT(n)    BIT_ADDR(GPIOB_ODR_Addr,n)
#define PC_OUT(n)    BIT_ADDR(GPIOC_ODR_Addr,n)
#define PD_OUT(n)    BIT_ADDR(GPIOD_ODR_Addr,n)
#define PE_OUT(n)    BIT_ADDR(GPIOE_ODR_Addr,n)
#define PF_OUT(n)    BIT_ADDR(GPIOF_ODR_Addr,n)
#define PG_OUT(n)    BIT_ADDR(GPIOG_ODR_Addr,n)

/*位带操作端口置1*/
#define PA_SET(n)    (BIT_ADDR(GPIOA_BSRR_Addr,n)=1)                                    	             							
#define PB_SET(n)    (BIT_ADDR(GPIOB_BSRR_Addr,n)=1)
#define PC_SET(n)    (BIT_ADDR(GPIOC_BSRR_Addr,n)=1)
#define PD_SET(n)    (BIT_ADDR(GPIOD_BSRR_Addr,n)=1)
#define PE_SET(n)    (BIT_ADDR(GPIOE_BSRR_Addr,n)=1)
#define PF_SET(n)    (BIT_ADDR(GPIOF_BSRR_Addr,n)=1)
#define PG_SET(n)    (BIT_ADDR(GPIOG_BSRR_Addr,n)=1)

/*位带操作端口清0*/
#define PA_CLR(n)    (BIT_ADDR(GPIOA_BRR_Addr,n)=1)
#define PB_CLR(n)    (BIT_ADDR(GPIOB_BRR_Addr,n)=1)
#define PC_CLR(n)    (BIT_ADDR(GPIOC_BRR_Addr,n)=1)
#define PD_CLR(n)    (BIT_ADDR(GPIOD_BRR_Addr,n)=1)
#define PE_CLR(n)    (BIT_ADDR(GPIOE_BRR_Addr,n)=1)
#define PF_CLR(n)    (BIT_ADDR(GPIOF_BRR_Addr,n)=1)
#define PG_CLR(n)    (BIT_ADDR(GPIOG_BRR_Addr,n)=1)
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址
void NVIC_Configuration(void);	//优先级分组

#endif
