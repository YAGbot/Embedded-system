#ifndef __SYS__H
#define __SYS__H
#include "stm32f10x.h"


/*位段区与别名区的映射*/
//寄存器地址addr的bitnum位对应Bitband区字地址值
#define BITBAND(addr,bitnum) ((addr & 0xF0000000)+0x2000000+((addr & 0xFFFFF)<<5)+(bitnum<<2)) 	

//将数值adrr强制换成指针类型并取该地址的内容
#define MEM_ADDR(addr)  *((volatile unsigned long *)(addr)) 									 

//地址为addr的寄存器的bitnum位对应别名区的内容
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr,bitnum)) 								 


/*端口寄存器IDR地址定义*/
#define GPIOA_IDR_Addr    (GPIOA_BASE+0x08) 														                                   
#define GPIOB_IDR_Addr    (GPIOB_BASE+0x08) 
#define GPIOC_IDR_Addr    (GPIOC_BASE+0x08)  
#define GPIOD_IDR_Addr    (GPIOD_BASE+0x08) 
#define GPIOE_IDR_Addr    (GPIOE_BASE+0x08)  
#define GPIOF_IDR_Addr    (GPIOF_BASE+0x08)  
#define GPIOG_IDR_Addr    (GPIOG_BASE+0x08) 

/*端口寄存器ODR地址定义*/
#define GPIOA_ODR_Addr    (GPIOA_BASE+0x0C) 														                                   
#define GPIOB_ODR_Addr    (GPIOB_BASE+0x0C) 
#define GPIOC_ODR_Addr    (GPIOC_BASE+0x0C)  
#define GPIOD_ODR_Addr    (GPIOD_BASE+0x0C) 
#define GPIOE_ODR_Addr    (GPIOE_BASE+0x0C)  
#define GPIOF_ODR_Addr    (GPIOF_BASE+0x0C)  
#define GPIOG_ODR_Addr    (GPIOG_BASE+0x0C) 

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

/*NVIC中断优先级分组方式宏*/
#define GroupType_M0L4	((u32)0x700)	//0抢占4响应	
#define GroupType_M1L3	((u32)0x600)	//1抢占3响应
#define GroupType_M2L2	((u32)0x500)	//2抢占2响应
#define GroupType_M3L1	((u32)0x400)	//3抢占1响应
#define GroupType_M4L0	((u32)0x300)	//4抢占0响应

/*函数声明*/
void NVIC_GroupType_Config(u32 GroupType);

#endif
