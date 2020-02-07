# include "tmp.h"
# include "adc.h"



void tmp_init(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );   //ʹ��GPIOA,ADC1ͨ��ʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//��Ƶ����6ʱ��Ϊ72M/6=12MHz
	ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1; //˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure); //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
	ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	ADC_ResetCalibration(ADC1); //����ָ����ADC1�ĸ�λ�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1)); //��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));		//��ȡָ��ADC1��У׼����,����״̬��ȴ�
}




//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��,��λ:��.)
short get_tmp(void)    //��ȡ�ڲ��¶ȴ������¶�ֵ
{
    u32 adcx;
    short result;
    double temperate;
    adcx = adc_get_average(ADC_Channel_16,20);  //��ȡͨ��16,20��ȡƽ��
    temperate=(float)adcx*(3.3/4096);       //��ѹֵ 
    temperate=(1.43-temperate)/0.0043+25;   //ת��Ϊ�¶�ֵ     
    result=temperate*=100;                  //����100��.
    return result;
}






















