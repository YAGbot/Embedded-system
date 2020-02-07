#include "usr_4g.h"
#include "usr_4g_timer.h"
#include "usr_4g_usart.h"
#include "volmeter.h"
#include "at24cxx.h"
#include "diwen.h"
#include "usart4.h"
#include "string.h" 
#include "uart.h"
#include "delay.h"
#include "stdlib.h"
#include <stdio.h>

/*************	本地变量声明	**************/
pack1 g_pack1;
pack2 g_pack2;

char ID[15];	

char socketA_str[50] = "AT+SOCKA=TCP,test.usr.cn,2317\r\n";	//socketA	
char socketB_str[50] = "AT+SOCKA=TCP,test.usr.cn,2317\r\n";	//socketB

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
	USART1_RX_STA=0;USART1_RX_REC_ATCOMMAD=1;
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
	USART1_RX_STA=0;USART1_RX_REC_ATCOMMAD=0;
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
	SIM7020_Snd_GSN();				//获取IMEI
	for(i=0; i<15; i++)
	{
		//buf[i] = gsm_uart_buff[i+9];		//868334030061703
		buf[i] = USART1_RX_BUF[i+2];		//868334030061703		//不回显
		ID[i] = buf[i];
	}
}

/*******************************************************************************
* 函数名 : USR4G_Enter_CMDMode
* 描述   : 退出透传模式进入AT命令模式
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
int USR4G_Enter_CMDMode(void)
{
	u8 try_num = 10;
	int res;
  do
  {
		res=gprs_send_cmd((u8*)"+++",(u8*)"a",200);	//检测到'a'
    if(res==0)			
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  res=gprs_send_cmd((u8*)"a",(u8*)"ok",200);
	return res;
}

void USR4G_DTUConfig(void)
{
	u8 ip_len1;
	u8 ip_len2;
	u8 socketB_flag=0;
	char buf[30];
	printf("进入配置\r\n");
	memset(buf,0,30);					//清空缓存
	AT24Cxx_ReadBuf(0,3,&ip_len1,1);		//读取IP1长度
	printf("ip_len1=%d",ip_len1);
	if( ( 10 <= ip_len1 ) && ( ip_len1 <= 40 ))			//输入了有效的IP
	{
		AT24Cxx_ReadBuf(0,100,(u8*)buf, ip_len1);	//从EEPROM中读取IP配置信息
		sprintf(socketA_str,"AT+SOCKA=TCP,%s\r\n",buf);
	}
	else
	{
		sprintf(socketA_str,"%s","AT+SOCKA=TCP,test.usr.cn,2317\r\n");		//使用默认配置
	}
	
	memset(buf,0,30);					//清空缓存
	AT24Cxx_ReadBuf(0,4,&ip_len2,1);		//读取IP2长度
	printf("ip_len2=%d",ip_len2);
	if( ( 10 <= ip_len2 ) && ( ip_len2 <= 40 ))			//输入了有效的IP
	{
		AT24Cxx_ReadBuf(0,150,(u8*)buf, ip_len2);	//从EEPROM中读取IP配置信息
		sprintf(socketB_str,"AT+SOCKB=TCP,%s\r\n",buf);
		socketB_flag=1;
	}
	else
	{
//		sprintf(socketB_str,"%s","AT+SOCKB=TCP,test.usr.cn,2317\r\n");		//使用默认配置	
	}
	//gprs_send_cmd((u8*)"AT+E=ON\r\n",(u8*)"OK",100);			//打开回显
	gprs_send_cmd((u8*)"AT+WKMOD=NET\r\n",(u8*)"OK",300);	//工作在NET模式
	gprs_send_cmd((u8*)"AT+SOCKAEN=ON\r\n",(u8*)"OK",300);	//使能SOCKETA
	gprs_send_cmd((u8*)socketA_str,(u8*)"OK",300);				//配置IP端口号
	gprs_send_cmd((u8*)"AT+SOCKASL=LONG\r\n",(u8*)"OK",300);	//配置为长连接
	gprs_send_cmd((u8*)"AT+SOCKATO=10\r\n",(u8*)"OK",300);	//设置超时重连时间为10s
	if(socketB_flag==1)
	{
		socketB_flag=0;
		gprs_send_cmd((u8*)"AT+SOCKBEN=ON\r\n",(u8*)"OK",300);	//使能SOCKETB
		gprs_send_cmd((u8*)socketB_str,(u8*)"OK",300);				//配置IP端口号
		gprs_send_cmd((u8*)"AT+SOCKBSL=LONG\r\n",(u8*)"OK",300);	//配置为长连接
		gprs_send_cmd((u8*)"AT+SOCKBTO=10\r\n",(u8*)"OK",300);	//设置超时重连时间为10s
	}
	else
		gprs_send_cmd((u8*)"AT+SOCKBEN=OFF\r\n",(u8*)"OK",300);	//关闭SOCKETB	//节省流量
	gprs_send_cmd((u8*)"AT+HEARTEN=ON\r\n",(u8*)"OK",300);		//开启心跳包功能
	gprs_send_cmd((u8*)"AT+HEARTDT=66\r\n",(u8*)"OK",300);		//设置心跳包
	gprs_send_cmd((u8*)"AT+HEARTSND=NET\r\n",(u8*)"OK",300);			//设置心跳包发至网络端
	gprs_send_cmd((u8*)"AT+HEARTTM=30\r\n",(u8*)"OK",300);			//设置心跳包发送时间
	gprs_send_cmd((u8*)"AT+CFGTF\r\n",(u8*)"OK",300);						//保存当前配置为默认配置
	gprs_send_cmd((u8*)"AT+Z\r\n",(u8*)"OK",300);	//软重启
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
    if(gprs_send_cmd((u8*)"AT+IMEI?\r\n",(u8*)"+IMEI:",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
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


/************************************************************************************
* 函数名称：Get_Data01
* 功能描述：获取参数数据函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void Get_Data01(void)
{
//	u8 *p;
//	u8 buf[]={0x01,0x01,0x01,0xAf,0xff,0xff,0xff,0x00,0x00,0x00,0x00};
//	p=&buf[3]; 								//数据开始地址
//	g_ELE_PAM.WH_P = Buf2Uint32(p); p+=4;							//正向有功电能
//	g_ELE_PAM.WH_M = Buf2Uint32(p);						//反向有功电能
	
	g_pack1.Ua1=(float)g_ELE_PAM.UA*g_ELE_PAM.U0*g_ELE_PAM.UBB/10000;			//A相电压
	g_pack1.Ub1=(float)g_ELE_PAM.UB*g_ELE_PAM.U0*g_ELE_PAM.UBB/10000;			//B相电压
	g_pack1.Uc1=(float)g_ELE_PAM.UC*g_ELE_PAM.U0*g_ELE_PAM.UBB/10000;			//C相电压
	
	g_pack1.Ia1=(float)g_ELE_PAM.IA*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;	//A相电流
	g_pack1.Ib1=(float)g_ELE_PAM.IB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;	//B相电流
	g_pack1.Ic1=(float)g_ELE_PAM.IC*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;	//C相电流
	
	g_pack1.P1=(float)g_ELE_PAM.P*g_ELE_PAM.U0*g_ELE_PAM.UBB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB*3/10000;	//总功率
	g_pack1.Pa1=(float)g_ELE_PAM.Pa*g_ELE_PAM.U0*g_ELE_PAM.UBB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;		//A相功率
	g_pack1.Pb1=(float)g_ELE_PAM.Pb*g_ELE_PAM.U0*g_ELE_PAM.UBB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;		//B相功率
	g_pack1.Pc1=(float)g_ELE_PAM.Pc*g_ELE_PAM.U0*g_ELE_PAM.UBB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;		//C相功率
	g_pack1.WH_P1=(float)g_ELE_PAM.WH_P*g_ELE_PAM.U0*g_ELE_PAM.I0/10*g_ELE_PAM.UBB*g_ELE_PAM.IBB/10800000;	//正向有功功率
	g_pack1.WH_M1=(float)g_ELE_PAM.WH_M*g_ELE_PAM.U0*g_ELE_PAM.I0/10*g_ELE_PAM.UBB*g_ELE_PAM.IBB/10800000;	//反向有功功率
	g_pack1.WH1=g_pack1.WH_P1-g_pack1.WH_M1;
}

/************************************************************************************
* 函数名称：Get_Data01
* 功能描述：获取参数数据函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void Get_Data02(void)
{
//	u8 *p;
//	u8 buf[]={0x01,0x01,0x01,0xAf,0xff,0xff,0xff,0x00,0x00,0x00,0x00};
//	p=&buf[3]; 								//数据开始地址
//	g_ELE_PAM2.WH_P = Buf2Uint32(p); p+=4;							//正向有功电能
//	g_ELE_PAM2.WH_M = Buf2Uint32(p);						//反向有功电能
	
	g_pack2.Ua1=(float)g_ELE_PAM2.UA*g_ELE_PAM2.U0*g_ELE_PAM2.UBB/10000;			//A相电压
	g_pack2.Ub1=(float)g_ELE_PAM2.UB*g_ELE_PAM2.U0*g_ELE_PAM2.UBB/10000;			//B相电压
	g_pack2.Uc1=(float)g_ELE_PAM2.UC*g_ELE_PAM2.U0*g_ELE_PAM2.UBB/10000;			//C相电压
	
	g_pack2.Ia1=(float)g_ELE_PAM2.IA*g_ELE_PAM2.I0/10*g_ELE_PAM2.IBB/10000;	//A相电流
	g_pack2.Ib1=(float)g_ELE_PAM2.IB*g_ELE_PAM2.I0/10*g_ELE_PAM2.IBB/10000;	//B相电流
	g_pack2.Ic1=(float)g_ELE_PAM2.IC*g_ELE_PAM2.I0/10*g_ELE_PAM2.IBB/10000;	//C相电流
	
	g_pack2.P1=(float)g_ELE_PAM2.P*g_ELE_PAM2.U0*g_ELE_PAM2.UBB*g_ELE_PAM2.I0/10*g_ELE_PAM2.IBB*3/10000;	//总功率
	g_pack2.Pa1=(float)g_ELE_PAM2.Pa*g_ELE_PAM2.U0*g_ELE_PAM2.UBB*g_ELE_PAM2.I0/10*g_ELE_PAM2.IBB/10000;		//A相功率
	g_pack2.Pb1=(float)g_ELE_PAM2.Pb*g_ELE_PAM2.U0*g_ELE_PAM2.UBB*g_ELE_PAM2.I0/10*g_ELE_PAM2.IBB/10000;		//B相功率
	g_pack2.Pc1=(float)g_ELE_PAM2.Pc*g_ELE_PAM2.U0*g_ELE_PAM2.UBB*g_ELE_PAM2.I0/10*g_ELE_PAM2.IBB/10000;		//C相功率
	g_pack2.WH_P1=(float)g_ELE_PAM2.WH_P*g_ELE_PAM2.U0*g_ELE_PAM2.I0/10*g_ELE_PAM2.UBB*g_ELE_PAM2.IBB/10800000;	//正向有功功率
	g_pack2.WH_M1=(float)g_ELE_PAM2.WH_M*g_ELE_PAM2.U0*g_ELE_PAM2.I0/10*g_ELE_PAM2.UBB*g_ELE_PAM2.IBB/10800000;	//反向有功功率
	g_pack2.WH1=g_pack2.WH_P1-g_pack2.WH_M1;
}

void GU900_Send_DataList(void)
{
	char data_buf[300];
	sprintf(data_buf,"{\"DataList\":[{\"Ua\":\"%.1f\",\"Ub\":\"%.1f\",\"Uc\":\"%.1f\",\"Ia\":\"%.1f\",\"Ib\":\"%.1f\",\"Ic\":\"%.1f\",\"P\":\"%.1f\","
	"\"Pa\":\"%.1f\",\"Pb\":\"%.1f\",\"Pc\":\"%.1f\",\"WH\":\"%.1f\"}],\"DevID\":\"HYDL%04dEP-AC-01\",\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"}\r\n",\
	g_pack1.Ua1,g_pack1.Ub1,g_pack1.Uc1,g_pack1.Ia1,g_pack1.Ib1,g_pack1.Ic1,g_pack1.P1,g_pack1.Pa1,g_pack1.Pb1,g_pack1.Pc1,g_pack1.WH1,\
	1,g_ELE_PAM.U0,g_ELE_PAM.I0/10,g_ELE_PAM.UBB,g_ELE_PAM.IBB);
	gsm_printf("%s",data_buf);
//	gsm_printf("{\"DataList\":[{\"Ua\":\"%fV\",\"Ub\":\"%fV\",\"Uc\":\"%fV\",\"Ia\":\"%fA\",\"Ib\":\"%fA\",\"Ic\":\"%fA\",\"P\":\"%fW\","
//	"\"Pa\":\"%fW\",\"Pb\":\"%fW\",\"Pc\":\"%fW\",\"WH\":\"%fKWh\"}],\"DevID\":\"HYHB%4EP-AC\",\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"}\n",\
//	g_pack1.Ua1,g_pack1.Ub1,g_pack1.Uc1,g_pack1.Ia1,g_pack1.Ib1,g_pack1.Ic1,g_pack1.P1,g_pack1.Pa1,g_pack1.Pb1,g_pack1.Pc1,g_pack1.WH1,\
//	200,g_ELE_PAM.U0,g_ELE_PAM.I0/10,g_ELE_PAM.UBB,g_ELE_PAM.IBB);
}

void GU900_Send_DataList02(void)
{
	char data_buf[300];
	sprintf(data_buf,"{\"DataList\":[{\"Ua\":\"%.1f\",\"Ub\":\"%.1f\",\"Uc\":\"%.1f\",\"Ia\":\"%.1f\",\"Ib\":\"%.1f\",\"Ic\":\"%.1f\",\"P\":\"%.1f\","
	"\"Pa\":\"%.1f\",\"Pb\":\"%.1f\",\"Pc\":\"%.1f\",\"WH\":\"%.1f\"}],\"DevID\":\"HYDL%04dEP-AC-02\",\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"}\r\n",\
	g_pack2.Ua1,g_pack2.Ub1,g_pack2.Uc1,g_pack2.Ia1,g_pack2.Ib1,g_pack2.Ic1,g_pack2.P1,g_pack2.Pa1,g_pack2.Pb1,g_pack2.Pc1,g_pack2.WH1,\
	1,g_ELE_PAM2.U0,g_ELE_PAM2.I0/10,g_ELE_PAM2.UBB,g_ELE_PAM2.IBB);
	gsm_printf("%s",data_buf);
//	gsm_printf("{\"DataList\":[{\"Ua\":\"%fV\",\"Ub\":\"%fV\",\"Uc\":\"%fV\",\"Ia\":\"%fA\",\"Ib\":\"%fA\",\"Ic\":\"%fA\",\"P\":\"%fW\","
//	"\"Pa\":\"%fW\",\"Pb\":\"%fW\",\"Pc\":\"%fW\",\"WH\":\"%fKWh\"}],\"DevID\":\"HYHB%4EP-AC\",\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"}\n",\
//	g_pack1.Ua1,g_pack1.Ub1,g_pack1.Uc1,g_pack1.Ia1,g_pack1.Ib1,g_pack1.Ic1,g_pack1.P1,g_pack1.Pa1,g_pack1.Pb1,g_pack1.Pc1,g_pack1.WH1,\
//	200,g_ELE_PAM.U0,g_ELE_PAM.I0/10,g_ELE_PAM.UBB,g_ELE_PAM.IBB);
}




