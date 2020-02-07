#include "stdio.h"
#include "math.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"
#include "rs485.h"
#include "diwen.h"
#include "timer.h"
#include "4G_usart.h"
#include "spi.h"
#include "flash.h"
#include "hx711.h"

/*****函数声明***********/
void fitter(void);		//软件滤波
void qingjiao_getDate(void);	//倾角
void fengsu_getDate(void); 	//风速
void calibrate_parameter(void);	//校准值参数计算
void config_copy(void);	//初始化配置参数
void Data_scan(void);		//触摸修改变量处理
/* 私有变量 */
u8 qingjiao[7]={0x5A,0xA5,0x20,0xf0,0x01,0x03,0x00};	//读取倾角传感器指令
u8 fengsu[8]={0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0a};	//读取倾角传感器指令
u16 addr;
/* 公共变量 */
char EndStr[500];
u8 ID_buff[11];
u32 FLASH_SIZE;
u8 Tx_Buffer[65];		//spi通信内存，按字节读写
u8 Rx_Buffer[65];
#define SIZE sizeof(Tx_Buffer)	 
u16 fengsuData;
u8 write_flag=0;		//写入标志，有一个变化就要重新写入flash
u8 check_id_flag=0;
u8 item=0;
//报警显示标志位
u8 waringview_gaodu=0;
u8 waringview_fududu=0;
u8 waringview_huizhuan=0;
u8 waringview_zhongliang=0;
u8 waringview_qingjiao=0;
u8 waringview_fengsu=0;
/***********角度变量***************/
float x_angle,y_angle;
/***********迪文屏触摸修改参数变量***************/
float top_position;
u16 top_sampl;
float bottom_position;
u16 bottom_smapl;
float near_range;
u16 near_smapl;
float far_range;
u16 far_smapl;
float current_rotation;
u16 current_rotationsmapl;
float right_rotation;
u16 right_rotationsmapl;
float empty_weight;
u16 empty_weightsmapl;
float current_weight;
u16 current_weightsmapl;
float upper_limit;
float near_limit;
float remote_limit;
int16_t left_limit;
int16_t right_limit;
float level_limit0,level_limit1,vertical_limit0,vertical_limit1,rate_limit0,rate_limit1,windspeed_limit0,windspeed_limit1,angle_limit0,angle_limit1;	//height塔机高度
float height=50;
/***********迪文屏显示参数变量***************/
float xiaobi_data,dabi_data,tagao,zuobiao_x,zuobiao_y,beilv;
float current_high;		//高度
float temp_current_high;	//临时值
float current_range;	//幅度
float temp_current_range;	//临时值
float current_rotation_data;	//回转
float temp_current_rotation_data;	//临时值
float current_weight;		//重量
float current_torque;		//力矩，百分比
float current_windspeed;		//风速
float current_angle;	//角度

char Dev_ID[10];	//设备ID

float weight_proportion=0; //重量比例系数
float high_proportion=0; //高度比例系数
float Range_proportion=0;	//幅度比例系数
float rotation_proportion=0;	//回转比例系数
int32_t weight_initial_value=0;	//初始值,相当于公式中的b
int32_t high_initial_value=0;	//初始值,相当于公式中的b
int32_t Range_initial_value=0;	//初始值,相当于公式中的b
int32_t rotation_initial_value=0;	//初始值,相当于公式中的b

/***报警标志位***/
u8 fengsubaojing_flag=0;
u8 fengsuyujing_flag=0;
u8 qingjiaobaojing_flag=0;			
u8 qingjiaoyujing_flag=0;			
u8 gaodubaojing_flag=0;
u8 fudubaojing_flag=0;
u8 huizhuanbaojing_flag=0;
u8 chaozhongbaojing_flag=0;	

u8 ALARM_FLAG_gaodu=0;	//报警触发标志，防止多次报警触发，0:遇到报警条件可触发报警 1：已触发报警需等待警戒状态解除后可触发报警
u8 ALARM_FLAG_fudu=0;
u8 ALARM_FLAG_huizhuan=0;
u8 ALARM_FLAG_zhongliang=0;
u8 ALARM_FLAG_fengsu=0;
u8 ALARM_FLAG_qingjiao=0;
/* 用于保存转换计算后的电压值 */	 
 __IO uint32_t ADC1ConvertedVoltage[3]; 
 __IO uint32_t ADC1ConvertedVoltage_temp[3]={6,6,6};
