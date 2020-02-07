/**
  ******************************************************************************
  * @file    clk.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/2/25
  * @brief   ϵͳʱ�� ��������
  ******************************************************************************
  */
#include "clk.h"



//JTAGģʽ����,��������JTAG��ģʽ
//mode:jtag,swdģʽ����;00,ȫʹ��;01,ʹ��SWD;10,ȫ�ر�;	   
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00		  
void jtag_set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR |= 1<<0;     //��������ʱ��	   
	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
	AFIO->MAPR|=temp;       //����jtagģʽ	
} 

 

/**
  * @brief  ϵͳʱ�ӳ�ʼ����72M
  * @retval ��
  */
void clk_72m(void)
{
	u32 StartUpCounter = 0, HSEStatus = 0;

	/* SYSCLK, HCLK, PCLK2, PCLK1 ʱ������ */
	/* ʹ���ⲿ����ʱ��(HSE) */
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	/* �ȴ��ⲿ����ʱ��(HSE)׼���ã������ʱ��ǿ���˳� */
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;  
	} while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	/* �жϸ���ʱ�Ӿ����Ƿ�׼���� */
	if ((RCC->CR & RCC_CR_HSERDY) != RESET)
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		HSEStatus = (uint32_t)0x00;
	}  

	/* ���׼���� */
	if (HSEStatus == (uint32_t)0x01)
	{
		/* ʹ��Ԥ��buf */
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

		/*	PLL ����: PLLCLK = HSE * 9 = 72 MHz */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
		RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

		/* ʹ�� PLL��Ƶ */
		RCC->CR |= RCC_CR_PLLON;

		/* �ȴ�PLL��Ƶʱ��׼���� */
		while((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}

		/* ����PLL��Ƶʱ����Ϊϵͳʱ��Դ */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

		/* Wait till PLL is used as system clock source */

		/* �ȴ�ϵͳʱ����ȫʹ��PLLʱ����Ϊʱ��Դ */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
		{
		}
	}
	else
	{ 
		/* ���HSE��������Ӧ�ó��򽫻��д����ʱ�����á��û��������������һЩ�������������erro */
	}
}


/**
  * @brief  ϵͳ����72M
  * @retval �� 
  */
void clk_init(void)
{
	/* ��RCCʱ����������ΪĬ�ϵ�����״̬(���ڵ���Ŀ��) */
	/* ���� HSION λ */
	RCC->CR |= (uint32_t)0x00000001;

	/* ��λ SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO ��־λ */
	RCC->CFGR &= (uint32_t)0xF0FF0000;

	/* ��λ HSEON, CSSON and PLLON ��־λ */
	RCC->CR &= (uint32_t)0xFEF6FFFF;

	/* ��λ HSEBYP ��־λ�� */
	RCC->CR &= (uint32_t)0xFFFBFFFF;

	/* ����PLLSRC��PLLXTPRE��PLLMUL��usbpre/otgfspre λ */
	RCC->CFGR &= (uint32_t)0xFF80FFFF;

	/* ���������жϺ�����ݹ�λ  */
	RCC->CIR = 0x009F0000;

	/* ����ϵͳʱ��Ƶ�ʡ�HCLK��PCLK2��PCLK1Ԥ���� */
	/* ����Flash�ӳ����ڲ�����Ԥץȡ������ */
	clk_72m();

	/* �ڲ��������ʸ�������¶�λ */
	SCB->VTOR = FLASH_BASE/* | VECT_TAB_OFFSET*/; 

    /* ����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

    jtag_set(SWD_ENABLE);

}






