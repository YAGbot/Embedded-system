
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "gpio/bsp_gpio.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ���ذ���IO���ų�ʼ��.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����ʹ�ú궨�巽������������źţ����������ֲ��ֻҪ���޸�bsp_key.h
  *           �ļ���غ궨��Ϳ��Է����޸����š�
  */
void INPUT_GPIO_Init(void)
{
   /* ����IOӲ����ʼ���ṹ����� */
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* ʹ��(����)INPUT���Ŷ�ӦIO�˿�ʱ�� */  
  INPUT1_RCC_CLK_ENABLE();

  
  /* ����INPUT GPIO:��������ģʽ */
  GPIO_InitStruct.Pin = INPUT1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT1_GPIO, &GPIO_InitStruct);   
  
  /* ����INPUT GPIO:��������ģʽ */
  GPIO_InitStruct.Pin = INPUT2_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT2_GPIO, &GPIO_InitStruct);  
  
  /* ����INPUT GPIO:��������ģʽ */
  GPIO_InitStruct.Pin = INPUT3_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT3_GPIO, &GPIO_InitStruct);   
  
  /* ����INPUT GPIO:��������ģʽ */
  GPIO_InitStruct.Pin = INPUT4_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT4_GPIO, &GPIO_InitStruct);   
  
//  /* �����ж����ȼ� */
//  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
//  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

//    /* �����ж����ȼ� */
//  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 1);
//  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
  * ��������: ���ذ���IO���ų�ʼ��.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����ʹ�ú궨�巽������������źţ����������ֲ��ֻҪ���޸�bsp_gpio.h
  *           �ļ���غ궨��Ϳ��Է����޸����š�
  */
void OUTPUT_GPIO_Init(void)
{
   /* ����IOӲ����ʼ���ṹ����� */
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* ʹ��(����)�������Ŷ�ӦIO�˿�ʱ�� */  
  OUTPUT1_RCC_CLK_ENABLE();  
  OUTPUT2_RCC_CLK_ENABLE();  
  OUTPUT3_RCC_CLK_ENABLE();  
  OUTPUT4_RCC_CLK_ENABLE();  
        
  /* ����OUTPUT GPIO:��������ģʽ */
  GPIO_InitStruct.Pin = OUTPUT1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT1_GPIO, &GPIO_InitStruct);   
  
  GPIO_InitStruct.Pin = OUTPUT2_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT2_GPIO, &GPIO_InitStruct);    

  GPIO_InitStruct.Pin = OUTPUT3_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT3_GPIO, &GPIO_InitStruct);   

  GPIO_InitStruct.Pin = OUTPUT4_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT4_GPIO, &GPIO_InitStruct);   
}


