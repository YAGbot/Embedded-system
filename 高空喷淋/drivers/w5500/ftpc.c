#include "ftpc.h"
#include "w5500.h"
#include "oled.h"

#define ABOR 1 //��ֹ����
#define ACCT 2 //�˺�����
#define ALLO 3//�����ļ�ǰ�ڷ������Ϸ���X���ֽ���?
#define  APPE 4 //���Ӵ�������
#define  CDUP 5 //�����Ŀ¼
#define  CWD 6 //�ı乤��Ŀ¼
#define  DELE 7//ɾ������
#define  HELP 8//��������

#define LIST 9 //�÷��������ͻ�����һ���б�
#define MKD 10//����Ŀ¼
#define MODE 11//ָ������ģʽ
#define NLIST 12//�����б�
#define NOOP 13//�޲�������

#define PASS 14 //������������û�������
#define PASV 15//��������
#define PORT 16//Ϊ������ָ������IP��ַ�ͱ��ض˿�
#define PWD 17//��ӡ��ǰ����Ŀ¼
#define QUIT 18//ע������
#define REIN 19//���³�ʼ������
#define REST 20 //������������
#define RETR 21 //�÷���������һ��
#define RMD 22 //ɾ��Ŀ¼
#define RNFR 23//��..��������RNTO����
#define RNTO 24//������Ϊ
#define SITE 25//վ���������
#define SMNT 26//�ṹװ������
#define STAT 27//״̬����
#define STOR	 28//�ļ����䵽������
#define STOU 29//��ΪΨһ�ļ�
#define STRU 30 //�������ݵĽṹ����
#define SYST 31//ϵͳ����
#define TYPE 32 //���ݵĴ��䷽ʽ
#define USER 33//�û����� ��PASS����һ��������½
#define FTP_GET 1
#define FTP_PUT 0
un_l2cval remote_ip;
uint16_t  remote_port;
un_l2cval local_ip;
//uint16_t  local_port =1267;
uint8_t connect_state_control_ftpc = 0;
uint8_t connect_state_data_ftpc = 0;
uint8_t gModeActivePassiveflag = 0;
//uint8_t FTP_destip[4] = {192,168,0,107};	// For FTP client examples; destination network info
//uint16_t FTP_destport = 1024;						// For FTP client examples; destination network info
uint8_t gMenuStart = 0;
uint8_t gDataSockReady = 0;
uint8_t gDataPutGetStart = 0;
uint8_t send_ok=0;
uint8_t count = 0;
static uint8_t gMsgBuf[20]={0,};

struct ftpc ftpc;
struct Command Command;


void ftpc_val_init(void)
{
	gMenuStart = 0;
	gModeActivePassiveflag = 0;
	connect_state_control_ftpc = 0;
	connect_state_data_ftpc = 0;
	gDataSockReady = 0;
	gDataPutGetStart = 0;
	send_ok=0;
	count = 0;	
	
}

