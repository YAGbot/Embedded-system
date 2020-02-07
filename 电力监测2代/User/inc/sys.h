#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"

//0,��֧��ucos
//1,֧��ucos
#define SYSTEM_SUPPORT_OS		0		//����ϵͳ�ļ����Ƿ�֧��UCOS
																	    
	 
//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
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

//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

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
//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ
void NVIC_Configuration(void);	//���ȼ�����

#endif
