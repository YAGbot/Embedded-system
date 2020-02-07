#include "ftpc.h"
#include "w5500.h"
#include "oled.h"

#define ABOR 1 //中止命令
#define ACCT 2 //账号命令
#define ALLO 3//发送文件前在服务器上分配X个字节命?
#define  APPE 4 //附加创建命令
#define  CDUP 5 //进入根目录
#define  CWD 6 //改变工作目录
#define  DELE 7//删除命令
#define  HELP 8//帮助命令

#define LIST 9 //让服务器给客户发送一份列表
#define MKD 10//创建目录
#define MODE 11//指定传输模式
#define NLIST 12//名称列表
#define NOOP 13//无操作命令

#define PASS 14 //向服务器发送用户的密码
#define PASV 15//被动命令
#define PORT 16//为服务器指定本地IP地址和本地端口
#define PWD 17//打印当前工作目录
#define QUIT 18//注销命令
#define REIN 19//重新初始化命令
#define REST 20 //重新启动命令
#define RETR 21 //让服务器发送一份
#define RMD 22 //删除目录
#define RNFR 23//把..重命名与RNTO连用
#define RNTO 24//重命名为
#define SITE 25//站点参数命令
#define SMNT 26//结构装配命令
#define STAT 27//状态命令
#define STOR	 28//文件传输到服务器
#define STOU 29//存为唯一文件
#define STRU 30 //传达数据的结构类型
#define SYST 31//系统命令
#define TYPE 32 //数据的传输方式
#define USER 33//用户名称 与PASS命令一起用来登陆
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

	/*********数据传输*******************************/
	switch(getSn_SR(DATA_SOCK))
	{
		case SOCK_ESTABLISHED :

#if FTP_GET			
			/**********下载数据****************/
			if((size = getSn_RX_RSR(DATA_SOCK)) > 0)//// 检测是否收到数据
			{ 
				memset(dbuf, 0, _MAX_SS);
				if(size > _MAX_SS) size = _MAX_SS - 1;
				ret = recv(DATA_SOCK,dbuf,size);
				dbuf[ret] = '\0';
				printf("Rcvd Command data: %s\r\n", dbuf);//打印收到的数据
				proc_ftpc((char *)dbuf, usr, pass);
			}
#endif			
#if FTP_PUT
			/********上传数据********/
			send(DATA_SOCK,"hello,zerochip\r\n",14);//上传数据
			send_ok =1;
			close(DATA_SOCK);//关闭数据传输socket
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
		
		
	/*****命令传输*********/
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
			//设置服务器为主动模式
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
			/* 发送下载wiznet.txt文件的命令，ftpserver中需要有该文件 */
			ftp_cmd(RETR, "./zerochip.txt", dat);
			send(CTRL_SOCK, (uint8_t *)dat, strlen(dat));
			oled_add_str("FTP GET zerochip.txt");
#endif
#if FTP_PUT						
			sprintf(dat,"STOR ./zerochip2.txt\r\n");//上传wiznet.txt文件的命令，ftpserver中需要有该文件
			send(CTRL_SOCK, (uint8_t *)dat, strlen(dat));
#endif					
		}	
#if FTP_PUT  
		if(send_ok==1)
		{
			/* 停止上传 */
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
*@brief 解析服务器的响应数据
*@param: buf ftp服务器的响应数据
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
			sprintf(dat,"USER %s\r\n", usr);//用户名
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
*@brief:ftpcommand  拼接ftp所需的命令
*@param:cmd FTP 命令
*@para:sbuf 命令内容
*@param:dbuf 目标buffer
*******************************/

void ftp_cmd(uint8_t cmd,char* sbuf,char*dbuf)
{
	switch (cmd)
	{	
		case ABOR://中止命令
			sprintf(dbuf,"ABOR\r\n");
			break;
		case ACCT://指定用户的账号信息 只能在PASS命令后收到332代码后发送:
			sprintf(dbuf,"ACCT %s\r\n",sbuf);
			break;
		case ALLO: //发送文件前在服务器分配X个字节
			sprintf(dbuf,"ALLO %s\r\n",sbuf);
		  break;
		case APPE://让服务器准备接收文件并将数据附加到指定的文件
			sprintf(dbuf,"APPE %s\r\n",sbuf);
		  break;
		case CDUP://把当前目录改为根文件目录
			sprintf(dbuf,"CDUP \r\n");
			break;
		case CWD://改变工作目录到指定路径
			sprintf(dbuf,"CWD %s\r\n",sbuf);
		  break;
		case DELE://删除服务器上指定的文件
			sprintf(dbuf,"DELE %s\r\n",sbuf);
		  break;
		case HELP://帮助命令
			sprintf(dbuf,"HELP %s\r\n",sbuf);
		  break;
		case LIST://列表命令服务器发送给客户端
			sprintf(dbuf,"LIST %s\r\n",sbuf);
		  break;
		case MKD://创建目录
			sprintf(dbuf,"MKD %s\r\n",sbuf);
		   break;
		case MODE://传输模式命令 S , B, C
			 sprintf(dbuf,"MODE %s\r\n",sbuf);
		   break;
		case NLIST://服务器发送文件名称列表
			 sprintf(dbuf,"NLISTT %s\r\n",sbuf);
		   break;
		case NOOP://什么都不做
			 sprintf(dbuf,"NOOP\r\n");
		   break;
		case PASS://密码命令
			  sprintf(dbuf,"PASS %s\r\n",sbuf);
		   break;
		case PASV://服务器被动模式
			  sprintf(dbuf,"PASV %s\r\n",sbuf);
		   break;
		case PORT://服务器主动模式
			  sprintf(dbuf,"PORT %s\r\n",sbuf);
		   break;
		case PWD://打印工作目录
			  sprintf(dbuf,"PWD\r\n");
		   break;
		case QUIT://注销命令
			  sprintf(dbuf,"QUIT\r\n");
		   break;
		case REIN://重新初始化命令
			  sprintf(dbuf,"REIN\r\n");
		   break;
		case REST://重启命令
			  sprintf(dbuf,"REST %s\r\n",sbuf);
		   break;
		case RETR://下载命令
		  sprintf(dbuf,"RETR %s\r\n",sbuf);
			break;
	   case RMD://删除目录
			  sprintf(dbuf,"RMD %s\r\n",sbuf);
		   break;
		 case RNFR://把文件重命名
			  sprintf(dbuf,"RNFR %s\r\n",sbuf);
		   break;
		 case RNTO://把文件重命名为
			  sprintf(dbuf,"RNTO %s\r\n",sbuf);
		   break;
		 case SITE://站点参数命令
			  sprintf(dbuf,"SITE %s\r\n",sbuf);
		   break;
		 case SMNT://结构挂载
			  sprintf(dbuf,"SMNT %s\r\n",sbuf);
		   break;
		 case STAT://状态命令
			  sprintf(dbuf,"STAT %s\r\n",sbuf);
		   break;
		 case STRU://指定传输数据的结构类型 F 文件结构 R记录结构 P 页结构
			  sprintf(dbuf,"STRU %s\r\n",sbuf);
		   break;
		 case SYST://服务器操作系统类型
			  sprintf(dbuf,"SYST\r\n");
		   break;
		 case TYPE://数据传输方式
			  sprintf(dbuf,"TYPE %s\r\n",sbuf);
		   break;
	   case USER://登陆服务器用户名称
			  sprintf(dbuf,"USER %s\r\n",sbuf);
		   break;
		default:
			break;
	}
}