uint8_t ftpc_run(uint8_t * dbuf, u8 * rip, u16 rport, u8 * lip, u16 lport, u8 * usr, u8 * pass)
{

	uint16_t size = 0;
	long ret = 0;
	uint32_t send_byte, recv_byte;
	uint32_t blocklen;
	uint32_t remain_filesize;
	uint32_t remain_datasize;
	uint8_t msg_c;
	uint8_t dat[50]={0,};
	uint32_t totalSize = 0, availableSize = 0;

	/*********���ݴ���*******************************/
	switch(getSn_SR(DATA_SOCK))
	{
		case SOCK_ESTABLISHED :

#if FTP_GET			
			/**********��������****************/
			if((size = getSn_RX_RSR(DATA_SOCK)) > 0)//// ����Ƿ��յ�����
			{ 
				memset(dbuf, 0, _MAX_SS);
				if(size > _MAX_SS) size = _MAX_SS - 1;
				ret = recv(DATA_SOCK,dbuf,size);
				dbuf[ret] = '\0';
				printf("Rcvd Command data: %s\r\n", dbuf);//��ӡ�յ�������
				proc_ftpc((char *)dbuf, usr, pass);
			}
#endif			
#if FTP_PUT
			/********�ϴ�����********/
			send(DATA_SOCK,"hello,zerochip\r\n",14);//�ϴ�����
			send_ok =1;
			close(DATA_SOCK);//�ر����ݴ���socket
#endif   
		break;
		case SOCK_CLOSE_WAIT :

			if((size = getSn_RX_RSR(DATA_SOCK)) > 0)
			{  
				memset(dbuf, 0, _MAX_SS);
				if(size > _MAX_SS) size = _MAX_SS - 1;
				ret = recv(DATA_SOCK,dbuf,size);
				dbuf[ret] = '\0';
				oled_add_str("FTP GET OK!");
				printf("Rcvd Command data: %s\r\n", dbuf);
				proc_ftpc((char *)dbuf, usr, pass);
			}
			disconnect(DATA_SOCK);
		break;
		case SOCK_CLOSED :
			if((size = getSn_RX_RSR(DATA_SOCK)) > 0)
			{ 
				memset(dbuf, 0, _MAX_SS);
				if(size > _MAX_SS) size = _MAX_SS - 1;
				ret = recv(DATA_SOCK,dbuf,size);
				dbuf[ret] = '\0';

				printf("Rcvd Command data: %s\r\n", dbuf);
				proc_ftpc((char *)dbuf, usr, pass);
			}
			socket(DATA_SOCK, Sn_MR_TCP,lport+1, 0x00);
		break;
		case SOCK_INIT :
			listen(DATA_SOCK);
		break;
		default :
		break;
	}
		
		
	/*****�����*********/
    switch(getSn_SR(CTRL_SOCK))
	{
		case SOCK_ESTABLISHED :
		if(!connect_state_control_ftpc)
		{
			oled_add_str("FTP CONNECTED");
			printf("%d:FTP Connected\r\n", CTRL_SOCK);
			strcpy(ftpc.workingdir, "/");		
			connect_state_control_ftpc = 1;
		}

		if(gMenuStart==1&&gModeActivePassiveflag==0 )
		{
			//���÷�����Ϊ����ģʽ
			sprintf(dat,"PORT %d,%d,%d,%d,%d,%d\r\n", lip[0], lip[1],lip[2], lip[3], (uint8_t)((lport+1)>>8), (uint8_t)((lport+1)&0x00ff));
			send(CTRL_SOCK, (uint8_t *)dat, strlen(dat));
			Command.First = f_dir;
			gMenuStart=0;
			gModeActivePassiveflag = 1;
		}
		if(gModeActivePassiveflag == 1&&count==0)
		{
			count=1;
#if FTP_GET 
			/* ��������wiznet.txt�ļ������ftpserver����Ҫ�и��ļ� */
			ftp_cmd(RETR, "./zerochip.txt", dat);
			send(CTRL_SOCK, (uint8_t *)dat, strlen(dat));
			oled_add_str("FTP GET zerochip.txt");
#endif
#if FTP_PUT						
			sprintf(dat,"STOR ./zerochip2.txt\r\n");//�ϴ�wiznet.txt�ļ������ftpserver����Ҫ�и��ļ�
			send(CTRL_SOCK, (uint8_t *)dat, strlen(dat));
#endif					
		}	
#if FTP_PUT  
		if(send_ok==1)
		{
			/* ֹͣ�ϴ� */
			ftp_cmd(ABOR, "", dat)
			send(CTRL_SOCK, (uint8_t *)dat, strlen(dat));
			send_ok=0;
		}
#endif         					 

		if((size = getSn_RX_RSR(CTRL_SOCK)) > 0)
		{ // Don't need to check SOCKERR_BUSY because it doesn't not occur.
			memset(dbuf, 0, _MAX_SS);
			if(size > _MAX_SS) size = _MAX_SS - 1;
			ret = recv(CTRL_SOCK,dbuf,size);
			dbuf[ret] = '\0';
			if(ret != size)
			{
				if(ret==0) return 0;
				if(ret < 0)
				{
					printf("%d:recv() error:%ld\r\n",CTRL_SOCK,ret);
					close(CTRL_SOCK);
					return ret;
				}
			}
			
			printf("Rcvd Command: %s\r\n", dbuf);
			proc_ftpc((char *)dbuf, usr, pass);
		}
		break;
		case SOCK_CLOSE_WAIT :
			printf("%d:CloseWait\r\n",CTRL_SOCK);
			disconnect(CTRL_SOCK);
			oled_add_str("CLOSE WAIT!");
			printf("%d:Closed\r\n",CTRL_SOCK);
		break;
		case SOCK_CLOSED :
			printf("%d:FTPStart\r\n",CTRL_SOCK);
			socket(CTRL_SOCK, Sn_MR_TCP, lport, 0x00);
		break;
		case SOCK_INIT :
			oled_add_str("CONNECTTING...");
			printf("%d:Opened\r\n",CTRL_SOCK);
			connect(CTRL_SOCK, rip, rport);
			connect_state_control_ftpc = 0;
			printf("%d:Connectting...\r\n",CTRL_SOCK);
			break;
		default:
		break;
	}

		
    return 0;
}

