#include "diwen.h"
#include "usart4.h"
#include "usr_4g.h"
#include "uart.h"
#include "at24cxx.h"
#include "delay.h"
#include "volmeter.h"
#include "string.h"
#include "uart.h"

u8 ip1_modfiy_flag=0;		//ip�޸ı�־
u8 ip2_modfiy_flag=0;		//ip�޸ı�־
u8 UBB_modfiy_flag=0;		//��ѹ���޸ı�־
u8 IBB_modfiy_flag=0;		//�������޸ı�־
u8 UBB2_modfiy_flag=0;		//��ѹ���޸ı�־
u8 IBB2_modfiy_flag=0;		//�������޸ı�־


const char mima[10]={0x5A,0xA5,0x07,0x82,0x00,0x84,0x5A,0x01,0x00,0x13};	//����ɹ��л�ָ��

void diwen_init(void)
{
	UART4_Init(115200);	//���������ڳ�ʼ��
}

/************************************************************************************
* �������ƣ�diwen_send_data
* ��������������������ʵʱ����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_send_data(u16 _reg,u32 _data)
{
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//����ͷ
	USART4_TX_BUF[USART4_TX_CNT++] = 0x07;	//���͵����ݳ���
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//дָ��
	USART4_TX_BUF[USART4_TX_CNT++] = _reg>>8;		//�Ĵ������ ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = _data >> 24;	//д������
	USART4_TX_BUF[USART4_TX_CNT++] = _data >> 16;	//д������
	USART4_TX_BUF[USART4_TX_CNT++] = _data >> 8;	//д������
	USART4_TX_BUF[USART4_TX_CNT++] = _data;	//д������
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
}

/************************************************************************************
* �������ƣ�diwen_write_line
* ��������������������������  A B C����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_write_line(u16 _Ia,u16 _Ib,u16 _Ic)
{
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//����ͷ
	USART4_TX_BUF[USART4_TX_CNT++] = 0x13;	//���͵����ݳ���
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//дָ��
	USART4_TX_BUF[USART4_TX_CNT++] = 0x03;	//������ָ��
	USART4_TX_BUF[USART4_TX_CNT++] = 0x10;	
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//����ͷ
	USART4_TX_BUF[USART4_TX_CNT++] = 0x03;		//3��ͨ��
	USART4_TX_BUF[USART4_TX_CNT++] = 0x00;		//�Զ�������
	USART4_TX_BUF[USART4_TX_CNT++] = 0x00;		//ͨ��0
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;		//1������
	USART4_TX_BUF[USART4_TX_CNT++] = _Ia>>8;		//A����� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = _Ia;		//A����� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;		//ͨ��1
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;		//1������
	USART4_TX_BUF[USART4_TX_CNT++] = _Ib>>8;		//B����� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = _Ib;		//B����� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = 0x02;		//ͨ��2
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;		//1������
	USART4_TX_BUF[USART4_TX_CNT++] = _Ic>>8;		//C����� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = _Ic;		//C����� ���ֽ�
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
}

void diwen_view(void)
{
	int Ua,Ub,Uc,Ia,Ib,Ic,Pa,Pb,Pc,P,WH,Ua2,Ub2,Uc2,Ia2,Ib2,Ic2,Pa2,Pb2,Pc2,P2,WH2;	//��������ʾ

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
	USART4_RX_STA=0;			//��־��ʼ��
}


void check_IMI_cmd(void)
{ 
	if(USART4_RX_STA&0X8000)	//���յ�һ��������
	{
		USART4_RX_STA=0;			//��־��ʼ��	 
		Data_Scan();
	}		
}

/************************************************************************************
* �������ƣ�Data_Scan
* ������������������������������		������24c02��ַ ��30 40 50 60 100 150(1 2 5 6 3 4) 
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void Data_Scan(void)
{
	u8 buf[30];
	char d_buf[50];
	u16 addr;
	u8 len;
	if(USART4_RX_BUF[7]==0x5A)	//���յ�ASCII����
	{
		len=USART4_RX_BUF[8];			//��λ�ô�ŵ��ǽ��յ�ASCII�����ݳ���
		printf("len=%d\r\n",len);
		addr=((u16)USART4_RX_BUF[4]<<8)|USART4_RX_BUF[5];		//�õ���ַ
		switch(addr)
		{
			case 0x34ff:				//������1
				IBB_modfiy_flag=1;
				memset(buf,0,30);					//��ջ���
				memcpy(buf,USART4_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				AT24Cxx_WriteByte(0,1,USART4_RX_BUF[8]);		//д�볤��
				AT24Cxx_WriteBuf(0,30,buf,len);
				printf("������1=%s",buf);
				break;
			
			case 0x44ff:				//������2
				IBB2_modfiy_flag=1;
				memset(buf,0,30);					//��ջ���
				memcpy(buf,USART4_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				AT24Cxx_WriteByte(0,5,USART4_RX_BUF[8]);		//д�볤��
				AT24Cxx_WriteBuf(0,50,buf,len);
				printf("������2=%s",buf);
				break;
			
			case 0x350f:				//��ѹ��1
				UBB_modfiy_flag=1;
				memset(buf,0,30);					//��ջ���
				memcpy(buf,USART4_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				AT24Cxx_WriteByte(0,2,USART4_RX_BUF[8]);		//д�볤��
				AT24Cxx_WriteBuf(0,40,buf,len);
				printf("��ѹ��1=%s",buf);
				break;
			
			case 0x450f:				//��ѹ��1
				UBB2_modfiy_flag=1;
				memset(buf,0,30);					//��ջ���
				memcpy(buf,USART4_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				AT24Cxx_WriteByte(0,6,USART4_RX_BUF[8]);		//д�볤��
				AT24Cxx_WriteBuf(0,60,buf,len);
				printf("��ѹ��2=%s",buf);
				break;
			
			case 0x351f:				//ip1
				ip1_modfiy_flag=1;
				memset(buf,0,30);					//��ջ���
				memcpy(buf,USART4_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				AT24Cxx_WriteByte(0,3,USART4_RX_BUF[8]);		//д�볤��
				AT24Cxx_WriteBuf(0,100,buf,len);
				printf("ip1=%s",buf);
				break;
			
			case 0x35ff:				//ip2
				ip2_modfiy_flag=1;
				memset(buf,0,30);					//��ջ���
				memcpy(buf,USART4_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				AT24Cxx_WriteByte(0,4,USART4_RX_BUF[8]);		//д�볤��
				AT24Cxx_WriteBuf(0,150,buf,len);
				printf("ip2=%s",buf);
				break;
			case 0x31ff:				//����
				memset(buf,0,30);					//��ջ���
				memcpy(buf,USART4_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				sprintf(d_buf,"%s",buf);
				if(strcmp(d_buf,"666666")==0)
				{
					printf("����ƥ��\r\n");
					USART4_SendDat((u8*)mima,10);
					delay_ms(20);
					USART4_RX_STA=0;			//��־��ʼ��
				}
				printf("����=%s\r\n",d_buf);
				break;
		default: break;				
	}
//		memset(buf,0,30);					//��ջ���
//		memcpy(buf,USART4_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
//		sprintf(d_buf,"AT+SOCKA=TCP,%s\r\n",buf);
//		printf("%s",d_buf);
	}
}
