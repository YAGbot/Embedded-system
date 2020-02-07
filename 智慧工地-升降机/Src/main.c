/* 包含头文件 ----------------------------------------------------------------*/
#include "stdio.h"
#include <math.h>
#include "stm32f4xx_hal.h"
#include "string.h" 	 
#include "led/bsp_led.h"
#include "BasicTIM/bsp_BasicTIM.h" 
#include "spiflash/bsp_spiweight.h"
#include "spiflash/bsp_spiflash.h"
#include "usart/bsp_usartx.h"
#include "usart/bsp_485_usart.h"
#include "HMI/bsp_usartx_HMI.h"
#include "usart/bsp_gsm_usart.h"
#include "adc/bsp_adc.h"
#include "gpio/bsp_gpio.h"
#include "hx711/hx711.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 获取缓冲区的长度 */
#define  FLASH_WriteAddress     0x0000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
#define  N 											40


/* 私有变量 ------------------------------------------------------------------*/
uint8_t qingjiao[7]={0x5A,0xA5,0x20,0xf0,0x01,0x03,0x00};	//读取倾角传感器指令
uint8_t shijian[7]={0x5A,0xA5,0x04,0x83,0x00,0x10,0x04};	//读取当前时间
uint8_t wendu[8]={0x05,0x03,0x00,0x00,0x00,0x02,0xC5,0x8F};
uint8_t  ID_buff[15]={0};
uint32_t DeviceID = 0;
uint32_t FlashID = 0;

uint8_t name[10];
uint8_t NameID[20];
char EndStr[300];

uint8_t USART1_RX_flg=0;
uint8_t register_time_flag=0;
uint8_t check_ID_flag=1;
uint8_t ALARM_FLAG=0;
uint8_t ALARM_FLAG_WEIGHT=0;	//报警触发标志，防止多次报警触发，0:遇到报警条件可触发报警 1：已触发报警需等待警戒状态解除后可触发报警
uint8_t ALARM_FLAG_HIGH=0;
uint8_t ALARM_FLAG_ANGLEX=0;
uint8_t ALARM_FLAG_ANGLEY=0;
uint8_t angle_warning=0;
uint8_t weight_warning=0;
uint8_t high_warning=0;
uint8_t IO1,IO2;	//门监测状态
int32_t Result_data=0;
uint8_t write_flag=0;		//写入标志，有一个变化就要重新写入flash
uint16_t count=0;

uint8_t Tx_Buffer[29] = {0};		//spi通信内存，按字节读写
uint8_t Rx_Buffer[29] = {0};

__IO uint8_t  weight_Zero_IsInit=1; //0: 校准值未获取  1：已获取校准值

float weight_proportion=0; //重量比例系数
float high_proportion=0; //高度比例系数
__IO int32_t weight_Zero_Data=8388608;  //零值	-- 电压为0时的实际采样值 
__IO int16_t weight_current;    //当前重量采样值
__IO int32_t weight_initial_value;	//初始值,相当于公式中的b
__IO uint16_t high_current;    //当前高度采样值
__IO int32_t high_initial_value;	//初始值,相当于公式中的b


__IO uint16_t previous_weight_count;	//记录上次重量

__IO int16_t weight_change=0;	//记录两次关门时的重量差
__IO uint16_t weight_close=0;	//记录每次关门启动时重量值
__IO uint16_t weight_count=0;	//当前重量值
__IO uint16_t weight_limit=0;	//重量阈值
__IO uint16_t weight_angle=0;	//重量百分比 当前重量/重量阈值  weight_count/weight_limit

float previous_high_count;	//记录上次高度
float high_count=0;	//当前高度值
__IO uint16_t high_limit=0;	//高度阈值
float high_speed=0;	//速度  ==（当前高度-上次高度）  /2.3  m/s
float net_high_temp;	//远程传输高度临时值
/*********重量值标定参数**********/
__IO uint16_t weight_zero;	//空载采样值
__IO uint16_t weight_add;		//加重后采样值
__IO uint16_t weight_zero_value;	//空载重量值
__IO uint16_t weight_add_value;		//加重后重量值

/*********高度传感器标定参数**********/ 
__IO uint16_t high_zero;	//轿厢置底采样值
__IO uint16_t high_top;		//轿厢置顶采样值
__IO uint16_t high_zero_value;	//轿厢置底实际离地面高度
__IO uint16_t high_top_value;		//轿厢置顶实际离地面高度

float angle_limit=0;	//倾角阈值
__IO uint16_t worktime_limit;	//默认工作时间设置
__IO uint16_t alreadyworktime;	//运行时间
/*************前后门标定参数*******************/
__IO uint16_t back_door;		//1:开  0：关
__IO uint16_t front_door;
__IO uint16_t work_load_cnt;		//装载次数
__IO uint16_t illegal_operation_cnt;	//违规次数
//__IO uint8_t workcycle_flag;//工作循环判断 1：电源 0：门锁
__IO uint8_t work_register_sta=0;	//人员注册状态 1;允许工作  0：不允许工作 由考勤机判定

__IO uint16_t timer_count=0;	//倒计时轮询网络状态计数
__IO uint16_t Work_Count=0;		//工作采集时间计数
__IO uint16_t Count=0;

uint8_t io1_temp,io2_temp;

