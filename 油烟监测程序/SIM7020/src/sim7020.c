#include "sim7020.h"
#include "sim7020_usart.h"
#include "sim7020_timer.h"
#include "string.h" 
#include "uart.h"
#include "delay.h"
#include "stdlib.h"
#include <stdio.h>
#include "io_check.h"

/*************	本地常量声明	**************/
u8 gsm_recv_flag = 0; //接收数据状态位
	
char rat_buf[10];
int rat_len=0;

MQTT_Msg g_MQTT_Msg;
char MsgID[15];

const char *ipstring = "AT+IPPORT=\"bcoil.thhy-tj.com\",61613\r";		//服务器ip
//const char *ipstring = "AT+IPPORT=\"61.181.255.77\",61613\r";		//服务器ip
const char *apnstr = "AT*MCGDEFCONT=\"IP\",\"3GNET\"\r";		//配置APN
const char *userstr = "AT+USERPWD=thhy,thhy550550\r";		//配置APN
char response = 0;

/************************************************** *********************************
* 函数名称：GSM_Init
* 功能描述：gsm初始化函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void GSM_Init(void)
{
	sim7020_usart_init(115200);
	TIM7_Init();
}

/************************************************************************************
* 函数名称：check_buff
* 功能描述：缓存内容检查函数
* 入口参数：buff：
* 出口参数：0：成功 ；-1：非子串
*************************************************************************************/
int check_buff(u8 *buff, char *str)
{
    //字符串查找
  if(strstr((const char *)buff,str)!=NULL)
    return 0;
  else
    return -1;
}

/************************************************************************************
* 函数名称：gprs_check_cmd
* 功能描述：字符串检索函数
* 入口参数：
* 出口参数：
*************************************************************************************/
u8* gprs_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART1_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART1_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//向gprs发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 gprs_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART1_RX_STA=0;USART2_RX_REC_ATCOMMAD=1;
	if((u32)cmd<=0XFF)
	{  
		USART1->DR=(u32)cmd;
	}else gsm_printf("%s",cmd);//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(11);			//这里延时如果不加进不去中断？
			if(USART1_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(gprs_check_cmd(ack))break;//得到有效数据 
				USART1_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	USART1_RX_STA=0;USART2_RX_REC_ATCOMMAD=0;
	return res;
} 


void strToHexStr(char *src, char *str) 
{
	char *p=src;
	int j=0;
	while(*p!=0)
	{ 
		j+=sprintf(str+j,"%x",*p++);
	}
}

/************************************************************************************
* 函数名称：sim7020_sta
* 功能描述：sim7020状态确认，并获取硬件信息
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void sim7020_sta(void)
{
	u8 i;
	SIM7020_Snd_AT();					//检查模块能否通信
	Set_ATE0();
	SIM7020_Snd_GSN();				//获取IMEI
	for(i=0; i<15; i++)
	{
		//buf[i] = gsm_uart_buff[i+9];		//868334030061703
		//g_MQTT_Msg.ID[i] = USART1_RX_BUF[i+2];		//868334030061703		//不回显
		MsgID[i] = USART1_RX_BUF[i+2];
	}
	SIM7020_Snd_CPIN();				//检查卡状态
//	SIM7020_Snd_CSQ();				//获取网络状况
//	SIM7020_Snd_CGREG();			//查看网络附着状态
	//SIM7020_Snd_CGACT();			//查看是否激活网络
}

/************************************************************************************
* 函数名称：sim7020_mqtt
* 功能描述：sim7020进行mqtt业务
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void sim7020_mqtt(void)
{
	SIM7020_Snd_APN();	//设置APN
	SIM7020_Sed_CLIENTID();
	SIM7020_IPPORT();
	SIM7020_USERPWD();
	SIM7020_Snd_ATW();
	delay_ms(3000);
	SIM7020_Snd_CFUN();
}


/*******************************************************************************
* 函数名 : Check_STATUS
* 描述   : 取消回显
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 Check_STATUS(void)
{
	u8 try_num = 10;
	int res;
  do
  {
		res=gprs_send_cmd((u8*)"AT+STATUS\r",(u8*)"MQTT CONNECT OK",100);								
    if(res==0)			
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************************
* 函数名 : Set_ATE0
* 描述   : 取消回显
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 Set_ATE0(void)
{
	u8 try_num = 10;
	int res;
  do
  {
		res=gprs_send_cmd((u8*)"ATE0\r",(u8*)"OK",100);								//取消回显		
    if(res==0)			//检测到OK代表NBIOT模块已经正常启动，可以发送其他AT指令
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : Mcu_sim7020_AT_OK
* 描述   : 发送AT，返回OK，测试uart5串口和BC95模块之间通信是否正常
* 输入   : 无
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
u8 SIM7020_Snd_AT(void)
{
	u8 try_num = 10;
	int res;
  do
  {
		res=gprs_send_cmd((u8*)"AT\r",(u8*)"OK",100);
    if(res==0)			//检测到OK代表NBIOT模块已经正常启动，可以发送其他AT指令
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : Mcu_sim7020_AT_OK
* 描述   : 发送AT，返回OK，测试uart5串口和BC95模块之间通信是否正常
* 输入   : 无
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
u8 SIM7020_Snd_APN(void)
{
	u8 try_num = 10;
	int res;
  do
  {
		res=gprs_send_cmd((u8*)"AT+APN=\"UNINET\",\"\",\"\"\r",(u8*)"OK",100);
    if(res==0)			//检测到OK代表NBIOT模块已经正常启动，可以发送其他AT指令
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : SIM7020_Snd_CPIN
* 描述   : 发送AT+CPIN?，返回+CPIN:READY，测试SIM卡是否在位
* 输入   : 无
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
u8 SIM7020_Snd_CPIN(void)
{
	u8 try_num = 10;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CPIN?\r",(u8*)"+CPIN:READY",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : SIM7020_Snd_CSQ
* 描述   : 发送AT+CSQ，返回不为+CSQ: 99, 99，测试获取到网络
* 输入   : 无
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
u8 SIM7020_Snd_CSQ(void)
{
	u8 try_num = 60;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CSQ\r",(u8*)"+CSQ: 0,0",100)!=0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(2000);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : SIM7020_Snd_CGREG
* 描述   : 发送AT+CGREG?，返回+CGREG: 0,1，检查PS网络附着
* 输入   : 无
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
u8 SIM7020_Snd_CGREG(void)
{
	u8 try_num = 10;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CGREG?\r",(u8*)"+CGREG: 0,1",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		else if(gprs_send_cmd((u8*)"AT+CGREG?\r",(u8*)"+CGREG: 0,5",100)==0)
		{
			delay_ms(1000);
    	break;
		}
		delay_ms(2000);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : SIM7020_Snd_CGACT
* 描述   : 发送AT+CGACT?，返回为+CGACT: 1,1，测试PDN 激活成功
* 输入   : 无
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
u8 SIM7020_Snd_CGACT(void)
{
	u8 try_num = 20;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CGACT?\r",(u8*)"+CGACT: 1,1",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : SIM7020_Snd_ATW
* 描述   : 保存配置参数
* 输入   : 
* 输出   :
* 返回   : 
********************************************************************/
void SIM7020_Snd_ATW(void)
{
	gprs_send_cmd((u8*)"AT&W\r",(u8*)"OK",100);
}


