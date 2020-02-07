/**
************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "BasicTIM/bsp_BasicTIM.h" 
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ������ʱ����ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void GENERAL_TIM5_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim5.Instance = GENERAL_TIM5;
  htim5.Init.Prescaler = GENERAL_TIM5_PRESCALER;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = GENERAL_TIM5_PERIOD;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim5);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig);  
}

/**
  * ��������: ������ʱ����ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void BASIC_TIM6_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;

  htim6.Instance = BASIC_TIM6;
  htim6.Init.Prescaler = BASIC_TIM6_PRESCALER;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = BASIC_TIM6_PERIOD;
  HAL_TIM_Base_Init(&htim6);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);
}

/**
  * ��������: ������ʱ����ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void BASIC_TIM7_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;

  htim7.Instance = BASIC_TIM7;
  htim7.Init.Prescaler = BASIC_TIM7_PRESCALER;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = BASIC_TIM7_PERIOD;
  HAL_TIM_Base_Init(&htim7);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig);
}

/**
  * ��������: ������ʱ��Ӳ����ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
  if(htim->Instance==GENERAL_TIM5)
  {
    /* ������ʱ������ʱ��ʹ�� */
    GENERAL_TIM5_RCC_CLK_ENABLE();

    /* �����ж����� */
    HAL_NVIC_SetPriority(GENERAL_TIM5_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(GENERAL_TIM5_IRQ);
  }
	if(htim->Instance==BASIC_TIM6)
  {
    /* ������ʱ������ʱ��ʹ�� */
    BASIC_TIM6_RCC_CLK_ENABLE();

    /* �����ж����� */
    HAL_NVIC_SetPriority(BASIC_TIM6_IRQ, 1,0);
    HAL_NVIC_EnableIRQ(BASIC_TIM6_IRQ);
  }
	if(htim->Instance==BASIC_TIM7)
  {
    /* ������ʱ������ʱ��ʹ�� */
    BASIC_TIM7_RCC_CLK_ENABLE();

    /* �����ж����� */
    HAL_NVIC_SetPriority(BASIC_TIM7_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(BASIC_TIM7_IRQ);
  }
}

/**
  * ��������: ������ʱ��Ӳ������ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim)
{

  if(htim->Instance==GENERAL_TIM5)
  {
    /* ������ʱ������ʱ�ӽ��� */
    GENERAL_TIM5_RCC_CLK_DISABLE();

    /* �ر������ж� */
    HAL_NVIC_DisableIRQ(GENERAL_TIM5_IRQ);
  }
	 if(htim->Instance==BASIC_TIM6)
  {
    /* ������ʱ������ʱ�ӽ��� */
    BASIC_TIM6_RCC_CLK_DISABLE();

    /* �ر������ж� */
    HAL_NVIC_DisableIRQ(BASIC_TIM6_IRQ);
  }
	 if(htim->Instance==BASIC_TIM7)
  {
    /* ������ʱ������ʱ�ӽ��� */
    BASIC_TIM7_RCC_CLK_DISABLE();

    /* �ر������ж� */
    HAL_NVIC_DisableIRQ(BASIC_TIM7_IRQ);
  }
} 