// 用于保存转换计算后的电压值	 
float ADC_ConvertedValueLocal;
// AD转换结果值
uint16_t ADC_ConvertedValue[40];
uint32_t DMA_Transfer_Complete_Count=0;

__IO uint32_t ad7190_data[4];	//ad7190采集值

/***********角度变量***************/
float x_angle,y_angle;

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
void SystemClock_Config(void);
void calibrate_parameter(void);
int32_t int_abs(int32_t value1,int32_t value2);
uint16_t filter(void);
void io_check(void);
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{
  /* 复位所有外设，初始化Flash接口和系统滴答定时器 */
  HAL_Init();
  /* 配置系统时钟 */
  SystemClock_Config();
  
  /* 板载LED初始化 */
  LED_GPIO_Init();
	/* 输入监测初始化 */
  INPUT_GPIO_Init();
	  /* 输出端口初始化 */
  OUTPUT_GPIO_Init();
	HAL_GPIO_WritePin(OUTPUT1_GPIO, OUTPUT1_GPIO_PIN, GPIO_PIN_RESET);	//默认值，光耦导通，OUTx输出端为低,有电压
	HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_RESET);	//220电压电路关闭，状态与其他相反，导通无电压
	HAL_GPIO_WritePin(OUTPUT3_GPIO, OUTPUT3_GPIO_PIN, GPIO_PIN_SET);  
//	HAL_GPIO_WritePin(OUTPUT1_GPIO, OUTPUT1_GPIO_PIN, GPIO_PIN_SET);  //输出高，光耦关闭无电压 
	//HAL_GPIO_WritePin(OUTPUT2_GPIO, OUTPUT2_GPIO_PIN, GPIO_PIN_RESET);  //光耦导通，OUTx输出端为低,有电压
	
  /* 初始化串口资源 */
	USARTx_Init();				//调试串口
	HMI_USARTx_Init();		//串口屏
	GSM_USARTx_Init();		//gsm模块
	RS485_USART_Init();		//485
  MX_USARTx_Init();			//串口1接考勤机
//	/* 初始化DMA */
//	MX_DMA_Init();

	/*HX711初始化*/
	ADInit();
	
//  /* ADC 初始化 */
//  MX_ADCx_Init();
///* 启动AD转换并使能DMA传输和中断 */
//  HAL_ADC_Start_DMA(&hadcx,(uint32_t *)ADC_ConvertedValue,40);  
	
	 /* 初始化SPI */
  MX_SPIFlash_Init();
	 /* Get SPI Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();
  
  HAL_Delay(2000);
	
	FlashID = SPI_FLASH_ReadID(); 
  printf("FlashID is 0x%X,  Manufacturer Device ID is 0x%X\n", FlashID, DeviceID);
	if (FlashID == SPI_FLASH_ID)  /* #define  sFLASH_ID  0XEF4018 */
	{	
		uint8_t i;
		printf("检测到华邦串行flash W25Q128 !\n");
    SPI_FLASH_BufferRead(Rx_Buffer,FLASH_ReadAddress,sizeof(Rx_Buffer));
		if(Rx_Buffer[28]==0x5a)		//有数据改动了，直接读取显示
		{
			/**********将rx内容更新到tx防止掉电后buf内容被清空写0到flash*******************/
			for(i=0;i<29;i++)
			{
				Tx_Buffer[i]=Rx_Buffer[i];
			}
			weight_zero_value=Rx_Buffer[0]*256+Rx_Buffer[1];
			diwen_send_data16(0x12b0,(uint16_t)weight_zero_value);	
			printf("weight_zero_value=%d\r\n",weight_zero_value);
			
			weight_zero=Rx_Buffer[2]*256+Rx_Buffer[3];
			diwen_send_data16(0x12c0,(uint16_t)weight_zero);	

			weight_add_value=Rx_Buffer[4]*256+Rx_Buffer[5];
			diwen_send_data16(0x12f0,(uint16_t)weight_add_value);	

			weight_add=Rx_Buffer[6]*256+Rx_Buffer[7];
			diwen_send_data16(0x1300,(uint16_t)weight_add);	

			weight_limit=Rx_Buffer[8]*256+Rx_Buffer[9];
			diwen_send_data16(0x1340,weight_limit);	

			back_door=Rx_Buffer[10]*256+Rx_Buffer[11];
			diwen_send_data16(0x1320,back_door);

			front_door=Rx_Buffer[12]*256+Rx_Buffer[13];
			diwen_send_data16(0x1310,front_door);	
		
			high_zero_value=(Rx_Buffer[14]*256+Rx_Buffer[15])/10;
			diwen_send_data16(0x1290,high_zero_value*10);	
			
			high_zero=Rx_Buffer[16]*256+Rx_Buffer[17];
			diwen_send_data16(0x12a0,high_zero);	
			
			high_top_value=(Rx_Buffer[18]*256+Rx_Buffer[19])/10;
			diwen_send_data16(0x12d0,high_top_value*10);	
			
			high_top=Rx_Buffer[20]*256+Rx_Buffer[21];
			diwen_send_data16(0x12e0,high_top);	
			
			high_limit=(Rx_Buffer[22]*256+Rx_Buffer[23])/10;
			diwen_send_data16(0x1330,high_limit*10);	
			
			angle_limit=(Rx_Buffer[24]*256+Rx_Buffer[25])/100;
			diwen_send_data16(0x1350,angle_limit*100);	
			
			worktime_limit=Rx_Buffer[26]*256+Rx_Buffer[27];
			diwen_send_data16(0x1360,(uint16_t)worktime_limit);	
			printf("worktime_limit=%d\r\n",worktime_limit);
    }
  }  	  
	
	
	calibrate_parameter();	//比例计算
	
	
   if(AD7190_Init()==0)
	{
    printf("获取不到 AD7190 !\n");
    while(1)
    {
      HAL_Delay(1000);
      if(AD7190_Init())
        break;
    }
  }
  printf("检测到  AD7190 !\n");
  ad7190_conf(); 
  /* 基本定时器初始化：10ms中断一次 */
	GENERAL_TIM5_Init();	//485
  BASIC_TIM6_Init();		//主函数定时器
	BASIC_TIM7_Init();	//串口屏
	
  /* 在中断模式下启动定时器 */
	HAL_TIM_Base_Start_IT(&htim5);	

	//使能接收，进入中断回调函数
	HAL_UART_Receive_IT(&husartx_HMI,&hmiRxBuffer,1);
	HAL_UART_Receive_IT(&husartx_GSM,&gsmRxBuffer,1);
  HAL_UART_Receive_IT(&usartx_1,&aRxBuffer,1);
