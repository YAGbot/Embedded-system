#include <stdio.h>
#include <string.h>
#include "config.h"
#include "w5500.h"
#include "smtp.h"
#include "socket.h"
#include "uart.h"

char hello[50]="HELO localhost";      //��ݱ�ʶ����
char hello_reply[]="250 OK";          //��ݱ�ʶ�ɹ���Ӧ
char AUTH[50]="AUTH LOGIN";           //��֤����
char AUTH_reply[]="334 dXNlcm5hbWU6";       //��֤�����ͳɹ���Ӧ
//char name_126[100]="wiznet2013@126.com";    //126��¼������ 
char base64name_126[200];                   // 126��¼��������base64����
char name_reply[]="334 UGFzc3dvcmQ6";       //���͵�¼���ɹ���Ӧ
//char password_126[50]="hello123";           //126 ��½��������
char base64password_126[100];               //base64 126��¼��������
char password_reply[]="235 Authentication successful";      //��½�ɹ���Ӧ
//char from[]="wiznet2013@126.com";             //����������
char from_reply[]="250 Mail OK";
//char to[]="1375343860@qq.com";            //�ռ�������
char to_reply[]="250 Mail OK";
char data_init[10]="data";                //�������ݴ���
char data_reply[]="354";                  //����ɹ���Ӧ HEAD
//char Cc[]="1375343860@qq.com";         //����������
//char subject[]="������о����";           //����
//char content[]="W5500ģ������ʼ����ԣ�";           //����
char mime_reply[]="250 Mail OK queued as";//�ʼ����ͳɹ���Ӧ
char mailfrom[50]="MAIL FROM:<>";
char rcptto[50]="rcpt to:<>";
char mime[256]="From:\r\n";
char mime1[50]="To:\r\n";
char mime2[50]="Cc:\r\n";
char mime3[50]="Subject:\r\n";
char mime4[50]="MIME-Version:1.0\r\nContent-Type:text/plain\r\n\r\n";
char mime5[50]="\r\n.\r\n";



static u8 login_email[20] = {0};
static u8 pass[20] = {0};
static u8 copy_email[20] = {0};
static u8 rec_email[20] = {0};
static u8 topical[20] = {0};
static u8 txt[20] = {0};

void mailmessage(void)    //�ʼ�������Ϣ����
{

	
		sprintf((char *)login_email ,"%s","wiznet2013@126.com");/* ��¼���� */
	sprintf((char *)pass ,"%s","hello123");/* �������� */
	sprintf((char *)rec_email ,"%s", "mxy@wason.cn");/* ���������� */
	sprintf((char *)copy_email ,"%s", "mxy@zerochip-hz.com");/* ���������� */
	sprintf((char *)topical,"%s", "ZeroChip");/* �������� */
	sprintf((char *)txt ,"%s", "Hang Zhou ZeroChip!");/* �������� */ 
	
	
	u16 len_from = strlen((char *)login_email);/* ���������䳤�� */
	u16 len_to = strlen((char *)rec_email);/* �ռ������䳤�� */
	u16 len_Cc = strlen((char *)copy_email);/* ��������ĳ��� */
	u16 len_sub = strlen((char *)topical);/* ���ⳤ�� */

	sprintf(hello, "HELO localhost\r\n");
	sprintf(AUTH, "AUTH LOGIN\r\n");


	base64encode((char *)login_email, base64name_126);
	base64encode((char *)pass, base64password_126);

	strcat(base64name_126, "\r\n");
	strcat(base64password_126, "\r\n");
	
	//str_insert(mailfrom, (char *)mikro.e_mail.login_email,11);
	//strcat(mailfrom, "\r\n");
	sprintf(mailfrom, "MAIL FROM:<%s>\r\n", login_email);

	//str_insert(rcptto, (char *)mikro.e_mail.rec_email,9);
	//strcat(rcptto, "\r\n");
	sprintf(rcptto, "rcpt to:<%s>\r\n", rec_email);

	//strcat(data_init, "\r\n");
	sprintf(data_init, "data\r\n");
	
	
		/*
	smtp.126.com 25
	smtp.qq.com �˿�465��587
	*/
	//sprintf((char *)mikro.e_mail.server ,"%s", "smtp.126.com");/* �ʼ����� */
//	mikro.e_mail.port = 25;
	


#if 0
	str_insert(mime,  (char *)mikro.e_mail.login_email,5);
	str_insert(mime1, (char *)mikro.e_mail.rec_email,3);
	str_insert(mime2, (char *)mikro.e_mail.copy_email,3);
	str_insert(mime3, (char *)mikro.e_mail.topical,8);/* ���� */
	str_insert(mime5, (char *)mikro.e_mail.txt,0);/* �ı� */

	strcat(mime, mime1);
	strcat(mime, mime2);
	strcat(mime, mime3);
	strcat(mime, mime4);
	strcat(mime, mime5);
#endif

	
#if 1
	sprintf(mime, "From:%s\r\nTo:%s\r\nCc:%s\r\nSubject:%s\r\n%s%s\r\n.\r\n", 
					login_email,
					rec_email,
					copy_email,
					topical,
					mime4,
					txt);
#endif


	printf("mime:%s\r\n",mime);
}

