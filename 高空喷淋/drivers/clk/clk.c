/**
  ******************************************************************************
  * @file    clk.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/2/25
  * @brief   系统时钟 驱动代码
  ******************************************************************************
  */
#include "clk.h"



//JTAG模式设置,用于设置JTAG的模式
//mode:jtag,swd模式设置;00,全使能;01,使能SWD;10,全关闭;	   
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00		  
void jtag_set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR |= 1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式	
} 

 

/**
  * @brief  系统时钟初始化到72M
  * @retval 无
  */
void clk_72m(void)
{
	u32 StartUpCounter = 0, HSEStatus = 0;

	/* SYSCLK, HCLK, PCLK2, PCLK1 时钟配置 */
	/* 使能外部高速时钟(HSE) */
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	/* 等待外部高速时钟(HSE)准备好，如果超时则强制退出 */
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;  
	} while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	/* 判断高速时钟晶振是否准备好 */
	if ((RCC->CR & RCC_CR_HSERDY) != RESET)
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		HSEStatus = (uint32_t)0x00;
	}  

	/* 如果准备好 */
	if (HSEStatus == (uint32_t)0x01)
	{
		/* 使能预存buf */
		FLASH->ACR |= FLASH_ACR_PRFTBE;

		/* Flash 2 wait state */
		FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
		FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;	

		/* HCLK = SYSCLK */
		RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

		/* PCLK2 = HCLK */
		RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

		/* PCLK1 = HCLK */
		RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

		/*	PLL 配置: PLLCLK = HSE * 9 = 72 MHz */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
		RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

		/* 使能 PLL倍频 */
		RCC->CR |= RCC_CR_PLLON;

		/* 等待PLL倍频时钟准备好 */
		while((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}

		/* 设置PLL倍频时钟作为系统时钟源 */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

		/* Wait till PLL is used as system clock source */

		/* 等待系统时钟完全使用PLL时钟作为时钟源 */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
		{
		}
	}
	else
	{ 
		/* 如果HSE不启动，应用程序将会有错误的时钟配置。用户可以在这里添加一些代码来处理这个erro */
	}
}


/**
  * @brief  系统函数72M
  * @retval 无 
  */
void clk_init(void)
{
	/* 将RCC时钟配置重置为默认的重置状态(用于调试目的) */
	/* 设置 HSION 位 */
	RCC->CR |= (uint32_t)0x00000001;

	/* 复位 SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO 标志位 */
	RCC->CFGR &= (uint32_t)0xF0FF0000;

	/* 复位 HSEON, CSSON and PLLON 标志位 */
	RCC->CR &= (uint32_t)0xFEF6FFFF;

	/* 复位 HSEBYP 标志位置 */
	RCC->CR &= (uint32_t)0xFFFBFFFF;

	/* 重设PLLSRC，PLLXTPRE，PLLMUL和usbpre/otgfspre 位 */
	RCC->CFGR &= (uint32_t)0xFF80FFFF;

	/* 禁用所有中断和清除暂挂位  */
	RCC->CIR = 0x009F0000;

	/* 配置系统时钟频率、HCLK、PCLK2和PCLK1预标器 */
	/* 配置Flash延迟周期并启用预抓取缓冲区 */
	clk_72m();

	/* 内部向量表的矢量表重新定位 */
	SCB->VTOR = FLASH_BASE/* | VECT_TAB_OFFSET*/; 

    /* 设置NVIC中断分组2:2位抢占优先级，2位响应优先级 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

    jtag_set(SWD_ENABLE);

}






