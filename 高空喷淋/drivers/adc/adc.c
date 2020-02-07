/**
  ******************************************************************************
  * @file    adc.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/2/25
  * @brief   ADC驱动
  ******************************************************************************
  */
#include "adc.h"
#include "delay.h"


/**
  * @brief  ADC初始化函数
  * @retval 无
  */
void adc_init(void)
{
    ADC_InitTypeDef ADC_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ADC时钟使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1, ENABLE);	  

    /*  设置ADC分频因子6 72M/6=12,ADC最大频率不能超过14M */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   

    /* 设置光敏、电位器、电池、ADC采集通道  */
    GPIO_InitStructure.GPIO_Pin = ADC_LIGHT_POT_BAT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(ADC_LIGHT_POT_BAT_PORT, &GPIO_InitStructure);	

    /* 设置按键 ADC采集通道  */
    GPIO_InitStructure.GPIO_Pin = ADC_KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(ADC_KEY_PORT, &GPIO_InitStructure);	

    /* 复位ADC1寄存器 */
    ADC_DeInit(ADC1); 

    /* ADC工作模式:ADC1和ADC2工作在独立模式 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
    /* 单通道模式 */
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
    /* 单次转换 */
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	
    /* 软件触发 */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
    /* ADC右对齐 */
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
    /* 通道转换数目 */
    ADC_InitStructure.ADC_NbrOfChannel = 1;	
    /* 初始化ADC */
    ADC_Init(ADC1, &ADC_InitStructure);	

    /* 开启内部温度传感器	 */
    ADC_TempSensorVrefintCmd(ENABLE); 
    
    /* 使能ADC */
    ADC_Cmd(ADC1, ENABLE);	
    /* 复位校准 */
    ADC_ResetCalibration(ADC1);	
    /* 等待复位校准结束 */
    while(ADC_GetResetCalibrationStatus(ADC1))
    {
        assert_param(0); 
    }
    /* 开启AD校准 */
    ADC_StartCalibration(ADC1);	 
    /* 等待校准结束 */
    while(ADC_GetCalibrationStatus(ADC1));	 
}

//获得ADC值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
u16 adc_get(u32 ch)   
{
    /* 设置指定ADC的规则组通道 */
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	

    /* 使能指定的ADC1的软件转换启动功能	 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		

    /* 等待转换结束 */
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));

    /* 返回ADC采样值 */
    return ADC_GetConversionValue(ADC1);	
}


//获取指定通道的转换值，取times次,然后平均 
//times:获取次数
//返回值:通道ch的times次转换结果平均值


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



