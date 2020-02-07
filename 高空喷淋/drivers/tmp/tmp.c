# include "tmp.h"
# include "adc.h"



void tmp_init(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );   //使能GPIOA,ADC1通道时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//分频因子6时钟为72M/6=12MHz
	ADC_DeInit(ADC1);  //将外设 ADC1 的全部寄存器重设为缺省值
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1; //顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure); //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
	ADC_TempSensorVrefintCmd(ENABLE); //开启内部温度传感器
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	ADC_ResetCalibration(ADC1); //重置指定的ADC1的复位寄存器
	while(ADC_GetResetCalibrationStatus(ADC1)); //获取ADC1重置校准寄存器的状态,设置状态则等待
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));		//获取指定ADC1的校准程序,设置状态则等待
}




//得到温度值
//返回值:温度值(扩大了100倍,单位:℃.)
short get_tmp(void)    //获取内部温度传感器温度值
{
    u32 adcx;
    short result;
    double temperate;
    adcx = adc_get_average(ADC_Channel_16,20);  //读取通道16,20次取平均
    temperate=(float)adcx*(3.3/4096);       //电压值 
    temperate=(1.43-temperate)/0.0043+25;   //转换为温度值     
    result=temperate*=100;                  //扩大100倍.
    return result;
}






















