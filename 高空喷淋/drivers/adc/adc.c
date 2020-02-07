/**
  ******************************************************************************
  * @file    adc.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/2/25
  * @brief   ADC����
  ******************************************************************************
  */
#include "adc.h"
#include "delay.h"


/**
  * @brief  ADC��ʼ������
  * @retval ��
  */
void adc_init(void)
{
    ADC_InitTypeDef ADC_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ADCʱ��ʹ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1, ENABLE);	  

    /*  ����ADC��Ƶ����6 72M/6=12,ADC���Ƶ�ʲ��ܳ���14M */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   

    /* ���ù�������λ������ء�ADC�ɼ�ͨ��  */
    GPIO_InitStructure.GPIO_Pin = ADC_LIGHT_POT_BAT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(ADC_LIGHT_POT_BAT_PORT, &GPIO_InitStructure);	

    /* ���ð��� ADC�ɼ�ͨ��  */
    GPIO_InitStructure.GPIO_Pin = ADC_KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(ADC_KEY_PORT, &GPIO_InitStructure);	

    /* ��λADC1�Ĵ��� */
    ADC_DeInit(ADC1); 

    /* ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
    /* ��ͨ��ģʽ */
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
    /* ����ת�� */
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	
    /* ������� */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
    /* ADC�Ҷ��� */
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
    /* ͨ��ת����Ŀ */
    ADC_InitStructure.ADC_NbrOfChannel = 1;	
    /* ��ʼ��ADC */
    ADC_Init(ADC1, &ADC_InitStructure);	

    /* �����ڲ��¶ȴ�����	 */
    ADC_TempSensorVrefintCmd(ENABLE); 
    
    /* ʹ��ADC */
    ADC_Cmd(ADC1, ENABLE);	
    /* ��λУ׼ */
    ADC_ResetCalibration(ADC1);	
    /* �ȴ���λУ׼���� */
    while(ADC_GetResetCalibrationStatus(ADC1))
    {
        assert_param(0); 
    }
    /* ����ADУ׼ */
    ADC_StartCalibration(ADC1);	 
    /* �ȴ�У׼���� */
    while(ADC_GetCalibrationStatus(ADC1));	 
}

//���ADCֵ
//ch: ͨ��ֵ 0~16��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
//����ֵ:ת�����
u16 adc_get(u32 ch)   
{
    /* ����ָ��ADC�Ĺ�����ͨ�� */
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	

    /* ʹ��ָ����ADC1�����ת����������	 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		

    /* �ȴ�ת������ */
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));

    /* ����ADC����ֵ */
    return ADC_GetConversionValue(ADC1);	
}


//��ȡָ��ͨ����ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ


u16 adc_get_average(u32 ch, u8 times)
{
	u32 temp_val = 0;
	u8 t = 0;
	
	for(t=0;t<times;t++)
	{
		temp_val += adc_get(ch);
	}
	
	return temp_val/times;
} 