/***
*@brief ��������������Ӧ����
*@param: buf ftp����������Ӧ����
************************/
char proc_ftpc(char * buf, u8 * usr, u8 * pass)
{
	
	uint16_t Responses;
	uint8_t dat[30]={0,};
  memset(dat,0,30);
	Responses =(buf[0]-'0')*100+(buf[1]-'0')*10+(buf[2]-'0');

	switch(Responses){
		case R_220:	/* Service ready for new user. */
			printf("\r\nInput your User ID > ");
			sprintf(dat,"USER %s\r\n", usr);//�û���
			printf("\r\n");
			send(CTRL_SOCK, (uint8_t *)dat, strlen(dat));
			break;

		case R_331:	/* User name okay, need password. */
			printf("\r\nInput your Password > ");
			sprintf(dat,"PASS %s\r\n", pass);
			printf("\r\n");
			send(CTRL_SOCK, (uint8_t *)dat, strlen(dat));
			break;
		case R_230:	/* User logged in, proceed */
			printf("\r\nUser logged in, proceed\r\n");

			sprintf(dat,"TYPE %c\r\n", TransferAscii);
			ftpc.type = ASCII_TYPE;
			send(CTRL_SOCK, (uint8_t *)dat, strlen(dat));
			break;
		case R_200:
			if((ftpc.dsock_mode==ACTIVE_MODE)&&gModeActivePassiveflag){
				ftpc.dsock_state = DATASOCK_READY;
				gModeActivePassiveflag = 0;
			}
			else{
				gMenuStart = 1;
			}
			break;
		case R_150:
			switch(Command.First){
			case f_dir:
				Command.First = f_nocmd;
				Command.Second = s_dir;
				gDataPutGetStart = 1;
				break;
			case f_get:
				Command.First = f_nocmd;
				Command.Second = s_get;
				gDataPutGetStart = 1;
				break;
			case f_put:
				Command.First = f_nocmd;
				Command.Second = s_put;
				gDataPutGetStart = 1;
				break;
			default :
				printf("Command.First = default\r\n");
				break;
			}
			break;
		case R_226:
			gMenuStart = 1;
			break;
		case R_227:
			if (pportc(buf) == -1)
			{
				printf("Bad port syntax\r\n");
			}
			else{
				printf("Go Open Data Sock...\r\n ");
				ftpc.dsock_mode = PASSIVE_MODE;
				ftpc.dsock_state = DATASOCK_READY;
			}
			break;
		default:
			printf("\r\nDefault Status = %d\r\n",(uint16_t)Responses);
			gDataSockReady = 1;
			break;
		}
	return 1;
}
int pportc(char * arg)
{
	int i;
	char* tok=0;
	strtok(arg,"(");
	for (i = 0; i < 4; i++)
	{
		if(i==0) tok = strtok(NULL,",\r\n");
		else	 tok = strtok(NULL,",");
		remote_ip.cVal[i] = (uint8_t)atoi(tok, 10);
		if (!tok){
			printf("bad pport : %s\r\n", arg);
			return -1;
		}
	}
	remote_port = 0;
	for (i = 0; i < 2; i++){
		tok = strtok(NULL,",\r\n");
		remote_port <<= 8;
		remote_port += atoi(tok, 10);
		if (!tok){
			printf("bad pport : %s\r\n", arg);
			return -1;
		}
	}
	printf("ip : %d.%d.%d.%d, port : %d\r\n", remote_ip.cVal[0], remote_ip.cVal[1], remote_ip.cVal[2], remote_ip.cVal[3], remote_port);
	return 0;
}
uint8_t* User_Keyboard_MSG()
{
	uint8_t i=0;
	do{
		gMsgBuf[i] = 1;
		i++;
	}while(gMsgBuf[i-1]!=0x0d);
	gMsgBuf[i-1]=0;
	return gMsgBuf;
}