uint8 SMTP_STATE = waitfor220;
s8 send_mail(u8 ch , char * pbuf)             
{
	switch(SMTP_STATE)
	{
		case waitfor220:
		   if(strstr((const char *)pbuf,"220")!=NULL)
			{
				send(ch,(const uint8 *)hello,strlen(hello));
				SMTP_STATE=waitforHELO250;   
			}
		   else return 2;
		break;
		case waitforHELO250:
			if(strstr((const char *)pbuf,hello_reply)!=NULL||strstr((const char *)pbuf,"transmission")==NULL)
			{
				send(ch,(const uint8 *)AUTH,strlen(AUTH));
				SMTP_STATE=waitforAUTH334;
			}
			else return 2;
		break;
		case waitforAUTH334:
			if(strstr((const char *)pbuf,AUTH_reply)!=NULL)
			{
				send(ch,(const uint8 *)base64name_126,strlen(base64name_126));
				SMTP_STATE=waitforuser334;
			}
			else return 2;
		break;
		case waitforuser334:
			if(strstr((const char *)pbuf,name_reply)!=NULL)
			{ 
				send(ch,(const uint8 *)base64password_126,strlen(base64password_126));
				SMTP_STATE=waitforpassword235;
			}
			else return 2;
		break;
		case waitforpassword235:
			if(strstr((const char *)pbuf,password_reply)!=NULL)
			{
				send(ch,(const uint8 *)mailfrom,strlen(mailfrom));
				SMTP_STATE=waitforsend250;
			}
			else return 2;
		break;
		case waitforsend250:
			if(strstr((const char *)pbuf,from_reply)!=NULL&&strstr((const char *)pbuf,"queued as")==NULL)
			{
				send(ch,(const uint8 *)rcptto,strlen(rcptto));
				SMTP_STATE=waitforrcpt250;
			}
			else return 2;			
		break;
		case waitforrcpt250:
			if(strstr((const char *)pbuf,to_reply)!=NULL)
			{
				send(ch,(const uint8 *)data_init,strlen(data_init));
				SMTP_STATE=waitfordate354;
			}
			else return 2;			
		break;
		case waitfordate354:
			if(strstr((const char *)pbuf,data_reply)!=NULL)
			{
				send(ch,(const uint8 *)mime,strlen(mime));  
				SMTP_STATE=waitformime250;
			}
			else return 2;			
		break;
		case waitformime250:
			if(strstr((const char *)pbuf,mime_reply)!=NULL)
			{
				printf("mail send OK\r\n"); 				
				return  1;
			}
			else return 2;			
		//break;
		default:
		break;
	}
	

	
	return 0;
}

void do_smtp(void)                     //SMTP ������
{
	#if 0
  uint8 ch=SOCK_SMTP;
  uint16 len;
  uint16 anyport=5000;
  uint8 Smtp_PORT=25;
  memset(RX_BUF,0,sizeof(RX_BUF));
  switch(getSn_SR(ch))
  {
    case SOCK_INIT:
      connect(ch, ConfigMsg.rip ,Smtp_PORT );
      break;
    case SOCK_ESTABLISHED:
      if(getSn_IR(ch) & Sn_IR_CON)
      {
        setSn_IR(ch, Sn_IR_CON);
      }
      if ((len = getSn_RX_RSR(ch)) > 0)		
      {
        while(!Mail_Send_OK)
        {
          memset(RX_BUF,0,sizeof(RX_BUF));
          len = recv(ch, (uint8*)RX_BUF,len);        
          send_mail();
        }
        disconnect(ch);
      }
      break;
    case SOCK_CLOSE_WAIT:   
      if ((len = getSn_RX_RSR(ch)) > 0)
      {       
        while(!Mail_Send_OK)
        {
          len = recv(ch, (uint8*)RX_BUF, len);
          send_mail();
        } 
      }
      disconnect(ch);
      break;
    case SOCK_CLOSED:                   
      socket(ch, Sn_MR_TCP,anyport++, 0x00);
      break;
    default:
    break;
  } 
  #endif
}


void base64encode(char *s, char *r)       //base64����ת������
{
  char padstr[4];
  char base64chars[]=
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  uint16 i=0,c;
  uint32 n;
  c=strlen(s)%3;
  if(c>0)   
  {
    for(i=0;c<3;c++)
    {
      padstr[i++]='=';
      
    }
  }
  padstr[i]=0;
  i=0;
  for(c=0;c<strlen(s);c+=3)
  {
    n=s[c];
    n<<=8;
    n+=s[c+1];
    if(c+2>strlen(s))
    {
      n&=0xff00;
      
    }
    n<<=8;
    n+=s[c+2];
    if(c+1>strlen(s))
    {
      n&=0xffff00;
    }
    r[i++]=base64chars[(n>>18)&63];
    r[i++]=base64chars[(n>>12)&63];
    r[i++]=base64chars[(n>>6)&63];
    r[i++]=base64chars[n&63];
  }
  i-=strlen(padstr);
  for(c=0;c<strlen(padstr);c++)
  {
    r[i++]=padstr[c];
    
  }
  r[i]=0;
}





void str_insert(char *s1,char *s2,int pos)       //���ַ���s2����s1��posλ��ʼ�ĵط�
{
  int i;
  int len=strlen(s2);
  for(i=0;i<len;i++)
  {
    *(s1+pos+len+i)=s1[pos+i];
    *(s1+pos+i)=s2[i];
    
  }
}