u32 ad=0;
/* 扩展变量 ------------------------------------------------------------------*/
extern __IO u16 ADC1ConvertedValue[64][ADC_NUMOFCHANNEL];


 int main(void)
 {	
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	control_init();
	LED_Init();
	uart_init(115200);	 //串口初始化为9600
	RS485_Init(36,9600);//485初始化
	UART4_Init(115200);	//迪文屏串口初始化	
	GSM_Init();			//4G模块初始化
	SPI_Flash_Init();  		//SPI FLASH 初始化 
	while(SPI_Flash_ReadID()!=W25Q64)							//检测不到W25Q64
	{
		printf("flash error\r\n");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
	FLASH_SIZE=8*1024*1024;	//FLASH 大小为8M字节
  /* ADC 初始化 */
	ADCx_Init();
	ADInit(); 
  TIM3_Init(999,7199);		//100ms中断
	delay_ms(2000);
	liebiao_view();
	time3_start();		//定时开启
  /* 本地参数初始化 */
	config_copy();
	calibrate_parameter();
	while(1)
	{
		if(net_flag==1)
		{
			net_flag=0;
			if(check_id_flag==0)		//设备ID获取 连续直至成功
			{
				/* 网络状态检查与设备ID获取 */
				if(gprs_send_cmd((u8*)"usr.cnAT+IMEI\r\n",(u8*)"+IMEI:",100)==0)
				{
					u8 i;
					u8 tmpbuff[7];
					for(i=0;i<6;i++)
					{
						tmpbuff[i]=UART5_RX_BUF[18+i];//无回显，有：24
					}
					tmpbuff[6]='\0';
					//printf("%s\r\n",tmpbuff);
					sprintf((char *)ID_buff,"THHY%s",tmpbuff);
					printf("%s",ID_buff);
					diwen_send_numascii(0x1510,ID_buff,11);
					SPI_Flash_Write((u8*)ID_buff,FLASH_SIZE-200,sizeof(ID_buff));		//从倒数第100个地址处开始,写入SIZE长度的数据
					check_id_flag=1;
				}
			}
			if(gprs_send_cmd((u8*)"usr.cnAT+SOCKALK\r\n",(u8*)"+SOCKALK:ON",100)==0)
			{
				printf("联网成功\r\n");
				diwen_send_numascii(0x1520,"OK",2);
			} 
			else
			{
				printf("联网失败\r\n");
				diwen_send_numascii(0x1520,"NO",2);
			}
			sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"jump\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
			"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"upper_limit\":\"%.1f\",\"near_limit\":\"%.1f\",\"remote_limit\":\"%.1f\",\"left_limit\":\"%d\",\"right_limit\":\"%d\",\"windspeed_limit\":\"%.1f\",\"angle_limit\":\"%.1f\",\"weight_limit\":\"%.1f\",\"height\":\"%.1f\"}",ID_buff,current_weight,\
			current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle,upper_limit,near_limit,remote_limit,left_limit,right_limit,windspeed_limit0,angle_limit0,my5010[1][1],height);
			//printf("%s",EndStr);
			//printf("111=%d\r\n",strlen(EndStr));
			gsm_printf("%s",EndStr);
		}
		/*************迪文触摸屏接收参数获取***************/
		if(USART4_RX_STA&0X8000)	//串口屏接收到一次数据了
		{
			USART4_RX_STA=0;			//标志初始化	 
			Data_scan();
		}
		
		/*100msAD数据采集*/
		if(ad_flag==1)
		{
			fitter();
			if(abs(ADC1ConvertedVoltage[0]-ADC1ConvertedVoltage_temp[0])>4)		//计算AD采样值大于1的变化才刷屏
			{
//				printf("ad=%d\r\n",ADC1ConvertedVoltage[0]);
//				printf("adtemp=%d\r\n",ADC1ConvertedVoltage_temp[0]);
				diwen_send_data16(0x1720,ADC1ConvertedVoltage[0]);
				ADC1ConvertedVoltage_temp[0]=ADC1ConvertedVoltage[0];
				//高度值
				current_high=((float)ADC1ConvertedVoltage[0]-top_sampl)/high_proportion+top_position;		//高度值
				if(current_high<0)		//如果采集到数据超出上限，强制为0
				{
					current_high=0;
				}
				if(current_high>500)		//如果采集到数据超出上限，强制为0
				{
					current_high=bottom_position+1;
				}
			//printf("current_high=%.1f\r\n",current_high);
//			if(fabs(current_high-temp_current_high)>=0.5f)		//实际跳动过大
//			{
					if(current_high>=0&&current_high<500)			//防止数据溢出
					{
						diwen_send_data16(0x1530,current_high*10);
						//data数据上报
						if(fabs(current_high-temp_current_high)>=1.5f)
						{
							sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
							"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\"}",ID_buff,current_weight,\
							current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
							//printf("%s",EndStr);
							gsm_printf("%s",EndStr);
						}
						temp_current_high=current_high;
					}
//			}
				if((current_high-upper_limit)<=0)		//上行报警
				{
					if(ALARM_FLAG_gaodu==0)
					{
						diwen_send_data16(0x1590,0);	//上行状态图标
						diwen_send_numascii(0x1650,"高度状态报警",12);
						gaodubaojing_flag=1;
						ALARM_FLAG_gaodu=1;
					}
				}
				if((current_high-bottom_position)>=0)		//下行报警
				{
					if(ALARM_FLAG_gaodu==0)
					{
						diwen_send_data16(0x1600,0);	//下行状态报警图标
						diwen_send_numascii(0x1650,"高度状态报警",12);
						gaodubaojing_flag=1;
						ALARM_FLAG_gaodu=1;
					}
				}
				if(((current_high-upper_limit)>0)&&((current_high-bottom_position)<0))
				{
					if(ALARM_FLAG_gaodu==1)
					{
						diwen_send_data16(0x1590,1);
						diwen_send_data16(0x1600,1);
					}
					ALARM_FLAG_gaodu=0;
					gaodubaojing_flag=0;
				}
			}
			if(abs(ADC1ConvertedVoltage[1]-ADC1ConvertedVoltage_temp[1])>4)
			{
				diwen_send_data16(0x1722,ADC1ConvertedVoltage[1]);
				ADC1ConvertedVoltage_temp[1]=ADC1ConvertedVoltage[1];
					//幅度值
				current_range=((float)ADC1ConvertedVoltage[1]-near_smapl)/Range_proportion+near_range;	//幅度值

				if(current_range<0)		//如果采集到数据超出上限，强制为0
				{
					current_range=0;
				}
				if(current_range>500)	
				{
					current_range=remote_limit+1;
				}
			
			
//			if(fabs(current_range-temp_current_range)>=0.5f)
//			{
				if(current_range>=0&&current_range<500)			//防止数据溢出
				{
					diwen_send_data16(0x1540,current_range*10);
					if(fabs(current_range-temp_current_range)>=1.5f)
					{
						//data数据上报
						sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
						"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\"}",ID_buff,current_weight,\
						current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
						//printf("%s",EndStr);
						gsm_printf("%s",EndStr);
					}
					temp_current_range=current_range;
				}
//			}
			
				if((current_range-near_limit)<0)		//近端报警
				{
					if(ALARM_FLAG_fudu==0)
					{
						diwen_send_data16(0x1620,0);
						diwen_send_numascii(0x1650,"小车行进报警",12);
						fudubaojing_flag=1;
						ALARM_FLAG_fudu=1;
					}
				}
				else if((current_range-remote_limit)>0)	//远端报警
				{
					if(ALARM_FLAG_fudu==0)
					{
						diwen_send_data16(0x1610,0);
						diwen_send_numascii(0x1650,"小车行进报警",12);
						fudubaojing_flag=1;
						ALARM_FLAG_fudu=1;
					}
				}
				else
				{
					if(ALARM_FLAG_fudu==1)
					{
						diwen_send_data16(0x1610,1);
						diwen_send_data16(0x1620,1);
					}
					fudubaojing_flag=0;
					ALARM_FLAG_fudu=0;
				//diwen_send_numascii(0x1650,"            ",12);
				}
			
			}
			if(abs(ADC1ConvertedVoltage[2]-ADC1ConvertedVoltage_temp[2])>4)
			{
				diwen_send_data16(0x1724,ADC1ConvertedVoltage[2]);
				ADC1ConvertedVoltage_temp[2]=ADC1ConvertedVoltage[2];
				//回转值
				current_rotation_data=((int32_t)ADC1ConvertedVoltage[2]-current_rotationsmapl)/rotation_proportion+current_rotation;		//回转值
			
//				if(fabs(current_rotation_data-temp_current_rotation_data)>=5)
//				{
					diwen_send_data16(0x1545,(int16_t)current_rotation_data*10);
					//printf("current_rotation_data=%f\r\n",current_rotation_data);
					if(current_rotation_data>=0)		
					{
						diwen_send_data16(0x0120,(int)current_rotation_data%360);
					}
					else
					{
						diwen_send_data16(0x0120,((int)current_rotation_data+360*4)%360);
					}
				//data数据上报
					if(fabs(current_rotation_data-temp_current_rotation_data)>=15)
					{
						sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
						"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\"}",ID_buff,current_weight,\
						current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
						//printf("%s",EndStr);
						gsm_printf("%s",EndStr);
					}
					temp_current_rotation_data=current_rotation_data;
//				}
			
			
				if(current_rotation_data<left_limit)	//不允许左转
				{
					if(ALARM_FLAG_huizhuan==0)
					{
						FIFO3=1;	//停止左转
						diwen_send_data16(0x1630,0);
						diwen_send_numascii(0x1650,"大臂回转报警",12);
						huizhuanbaojing_flag=1;
						ALARM_FLAG_huizhuan=1;
					}
				}
				else if(current_rotation_data>right_limit)	//不允许右转
				{
					if(ALARM_FLAG_huizhuan==0)
					{
						FIFO4=1;	//停止右转
						diwen_send_data16(0x1640,0);
						diwen_send_numascii(0x1650,"大臂回转报警",12);
						huizhuanbaojing_flag=1;
						ALARM_FLAG_huizhuan=1;
					}
				}
				else
				{
					if(ALARM_FLAG_huizhuan==1)
					{
						FIFO3=0;
						FIFO4=0;
						diwen_send_data16(0x1630,1);
						diwen_send_data16(0x1640,1);
					}
					huizhuanbaojing_flag=0;
					ALARM_FLAG_huizhuan=0;
				}
			}	
			
		}
				
		
		/************ 传感器数据采集 ***********************/
		if(worktimes_flag==1)
		{
			time3_stop();
			worktimes_flag=0;
			/*******485类传感器数据采集***************
			**************************************/
			memset(RS485_RX_BUF,0,64);
			RS485_Send_Data(fengsu,8);
			delay_ms(100);
			fengsu_getDate();
			/*************倾角传感器采集***************/
			memset(RS485_RX_BUF,0,64);
			RS485_Send_Data(qingjiao,7);
			delay_ms(100);
			qingjiao_getDate();
			
			
			if((ALARM_FLAG_gaodu||ALARM_FLAG_fudu)==0)	//画吊钩
			{
				diwen_send_rectangle(0x2000,134+(temp_current_range)*260/50,90,136+(temp_current_range)*260/50,(temp_current_high)*300/bottom_position+90);
				diwen_send_rectangle(0x2100,125+(temp_current_range)*260/50,(temp_current_high)*300/bottom_position+90,145+(temp_current_range)*260/50,(temp_current_high)*300/bottom_position+110);
			}
			

			ad=ReadAD()/838.8608;
			//printf("ad=%d\r\n",ad);
			diwen_send_data16(0x1726,ad);
			if(ad<empty_weightsmapl)		//如果采样值小，强制为0
			{
				ad=empty_weightsmapl;
			}
			current_weight=(ad-empty_weightsmapl)/weight_proportion;	//重量值
			diwen_send_data16(0x1550,current_weight*1000);
			
			//printf("current_range=%f,current_weight=%.4f\r\n",current_range,current_weight);
			if(current_range>0&&current_range<my5010[0][1])
			{
				current_torque=	current_weight*current_range/my5010[0][1]/my5010[1][1];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][1])
				{
					//printf("不超重");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("超重");
					diwen_send_numascii(0x1650,"吊钩超重报警",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][1]&&current_range<my5010[0][2])
			{
				current_torque=	current_weight*current_range/my5010[0][2]/my5010[1][2];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][2])
				{
					//printf("不超重");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("超重");
					diwen_send_numascii(0x1650,"吊钩超重报警",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][2]&&current_range<my5010[0][3])
			{
				current_torque=	current_weight*current_range/my5010[0][3]/my5010[1][3];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][3])
				{
					//printf("不超重");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("超重");
					diwen_send_numascii(0x1650,"吊钩超重报警",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][3]&&current_range<my5010[0][4])
			{
				current_torque=	current_weight*current_range/my5010[0][4]/my5010[1][4];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][4])
				{
					//printf("不超重");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("超重");
					diwen_send_numascii(0x1650,"吊钩超重报警",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][4]&&current_range<my5010[0][5])
			{
				current_torque=	current_weight*current_range/my5010[0][5]/my5010[1][5];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][5])
				{
					//printf("不超重");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("超重");
					diwen_send_numascii(0x1650,"吊钩超重报警",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][5]&&current_range<my5010[0][6])
			{
				current_torque=	current_weight*current_range/my5010[0][6]/my5010[1][6];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][6])
				{
					//printf("不超重");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("超重");
					diwen_send_numascii(0x1650,"吊钩超重报警",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][6]&&current_range<my5010[0][7])
			{
				current_torque=	current_weight*current_range/my5010[0][7]/my5010[1][7];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][7])
				{
					//printf("不超重");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("超重");
					diwen_send_numascii(0x1650,"吊钩超重报警",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][7]&&current_range<my5010[0][8])
			{
				current_torque=	current_weight*current_range/my5010[0][8]/my5010[1][8];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][8])
				{
					//printf("不超重");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else 
				{
					//printf("超重");
					diwen_send_numascii(0x1650,"吊钩超重报警",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][8])
			{
				current_torque=	current_weight*remote_limit/my5010[0][8]/my5010[1][8];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][8])
				{
					//printf("不超重");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else 
				{
					//printf("超重");
					diwen_send_numascii(0x1650,"吊钩超重报警",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
				/*******阈值判断**********/		
			if((ALARM_FLAG_zhongliang||ALARM_FLAG_gaodu||ALARM_FLAG_fudu||ALARM_FLAG_huizhuan||ALARM_FLAG_fengsu||ALARM_FLAG_qingjiao)==0)
			{
				diwen_send_numascii(0x1650,"            ",12);
			}	
			if((ALARM_FLAG_zhongliang||ALARM_FLAG_fengsu||ALARM_FLAG_qingjiao)==0)
			{
				if((ALARM_FLAG_gaodu==1)&&item==2)
				{
					diwen_send_numascii(0x1650,"高度状态报警",12);
				}
				if((ALARM_FLAG_fudu==1)&&item==4)
				{
					diwen_send_numascii(0x1650,"小车行进报警",12);
				}
				if((ALARM_FLAG_huizhuan==1)&&item==6)
				{
					diwen_send_numascii(0x1650,"大臂回转报警",12);
				}
			}
			
			if(item==6)
			{
				item=0;
			}
			item++;
			time3_start();
			/********************************************************************/
		} 
		
		
		/*****报警标志位判断*******/
		if((fengsubaojing_flag||qingjiaobaojing_flag||gaodubaojing_flag||fudubaojing_flag||huizhuanbaojing_flag||chaozhongbaojing_flag)==1)
		{
			if(fengsubaojing_flag==1)
			{
				//data数据上报
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"windspeed\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(qingjiaobaojing_flag==1)
			{
				//data数据上报
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overangle\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(gaodubaojing_flag==1)
			{
				//data数据上报
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overhigh\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(fudubaojing_flag==1)
			{
				//data数据上报
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overfudu\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(huizhuanbaojing_flag==1)
			{
				//data数据上报
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overhuizhuan\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(chaozhongbaojing_flag==1)
			{
				//data数据上报
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overweight\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			fengsubaojing_flag=0;
			qingjiaobaojing_flag=0;
			gaodubaojing_flag=0;
			fudubaojing_flag=0;
			huizhuanbaojing_flag=0;
			chaozhongbaojing_flag=0;
		}
		

		//printf("***************%d.%d.%d.%d.%d.%d\r\n",ALARM_FLAG_zhongliang,ALARM_FLAG_gaodu,ALARM_FLAG_fudu,ALARM_FLAG_huizhuan,ALARM_FLAG_fengsu,ALARM_FLAG_qingjiao);
		if(ALARM_FLAG_zhongliang==1)		//超重塔钩不能上升只能下降
		{
			FIFO1=1;	//控制上升状态关
			FIFO2=1;	//控制前行
		}
		else
		{
			FIFO1=0;
			FIFO2=0;
		}
	}	 
}

///***均值滤波程序****/
//void fitter(void)
//{
//	u8 i,j;
//	ADC1ConvertedVoltage[0] = 0;
//  ADC1ConvertedVoltage[1] = 0;
//	ADC1ConvertedVoltage[2] = 0;
//  //ADC1ConvertedVoltage[3] = 0;
//	for(i=0;i<3;i++)
//  { 
//		for(j=0;j<64;j++)
//		{
//     ADC1ConvertedVoltage[i]+= ADC1ConvertedValue[j][i];
//    }
//		 ADC1ConvertedVoltage[i] = ADC1ConvertedVoltage[i]/64;
//	}
//}

/***均值滤波程序****/
void fitter(void)
{
	u8 i,j,count;
	u16 temp=0;
	u16 value_buf1[64];
	u16 value_buf2[64];
	u16 value_buf3[64];
	ADC1ConvertedVoltage[0] = 0;
  ADC1ConvertedVoltage[1] = 0;
	ADC1ConvertedVoltage[2] = 0;
	for(count=0;count<64;count++)
  {
		value_buf1[count] = ADC1ConvertedValue[count][0];//等待AD转换
		value_buf2[count] = ADC1ConvertedValue[count][1];//等待AD转换
		value_buf3[count] = ADC1ConvertedValue[count][2];//等待AD转换
  }
	
	for(j=0;j<64-1;j++)  //排序
   {
     for(i=0;i<64-j-1;i++)        
     {
       if ( value_buf1[i] > value_buf1[i+1] )
        {
          temp = value_buf1[i];
          value_buf1[i] = value_buf1[i+1];
          value_buf1[i+1] = temp;
        }
     }
    }
	 for(j=0;j<64-1;j++)  //排序
   {
     for(i=0;i<64-j-1;i++)        
     {
       if ( value_buf2[i] > value_buf2[i+1] )
        {
          temp = value_buf2[i];
          value_buf2[i] = value_buf2[i+1];
          value_buf2[i+1] = temp;
        }
     }
    }
	 for(j=0;j<64-1;j++)  //排序
   {
     for(i=0;i<64-j-1;i++)        
     {
       if ( value_buf3[i] > value_buf3[i+1] )
        {
          temp = value_buf3[i];
          value_buf3[i] = value_buf3[i+1];
          value_buf3[i+1] = temp;
        }
     }
    }
	for(count=10;count<64-10;count++)                                //中位求平均 由Flag标志控制 
		{
			ADC1ConvertedVoltage[0]+=value_buf1[count];
			ADC1ConvertedVoltage[1]+=value_buf2[count];
			ADC1ConvertedVoltage[2]+=value_buf3[count];
		}
		ADC1ConvertedVoltage[0]=ADC1ConvertedVoltage[0]/44;
		ADC1ConvertedVoltage[1]=ADC1ConvertedVoltage[1]/44;
		ADC1ConvertedVoltage[2]=ADC1ConvertedVoltage[2]/44;
		
}

/**
  * 函数功能: 风速读取函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void fengsu_getDate(void)
{
	if(rs485_recv_flag==1)	//485接收到一次数据了
		{
			rs485_recv_flag=0;			//标志初始化		
			RS485_RX_CNT=0;			
			//数据处理
			if(RS485_RX_BUF[2]==0x02)	//接收到数据了
			{
				fengsuData=RS485_RX_BUF[3]*256+RS485_RX_BUF[4];
				diwen_send_data16(0x1570,fengsuData);
				current_windspeed=fengsuData/10;
				//printf("fengsu=%.1fm/s\r\n",current_windspeed);
				if(current_windspeed>windspeed_limit0)
				{
					if(ALARM_FLAG_fengsu==0)
					{
						diwen_send_numascii(0x1650,"环境风速报警",12);
						ALARM_FLAG_fengsu=1;
						fengsubaojing_flag=1;
					}
					
				}
				else if(current_windspeed>windspeed_limit1&&current_windspeed<windspeed_limit0)
				{
					fengsuyujing_flag=1;
					fengsubaojing_flag=0;
					ALARM_FLAG_fengsu=0;
				}
				else
				{
					fengsubaojing_flag=0;
					fengsuyujing_flag=0;
					ALARM_FLAG_fengsu=0;
				}
				
			}
		}	
		else
				printf("error\r\n");
}

/**
  * 函数功能: 倾角传感器读取函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void qingjiao_getDate(void)
{
	if(rs485_recv_flag==1)	//485接收到一次数据了
		{
			rs485_recv_flag=0;			//标志初始化		
			RS485_RX_CNT=0;			
				//数据比对
			if(RS485_RX_BUF[7]==0x01)	//正数
			{
				x_angle=RS485_RX_BUF[8]+(float)(RS485_RX_BUF[9]+RS485_RX_BUF[10]*256)/1000;
				diwen_send_data16(0x1580,x_angle*10);
				//printf("x_angle=%0.1f\r\n",x_angle);
			}
			if(RS485_RX_BUF[7]==0x00)	//负数
			{
				x_angle=0-(RS485_RX_BUF[8]+(float)(RS485_RX_BUF[9]+RS485_RX_BUF[10]*256)/1000);
				diwen_send_data16(0x1580,x_angle*10);
				//printf("x_angle=%0.1f\r\n",x_angle);
			}
			if(RS485_RX_BUF[13]==0x01)	//正数
			{
				y_angle=RS485_RX_BUF[14]+(float)(RS485_RX_BUF[15]+RS485_RX_BUF[16]*256)/1000;
				//diwen_send_data16(0x1060,y_angle*10);
				//printf("y_angle=%0.1f\r\n",y_angle);
			}
			if(RS485_RX_BUF[13]==0x00)	//负数
			{
				y_angle=0-(RS485_RX_BUF[14]+(float)(RS485_RX_BUF[15]+RS485_RX_BUF[16]*256)/1000);
				//diwen_send_data16(0x1060,y_angle*10);
				//printf("y_angle=%0.1f\r\n",y_angle);
			}		
		}
		current_angle=fabs(x_angle);
		if(current_angle>angle_limit0) 
		{
			if(ALARM_FLAG_qingjiao==0)
			{
				diwen_send_numascii(0x1650,"塔机倾角报警",12);
				ALARM_FLAG_qingjiao=1;
				qingjiaobaojing_flag=1;
			}
			
		}
		else if(current_angle>angle_limit1&&current_angle<angle_limit0)
		{
			qingjiaoyujing_flag=1;
			qingjiaobaojing_flag=0;
			ALARM_FLAG_qingjiao=0;
		}
		else
		{
			qingjiaobaojing_flag=0;
			qingjiaoyujing_flag=0;
			ALARM_FLAG_qingjiao=0;
		}
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
	weight_proportion=(float)(current_weightsmapl-empty_weightsmapl)/(current_weight-empty_weight);
	weight_initial_value=current_weightsmapl-weight_proportion*current_weight;
	printf("%0.1f->%d\r\n",weight_proportion,weight_initial_value);
	
	/*************高度校准系数计算*****************/
	high_proportion=(float)(bottom_smapl-top_sampl)/(bottom_position-top_position);
	high_initial_value=bottom_smapl-high_proportion*bottom_position;
	printf("%0.3f->%d\r\n",high_proportion,high_initial_value);
	
	/*************幅度校准系数计算*****************/
	Range_proportion=(float)(far_smapl-near_smapl)/(far_range-near_range);
	Range_initial_value=far_smapl-Range_proportion*far_range;
	printf("%0.3f->%d\r\n",Range_proportion,Range_initial_value);
	
	/*************回转校准系数计算*****************/
	rotation_proportion=(float)(right_rotationsmapl-current_rotationsmapl)/(right_rotation-current_rotation);
	rotation_initial_value=right_rotationsmapl-rotation_proportion*right_rotation;
	printf("%0.3f->%d\r\n",rotation_proportion,rotation_initial_value);
}

/**
  * 函数功能: 数据初始化函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void config_copy(void)
{
	 printf("----开始数据拷贝------\r\n"); 
	SPI_Flash_Read((u8*)Rx_Buffer,FLASH_SIZE-100,SIZE);				//从倒数第100个地址处开始,读70个字节
	if(Rx_Buffer[64]==0x5a)		//有数据改动了，直接读取显示
	{
		u8 i;
		/**********将rx内容更新到tx防止掉电后buf内容被清空写0到flash*******************/
		for(i=0;i<65;i++)
		{
			Tx_Buffer[i]=Rx_Buffer[i];
		}
		top_position=(float)(Rx_Buffer[0]*256+Rx_Buffer[1])/10;
		diwen_send_data16(0x1728,top_position*10);
		printf("top_position=%.1f\r\n",top_position);
		
		top_sampl=Rx_Buffer[2]*256+Rx_Buffer[3];
		diwen_send_data16(0x1736,top_sampl);
		printf("top_sampl=%d\r\n",top_sampl);
		
		bottom_position=(float)(Rx_Buffer[4]*256+Rx_Buffer[5])/10;
		diwen_send_data16(0x1744,bottom_position*10);
		printf("bottom_position=%.1f\r\n",bottom_position);
		
		bottom_smapl=Rx_Buffer[6]*256+Rx_Buffer[7];
		diwen_send_data16(0x1752,bottom_smapl);
		printf("bottom_smapl=%d\r\n",bottom_smapl);
		
		near_range=(float)(Rx_Buffer[8]*256+Rx_Buffer[9])/10;
		diwen_send_data16(0x1730,near_range*10);
		printf("near_range=%.1f\r\n",near_range);
		
		near_smapl=Rx_Buffer[10]*256+Rx_Buffer[11];
		diwen_send_data16(0x1738,near_smapl);
		printf("near_smapl=%d\r\n",near_smapl);
		
		far_range=(float)(Rx_Buffer[12]*256+Rx_Buffer[13])/10;
		diwen_send_data16(0x1746,far_range*10);
		printf("far_range=%.1f\r\n",far_range);
		
		far_smapl=Rx_Buffer[14]*256+Rx_Buffer[15];
		diwen_send_data16(0x1754,far_smapl);
		printf("far_smapl=%d\r\n",far_smapl);
		
		current_rotation=(float)(Rx_Buffer[16]*256+Rx_Buffer[17])/10;
		diwen_send_data16(0x1732,current_rotation*10);
		printf("current_rotation=%.1f\r\n",current_rotation);
		
		current_rotationsmapl=Rx_Buffer[18]*256+Rx_Buffer[19];
		diwen_send_data16(0x1740,current_rotationsmapl);
		printf("current_rotationsmapl=%d\r\n",current_rotationsmapl);
		
		right_rotation=(float)(Rx_Buffer[20]*256+Rx_Buffer[21])/10;
		diwen_send_data16(0x1748,right_rotation*10);
		printf("right_rotation=%.1f\r\n",right_rotation);
		
		right_rotationsmapl=Rx_Buffer[22]*256+Rx_Buffer[23];
		diwen_send_data16(0x1756,right_rotationsmapl);
		printf("right_rotationsmapl=%d\r\n",right_rotationsmapl);
		
		empty_weight=(float)(Rx_Buffer[24]*256+Rx_Buffer[25])/1000;
		diwen_send_data16(0x1734,empty_weight*1000);
		printf("empty_weight=%.3f\r\n",empty_weight);
		
		empty_weightsmapl=Rx_Buffer[26]*256+Rx_Buffer[27];
		diwen_send_data16(0x1742,empty_weightsmapl);
		printf("empty_weightsmapl=%d\r\n",empty_weightsmapl);
		
		current_weight=(float)(Rx_Buffer[28]*256+Rx_Buffer[29])/1000;
		diwen_send_data16(0x1750,current_weight*1000);
		printf("current_weight=%.1f\r\n",current_weight);
		
		current_weightsmapl=Rx_Buffer[30]*256+Rx_Buffer[31];
		diwen_send_data16(0x1758,current_weightsmapl);
		printf("current_weightsmapl=%d\r\n",current_weightsmapl);
		
		upper_limit=(float)(Rx_Buffer[32]*256+Rx_Buffer[33])/10;
		diwen_send_data16(0x1762,upper_limit*10);
		printf("upper_limit=%.1f\r\n",upper_limit);
		
		near_limit=(float)(Rx_Buffer[34]*256+Rx_Buffer[35])/10;
		diwen_send_data16(0x1766,near_limit*10);
		printf("near_limit=%.1f\r\n",near_limit);
		
		remote_limit=(float)(Rx_Buffer[36]*256+Rx_Buffer[37])/10;
		diwen_send_data16(0x1768,remote_limit*10);
		printf("remote_limit=%.1f\r\n",remote_limit);
		
		left_limit=(int16_t)(Rx_Buffer[38]*256+Rx_Buffer[39])/10;
		diwen_send_data16(0x1772,left_limit*10);
		printf("left_limit=%d\r\n",left_limit);
		
		right_limit=(float)(Rx_Buffer[40]*256+Rx_Buffer[41])/10;
		diwen_send_data16(0x1774,right_limit*10);
		printf("right_limit=%df\r\n",right_limit);
		
		level_limit0=(float)(Rx_Buffer[42]*256+Rx_Buffer[43])/10;
		diwen_send_data16(0x1776,level_limit0*10);
		printf("level_limit0=%.1f\r\n",level_limit0);
		
		level_limit1=(float)(Rx_Buffer[44]*256+Rx_Buffer[45])/10;
		diwen_send_data16(0x1790,level_limit1*10);
		printf("level_limit1=%.1f\r\n",level_limit1);
		
		vertical_limit0=(float)(Rx_Buffer[46]*256+Rx_Buffer[47])/10;
		diwen_send_data16(0x1778,vertical_limit0*10);
		printf("vertical_limit0=%.1f\r\n",vertical_limit0);
		
		vertical_limit1=(float)(Rx_Buffer[48]*256+Rx_Buffer[49])/10;
		diwen_send_data16(0x1792,vertical_limit1*10);
		printf("vertical_limit1=%.1f\r\n",vertical_limit1);
		
		rate_limit0=(float)(Rx_Buffer[50]*256+Rx_Buffer[51])/10;
		diwen_send_data16(0x1780,rate_limit0*10);
		printf("rate_limit0=%.1f\r\n",rate_limit0);
		
		rate_limit1=(float)(Rx_Buffer[52]*256+Rx_Buffer[53])/10;
		diwen_send_data16(0x1794,rate_limit1*10);
		printf("rate_limit1=%.1f\r\n",rate_limit1);
		
		windspeed_limit0=(float)(Rx_Buffer[54]*256+Rx_Buffer[55])/10;
		diwen_send_data16(0x1782,windspeed_limit0*10);
		printf("windspeed_limit0=%.1f\r\n",windspeed_limit0);
		
		windspeed_limit1=(float)(Rx_Buffer[56]*256+Rx_Buffer[57])/10;
		diwen_send_data16(0x1796,windspeed_limit1*10);
		printf("windspeed_limit1=%.1f\r\n",windspeed_limit1);
		
		angle_limit0=(float)(Rx_Buffer[58]*256+Rx_Buffer[59])/10;
		diwen_send_data16(0x1784,angle_limit0*10);
		printf("angle_limit0=%.1f\r\n",angle_limit0);
		
		angle_limit1=(float)(Rx_Buffer[60]*256+Rx_Buffer[61])/10;
		diwen_send_data16(0x1798,angle_limit1*10);
		printf("angle_limit1=%.1f\r\n",angle_limit1);
		
		height=(float)(Rx_Buffer[62]*256+Rx_Buffer[63])/10;
		diwen_send_data16(0x1680,height*10);
		printf("height=%.1f\r\n",height);
	}
	delay_ms(1000);
	printf("----开始读取ID号------\r\n");
	SPI_Flash_Read(ID_buff,FLASH_SIZE-200,sizeof(ID_buff));				//从倒数第100个地址处开始,读70个字节
	if(ID_buff[0]=='T')
	{
		printf("加载完成");
		diwen_send_numascii(0x1510,ID_buff,11);
		check_id_flag=1;
	}
}

/**
  * 函数功能: 触摸修改数据处理函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void Data_scan(void)
{
	u8 h,l;
	if(USART4_RX_BUF[3]==0x83)		//变量修改
			{
				addr=((u16)USART4_RX_BUF[4]<<8)|USART4_RX_BUF[5];		//得到地址
				
				switch(addr)
				{
					case 0x1728:		//吊钩置顶，吊钩距大臂距离
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[0]=h;
						Tx_Buffer[1]=l;
						top_position=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("top_position=%.1f\r\n",top_position);
					break;
					case 0x1736:		//吊钩置顶，对应采样值
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[2]=h;
						Tx_Buffer[3]=l;
						top_sampl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						write_flag=1;
						printf("top_sampl=%d\r\n",top_sampl);
					break;
					case 0x1744:		//吊钩置底，吊钩距大臂距离
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[4]=h;
						Tx_Buffer[5]=l;
						bottom_position=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("bottom_position=%.1f\r\n",bottom_position);
					break;
					case 0x1752:		//吊钩置底，对应采样值
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[6]=h;
						Tx_Buffer[7]=l;
						bottom_smapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						write_flag=1;
						printf("bottom_smapl=%d\r\n",bottom_smapl);
					break;
					case 0x1730:		//小车至近，距离中心距离
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[8]=h;
						Tx_Buffer[9]=l;
						near_range=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("near_range=%.1f\r\n",near_range);
					break;
					case 0x1738:		//小车至近，采样值
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[10]=h;
						Tx_Buffer[11]=l;
						near_smapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						write_flag=1;
						printf("near_smapl=%d\r\n",near_smapl);
					break;
					case 0x1746:		//小车至远，距离中心距离
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[12]=h;
						Tx_Buffer[13]=l;
						far_range=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("far_range=%.1f\r\n",far_range);
					break;
					case 0x1754:		//小车至远采样值
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[14]=h;
						Tx_Buffer[15]=l;
						far_smapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						write_flag=1;
						printf("far_smapl=%d\r\n",far_smapl);
					break;
					case 0x1732:		//此刻大臂转向 
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[16]=h;
						Tx_Buffer[17]=l;
						current_rotation=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("current_rotation=%.1f\r\n",current_rotation);
					break;
					case 0x1740:		//此刻大臂转向采样值
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[18]=h;
						Tx_Buffer[19]=l;
						current_rotationsmapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						
						write_flag=1;
						printf("current_rotationsmapl=%d\r\n",current_rotationsmapl);
					break;
					case 0x1748:	//大臂右转，此刻大臂转向角度
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[20]=h;
						Tx_Buffer[21]=l;
						right_rotation=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
					
						write_flag=1;
						printf("right_rotation=%.1f\r\n",right_rotation);
					break;
					case 0x1756:	//大臂右转，此刻采样值
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[22]=h;
						Tx_Buffer[23]=l;
						right_rotationsmapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						
						write_flag=1;
						printf("right_rotationsmapl=%d\r\n",right_rotationsmapl);
					break;
					case 0x1734:	//空钩实际重量
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[24]=h;
						Tx_Buffer[25]=l;
						empty_weight=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/1000;
						
						write_flag=1;
						printf("empty_weight=%.3f\r\n",empty_weight);
					break;
					case 0x1742:	//空钩采样值
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[26]=h;
						Tx_Buffer[27]=l;
						empty_weightsmapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						
						write_flag=1;
						printf("empty_weightsmapl=%d\r\n",empty_weightsmapl);
					break;
					case 0x1750:	//吊起物体时重量
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[28]=h;
						Tx_Buffer[29]=l;
						current_weight=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/1000;
						
						write_flag=1;
						printf("current_weight=%.3f\r\n",current_weight);
					break;
					case 0x1758:	//吊起物体时采样值
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[30]=h;
						Tx_Buffer[31]=l;
						current_weightsmapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						
						write_flag=1;
						printf("current_weightsmapl=%d\r\n",current_weightsmapl);
					break;
					case 0x1762:	//上限位
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[32]=h;
						Tx_Buffer[33]=l;
						upper_limit=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("upper_limit=%.1f\r\n",upper_limit);
					break;
					case 0x1766:	//近限位
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[34]=h;
						Tx_Buffer[35]=l;
						near_limit=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("near_limit=%.1f\r\n",near_limit);
					break;
					case 0x1768:	//远限位
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[36]=h;
						Tx_Buffer[37]=l;
						remote_limit=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("remote_limit=%.1f\r\n",remote_limit);
					break;
					case 0x1772:	//左转限位
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[38]=h;
						Tx_Buffer[39]=l;
						left_limit=(int16_t)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("left_limit=%d\r\n",left_limit);
					break;
					case 0x1774:	//右转限位
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[40]=h;
						Tx_Buffer[41]=l;
						right_limit=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("right_limit=%d\r\n",right_limit);
					break; 
					case 0x1776:	//防碰撞水平距离报警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[42]=h;
						Tx_Buffer[43]=l;
						level_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("level_limit0=%.1f\r\n",level_limit0);
					break;
					case 0x1790:	//防碰撞水平距离预警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[44]=h;
						Tx_Buffer[45]=l;
						level_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("level_limit1=%.1f\r\n",level_limit1);
					break;
					case 0x1778:	//防碰撞竖直距离报警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[46]=h;
						Tx_Buffer[47]=l;
						vertical_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("vertical_limit0=%.1f\r\n",vertical_limit0);
					break;
					case 0x1792:	//防碰撞竖直距离预警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[48]=h;
						Tx_Buffer[49]=l;
						vertical_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("vertical_limit1=%.1f\r\n",vertical_limit1);
					break;
					case 0x1780:	//起重比率报警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[50]=h;
						Tx_Buffer[51]=l;
						rate_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("rate_limit0=%.1f\r\n",rate_limit0);
					break;
					case 0x1794:	//起重比率预警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[52]=h;
						Tx_Buffer[53]=l;
						rate_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("rate_limit1=%.1f\r\n",rate_limit1);
					break;
					case 0x1782:	//风速报警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[54]=h;
						Tx_Buffer[55]=l;
						windspeed_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("windspeed_limit0=%.1f\r\n",windspeed_limit0);
					break;
					case 0x1796:	//风速预警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[56]=h;
						Tx_Buffer[57]=l;
						windspeed_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("windspeed_limit1=%.1f\r\n",windspeed_limit1);
					break;
					case 0x1784:	//倾角报警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[58]=h;
						Tx_Buffer[59]=l;
						angle_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("angle_limit0=%.1f\r\n",angle_limit0);
					break;
					case 0x1798:	//倾角预警
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[60]=h;
						Tx_Buffer[61]=l;
						angle_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("angle_limit1=%.1f\r\n",angle_limit1);
					break;
					case 0x1680:	//塔机高度
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[62]=h;
						Tx_Buffer[63]=l;
						height=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("height=%.1f\r\n",height);
					break;
					default: break;	
				}
				if(write_flag==1)
				{
					Tx_Buffer[64]=0x5a;
					SPI_Flash_Write((u8*)Tx_Buffer,FLASH_SIZE-100,SIZE);		//从倒数第100个地址处开始,写入SIZE长度的数据
					write_flag=0;
				}
			}
}