//	//读前后门初始状态
	IO1=HAL_GPIO_ReadPin(INPUT1_GPIO,INPUT1_GPIO_PIN);
	IO2=HAL_GPIO_ReadPin(INPUT2_GPIO,INPUT2_GPIO_PIN);
	printf("IO1:%d,IO2:%d\r\n",IO1,IO2);
	if(front_door==1)
	{
		IO1=!IO1;
	}
	if(back_door==1)
	{
		IO2=!IO2;
	}
	io1_temp=IO1;
	io2_temp=IO2;
	io_check();
	
  LED2_ON;
	//HAL_Delay(10000);
	if(GSM_cmd("usr.cnAT+IMEI\r\n","+IMEI:",100)==GSM_TRUE)
	{
		uint8_t i;
		uint8_t tmpbuff[7];
		for(i=0;i<6;i++)
		{
			tmpbuff[i]=gsm_uart_buff[18+i];//无回显，有：24
		}
		tmpbuff[6]='\0';
		printf("%s\r\n",gsm_uart_buff);
		sprintf((char *)ID_buff,"THHY%s",tmpbuff);
		printf("%s",ID_buff);
		check_ID_flag=0;
	}
	diwen_send_numascii(0x1130,ID_buff,10);	//发送ID号至显示屏
	
	diwen_send_numascii(0x1070,name,8);		
	diwen_send_numascii(0x1080,NameID,8);
	
	diwen_send_data16(0x10f0,0x00);
	diwen_send_data16(0x1150,0x00);
	//清除装载计数
	diwen_send_data16(0x1160,work_load_cnt);
	diwen_send_data16(0x1170,illegal_operation_cnt);
	
	HAL_Delay(100);
	
  /* 无限循环 */
  while (1)
  {
		
		/***************判断工作运行时长************************/
		if((work_register_sta==1)&&(Count>=6000))
		{ 
			Count=0;
			worktime_limit--;
			alreadyworktime++;
			
			diwen_send_data16(0x10f0,worktime_limit);
			diwen_send_data16(0x1150,alreadyworktime);
			if(worktime_limit==0)		//本次工作时间结束 
			{
				//关闭电源
				HAL_GPIO_WritePin(OUTPUT1_GPIO, OUTPUT1_GPIO_PIN, GPIO_PIN_RESET);  //打开考勤机
				HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_RESET);  //关闭控制系统
				worktime_limit=Rx_Buffer[26]*256+Rx_Buffer[27];	//复位工作时长
				diwen_send_data16(0x10f0,0x00);
				diwen_send_data16(0x1150,0x00);
				//清除装载计数
				diwen_send_data16(0x1160,work_load_cnt);
				diwen_send_data16(0x1170,illegal_operation_cnt);
				alreadyworktime=0;
				work_register_sta=0;
				work_load_cnt=0;
				illegal_operation_cnt=0;
			}
		}
		/*******************获取当前注册时间*********************************/
		if(register_time_flag==1)
		{
			register_time_flag=0;
		//获取当前注册时间
			HMI_USARTx_RX_STA=0;
			memset(HMI_USARTx_RX_BUF,0,200);
			HMI_USARTx_SendDat(shijian,7);
			HAL_Delay(50);
			uint8_t year,month,day,hour,minute,sec;
			if(HMI_USARTx_RX_STA&0X8000)	//串口屏接收到一次数据了
			{
				USART_Tx(HMI_USARTx_RX_BUF,20);
				HMI_USARTx_RX_STA=0;			//标志初始化	 
				if(HMI_USARTx_RX_BUF[3+6]==0x83)	//接收到变量修改了
				{
					year=HMI_USARTx_RX_BUF[7+6];
					month=HMI_USARTx_RX_BUF[8+6];
					day=HMI_USARTx_RX_BUF[9+6];
					hour=HMI_USARTx_RX_BUF[11+6];
					minute=HMI_USARTx_RX_BUF[12+6];
					sec=HMI_USARTx_RX_BUF[13+6];
					printf("%d.%d.%d.%d.%d.%d\r\n",2000+year,month,day,hour,minute,sec);
					diwen_send_data16(0x1090,2000+year);
					diwen_send_data16(0x10a0,month);
					diwen_send_data16(0x10b0,day);
					diwen_send_data16(0x10c0,hour);
					diwen_send_data16(0x10d0,minute);
					diwen_send_data16(0x10e0,sec);
				}
		}
	}
		/***********************查询网络连接状态&&数据发送************************************/
    if(timer_count>=3000)		//30s计时 查询网络&&周期心跳数据
    {
			if(check_ID_flag==1)
			{
				if(GSM_cmd("usr.cnAT+IMEI\r\n","+IMEI:",100)==GSM_TRUE)
				{
					uint8_t i;
					uint8_t tmpbuff[7];
					for(i=0;i<6;i++)
					{
						tmpbuff[i]=gsm_uart_buff[18+i];//无回显，有：24
					}
					tmpbuff[6]='\0';
					printf("%s\r\n",gsm_uart_buff);
					sprintf((char *)ID_buff,"THHY%s",tmpbuff);
					printf("%s",ID_buff);
					check_ID_flag=0;
				}
	
				diwen_send_numascii(0x1130,ID_buff,10);	//发送ID号至显示屏
			}
			HAL_Delay(100);
			/*4G模块查询网络状态*/
			if(GSM_cmd("usr.cnAT+SOCKALK\r\n","+SOCKALK:ON",100)==GSM_TRUE)
			{
				printf("%s",gsm_uart_buff);
				printf("联网成功\r\n");
				clean_rebuff();                 //清空了接收缓冲区数据,以等待服务器远程接收
				diwen_send_asci3(0x1140,0xd2d1,0xc1ac,0xbdd3);	//发送已连接状态
			}
			else 
			{
				printf("%s",gsm_uart_buff);
				diwen_send_asci3(0x1140,0xceb4,0xc1ac,0xbdd3);	//发送未连接状态
			}
			sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"jump\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
			"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightlimit\":\"%d\",\"highlimit\":\"%d\",\"anglelimit\":\"%.1f\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,\
			high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_limit,high_limit,angle_limit,name,NameID,alreadyworktime);
			printf("%d\r\n",strlen(EndStr));
			printf("%s",EndStr);
			GSM_USART_printf("%s",EndStr);
			
      timer_count=0;
      LED1_TOGGLE;
      LED2_TOGGLE;
      LED3_TOGGLE;
    }
		/*********************周期工作采集*********************************************/
		if(Work_Count>=100)		//1s计时
		{
			HAL_TIM_Base_Stop_IT(&htim5);
			
			//	//读前后门初始状态
			IO1=HAL_GPIO_ReadPin(INPUT1_GPIO,INPUT1_GPIO_PIN);
			IO2=HAL_GPIO_ReadPin(INPUT2_GPIO,INPUT2_GPIO_PIN);
			//printf("IO1:%d,IO2:%d\r\n",IO1,IO2);
			if(front_door==1)
			{
				IO1=!IO1;
			}
			if(back_door==1)
			{
				IO2=!IO2;
			}
			if((IO1!=io1_temp)||(IO2!=io2_temp))	//有门状态更新了
			{
				io_check();
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
	"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightlimit\":\"%d\",\"highlimit\":\"%d\",\"anglelimit\":\"%.1f\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_limit,high_limit,angle_limit,name,NameID,alreadyworktime);
	//printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
			}
			
			io1_temp=IO1;
			io2_temp=IO2;		//记录最新门状态
			
			
			unsigned long ad;
				/**************重量传感器采集***************/ 
			ad=ReadAD()/838.8608;
			//printf("ad=%d\r\n",ad);  
			weight_current=ad-10000;	//当前重量采样值
			if(weight_current<0)
			{
				weight_current=0; 
			}
			diwen_send_data16(0x1280,weight_current);
			weight_count=(weight_current-weight_zero)*weight_proportion;	//当前重量值
			if((ALARM_FLAG_WEIGHT==0)&&(weight_count>=weight_limit))		//超出阈值
			{
				weight_warning=1;
				ALARM_FLAG_WEIGHT=1;	//报警后赋为1 waring =0
			}

			if((ALARM_FLAG_WEIGHT==1)&&(weight_count<weight_limit))
			{
				ALARM_FLAG_WEIGHT=0;
				ALARM_FLAG=1;
			}

			previous_weight_count=weight_count;		//记录为上次重量值
			diwen_send_data16(0x1010,weight_count);
			weight_angle=weight_count*180/weight_limit;
			diwen_send_data16(0x1000,weight_angle);
			//printf("->%d,重量:%dKg,比例：%d\r\n",weight_current,weight_count,weight_angle);
			
			
			/**************高度传感器采集***************/      
			if(AD7190_RDY_STATE==0)
			{
				uint8_t sample[4]={0};
				HAL_SPI_Receive(&hspi_weight,sample,4,0xFFFFFF); 
				if((sample[3]&0x80)==0)
				{
					uint8_t temp=(sample[3]&0x07);                                                      
					ad7190_data[temp]=(sample[0]<<16) | (sample[1]<<8) | (sample[2]);
					high_current=((float)ad7190_data[temp]/0x800000-1)*2997;//万用表实测参考电压2997mV
				}
			}
			//printf("high_current=%d\r\n",high_current);
			diwen_send_data16(0x1270,high_current);
			high_count=(high_current-high_zero)*high_proportion;	//当前高度值
			if(ALARM_FLAG_HIGH==0)
			{
				diwen_send_data16(0x1030,(uint16_t)((high_count+0.05f)*10));
			}
			else
				diwen_send_data16(0x1030,(uint16_t)high_limit*10);
			//printf("high_count=%0.3f\r\n",high_count);
			if(fabs(high_count-net_high_temp)>=1)
			{
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightlimit\":\"%d\",\"highlimit\":\"%d\",\"anglelimit\":\"%.1f\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_limit,high_limit,angle_limit,name,NameID,alreadyworktime);
				printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
				net_high_temp=high_count;
			}
			
			if((ALARM_FLAG_HIGH==0)&&(high_count>=high_limit))		//超出阈值
			{
				high_warning=1;
				ALARM_FLAG_HIGH=1;	//报警后赋为1 waring =0
			}
			if((ALARM_FLAG_HIGH==1)&&(high_count<high_limit))
			{
				ALARM_FLAG_HIGH=0;
				ALARM_FLAG=1;		//恢复发送标志
			}
			
			//high_speed=high_count-previous_high_count;		//计时1s采集时间
			high_speed=fabs(high_count-previous_high_count);
			//printf("high_speed=%0.3f\r\n",high_speed);
			if((high_count>previous_high_count)&&(high_speed>0.25f))
			{
				diwen_send_data16(0x1020,0x0001);
				diwen_send_data16(0x1040,(uint16_t)((high_speed+0.05f)*10));
			}
			else if((high_count<previous_high_count)&&(high_speed>0.25f))
			{
				diwen_send_data16(0x1020,0x0002);
				diwen_send_data16(0x1040,(uint16_t)((high_speed+0.05f)*10));
			}
			else if(high_speed<0.25f)
			{
				diwen_send_data16(0x1020,0x0000);
				diwen_send_data16(0x1040,0x0000);
			}
				previous_high_count=high_count;		//记录为上次高度值
			

			
			/*************倾角传感器采集***************/
			memset(rs485Rx_Buf,0,256);
			//UART_Tx(qingjiao,7);
			UART_Tx(wendu,8);
			HAL_Delay(100);
			
			if(RS485_USARTx_RX_STA&0X8000)	//485接收到一次数据了
			{
				RS485_USARTx_RX_STA=0;			//标志初始化	 
				//数据比对
				if(rs485Rx_Buf[7]==0x01)	//正数
				{
					x_angle=rs485Rx_Buf[8]+(float)(rs485Rx_Buf[9]+rs485Rx_Buf[10]*256)/1000;
					diwen_send_data16(0x1050,x_angle*10);
					//printf("x_angle=%0.1f\r\n",x_angle);
				}
				if(rs485Rx_Buf[7]==0x00)	//负数
				{
					x_angle=0-(rs485Rx_Buf[8]+(float)(rs485Rx_Buf[9]+rs485Rx_Buf[10]*256)/1000);
					diwen_send_data16(0x1050,x_angle*10);
					//printf("x_angle=%0.1f\r\n",x_angle);
				}
				if(rs485Rx_Buf[13]==0x01)	//正数
				{
					y_angle=rs485Rx_Buf[14]+(float)(rs485Rx_Buf[15]+rs485Rx_Buf[16]*256)/1000;
					diwen_send_data16(0x1060,y_angle*10);
					//printf("y_angle=%0.1f\r\n",y_angle);
				}
				if(rs485Rx_Buf[13]==0x00)	//负数
				{
					y_angle=0-(rs485Rx_Buf[14]+(float)(rs485Rx_Buf[15]+rs485Rx_Buf[16]*256)/1000);
					diwen_send_data16(0x1060,y_angle*10);
					//printf("y_angle=%0.1f\r\n",y_angle);
				}
				
				
				if((ALARM_FLAG_ANGLEX==0)&&(fabs(x_angle)>=angle_limit))		//超出阈值
				{
					angle_warning=1;
					ALARM_FLAG_ANGLEX=1;	//报警后赋为1 waring =0
				}
				if((ALARM_FLAG_ANGLEX==1)&&(fabs(x_angle)<angle_limit))
				{
					ALARM_FLAG_ANGLEX=0;
					ALARM_FLAG=1;		//恢复发送标志
				}
				if((ALARM_FLAG_ANGLEY==0)&&(fabs(y_angle)>=angle_limit))		//超出阈值
				{
					angle_warning=1;
					ALARM_FLAG_ANGLEY=1;	//报警后赋为1 waring =0
				}
				if((ALARM_FLAG_ANGLEY==1)&&(fabs(y_angle)<angle_limit))
				{
					ALARM_FLAG_ANGLEY=0;
					ALARM_FLAG=1;		//恢复发送标志
				}
				
			}		
			//else 
				//printf("485通讯错误\r\n");
			/*判断是否报警与切断电源*/
			if((ALARM_FLAG_WEIGHT|ALARM_FLAG_HIGH|ALARM_FLAG_ANGLEX|ALARM_FLAG_ANGLEY)==1)
			{
				diwen_send_data16(0x1120,0x0001);	//禁止起升
				HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_RESET);		//关电
				HAL_GPIO_WritePin(OUTPUT3_GPIO, OUTPUT3_GPIO_PIN, GPIO_PIN_RESET);  //打开报警
			}
			else
			{
				if(((IO1||IO2)==0)&&(work_register_sta==1))		//如果前后门关闭，且人脸认证状态
				{
					diwen_send_data16(0x1120,0x0000);	//允许起升
					HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_SET);	//开电
				}
				HAL_GPIO_WritePin(OUTPUT3_GPIO, OUTPUT3_GPIO_PIN, GPIO_PIN_SET);  //关闭报警
			}
			
			//printf("USART1_RX_STA=%d\r\n",USART1_RX_STA);
			Work_Count=0;
			HAL_TIM_Base_Start_IT(&htim5);
		}
		/****************考勤机***************************/
		if(USART1_RX_flg==1)	//USART1接收到一次数据了
		{
			USART1_RX_flg=0;
			uint8_t i;
			printf("name=%s\r\n",usart1Rx_Buf);
			for(i=0;i<8;i++)
			{
				name[i]=usart1Rx_Buf[21+i];
			}
			printf("name=%s\r\n",name);
			diwen_send_numascii(0x1070,name,8);	
			for(i=0;i<18;i++)
			{
				NameID[i]=usart1Rx_Buf[1+i];
			}
			diwen_send_numascii(0x1080,NameID,18);	
			
			/*************人员信息录入成功，关闭考勤机*********************/
			work_register_sta=1;			//信息录入成功标志,开始倒计时
			HAL_GPIO_WritePin(OUTPUT1_GPIO, OUTPUT1_GPIO_PIN, GPIO_PIN_SET);  //输出高，光耦关闭无电压 关闭考勤机
			HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_SET);  //打开控制系统
			
			//打开电源
			diwen_send_data16(0x10f0,worktime_limit);
			diwen_send_data16(0x1150,alreadyworktime);
			//清除装载计数
			diwen_send_data16(0x1160,work_load_cnt);
			diwen_send_data16(0x1170,illegal_operation_cnt);
			io_check();
			
			memset(usart1Rx_Buf,0,256);
			
			register_time_flag=1;
			
		}
		
		
		/*******************************************/
		
		/*********************GSM数据通信*********************************/
		if((high_warning==1)||(weight_warning==1)||(angle_warning==1))			//标志位有报警变化 
		{
			if(high_warning==1)
			{
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\",\"info\":\"crash\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,name,NameID,alreadyworktime);
				printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
				HAL_Delay(100);
			}
			if(weight_warning==1)
			{
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\",\"info\":\"overweight\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,name,NameID,alreadyworktime);
				printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
				HAL_Delay(100);
			}
			if(angle_warning==1)
			{
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\",\"info\":\"overangle\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,name,NameID,alreadyworktime);
				printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
				HAL_Delay(100);
			}
			high_warning=0;
			weight_warning=0;
			angle_warning=0;
			
		}

		if(ALARM_FLAG==1)
		{
			ALARM_FLAG=0;
			sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightlimit\":\"%d\",\"highlimit\":\"%d\",\"anglelimit\":\"%.1f\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_limit,high_limit,angle_limit,name,NameID,alreadyworktime);
			printf("EndStr=%s\r\n",EndStr);
			GSM_USART_printf("%s",EndStr);
		}
		
		/******************串口屏接收数据处理************************/
		if(HMI_USARTx_RX_STA&0X8000)	//串口屏接收到一次数据了
		{
			HMI_USARTx_RX_STA=0;			//标志初始化	 
			uint16_t addr;
			
			if(HMI_USARTx_RX_BUF[3]==0x83)	//接收到变量修改了
			{
				addr=((uint16_t)HMI_USARTx_RX_BUF[4]<<8)|HMI_USARTx_RX_BUF[5];		//得到地址
				switch(addr)
				{
					case 0x12b0:				//空载重量设置
						weight_zero_value=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[0]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[1]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("空载重量=%d\r\n",weight_zero_value);
						break;
					case 0x12c0:				//空载采样值设置
						weight_zero=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[2]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[3]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("空载采样值=%d\r\n",weight_zero);
					break;
					case 0x12f0:				//装载重物重量设置
						weight_add_value=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[4]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[5]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("装载重物=%d\r\n",weight_add_value);
					break;
					case 0x1300:				//装载重物采样值
						weight_add=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[6]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[7]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("装载采样值=%d\r\n",weight_add);
					break;
					case 0x1340:				//重量阈值
						weight_limit=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[8]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[9]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("重量阈值=%d\r\n",weight_limit);
					break;
					
					case 0x1320:				//后门标定
						back_door=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[10]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[11]=HMI_USARTx_RX_BUF[8];
						if(back_door==1)
						{
							IO2=!IO2;
						}
						write_flag=1;
						io_check();
						printf("后门=%d\r\n",back_door);
					break;
					case 0x1310:				//前门标定
						front_door=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[12]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[13]=HMI_USARTx_RX_BUF[8];
						if(front_door==1)
						{
							IO1=!IO1;
						}
						write_flag=1;
						io_check();
						printf("前门=%d\r\n",front_door);
					break;
					
					case 0x1290:				//轿厢置底距地面高度
						high_zero_value=(HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8])/10;
						Tx_Buffer[14]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[15]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("轿厢置底距地面高度=%d\r\n",high_zero_value);
						break;
					case 0x12a0:				//轿厢置底采样值
						high_zero=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[16]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[17]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("轿厢置底采样值=%d\r\n",high_zero);
					break;
					case 0x12d0:				//轿厢置顶距地面高度
						high_top_value=(HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8])/10;
						Tx_Buffer[18]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[19]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("轿厢置顶距地面高度=%d\r\n",high_top_value);
					break;
					case 0x12e0:				//轿厢置顶采样值
						high_top=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[20]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[21]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("轿厢置顶采样值=%d\r\n",high_top);
					break;
					case 0x1330:				//高度阈值
						high_limit=(HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8])/10;
						Tx_Buffer[22]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[23]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("高度阈值=%d\r\n",high_limit);
					break;
					case 0x1350:				//倾角阈值
						angle_limit=(float)(HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8])/100;
						Tx_Buffer[24]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[25]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("倾角阈值=%0.2f\r\n",angle_limit);
					break;			
					case 0x1360:				//有效工作时间设置
						worktime_limit=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[26]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[27]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("有效工作时间=%d\r\n",worktime_limit);
					break;
					
					default: break;			

					
				}
				if(write_flag==1)
				{
					Tx_Buffer[28]=0x5a;
					/* 擦除SPI的扇区以写入 */
					SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 				
					/* 将发送缓冲区的数据写到flash中 */ 	
					SPI_FLASH_BufferWrite(Tx_Buffer,FLASH_WriteAddress, sizeof(Tx_Buffer));
					//calibrate_parameter();
					write_flag=0;
				}
			}	
		}		
		
	}
}

