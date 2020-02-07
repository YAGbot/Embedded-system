#include "diwen.h"
#include "usart4.h"
#include "usr_4g.h"
#include "uart.h"
#include "at24cxx.h"
#include "delay.h"
#include "volmeter.h"
#include "string.h"
#include "uart.h"

u8 ip1_modfiy_flag=0;		//ip修改标志
u8 ip2_modfiy_flag=0;		//ip修改标志
u8 UBB_modfiy_flag=0;		//电压比修改标志
u8 IBB_modfiy_flag=0;		//电流比修改标志
u8 UBB2_modfiy_flag=0;		//电压比修改标志
u8 IBB2_modfiy_flag=0;		//电流比修改标志


const char mima[10]={0x5A,0xA5,0x07,0x82,0x00,0x84,0x5A,0x01,0x00,0x13};	//密码成功切换指令

void diwen_init(void)
{
	UART4_Init(115200);	//迪文屏串口初始化
}

/************************************************************************************
* 函数名称：diwen_send_data
* 功能描述：迪文屏发送实时数据
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_send_data(u16 _reg,u32 _data)
{
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//数据头
	USART4_TX_BUF[USART4_TX_CNT++] = 0x07;	//发送的数据长度
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//写指令
	USART4_TX_BUF[USART4_TX_CNT++] = _reg>>8;		//寄存器编号 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = _reg;		//寄存器编号 低字节
	USART4_TX_BUF[USART4_TX_CNT++] = _data >> 24;	//写入数据
	USART4_TX_BUF[USART4_TX_CNT++] = _data >> 16;	//写入数据
	USART4_TX_BUF[USART4_TX_CNT++] = _data >> 8;	//写入数据
	USART4_TX_BUF[USART4_TX_CNT++] = _data;	//写入数据
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
}

/************************************************************************************
* 函数名称：diwen_write_line
* 功能描述：迪文屏画电流曲线  A B C三相
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_write_line(u16 _Ia,u16 _Ib,u16 _Ic)
{
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//数据头
	USART4_TX_BUF[USART4_TX_CNT++] = 0x13;	//发送的数据长度
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//写指令
	USART4_TX_BUF[USART4_TX_CNT++] = 0x03;	//画曲线指令
	USART4_TX_BUF[USART4_TX_CNT++] = 0x10;	
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//数据头
	USART4_TX_BUF[USART4_TX_CNT++] = 0x03;		//3个通道
	USART4_TX_BUF[USART4_TX_CNT++] = 0x00;		//自定义数据
	USART4_TX_BUF[USART4_TX_CNT++] = 0x00;		//通道0
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;		//1个数据
	USART4_TX_BUF[USART4_TX_CNT++] = _Ia>>8;		//A相电流 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = _Ia;		//A相电流 低字节
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;		//通道1
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;		//1个数据
	USART4_TX_BUF[USART4_TX_CNT++] = _Ib>>8;		//B相电流 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = _Ib;		//B相电流 低字节
	USART4_TX_BUF[USART4_TX_CNT++] = 0x02;		//通道2
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;		//1个数据
	USART4_TX_BUF[USART4_TX_CNT++] = _Ic>>8;		//C相电流 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = _Ic;		//C相电流 低字节
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
}

void diwen_view(void)
{
	int Ua,Ub,Uc,Ia,Ib,Ic,Pa,Pb,Pc,P,WH,Ua2,Ub2,Uc2,Ia2,Ib2,Ic2,Pa2,Pb2,Pc2,P2,WH2;	//迪文屏显示

//	Ua=1000;
//	Ub=3808;
//	Uc=3004;
//	Ia=5001;
//	Ib=6008;
//	Ic=10005;
//	Pa=-250000;
//	Pb=-452222;
//	Pc=-568888;
//	P=-5452525;
//	WH=9475754;
//	
//	Ua2=3802;
//	Ub2=3809;
//	Uc2=2453;
//	Ia2=4001;
//	Ib2=8001;
//	Ic2=12015;
//	Pa2=250000;
//	Pb2=452222;
//	Pc2=568888;
//	P2=5452525;
//	WH2=8596644;
//	Ua=0;
//	Ub=0;
//	Uc=0;
//	Ia=0;
//	Ib=0;
//	Ic=0;
//	Pa=0;
//	Pb=0;
//	Pc=0;
//	P=0;
//	WH=0;
	Ua=(int)(g_pack1.Ua1*10);
	Ub=(int)(g_pack1.Ub1*10);
	Uc=(int)(g_pack1.Uc1*10);
	Ia=(int)(g_pack1.Ia1*10);
	Ib=(int)(g_pack1.Ib1*10);
	Ic=(int)(g_pack1.Ic1*10);
	Pa=(int)(g_pack1.Pa1*10);
	Pb=(int)(g_pack1.Pb1*10);
	Pc=(int)(g_pack1.Pc1*10);
	P=(int)(g_pack1.P1*10);
	WH=(int)g_pack1.WH1;
	
	Ua2=(int)(g_pack2.Ua1*10);
	Ub2=(int)(g_pack2.Ub1*10);
	Uc2=(int)(g_pack2.Uc1*10);
	Ia2=(int)(g_pack2.Ia1*10);
	Ib2=(int)(g_pack2.Ib1*10);
	Ic2=(int)(g_pack2.Ic1*10);
	Pa2=(int)(g_pack2.Pa1*10);
	Pb2=(int)(g_pack2.Pb1*10);
	Pc2=(int)(g_pack2.Pc1*10);
	P2=(int)(g_pack2.P1*10);
	WH2=(int)g_pack2.WH1;
	
	diwen_send_data(0x3070,Ua);
	delay_ms(20);
	diwen_send_data(0x3080,Ub);
	delay_ms(20);
	diwen_send_data(0x3090,Uc);
	delay_ms(20);
	diwen_send_data(0x3010,Ia);
	delay_ms(20);
	diwen_send_data(0x3020,Ib);
	delay_ms(20);
	diwen_send_data(0x3030,Ic);
	delay_ms(20);
	diwen_send_data(0x3100,Pa);
	delay_ms(20);
	diwen_send_data(0x3110,Pb);
	delay_ms(20);
	diwen_send_data(0x3120,Pc);
	delay_ms(20);
	diwen_send_data(0x3130,P);
	delay_ms(20);
	diwen_send_data(0x3140,WH);
	delay_ms(20);
	
	
	diwen_send_data(0x4070,Ua2);
	delay_ms(20);
	diwen_send_data(0x4080,Ub2);
	delay_ms(20);
	diwen_send_data(0x4090,Uc2);
	delay_ms(20);
	diwen_send_data(0x4010,Ia2);
	delay_ms(20);
	diwen_send_data(0x4020,Ib2);
	delay_ms(20);
	diwen_send_data(0x4030,Ic2);
	delay_ms(20);
	diwen_send_data(0x4100,Pa2);
	delay_ms(20);
	diwen_send_data(0x4110,Pb2);
	delay_ms(20);
	diwen_send_data(0x4120,Pc2);
	delay_ms(20);
	diwen_send_data(0x4130,P2);
	delay_ms(20);
	diwen_send_data(0x4140,WH2);
	delay_ms(20);
	USART4_RX_STA=0;			//标志初始化
}


void check_IMI_cmd(void)
{ 
	if(USART4_RX_STA&0X8000)	//接收到一次数据了
	{
		USART4_RX_STA=0;			//标志初始化	 
		Data_Scan();
	}		
}

/************************************************************************************
* 函数名称：Data_Scan
* 功能描述：迪文屏触摸参数处理函数		保存至24c02地址 ：30 40 50 60 100 150(1 2 5 6 3 4) 
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void Data_Scan(void)
{
	u8 buf[30];
	char d_buf[50];
	u16 addr;
	u8 len;
	if(USART4_RX_BUF[7]==0x5A)	//接收到ASCII码了
	{
		len=USART4_RX_BUF[8];			//该位置存放的是接收的ASCII码数据长度
		printf("len=%d\r\n",len);
		addr=((u16)USART4_RX_BUF[4]<<8)|USART4_RX_BUF[5];		//得到地址
		switch(addr)
		{
			case 0x34ff:				//电流比1
				IBB_modfiy_flag=1;
				memset(buf,0,30);					//清空缓存
				memcpy(buf,USART4_RX_BUF+9,len);	//将ASCII码值存入缓存
				AT24Cxx_WriteByte(0,1,USART4_RX_BUF[8]);		//写入长度
				AT24Cxx_WriteBuf(0,30,buf,len);
				printf("电流比1=%s",buf);
				break;
			
			case 0x44ff:				//电流比2
				IBB2_modfiy_flag=1;
				memset(buf,0,30);					//清空缓存
				memcpy(buf,USART4_RX_BUF+9,len);	//将ASCII码值存入缓存
				AT24Cxx_WriteByte(0,5,USART4_RX_BUF[8]);		//写入长度
				AT24Cxx_WriteBuf(0,50,buf,len);
				printf("电流比2=%s",buf);
				break;
			
			case 0x350f:				//电压比1
				UBB_modfiy_flag=1;
				memset(buf,0,30);					//清空缓存
				memcpy(buf,USART4_RX_BUF+9,len);	//将ASCII码值存入缓存
				AT24Cxx_WriteByte(0,2,USART4_RX_BUF[8]);		//写入长度
				AT24Cxx_WriteBuf(0,40,buf,len);
				printf("电压比1=%s",buf);
				break;
			
			case 0x450f:				//电压比1
				UBB2_modfiy_flag=1;
				memset(buf,0,30);					//清空缓存
				memcpy(buf,USART4_RX_BUF+9,len);	//将ASCII码值存入缓存
				AT24Cxx_WriteByte(0,6,USART4_RX_BUF[8]);		//写入长度
				AT24Cxx_WriteBuf(0,60,buf,len);
				printf("电压比2=%s",buf);
				break;
			
			case 0x351f:				//ip1
				ip1_modfiy_flag=1;
				memset(buf,0,30);					//清空缓存
				memcpy(buf,USART4_RX_BUF+9,len);	//将ASCII码值存入缓存
				AT24Cxx_WriteByte(0,3,USART4_RX_BUF[8]);		//写入长度
				AT24Cxx_WriteBuf(0,100,buf,len);
				printf("ip1=%s",buf);
				break;
			
			case 0x35ff:				//ip2
				ip2_modfiy_flag=1;
				memset(buf,0,30);					//清空缓存
				memcpy(buf,USART4_RX_BUF+9,len);	//将ASCII码值存入缓存
				AT24Cxx_WriteByte(0,4,USART4_RX_BUF[8]);		//写入长度
				AT24Cxx_WriteBuf(0,150,buf,len);
				printf("ip2=%s",buf);
				break;
			case 0x31ff:				//密码
				memset(buf,0,30);					//清空缓存
				memcpy(buf,USART4_RX_BUF+9,len);	//将ASCII码值存入缓存
				sprintf(d_buf,"%s",buf);
				if(strcmp(d_buf,"666666")==0)
				{
					printf("密码匹配\r\n");
					USART4_SendDat((u8*)mima,10);
					delay_ms(20);
					USART4_RX_STA=0;			//标志初始化
				}
				printf("密码=%s\r\n",d_buf);
				break;
		default: break;				
	}
//		memset(buf,0,30);					//清空缓存
//		memcpy(buf,USART4_RX_BUF+9,len);	//将ASCII码值存入缓存
//		sprintf(d_buf,"AT+SOCKA=TCP,%s\r\n",buf);
//		printf("%s",d_buf);
	}
}