/**
*@brief:ftpcommand  ƴ��ftp���������
*@param:cmd FTP ����
*@para:sbuf ��������
*@param:dbuf Ŀ��buffer
*******************************/

void ftp_cmd(uint8_t cmd,char* sbuf,char*dbuf)
{
	switch (cmd)
	{	
		case ABOR://��ֹ����
			sprintf(dbuf,"ABOR\r\n");
			break;
		case ACCT://ָ���û����˺���Ϣ ֻ����PASS������յ�332�������:
			sprintf(dbuf,"ACCT %s\r\n",sbuf);
			break;
		case ALLO: //�����ļ�ǰ�ڷ���������X���ֽ�
			sprintf(dbuf,"ALLO %s\r\n",sbuf);
		  break;
		case APPE://�÷�����׼�������ļ��������ݸ��ӵ�ָ�����ļ�
			sprintf(dbuf,"APPE %s\r\n",sbuf);
		  break;
		case CDUP://�ѵ�ǰĿ¼��Ϊ���ļ�Ŀ¼
			sprintf(dbuf,"CDUP \r\n");
			break;
		case CWD://�ı乤��Ŀ¼��ָ��·��
			sprintf(dbuf,"CWD %s\r\n",sbuf);
		  break;
		case DELE://ɾ����������ָ�����ļ�
			sprintf(dbuf,"DELE %s\r\n",sbuf);
		  break;
		case HELP://��������
			sprintf(dbuf,"HELP %s\r\n",sbuf);
		  break;
		case LIST://�б�������������͸��ͻ���
			sprintf(dbuf,"LIST %s\r\n",sbuf);
		  break;
		case MKD://����Ŀ¼
			sprintf(dbuf,"MKD %s\r\n",sbuf);
		   break;
		case MODE://����ģʽ���� S , B, C
			 sprintf(dbuf,"MODE %s\r\n",sbuf);
		   break;
		case NLIST://�����������ļ������б�
			 sprintf(dbuf,"NLISTT %s\r\n",sbuf);
		   break;
		case NOOP://ʲô������
			 sprintf(dbuf,"NOOP\r\n");
		   break;
		case PASS://��������
			  sprintf(dbuf,"PASS %s\r\n",sbuf);
		   break;
		case PASV://����������ģʽ
			  sprintf(dbuf,"PASV %s\r\n",sbuf);
		   break;
		case PORT://����������ģʽ
			  sprintf(dbuf,"PORT %s\r\n",sbuf);
		   break;
		case PWD://��ӡ����Ŀ¼
			  sprintf(dbuf,"PWD\r\n");
		   break;
		case QUIT://ע������
			  sprintf(dbuf,"QUIT\r\n");
		   break;
		case REIN://���³�ʼ������
			  sprintf(dbuf,"REIN\r\n");
		   break;
		case REST://��������
			  sprintf(dbuf,"REST %s\r\n",sbuf);
		   break;
		case RETR://��������
		  sprintf(dbuf,"RETR %s\r\n",sbuf);
			break;
	   case RMD://ɾ��Ŀ¼
			  sprintf(dbuf,"RMD %s\r\n",sbuf);
		   break;
		 case RNFR://���ļ�������
			  sprintf(dbuf,"RNFR %s\r\n",sbuf);
		   break;
		 case RNTO://���ļ�������Ϊ
			  sprintf(dbuf,"RNTO %s\r\n",sbuf);
		   break;
		 case SITE://վ���������
			  sprintf(dbuf,"SITE %s\r\n",sbuf);
		   break;
		 case SMNT://�ṹ����
			  sprintf(dbuf,"SMNT %s\r\n",sbuf);
		   break;
		 case STAT://״̬����
			  sprintf(dbuf,"STAT %s\r\n",sbuf);
		   break;
		 case STRU://ָ���������ݵĽṹ���� F �ļ��ṹ R��¼�ṹ P ҳ�ṹ
			  sprintf(dbuf,"STRU %s\r\n",sbuf);
		   break;
		 case SYST://����������ϵͳ����
			  sprintf(dbuf,"SYST\r\n");
		   break;
		 case TYPE://���ݴ��䷽ʽ
			  sprintf(dbuf,"TYPE %s\r\n",sbuf);
		   break;
	   case USER://��½�������û�����
			  sprintf(dbuf,"USER %s\r\n",sbuf);
		   break;
		default:
			break;
	}
}