/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  __HAL_RCC_PWR_CLK_ENABLE();                                     //使能PWR时钟

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  //设置调压器输出电压级别1

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;      // 外部晶振，8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;                        //打开HSE 
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                    //打开PLL
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;            //PLL时钟源选择HSE
  RCC_OscInitStruct.PLL.PLLM = 8;                                 //8分频MHz
  RCC_OscInitStruct.PLL.PLLN = 336;                               //336倍频
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                     //2分频，得到168MHz主时钟
  RCC_OscInitStruct.PLL.PLLQ = 7;                                 //USB/SDIO/随机数产生器等的主PLL分频系数
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // 系统时钟：168MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHB时钟： 168MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1时钟：42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2时钟：84MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_RCC_EnableCSS();                                            // 使能CSS功能，优先使用外部晶振，内部时钟源为备用
  
 	// HAL_RCC_GetHCLKFreq()/1000    1ms中断一次
	// HAL_RCC_GetHCLKFreq()/100000	 10us中断一次
	// HAL_RCC_GetHCLKFreq()/1000000 1us中断一次
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);                // 配置并启动系统滴答定时器
  /* 系统滴答定时器时钟源 */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* 系统滴答定时器中断优先级配置 */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * 函数功能: 对设定值进行校准比例和初值函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void calibrate_parameter(void)
{
	/***********重量校准系数计算*************/
	weight_proportion=(float)(weight_add_value-weight_zero_value)/(weight_add-weight_zero);
	weight_initial_value=weight_add_value-weight_proportion*weight_add;
	printf("%0.1f->%d",weight_proportion,weight_initial_value);
	
	/*************高度校准系数计算*****************/
	high_proportion=(float)(high_top_value-high_zero_value)/(high_top-high_zero);
	high_initial_value=high_top_value-high_proportion*high_top;
	printf("%0.3f->%d",high_proportion,high_initial_value);
}

