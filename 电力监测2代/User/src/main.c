#include "sys.h"   
#include "delay.h"
#include "timer.h"
#include "uart.h"
#include "rs485.h"
#include "volmeter.h"
#include "io_check.h"
#include "usr_4g.h"
#include "usr_4g_usart.h"
#include "usart4.h"
#include "diwen.h"
#include "at24cxx.h"
#include "string.h"
/* Include ----------------------------------------------------------------------------*/
#include "ff.h"
//#include "rtc.h"
#include "sd_card.h"
#include "malloc.h"
#include "file_system.h"
/* Functions --------------------------------------------------------------------------*/


int main(void)
{
	char DataStr[300] = {0};
	//char Time[20] = {0};
	u8 res;
	NVIC_Configuration();	//设置为组2
	delay_init();
	uart5_init(9600);		//调试串口初始化
	diwen_init();		//迪文屏初始化
	AT24Cxx_init();
	IO_check_init();
	TIM3_Init(7199,9999);
	delay_ms(3000);
	
	RS485_Init(36,9600);//485初始化
	delay_ms(2000);
	Write_Config();		//读取eeroom数据写入配置
	delay_ms(2000);
	Read_Config(SlaveAddr);		//读配置
	printf("\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"",g_ELE_PAM.U0,g_ELE_PAM.I0/10,g_ELE_PAM.UBB,g_ELE_PAM.IBB);
	diwen_send_data(0x3800,g_ELE_PAM.UBB);
	delay_ms(20);
	diwen_send_data(0x3810,g_ELE_PAM.IBB);
	delay_ms(20);
	Read_Config(SlaveAddr2);		//读配置
	printf("\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"",g_ELE_PAM2.U0,g_ELE_PAM2.I0/10,g_ELE_PAM2.UBB,g_ELE_PAM2.IBB);
	diwen_send_data(0x4800,g_ELE_PAM2.UBB);
	delay_ms(20);
	diwen_send_data(0x4810,g_ELE_PAM2.IBB);
	USART4_RX_STA=0;			//标志初始化
	delay_ms(20);
	
	GSM_Init();				//gprs初始化
	delay_ms(30000);		//延时等待4G模块进入透传
	USR4G_Enter_CMDMode();		//退出透传模式进入AT配置模式
	USR4G_DTUConfig();				//配置相关参数

	/************************/
	//RTC_Init();
	MEM_Init();	
	res=SDcard_Init();
	if(res==SD_OK)
	{
		printf("ssd_ok\r\n");
	}
	else printf("ssd_error\r\n");
	
	FileSystem_Mount("0:");		//挂载sd卡  设备号
	FileSystem_FileCreate();
	
	/************************/

	time3_start();
	
	while(1)
	{
		if(worktimes_flag==1)
		{
			time3_stop();
			worktimes_flag=0;
			Read_Parameters(SlaveAddr);//读各相电参数1
			Read_ElectricEnergy(SlaveAddr);//读电能1
			Get_Data01();			//计算数据
			GU900_Send_DataList();	//数据发送
			delay_ms(2000);			//这个延时是由于上位机下发的，能否减少时间看实际上位机接收
			Read_Parameters(SlaveAddr2);//读各相电参数1
			Read_ElectricEnergy(SlaveAddr2);//读电能1
			Get_Data02();			//计算数据
			GU900_Send_DataList02();//数据2发送

			diwen_view();			//迪文屏显示
			
			if(res==SD_OK)			//有sd卡就写，没有就不写
			{
			/************************/
//			Read_TimeStr(Time);
//			printf("Time:%s\r\n", Time);
			sprintf(DataStr,"{\"DataList\":[{\"Ua\":\"%.1fV\",\"Ub\":\"%.1fV\",\"Uc\":\"%.1fV\",\"Ia\":\"%.1fA\",\"Ib\":\"%.1fA\",\"Ic\":\"%.1fA\",\"P\":\"%.1fW\","
	"\"Pa\":\"%.1fW\",\"Pb\":\"%.1fW\",\"Pc\":\"%.1fW\",\"WH\":\"%.1fKWh\"}],\"DevID\":\"HYDL%04dEP-AC-01\",\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"}"
			"{\"DataList\":[{\"Ua\":\"%.1fV\",\"Ub\":\"%.1fV\",\"Uc\":\"%.1fV\",\"Ia\":\"%.1fA\",\"Ib\":\"%.1fA\",\"Ic\":\"%.1fA\",\"P\":\"%.1fW\","
			"\"Pa\":\"%.1fW\",\"Pb\":\"%.1fW\",\"Pc\":\"%.1fW\",\"WH\":\"%.1fKWh\"}],\"DevID\":\"HYDL%04dEP-AC-02\",\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"}\r\n",\
	g_pack1.Ua1,g_pack1.Ub1,g_pack1.Uc1,g_pack1.Ia1,g_pack1.Ib1,g_pack1.Ic1,g_pack1.P1,g_pack1.Pa1,g_pack1.Pb1,g_pack1.Pc1,g_pack1.WH1,1,g_ELE_PAM.U0,g_ELE_PAM.I0/10,g_ELE_PAM.UBB,g_ELE_PAM.IBB,\
	g_pack2.Ua1,g_pack2.Ub1,g_pack2.Uc1,g_pack2.Ia1,g_pack2.Ib1,g_pack2.Ic1,g_pack2.P1,g_pack2.Pa1,g_pack2.Pb1,g_pack2.Pc1,g_pack2.WH1,1,g_ELE_PAM2.U0,g_ELE_PAM2.I0/10,g_ELE_PAM2.UBB,g_ELE_PAM2.IBB);
//			DataStr_WriteWithTime(DataStr);
			DataStr_Write(DataStr);
			/************************/
			}
			time3_start();
		}
		if(ip1_modfiy_flag==1||ip2_modfiy_flag==1)		//检测到修改ip标志
		{
			time3_stop();
			ip1_modfiy_flag=0;
			ip2_modfiy_flag=0;
			USR4G_Enter_CMDMode();		//退出透传模式进入AT配置模式
			USR4G_DTUConfig();				//配置相关参数
			time3_start();
		}
		if(UBB_modfiy_flag==1||IBB_modfiy_flag==1||UBB2_modfiy_flag==1||IBB2_modfiy_flag==1)
		{
			time3_stop();
			UBB_modfiy_flag=0;
			IBB_modfiy_flag=0;
			UBB2_modfiy_flag=0;
			IBB2_modfiy_flag=0;
			delay_ms(500);
			Write_Config();
			delay_ms(500);
			Read_Config(SlaveAddr);		//读配置
			printf("\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"",g_ELE_PAM.U0,g_ELE_PAM.I0/10,g_ELE_PAM.UBB,g_ELE_PAM.IBB);
			diwen_send_data(0x3800,g_ELE_PAM.UBB);
			delay_ms(20);
			diwen_send_data(0x3810,g_ELE_PAM.IBB);
			delay_ms(20);
			Read_Config(SlaveAddr2);		//读配置
			printf("\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"",g_ELE_PAM2.U0,g_ELE_PAM2.I0/10,g_ELE_PAM2.UBB,g_ELE_PAM2.IBB);
			diwen_send_data(0x4800,g_ELE_PAM2.UBB);
			delay_ms(20);
			diwen_send_data(0x4810,g_ELE_PAM2.IBB);
			USART4_RX_STA=0;			//标志初始化
			delay_ms(20);
			time3_start();
		}
		check_IMI_cmd();//扫描是否有修改数据
	}
}


