#include "sim7020.h"
#include "sim7020_usart.h"
#include "sim7020_timer.h"
#include "string.h" 
#include "uart.h"
#include "delay.h"
#include "stdlib.h"
#include <stdio.h>
#include "ping.h"

/*************	本地常量声明	**************/
u8 gsm_recv_flag = 0; //gu900接收数据状态位
u8 send_Msgflag =0;		//数据发送状态位

char ID[15];	
char rat_buf[10];
int rat_len=0;

MQTT_Msg g_MQTT_Msg;
const char *string = "AT+CMQNEW=\"61.181.255.77\",\"61613\",12000,200\r\n";		//创建MQTT连接
const char *apnstr = "AT*MCGDEFCONT=\"IP\",\"3GNET\"\r\n";		//配置APN
char response = 0;

/************************************************************************************
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
	}else gsm_printf("%s\r\n",cmd);//发送命令
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
	char buf[15]={0};
	u8 i;
	SIM7020_Snd_AT();					//检查模块能否通信
	Set_ATE0();
	SIM7020_Snd_CPIN();				//检查卡状态
	SIM7020_Snd_CSQ();				//获取网络状况
	SIM7020_Snd_CGREG();			//查看网络附着状态
	SIM7020_Snd_CGACT();			//查看是否激活网络
	SIM7020_Snd_GSN();				//获取IMEI
	for(i=0; i<15; i++)
	{
		//buf[i] = gsm_uart_buff[i+9];		//868334030061703
		buf[i] = USART1_RX_BUF[i+2];		//868334030061703		//不回显
		ID[i] = buf[i];
	}
	//printf("%s",buf);
	strToHexStr(buf,g_MQTT_Msg.imeid);
	//printf("imeid=%s\r\n",g_MQTT_Msg.imeid);
}

/************************************************************************************
* 函数名称：sim7020_mqtt
* 功能描述：sim7020进行mqtt业务
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void sim7020_mqtt(void)
{
	printf("关闭mqtt连接\r\n");
	SIM7020_Sed_CMQDISCON();//关闭mqtt连接
	delay_ms(1000);
	
	printf("创建新连接\r\n");	
	SIM7020_New_Mqtt();			//创建新连接
	delay_ms(1000);
	
	printf("发送请求\r\n");
	SIM7020_Sed_CMQCON();		//发送请求
	delay_ms(1000);
	
	printf("订阅\r\n");
	SIM7020_Sed_CMQSUB();		//订阅
	delay_ms(1000);
}


/*******************************************************************************
* 函数名 : Set_ATE0
* 描述   : 取消回显
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Set_ATE0(void)
{
	gprs_send_cmd((u8*)"ATE0\r\n",(u8*)"OK",100);								//取消回显		
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
		res=gprs_send_cmd((u8*)"AT\r\n",(u8*)"OK",100);
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
    if(gprs_send_cmd((u8*)"AT+CPIN?\r\n",(u8*)"+CPIN: READY",100)==0)	
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
    if(gprs_send_cmd((u8*)"AT+CSQ\r\n",(u8*)"+CSQ: 0,0",100)!=0)	
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
	u8 try_num = 20;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CGREG?\r\n",(u8*)"+CGREG: 0,1",100)==0)	
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
    if(gprs_send_cmd((u8*)"AT+CGACT?\r\n",(u8*)"+CGACT: 1,1",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : SIM7020_Snd_CFUN
* 描述   : 发送AT+CFUN，射频开关
* 输入   : sta
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
void SIM7020_Snd_CFUN(u8 sta)
{
	if(sta)
	{
		gprs_send_cmd((u8*)"AT+CFUN=1",(u8*)"+CPIN: READY",100);
	}
	else
		gprs_send_cmd((u8*)"AT+CFUN=0",(u8*)"+CPIN: NOT READY",100);
}

/*******************************************************************
* 函数名 : SIM7020_Snd_APN
* 描述   : 发送AT*MCGDEFCONT，返回为OK，手动配置APN
* 输入   : 无
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
void SIM7020_Snd_APN(void)
{
	gprs_send_cmd((u8*)apnstr,(u8*)"OK",100);
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
    if(gprs_send_cmd((u8*)"AT+GSN\r\n",(u8*)"OK",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : SIM7020_Snd_CCLK
* 描述   : 发送AT+CCLK?，返回OK，获取网络时间
* 输入   : 无
* 输出   :
* 返回   : 0 : 成功   非零 ：失败
* 注意   : 发送AT，返回OK
********************************************************************/
u8 SIM7020_Snd_CCLK(void)
{
	u8 try_num = 10;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CCLK?\r\n",(u8*)"OK",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* 函数名 : SIM7020_New_Mqtt
* 描述   : SIM7020实现mqtt业务
* 输入   : 无
* 输出   :
* 返回   :
* 注意   : 发送AT，返回OK
********************************************************************/
void SIM7020_New_Mqtt(void)
{
	gprs_send_cmd((u8*)string,(u8*)"OK",500);
}

/*******************************************************************
* 函数名 : SIM7020_Sed_CMQCON
* 描述   : SIM7020发送mqtt请求
* 输入   : 无
* 输出   :
* 返回   : 
* 注意   : 发送AT，返回OK
********************************************************************/
void SIM7020_Sed_CMQCON(void)
{
	//printf("AT+CMQCON=0,3,\"%s\",600,0,0,\"thhy\",\"thhy550550\"\r\n",ID);
	//gsm_printf("AT+CMQCON=0,3,\"%s\",60,0,0,\"thhy\",\"thhy550550\"\r\n",ID);
	char rqbuf[100];
	sprintf(rqbuf,"AT+CMQCON=0,3,\"%s\",600,0,0,\"thhy\",\"thhy550550\"\r\n",ID);
	gprs_send_cmd((u8*)rqbuf,(u8*)"OK",200);
}	


/*******************************************************************
* 函数名 : SIM7020_Sed_CMQCON
* 描述   : SIM7020发送mqtt请求
* 输入   : 无
* 输出   :
* 返回   : 
* 注意   : 发送AT，返回OK	
********************************************************************/
void SIM7020_Sed_CMQPUB(void)
{
	u8 num = 3;
	int ret;
	char buf[150];
	char msg_buf[150];
	char send_buf[200];
	send_Msgflag = 1;
	
	sprintf(buf,"{\"ID_IMEI\":\"%s\",\"video\":\"%d\",\"ele\":\"%d\",\"net\":\"%d\"}",ID,g_MQTT_Msg.video_sta,g_MQTT_Msg.power_sta,Net_Info.Loss_Rate);
	strToHexStr(buf,msg_buf);
	sprintf(send_buf,"AT+CMQPUB=0,\"Topic\",1,0,0,%d,\"%s\"\r\n",strlen(msg_buf),msg_buf);	
	
	while(num)
	{
		--num;
		ret=gprs_send_cmd((u8*)send_buf,(u8*)"OK",1000);
		if(ret==0)
		{
			printf("发送结束\r\n");
			error_count=0;	//重新计数
			break;
		}		
		delay_ms(2000);
	}
	send_Msgflag = 0;
}




/*******************************************************************************
* 函数名 : itoi
* 描述   : 数字转化为字符串
* 输入   :
* 输出   :
* 返回   :
* 注意   :
*******************************************************************************/
void itoi(char *str, unsigned long num)
{
	char index[]="0123456789";
	char i = 0, j = 0;
    char temp;

    do
    {
        str[i++] = index[num%10];
        num /= 10;
    }while(num);

    str[i]='\0';

    for(j=0; j<=(i-1)/2; j++)
    {
        temp=str[j];
        str[j]=str[i-j-1];
        str[i-j-1]=temp;
    }
}

/*******************************************************************************
* 函数名 : itoc
* 描述   : 单个数字转化为字符
* 输入   : ichar数字
* 输出   :
* 返回   : 字符
* 注意   :
*******************************************************************************/
char itoc(unsigned char ichar)
{
	char index[]="0123456789";

    return index[ichar%10];
}



void SIM7020_Sed_CMQSUB(void)
{
	gprs_send_cmd((u8*)"AT+CMQSUB=0,\"myTopic\",1\r\n",(u8*)"OK",1000);
}

void SIM7020_Sed_CMQDISCON(void)
{
	gprs_send_cmd((u8*)"AT+CMQDISCON=0\r\n",(u8*)"OK",1000);
}


void SIM7020_Sed_CMQUNSUB(void)
{
	gprs_send_cmd((u8*)"AT+CMQUNSUB=0,\"myTopic\"\r\n",(u8*)"OK",1000);
}