/**
  * 函数功能: 求差值绝对值函数
  * 输入参数: 无
  * 返 回 值: 差值绝对值
  * 说    明: 无
  */
int32_t int_abs(int32_t value1,int32_t value2)
{
  if(value1>value2)
    return (value1-value2);
  else
    return (value2-value1);
}

/**
  * 函数功能: 中值平均滤波
  * 输入参数: 无
  * 返 回 值: 滤波后的值
  * 说    明: 无
  */
uint16_t filter(void)
{
	uint16_t value_buf[N],temp=0;   
	uint16_t sum=0;  
	unsigned char count,i,j;
	for(count=0;count<N;count++)
  {
		value_buf[count] = ADC_ConvertedValue[count];//等待AD转换
  }
                        
   for(j=0;j<N-1;j++)  //排序
   {
     for(i=0;i<N-j-1;i++)        
     {
       if ( value_buf[i] > value_buf[i+1] )
        {
          temp = value_buf[i];
          value_buf[i] = value_buf[i+1];
          value_buf[i+1] = temp;
        }
     }
    }
	 for(count=15;count<N-15;count++)                                //中位求平均 由Flag标志控制 
		{
     sum+=value_buf[count];
		}
		printf("sum=%f\r\n",sum*3.3/40960);
   return (uint16_t)(sum/(N-30));
}

