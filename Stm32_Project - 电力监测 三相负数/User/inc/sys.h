#ifndef __SYS__H
#define __SYS__H
#include "stm32f10x.h"


/*λ�������������ӳ��*/
//�Ĵ�����ַaddr��bitnumλ��ӦBitband���ֵ�ֵַ
#define BITBAND(addr,bitnum) ((addr & 0xF0000000)+0x2000000+((addr & 0xFFFFF)<<5)+(bitnum<<2)) 	

//����ֵadrrǿ�ƻ���ָ�����Ͳ�ȡ�õ�ַ������
#define MEM_ADDR(addr)  *((volatile unsigned long *)(addr)) 									 

//��ַΪaddr�ļĴ�����bitnumλ��Ӧ������������
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr,bitnum)) 								 


/*�˿ڼĴ���IDR��ַ����*/
#define GPIOA_IDR_Addr    (GPIOA_BASE+0x08) 														                                   
#define GPIOB_IDR_Addr    (GPIOB_BASE+0x08) 
#define GPIOC_IDR_Addr    (GPIOC_BASE+0x08)  
#define GPIOD_IDR_Addr    (GPIOD_BASE+0x08) 
#define GPIOE_IDR_Addr    (GPIOE_BASE+0x08)  
#define GPIOF_IDR_Addr    (GPIOF_BASE+0x08)  
#define GPIOG_IDR_Addr    (GPIOG_BASE+0x08) 

/*�˿ڼĴ���ODR��ַ����*/
#define GPIOA_ODR_Addr    (GPIOA_BASE+0x0C) 														                                   
#define GPIOB_ODR_Addr    (GPIOB_BASE+0x0C) 
#define GPIOC_ODR_Addr    (GPIOC_BASE+0x0C)  
#define GPIOD_ODR_Addr    (GPIOD_BASE+0x0C) 
#define GPIOE_ODR_Addr    (GPIOE_BASE+0x0C)  
#define GPIOF_ODR_Addr    (GPIOF_BASE+0x0C)  
#define GPIOG_ODR_Addr    (GPIOG_BASE+0x0C) 

/*�˿ڼĴ���BSRR��ַ����*/
#define GPIOA_BSRR_Addr   (GPIOA_BASE+0x10) 						 
#define GPIOB_BSRR_Addr   (GPIOB_BASE+0x10)
#define GPIOC_BSRR_Addr   (GPIOC_BASE+0x10)
#define GPIOD_BSRR_Addr   (GPIOD_BASE+0x10)
#define GPIOE_BSRR_Addr   (GPIOE_BASE+0x10)
#define GPIOF_BSRR_Addr   (GPIOF_BASE+0x10)
#define GPIOG_BSRR_Addr   (GPIOG_BASE+0x10)

/*�˿ڼĴ���BRR��ַ����*/
#define GPIOA_BRR_Addr    (GPIOA_BASE+0x14)
#define GPIOB_BRR_Addr    (GPIOB_BASE+0x14)
#define GPIOC_BRR_Addr    (GPIOC_BASE+0x14)
#define GPIOD_BRR_Addr    (GPIOD_BASE+0x14)
#define GPIOE_BRR_Addr    (GPIOE_BASE+0x14)
#define GPIOF_BRR_Addr    (GPIOF_BASE+0x14)
#define GPIOG_BRR_Addr    (GPIOG_BASE+0x14)						

/*λ�������˿ڶ�*/ 
#define PA_RED(n)    BIT_ADDR(GPIOA_IDR_Addr,n)		                                                                             
#define PB_RED(n)    BIT_ADDR(GPIOB_IDR_Addr,n)
#define PC_RED(n)    BIT_ADDR(GPIOC_IDR_Addr,n)
#define PD_RED(n)    BIT_ADDR(GPIOD_IDR_Addr,n)
#define PE_RED(n)    BIT_ADDR(GPIOE_IDR_Addr,n)
#define PF_RED(n)    BIT_ADDR(GPIOF_IDR_Addr,n)
#define PG_RED(n)    BIT_ADDR(GPIOG_IDR_Addr,n)

/*λ�������˿����*/ 
#define PA_OUT(n)    BIT_ADDR(GPIOA_ODR_Addr,n)		                                                                             
#define PB_OUT(n)    BIT_ADDR(GPIOB_ODR_Addr,n)
#define PC_OUT(n)    BIT_ADDR(GPIOC_ODR_Addr,n)
#define PD_OUT(n)    BIT_ADDR(GPIOD_ODR_Addr,n)
#define PE_OUT(n)    BIT_ADDR(GPIOE_ODR_Addr,n)
#define PF_OUT(n)    BIT_ADDR(GPIOF_ODR_Addr,n)
#define PG_OUT(n)    BIT_ADDR(GPIOG_ODR_Addr,n)

/*λ�������˿���1*/
#define PA_SET(n)    (BIT_ADDR(GPIOA_BSRR_Addr,n)=1)                                    	             							
#define PB_SET(n)    (BIT_ADDR(GPIOB_BSRR_Addr,n)=1)
#define PC_SET(n)    (BIT_ADDR(GPIOC_BSRR_Addr,n)=1)
#define PD_SET(n)    (BIT_ADDR(GPIOD_BSRR_Addr,n)=1)
#define PE_SET(n)    (BIT_ADDR(GPIOE_BSRR_Addr,n)=1)
#define PF_SET(n)    (BIT_ADDR(GPIOF_BSRR_Addr,n)=1)
#define PG_SET(n)    (BIT_ADDR(GPIOG_BSRR_Addr,n)=1)

/*λ�������˿���0*/
#define PA_CLR(n)    (BIT_ADDR(GPIOA_BRR_Addr,n)=1)
#define PB_CLR(n)    (BIT_ADDR(GPIOB_BRR_Addr,n)=1)
#define PC_CLR(n)    (BIT_ADDR(GPIOC_BRR_Addr,n)=1)
#define PD_CLR(n)    (BIT_ADDR(GPIOD_BRR_Addr,n)=1)
#define PE_CLR(n)    (BIT_ADDR(GPIOE_BRR_Addr,n)=1)
#define PF_CLR(n)    (BIT_ADDR(GPIOF_BRR_Addr,n)=1)
#define PG_CLR(n)    (BIT_ADDR(GPIOG_BRR_Addr,n)=1)	

/*NVIC�ж����ȼ����鷽ʽ��*/
#define GroupType_M0L4	((u32)0x700)	//0��ռ4��Ӧ	
#define GroupType_M1L3	((u32)0x600)	//1��ռ3��Ӧ
#define GroupType_M2L2	((u32)0x500)	//2��ռ2��Ӧ
#define GroupType_M3L1	((u32)0x400)	//3��ռ1��Ӧ
#define GroupType_M4L0	((u32)0x300)	//4��ռ0��Ӧ

/*��������*/
void NVIC_GroupType_Config(u32 GroupType);

#endif