/*******************************************************************
* 函数名 : SIM7020_Snd_CFUN
* 描述   : 发送AT+CFUN，射频开关,重启模块
* 输入   : 
* 输出   :
* 返回   : 45S 内附着 GPRS 网络成功模块会主动上报+STATUS: MQTT READY，否则模块会主动重启
					 模块主动上报+STATUS: MQTT READY 后立即连接服务器，连接成功时模块会主动上报+STATUS: MQTT CONNECT OK
********************************************************************/
void SIM7020_Snd_CFUN(void)
{
	gprs_send_cmd((u8*)"AT+CFUN=1,1\r",(u8*)"AT Ready",100);
}


/*******************************************************************
* 函数名 : SIM7020_Snd_GSN
* 描述   : 发送AT+GSN，返回OK，获取imei
* 输入   : 无
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
u8 SIM7020_Snd_GSN(void)
{
	u8 try_num = 10;
  do
  {
    if(gprs_send_cmd((u8*)"AT+GSN\r",(u8*)"OK",100)==0)	
    {
			delay_ms(1000);
    	break; 
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}


/*******************************************************************
* 函数名 : SIM7020_IPPORT
* 描述   : 配置mqtt服务器
* 输入   : 无
* 输出   :
* 返回   :
* 注意   : 发送AT，返回OK
********************************************************************/
void SIM7020_IPPORT(void)
{
	gprs_send_cmd((u8*)ipstring,(u8*)"OK",100);
}


/*******************************************************************
* 函数名 : SIM7020_Sed_CLIENTID
* 描述   : 配置设备ID
* 输入   : 无
* 输出   :
* 返回   : 
* 注意   : 
********************************************************************/
void SIM7020_Sed_CLIENTID(void)
{
	char idbuf[50];
	sprintf(idbuf,"AT+CLIENTID=\"%s\"\r",MsgID);
	gprs_send_cmd((u8*)idbuf,(u8*)"OK",100);
}	

/*******************************************************************
* 函数名 : SIM7020_USERPWD
* 描述   : 配置用户名和密码
* 输入   : 无
* 输出   :
* 返回   :
* 注意   : 发送AT，返回OK
********************************************************************/
void SIM7020_USERPWD(void)
{
	gprs_send_cmd((u8*)userstr,(u8*)"OK",100);
}

/*******************************************************************
* 函数名 : SIM7020_PUBLISH
* 描述   : mqtt发布
* 输入   : 无
* 输出   :
* 返回   : {"ID_IMEI":"845321478625451","cleaner1":"0","fan1":"0","cleaner2":"1","fan2":"1"}	//0：通电 1:断电
* 注意   : 发送AT，返回OK	
********************************************************************/
void SIM7020_PUBLISH(void)
{
	char msg_buf[150];
	char send_buf[200];
	
	//sprintf(msg_buf,"{\"ID_IMEI\":\"%s\",\"video\":\"%d\",\"ele\":\"%d\",\"net\":\"%d\"}",g_MQTT_Msg.ID,g_MQTT_Msg.video_sta,g_MQTT_Msg.power_sta,Loss_Rate);
	sprintf(msg_buf,"{\"ID_IMEI\":\"%s\",\"cleaner1\":\"%d\",\"fan1\":\"%d\",\"cleaner2\":\"%d\",\"fan2\":\"%d\"}",MsgID,FIFO1,FIFO2,FIFO3,FIFO4);		
	sprintf(send_buf,"AT+PUBLISH=\"THHYOMT\",%d,2\r",strlen(msg_buf));
	printf("%s\r",msg_buf);
	gprs_send_cmd((u8*)send_buf,(u8*)">",1000);
	gprs_send_cmd((u8*)msg_buf,(u8*)"OK",1000);
	
}


/*******************************************************************
* 函数名 : SIM7020_SUBSCRIBE
* 描述   : mqtt订阅
* 输入   : 无
* 输出   :
* 返回   : 
* 注意   : 发送AT，返回OK	
********************************************************************/
void SIM7020_SUBSCRIBE(void)
{
	gprs_send_cmd((u8*)"AT+CMQSUB=0,\"myTopic\",1\r",(u8*)"OK",1000);
}