/**
  * 函数功能: IO口检测功能
  * 输入参数: 无
  * 返 回 值: 
  * 说    明: 无
  */
void io_check(void)
{
	//printf("io1%d,io2%d,front_door%d,back_door%d\r\n",IO1,IO2,front_door,back_door);
	/**************前门********************/
	
	if(IO1==0)
	{
		diwen_send_data16(0x1100,0x0000);
		//diwen_send_data16(0x1310,0x0000);
	}
	else if(IO1==1)
	{
		diwen_send_data16(0x1100,0x0001);
		//diwen_send_data16(0x1310,0x0001);
	} 
	/****************后门***************************/
	if(IO2==0)
	{
		diwen_send_data16(0x1110,0x0000);
		//diwen_send_data16(0x1320,0x0000);
	}
	else if(IO2==1)
	{
		diwen_send_data16(0x1110,0x0001);
		//diwen_send_data16(0x1320,0x0001);
	} 
	/***********起升状态*************/
	if(((IO1||IO2)==0)&&(work_register_sta==1)&&((ALARM_FLAG_WEIGHT|ALARM_FLAG_HIGH|ALARM_FLAG_ANGLEX|ALARM_FLAG_ANGLEY)==0))	//前后门关闭且被识别状态,并且无报警下才算一次正常装载
	{
		HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_SET);	//开电
		diwen_send_data16(0x1120,0x0000);
		work_load_cnt++;		//关门装载计数一次
		diwen_send_data16(0x1160,work_load_cnt);
		if(weight_count>=weight_limit)		//超重 计入违章
		{
			illegal_operation_cnt++;
			diwen_send_data16(0x1170,illegal_operation_cnt);
		}
		weight_change=weight_count-weight_close;		//重量差=当前重量-上次关门重量
		weight_close=weight_count;								//将本次重量记录为上次关门重量
		sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"load\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
		"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightchange\":\"%d\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_change,name,NameID,alreadyworktime);
		//printf("EndStr=%s\r\n",EndStr);
		GSM_USART_printf("%s",EndStr);
	}
	else
	{
		diwen_send_data16(0x1120,0x0001);
		HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_RESET);	//关电
	}
}

/**
  * 函数功能: 非阻塞模式下定时器的回调函数
  * 输入参数: htim：定时器句柄
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim6)
	{
		//printf("接收数据长度:%d\r\n",RS485_USARTx_RX_STA);
		HAL_UART_Transmit(&husartx,rs485Rx_Buf,RS485_USARTx_RX_STA,0xffff);    //发送接收到的数据
		RS485_USARTx_RX_STA|=1<<15;	//标记接收完成
		HAL_TIM_Base_Stop_IT(&htim6);				//关闭定时器
	}
	if(htim == &htim5)
	{
		Count++;
		timer_count++;
		Work_Count++;
	}
	if(htim == &htim7)
	{
//		printf("接收数据长度:%d\r\n",HMI_USARTx_RX_STA);
//		HAL_UART_Transmit(&husartx,HMI_USARTx_RX_BUF,HMI_USARTx_RX_STA,0xffff);    //发送接收到的数据
		HMI_USARTx_RX_STA|=1<<15;	//标记接收完成
		
		HAL_TIM_Base_Stop_IT(&htim7);				//关闭定时器
	}
}
/** 
  * 函数功能: 串口接收中断回调函数
  * 输入参数: 串口句柄
  * 返 回 值: 无
  * 说    明: 使用一个定时器的比较中断和更新中断作为接收超时判断
  *           只要接收到数据就将定时器计数器清0,当发生比较中断的时候
  *           说明已经超时1.5个字符的时间,认定为帧错误,如果是更新中断
  *           则认为是接受完成
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//485串口接收回调
  if(huart == &husart_485)
  {
		__HAL_TIM_SET_COUNTER(&htim6,0);	//清零计数器
		if(RS485_USARTx_RX_STA==0)	//还能接收数据，开启定时器
		{
			__HAL_TIM_CLEAR_FLAG(&htim6,TIM_FLAG_UPDATE);
      HAL_TIM_Base_Start_IT(&htim6);
		}
		rs485Rx_Buf[RS485_USARTx_RX_STA] = rs485RxBuffer;
    RS485_USARTx_RX_STA++;
		HAL_UART_Receive_IT(&husart_485,&rs485RxBuffer,1);	//再次开启接收
  }
	
	//gsm串口接收回调
	if(huart == &husartx_GSM)
	{
		if(gsmuart_p<GSM_UART_BUFF_SIZE)
		{
			gsm_uart_buff[gsmuart_p] =gsmRxBuffer; 
			gsmuart_p++; 
			HAL_UART_Receive_IT(&husartx_GSM,&gsmRxBuffer,1);
		}
		else
		{
			clean_rebuff();       
		}
	}
	//串口屏串口接收回调
	if(huart == &husartx_HMI)
	{
		__HAL_TIM_SET_COUNTER(&htim7,0);	//清零计数器
		if(HMI_USARTx_RX_STA==0)	//还能接收数据，开启定时器
		{
			__HAL_TIM_CLEAR_FLAG(&htim7,TIM_FLAG_UPDATE);
      HAL_TIM_Base_Start_IT(&htim7);
		}
		HMI_USARTx_RX_BUF[HMI_USARTx_RX_STA] = hmiRxBuffer;
    HMI_USARTx_RX_STA++;
		HAL_UART_Receive_IT(&husartx_HMI,&hmiRxBuffer,1);	//再次开启接收
	}
	
	if(huart == &usartx_1)	//考勤机串口
	{
		if((USART1_RX_STA==0)&&(aRxBuffer!=0x26))
		{
			USART1_RX_STA=0;
			USART1_RX_flg=0;
		}
		else
		{
			usart1Rx_Buf[USART1_RX_STA] = aRxBuffer;
			USART1_RX_STA++;
			
			if(aRxBuffer==0x26)
			{
				if(USART1_RX_STA!=1)
				{
					//USART1_RX_STA|=1<<15;	//标记接收完成
					USART1_RX_STA=0;
					USART1_RX_flg=1;
				}
			}
		}
		HAL_UART_Receive_IT(&usartx_1,&aRxBuffer,1);
	}
}

