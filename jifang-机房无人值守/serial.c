#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <termios.h>
#include <limits.h>
#include <pthread.h>
#include "libemqtt.h"
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include "cJSON.h"
#include <linux/watchdog.h>
#include "gps.h"
#define WDT "/dev/watchdog"
//mqtt
unsigned int tt,yy;
int socket_id;
mqtt_broker_handle_t broker;
int keepalive = 36;
#define RCVBUFSIZE 1024
uint8_t packet_buffer[RCVBUFSIZE];
char End_Data[2000];
char str1[2000];
float a,b,c,d,e;
char ip[20];
//‰∏≤Âè£ÂèÇÊï∞ÈÉ®ÂàÜ
#define DEV_NAME "/dev/ttySP3"
#define DEV_NAME1 "/dev/ttySP0"

#define BEEP_FILE_NAME  "/sys/class/leds/beep/brightness"
#define EXPORT_PATH  "/sys/class/gpio/export"
#define GPIO3 "88"//2-24
#define DEV_PATH3    "/sys/class/gpio/gpio88/value"
#define DIRECT_PATH3 "/sys/class/gpio/gpio88/direction"

#define GPIO4 "89"//2-25
#define DEV_PATH4    "/sys/class/gpio/gpio89/value"
#define DIRECT_PATH4 "/sys/class/gpio/gpio89/direction"

int fd_export,fd_export1;
int fd_dir3,fd_dir4;
int fd_dev3,fd_dev4;
char buf;
unsigned char frequence;


int iFd;
int iFd2;
unsigned char Flag_temp=1,Flag_smok,Flag_water,Flag_Nb,Flag_temp1,Flag_smok1,Flag_water1;
unsigned char Sendok_flag=1,add_flag=0,Network_flag,infor_type=1,Alarm=0,smokBeep=0,waterBeep=0,tempBeep=0,humiBeep=0,IO1Beep=0,IO2Beep=0;		//…œ¥Œ∑¢ÀÕ«Èøˆ£¨Õ¯¬Á◊¥Ã¨«Èøˆ£¨–≈œ¢¿‡–Õ£¨±®æØ◊¥Ã¨
unsigned char temrecover=0,humrecover=0,smokrecover=0,waterrecover=0,tem1recover=0,hum1recover=0,smok1recover=0,water1recover=0;
unsigned char Temp_str[100],Smok_str[50],Water_str[50],Power[50],Temp_str1[100],Smok_str1[50],Water_str1[50],Power1[50];
char *clr="CLR(5)\r\n";
//char *sensorerror = "CLR(5);DCV24(0,10,'¥´∏–∆˜π ’œ:!',11)\r\n";
//char *neterror = "CLR(5);DCV24(0,10,'Õ¯¬Áπ ’œ!',11)\r\n";	
const char *cmd1="SBC(5);DCV24(0,10,'Œ¬∂»1:',11);DCV24(0,40,' ™∂»1:',11);DCV16(15,70,'—Ã∏–1:',11);DCV16(15,90,'ÀÆΩ˛1:',11);DCV16(15,110,' –µÁ1:',11)";
const char *cmd2="SBC(5);DCV24(0,10,'Œ¬∂»2:',11);DCV24(0,40,' ™∂»2:',11);DCV16(15,70,'—Ã∏–2:',11);DCV16(15,90,'ÀÆΩ˛2:',11);DCV16(15,110,' –µÁ2:',11)";
char display_buf[500],smok_buf[50],temp_buf[50],water_buf[50],power_buf[50],smok_buf1[50],temp_buf1[50],water_buf1[50],power_buf1[50];
//unsigned char buf1[8]={0x11,0x12,0x13,0x14,0x15,0x16,0x70,0x87};
unsigned char flag1,flag2,flag3,flag4,flag5,flag6;
unsigned char templevelH,humilevelH;
unsigned short smoklevel;
unsigned char templevelL=0;
unsigned char humilevelL=0;
char topic[20],topic1[20],mn[20];
int file_size2(char * filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;
    return size;
}
char CheckSum(unsigned char *ptr,unsigned char len)
{
 unsigned char i,end;
 unsigned int sumsum;
 if(len==18)//ÂõõÊ∞îÊ†°È™å
 {
	 for(i=5;i<16;i++)
	 {
		 sumsum+=ptr[i];
	 }
	 end=sumsum%256;
	 return end;
 }
 if(len==10)//‰∏§Â∞òÊ†°È™å
 {
	 for(i=2;i<8;i++)
	 {
		 sumsum+=ptr[i];
	 }
	 end=sumsum%256;
	 return end;
 }

}
unsigned char getCRC16(unsigned char *ptr,unsigned char len)
{
    int i;
    unsigned int crc = 0xFFFF;
    while(len--)
    {
        crc ^= *ptr++;
        for(i=0; i<8; i++)
    	{
            if(crc&1)
       	{
                crc >>= 1;
                crc ^= 0xA001;
        	}
       	else
   		{
                crc >>= 1;
        	}
        }

    }
   return(crc);
}


int send_packet(void* socket_info, const void* buf, unsigned int count)
{
	int fd = *((int*)socket_info);
	return send(fd, buf, count, 0);
}
int init_socket(mqtt_broker_handle_t* broker, const char* hostname, short port)
{
	int flag = 1;
	// Create the socket
	if((socket_id = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	// Disable Nagle Algorithm
	if (setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
		return -2;
	struct sockaddr_in socket_address;
	// Create the stuff we need to connect
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(port);
	socket_address.sin_addr.s_addr = inet_addr(hostname);

	// Connect the socket
	if((connect(socket_id, (struct sockaddr*)&socket_address, sizeof(socket_address))) < 0)
		return -1;

	// MQTT stuffs
	mqtt_set_alive(broker, keepalive);
	broker->socket_info = (void*)&socket_id;
	broker->send = send_packet;

	return 0;
}
int close_socket(mqtt_broker_handle_t* broker)
{
	int fd = *((int*)broker->socket_info);
	return close(fd);
}
int read_packet(int timeout)
{
	if(timeout > 0)
	{
		fd_set readfds;
		struct timeval tmv;

		// Initialize the file descriptor set
		FD_ZERO (&readfds);
		FD_SET (socket_id, &readfds);

		// Initialize the timeout data structure
		tmv.tv_sec = timeout;
		tmv.tv_usec = 0;

		// select returns 0 if timeout, 1 if input available, -1 if error
		if(select(1, &readfds, NULL, NULL, &tmv))
			return -2;
	}

	int total_bytes = 0, bytes_rcvd, packet_length;
	memset(packet_buffer, 0, sizeof(packet_buffer));

	while(total_bytes < 2) // Reading fixed header
	{
		if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes
	}

	packet_length = packet_buffer[1] + 2; // Remaining length + fixed header length

	while(total_bytes < packet_length) // Reading the packet
	{
		if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes
	}

	return packet_length;
}


void  *threadWork(void *threadid)
{

	FILE *fq1=NULL;
	FILE *fq=NULL;
	//char dv[]="/opt/setting.txt";
	char dv[]="/opt/setting.txt";
	char buf_rc[100];
	char *des1,*des2,*des3,*des4;
	unsigned lens1,lens2,lens3;
	char buf_rec1[150];
	#define RCVBUFSIZE 1024
	uint8_t packet_buffer[RCVBUFSIZE];
	int keepalive = 60;
	mqtt_broker_handle_t broker;
	int socket_id;
	unsigned char value_len;
	int i;
	int send_packet(void* socket_info, const void* buf, unsigned int count)
	{
		int fd = *((int*)socket_info);
		return send(fd, buf, count, 0);
	}

	int init_socket(mqtt_broker_handle_t* broker, const char* hostname, short port, int keepalive)
	{
		int flag = 1;
		// Create the socket
		if((socket_id = socket(PF_INET, SOCK_STREAM, 0)) < 0)
			return -1;
		// Disable Nagle Algorithm
		if (setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
			return -2;

		struct sockaddr_in socket_address;
		// Create the stuff we need to connect
		socket_address.sin_family = AF_INET;
		socket_address.sin_port = htons(port);
		socket_address.sin_addr.s_addr = inet_addr(hostname);

		// Connect the socket
		if((connect(socket_id, (struct sockaddr*)&socket_address, sizeof(socket_address))) < 0)
			return -1;

		// MQTT stuffs
		mqtt_set_alive(broker, keepalive);
		broker->socket_info = (void*)&socket_id;
		broker->send = send_packet;

		return 0;
	}

	int close_socket(mqtt_broker_handle_t* broker)
	{
		int fd = *((int*)broker->socket_info);
		return close(fd);
	}

	int read_packet(int timeout)
	{
		if(timeout > 0)
		{
			fd_set readfds;
			struct timeval tmv;

			// Initialize the file descriptor set
			FD_ZERO (&readfds);
			FD_SET (socket_id, &readfds);

			// Initialize the timeout data structure
			tmv.tv_sec = timeout;
			tmv.tv_usec = 0;

			// select returns 0 if timeout, 1 if input available, -1 if error
			if(select(1, &readfds, NULL, NULL, &tmv))
				return -2;
		}
		int total_bytes = 0, bytes_rcvd, packet_length;
		memset(packet_buffer, 0, sizeof(packet_buffer));
		if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0) {
			return -1;
		}
		total_bytes += bytes_rcvd; // Keep tally of total bytes
		if (total_bytes < 2)
			return -1;
		// now we have the full fixed header in packet_buffer
		// parse it for remaining length and number of bytes
		uint16_t rem_len = mqtt_parse_rem_len(packet_buffer);
		uint8_t rem_len_bytes = mqtt_num_rem_len_bytes(packet_buffer);

		//packet_length = packet_buffer[1] + 2; // Remaining length + fixed header length
		// total packet length = remaining length + byte 1 of fixed header + remaning length part of fixed header
		packet_length = rem_len + rem_len_bytes + 1;

		while(total_bytes < packet_length) // Reading the packet
		{
			if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0)
				return -1;
			total_bytes += bytes_rcvd; // Keep tally of total bytes
		}
		return packet_length;
	}
	void alive(int sig)
	{
		printf("Timeout! Sending ping...\n");
		mqtt_ping(&broker);
		alarm(keepalive);
	}
	void term(int sig)
	{
		printf("Goodbye!\n");
		// >>>>> DISCONNECT
		mqtt_disconnect(&broker);
		close_socket(&broker);
		exit(0);
	}
	char *key1;
	char *value1;
	char *ID;
	char *Allupd;
	int packet_length;
	uint16_t msg_id, msg_id_rcv;
	strcat(topic1,topic);
	strcat(topic1,"_R");
	printf("......................................\r\n");
	printf("topic1:%s\r\n",topic1);
	    //////////////////////////////////////
	//mqtt_init(&broker, "Epc287_JF2_R");
	mqtt_init(&broker, topic1);
	//mqtt_init_auth(&broker, "admin", "password");
	mqtt_init_auth(&broker, "thhy", "thhy550550");
	init_socket(&broker, ip, 61613, keepalive);

	// >>>>> CONNECT
	mqtt_connect(&broker);
	// <<<<< CONNACK
	packet_length = read_packet(1);
	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) on read packet!\n", packet_length);
		return -1;
	}

	if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_CONNACK)
	{//"Temp:%0.1f;Humi:%0.1f;"
		fprintf(stderr, "CONNACK expected!\n");
		return -2;
	}

	if(packet_buffer[3] != 0x00)
	{
		fprintf(stderr, "CONNACK failed!\n");
		return -2;
	}

	// Signals after connect MQTT
	signal(SIGALRM, alive);
	alarm(keepalive);
	signal(SIGINT, term);

	// >>>>> SUBSCRIBE
	//mqtt_subscribe(&broker, topic1, &msg_id);//FP_Epc287_Demo8
	mqtt_subscribe(&broker, "THHYCLOUD", &msg_id);//∂©‘ƒÕ¨“ª÷˜Ã‚
	// <<<<< SUBACK
	packet_length = read_packet(1);
	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) on read packet!\n", packet_length);
		return -1;
	}

	if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_SUBACK)
	{
		fprintf(stderr, "SUBACK expected!\n");
		return -2;
	}

	msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
	if(msg_id != msg_id_rcv)
	{
		fprintf(stderr, "%d message id was expected, but %d message id was found!\n", msg_id, msg_id_rcv);
		return -3;
	}
	while(1)
	{
		packet_length = read_packet(0);
		if(packet_length == -1)
		{
			fprintf(stderr, "Error(%d) on read packet!\n", packet_length);
			//return -1;
		}
		else if(packet_length > 0)
		{
			printf("Packet Header: 0x%x...\n", packet_buffer[0]);
			if(MQTTParseMessageType(packet_buffer) == MQTT_MSG_PUBLISH)
			{
				uint8_t topic[255] ;
				uint16_t len,msg[1024];
				len = mqtt_parse_pub_topic(packet_buffer, topic);
				topic[len] = '\0'; // for printf
				len = mqtt_parse_publish_msg(packet_buffer, msg);
				msg[len] = '\0'; // for printf
				printf("%s %s\n", topic, msg);
				cJSON *root=cJSON_Parse(msg);
				memset(msg,0,1024);
				if(root!=NULL)
				{
					    ID=cJSON_GetObjectItem(root,"MNID")->valuestring;
						printf("MNID=%s\r\n",ID);
					    key1=cJSON_GetObjectItem(root,"equipment")->valuestring;
					    printf("equipment=%s\r\n",key1);
						value1=cJSON_GetObjectItem(root,"value")->valuestring;
						printf("value=%s\r\n",value1);
					   Allupd=cJSON_GetObjectItem(root,"update")->valuestring;
					   	printf("update=%s\r\n",Allupd);
					char *temperaturemax=cJSON_GetObjectItem(root,"temperaturemax")->valuestring;
					templevelH=atoi(temperaturemax);
					printf("temperaturemax=%s,%d\r\n",temperaturemax,templevelH);
					char *humiditymax=cJSON_GetObjectItem(root,"humiditymax")->valuestring;
					humilevelH=atoi(humiditymax);
					printf("humiditymax=%s,%d\r\n",humiditymax,humilevelH);
						printf("..........................\r\n");

					if(!strcmp(Allupd,"yes"))
					{
						//system("wget -O /opt/dir/jifang http://61.181.255.77/getAPP/jifang");
						system("wget -O /opt/dir/jifang http://zyb.zhangyibo.cn/opt/jifang");
						sleep(5);
						system("cp /opt/dir/jifang /opt");
						sleep(2);
						system("wr chmod 777 /opt/jifang");
						sleep(1);
						system("rm /opt/dir/jifang");
						sleep(2);
						system("reboot");
					}
					if(!strcmp(ID,topic1))
					{
						if(!strcmp(key1,"freq"))
						{
							printf("-++++++++++++++++++++++.\r\n");
							 value_len=strlen(value1);	//¥À¥¶∂ºŒ™4ŒªπÃø…Õ≥“ª
							 fq1=fopen(dv,"r");
							 fread(buf_rec1,1,sizeof(buf_rec1),fq1);
							 fclose(fq1);
							 des1=strstr(buf_rec1,"y=");
							 des2=strstr(buf_rec1,"ip");
							 des3=strstr(buf_rec1,"temp=");
							 des4=strstr(buf_rec1,"humi=");
							 lens1=des1-des2+2;
							 lens2=des3-des2+5;
							 lens3=des4-des2+5;
							 printf("lens:%d\n",lens1);
							 printf("lens:%d\n",lens2);
							 printf("lens:%d\n",lens3);
							 
							 for(i=0;i<4;i++)
							 {
								 buf_rec1[lens1+i]='0';
								 buf_rec1[lens2+i]='0';
								 buf_rec1[lens3+i]='0';
							 }
							 for(i=0;i<value_len;i++)
							 {
								 buf_rec1[lens1+3-i]=value1[value_len-i-1];
								 buf_rec1[lens2+3-i]=temperaturemax[value_len-i-1];
								 buf_rec1[lens3+3-i]=humiditymax[value_len-i-1];
							 }
							 fq1=fopen(dv,"w");
							 fwrite(buf_rec1,1,sizeof(buf_rec1),fq1);
							 printf("buf_rc1:%s\n",buf_rec1);
							 frequence=(buf_rec1[lens1]-48)*1000+(buf_rec1[lens1+1]-48)*100+(buf_rec1[lens1+2]-48)*10+buf_rec1[lens1+3]-48;
							 tt=0;
							 printf("frequence:%d\r\n",frequence);
							 fclose(fq1);
						}
						if(!strcmp(value1,"reboot"))
						{
							printf("value:%s\r\n",value1);
							system("reboot");
						}
						if(!strcmp(value1,"download"))
						{
							//system("wget -O /opt/dir/jifang http://61.181.255.77/getAPP/jifang");
							system("wget -O /opt/dir/jifang http://zyb.zhangyibo.cn/opt/jifang");
							sleep(5);
							system("cp /opt/dir/jifang /opt");
							sleep(2);
							system("wr chmod 777 /opt/jifang");
							sleep(1);
							system("rm /opt/dir/jifang");
							sleep(2);
							system("reboot");
						}
					}

		       }

	      }
		}
  }
}


unsigned char bcd_decimal(unsigned char bcd)
{
	return bcd-(bcd>>4)*6;
}

void *display(void *threadid)
{
	struct termios opt;
	unsigned char t=60;
	
	  iFd2 = open(DEV_NAME1, O_RDWR | O_NOCTTY);
		if(iFd2 < 0) {
	            perror(DEV_NAME1);
		}
	    tcgetattr(iFd2, &opt);
	    if (tcgetattr(iFd2,   &opt)<0) {
	             return   -1;
	    }
	    opt.c_lflag	&= ~(ECHO | ICANON | IEXTEN | ISIG);
	    opt.c_iflag	&= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	    opt.c_oflag 	&= ~(OPOST);
	    opt.c_cflag 	&= ~(CSIZE | PARENB);
	    opt.c_cflag 	|=  CS8;

	    opt.c_cc[VMIN] 	= 100;
	    opt.c_cc[VTIME]	= 1;

	    cfsetispeed(&opt, B9600);
	    cfsetospeed(&opt, B9600);
		

	    if (tcsetattr(iFd2,   TCSANOW,   &opt)<0) {
	        	return   -1;
	    }
	    tcflush(iFd2,TCIOFLUSH);
		while(t)
			{
				sleep(1);
				t--;
			}
	    write(iFd2, clr, strlen(clr));
		sleep(1);
	   while(1)
	   	{
			sprintf((char*)display_buf,"%s;%s;%s;%s;%s\r\n",cmd1,temp_buf,smok_buf,water_buf,power_buf);
			//printf("display_buf1=%s\r\n",display_buf);
			write(iFd2, display_buf, strlen(display_buf));
			sleep(5);
			sprintf((char*)display_buf,"%s;%s;%s;%s;%s\r\n",cmd2,temp_buf1,smok_buf1,water_buf1,power_buf1);
			//printf("display_buf2=%s\r\n",display_buf);
			write(iFd2, display_buf, strlen(display_buf));
			sleep(5);
	   	}
}

void *beep_warning(void *threadid)
{
	int fd;
	int ret = 0;
	fd = open("/sys/class/leds/led-run/trigger", O_RDWR);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	ret = write(fd, "none", 4);
	if(ret < 0) {
		perror("write");
		return -1;
	}
	
	fd=open(BEEP_FILE_NAME, O_RDWR);
	if (fd < 0) {
         perror(BEEP_FILE_NAME);
		 return -1;
    	}
	
	//ret = write(fd, "0", 1);
	while(1)
		{
			//printf("beep=%d%d%d%d%d%d",smokBeep,waterBeep,tempBeep,humiBeep,IO1Beep,IO2Beep);
			if(smokBeep|waterBeep|tempBeep|humiBeep|IO1Beep|IO2Beep)
			{
				//printf("BEEPWARNING\r\n");
				//printf("beep=%d%d%d%d%d%d",smokBeep,waterBeep,tempBeep,humiBeep,IO1Beep,IO2Beep);
				ret = write(fd, "1", 1);
				sleep(1);
				ret = write(fd, "0", 1);
			}
			sleep(1);
		}
}

void *serial(void *threadid)
{
    unsigned char ucBuf[100];
    unsigned char crc1,crc2,f;
    unsigned char p1,p2,p3;
    unsigned int p;
    float t,t1,q;
    int len,i;
    //unsigned char buf1[2]={0x2F,0x03};
    unsigned short Sp,Di,Temp,Humi;
    while(1)
  {
     len = read(iFd, ucBuf,100);
     if(len<0)
     {
		printf("read error!");
		continue;
     }
//     if(len==18)
//     {
//		 crc=CheckSum(ucBuf,18);
//		 if((crc==ucBuf[16])&&(ucBuf[0]==0xFF)&&(ucBuf[17]==0xDD))
//		 {
//			 if(ucBuf[3]==0x07)
//			 {
//				 p1=bcd_decimal(ucBuf[10]);
//				 p2=bcd_decimal(ucBuf[11]);
//				 p3=bcd_decimal(ucBuf[12]);
//				 p=p1*100+p2;
//				 t=p+p3*0.01;
//				 t=28/22.4*t;//ÂéüÂßãÊµìÂ∫¶Ôºàmg/m3Ôºâ
//				 t=a*t*t*t*t+b*t*t*t+c*t*t+d*t+e;
//				 sprintf((char*)Co_str,"a21005-Rtd=%0.1f,a21005-Flag=N;",t);
//				// printf("Co_str:%s\r\n",Co_str);
//				 Flag_Co=0;
//				 Flag_So2=1;
//				 Flag_Nb=0;
//			 }
//			 if(ucBuf[3]==0x08)
//			 {
//				 p1=bcd_decimal(ucBuf[10]);
//				 p2=bcd_decimal(ucBuf[11]);
//				 p3=bcd_decimal(ucBuf[12]);
//				 p=p1*100+p2*0.1;
//				 t=p+p3*0.001;
//				 t=64/22.4*t*1000;
//				 t=a*t*t*t*t+b*t*t*t+c*t*t+d*t+e;
//				 sprintf((char*)So2_str,"a21026-Rtd=%0.1f,a21026-Flag=N;",t);
//				// printf("So2_str:%s\r\n",So2_str);
//				 Flag_So2=0;
//				 Flag_O3=1;
//				 Flag_Nb=0;
//			 }
//			 if(ucBuf[3]==0x09)
//			 {
//				 p1=bcd_decimal(ucBuf[10]);
//				 p2=bcd_decimal(ucBuf[11]);
//				 p3=bcd_decimal(ucBuf[12]);
//				 p=p1*100+p2*0.1;
//				 t=p+p3*0.001;
//				 t=48/22.4*t*1000;
//				 t=a*t*t*t*t+b*t*t*t+c*t*t+d*t+e;
//				 sprintf((char*)O3_str,"a05024-Rtd=%0.1f,a05024-Flag=N;",t);
//				// printf("O3_str:%s\r\n",O3_str);
//				 Flag_O3=0;
//				 Flag_No2=1;
//				 Flag_Nb=0;
//			 }
//			 if(ucBuf[3]==0x0A)
//			 {
//				 p1=bcd_decimal(ucBuf[10]);
//				 p2=bcd_decimal(ucBuf[11]);
//				 p3=bcd_decimal(ucBuf[12]);
//				 p=p1*100+p2*0.1;
//				 t=p+p3*0.001;
//				 t=46/22.4*t*1000;
//				 t=a*t*t*t*t+b*t*t*t+c*t*t+d*t+e;
//				 sprintf((char*)No2_str,"a21004-Rtd=%0.1f,a21004-Flag=N;",t);
//				// printf("No2_str:%s\r\n",No2_str);
//				 Flag_No2=0;
//				 Flag_Pm=1;
//				 Flag_Nb=0;
//			 }
//		 }
//     }
//     if(len==10)
//         {
//        	 crc=CheckSum(ucBuf,10);
//        	 if((crc==ucBuf[8])&&(ucBuf[0]==0xAA)&&(ucBuf[9]==0xAB))
//        	 {
//    			 pm25=*(unsigned short*)(&ucBuf[2]);
//    			 pm10=*(unsigned short*)(&ucBuf[4]);
//    			 t=pm25*0.1;
//    			 t=a*t*t*t*t+b*t*t*t+c*t*t+d*t+e;
//    			 q=pm10*0.1;
//    			 t=a*t*t*t*t+b*t*t*t+c*t*t+d*t+e;
//    			 sprintf((char*)Pm_str,"a34004-Rtd=%0.1f,a34004-Flag=N;a34002-Rtd=%0.1f,a34002-Flag=N;",t,q);
//    			 //printf("Pm_str:%s\r\n",Pm_str);
//    			 Flag_Pm=0;
//    			 Flag_Sp=1;
//    			 Flag_Nb=0;
//        	 }
//         }
     if(len==7)
     {
		  crc1=getCRC16(ucBuf,len-2);
		  crc2=getCRC16(ucBuf,len-1);
		  if((ucBuf[len-2]==crc1)&&(ucBuf[len-1]==crc2))
		 {
			  if(ucBuf[0]==0x02)
			  {
			     Sp=ucBuf[3]*256+ucBuf[4];
				 
				 if(Sp>=smoklevel)
				 	{
				 		sprintf((char*)Smok_str,"%s","\"Smok\":\"1\",");
						sprintf((char*)smok_buf,"%s","DC1V6(70,70,'“Ï≥£  ',1)");
				 		Alarm=1;	//±®æØ
				 		smokBeep=1;
						infor_type=0;	//–≈œ¢¿‡–Õ£¨±®æØ¥•∑¢
						smokrecover=1;	//µ»¥˝ª÷∏¥
				 	}
				 else
				 	{
				 		smokBeep=0;
				 		sprintf((char*)Smok_str,"%s","\"Smok\":\"0\",");
				 		sprintf((char*)smok_buf,"%s","DCV16(70,70,'’˝≥£  ',11)");
				 	}
				 if((smokrecover==1)&&(Sp<smoklevel))
				 	{
				 		Alarm=1;	//±®æØ
				 		smokrecover=0;
				 	}
			     printf("smok=%d",Sp);
			     Flag_smok=0;
			     Flag_water=1;
			     Flag_Nb=0;
			     flag2=0;

			  }
			  if(ucBuf[0]==0x06)
			  {
				 Sp=ucBuf[3]*256+ucBuf[4];
				 if(Sp>=smoklevel)
				 	{
				 		sprintf((char*)Smok_str1,"%s","\"Smok1\":\"1\",");
						sprintf((char*)smok_buf1,"%s","DCV16(70,70,'“Ï≥£  ',1)");
				 		Alarm=1;	//±®æØ
				 		smokBeep=1;
						infor_type=0;	//–≈œ¢¿‡–Õ£¨±®æØ¥•∑¢
						smok1recover=1;	//µ»¥˝ª÷∏¥
				 	}
				 else
				 	{
				 		smokBeep=0;
				 		sprintf((char*)Smok_str1,"%s","\"Smok1\":\"0\",");
				 		sprintf((char*)smok_buf1,"%s","DCV16(70,70,'’˝≥£  ',11)");
				 	}
				 if((smok1recover==1)&&(Sp<smoklevel))
				 	{
				 		Alarm=1;	//±®æØ
				 		smok1recover=0;
				 	}
				 printf("smok=%d",Sp);
				 Flag_smok1=0;
				 Flag_water1=1;
				 Flag_Nb=0;
				 flag5=0;
			  }
			  
			     if(ucBuf[0]==0x03)
				  {
					 Di=ucBuf[4];
					 if(Di==2)
				 	{
				 		 sprintf((char*)Water_str,"%s","\"Water\":\"1\",");
						 sprintf((char*)water_buf,"%s","DCV16(70,90,'“Ï≥£  ',1)");
				 		Alarm=1;	//±®æØ
				 		waterBeep=1;
						infor_type=0;	//–≈œ¢¿‡–Õ£¨±®æØ¥•∑¢
						waterrecover=1;	//µ»¥˝ª÷∏¥
				 	}
					 else
					 {
					 	waterBeep=0;
					 	sprintf((char*)Water_str,"%s","\"Water\":\"0\",");
					 	sprintf((char*)water_buf,"%s","DCV16(70,90,'’˝≥£  ',11)");
					 }
					 if((waterrecover==1)&&(Di==1))
				 	{
				 		Alarm=1;	//±®æØ
				 		waterrecover=0;
				 	}
					 printf("%s",Water_str);
					 Flag_water=0;
					 Flag_temp1=1;
					 Flag_Nb=0;
					 flag3=0;
				  }
			     if(ucBuf[0]==0x04)
				  {
					 Di=ucBuf[4];
					 if(Di==2)
				 	{
				 		sprintf((char*)Water_str1,"%s","\"Water1\":\"1\",");
						sprintf((char*)water_buf1,"%s","DCV16(70,90,'“Ï≥£  ',1)");
				 		Alarm=1;	//±®æØ
				 		waterBeep=1;
						infor_type=0;	//–≈œ¢¿‡–Õ£¨±®æØ¥•∑¢
						water1recover=1;	//µ»¥˝ª÷∏¥
				 	}
					 else
					 {
					 	waterBeep=0;
					 	sprintf((char*)Water_str1,"%s","\"Water1\":\"0\",");
					 	sprintf((char*)water_buf1,"%s","DCV16(70,90,'’˝≥£  ',11)");
					 }	
					  if((water1recover==1)&&(Di==1))
				 	{
				 		Alarm=1;	//±®æØ
				 		water1recover=0;
				 	}
					 printf("%s",Water_str1);
					 Flag_water1=0;
					 Flag_temp=1;
					 Flag_Nb=0;
					 flag6=0;
				  }

		 }
     }
     if(len==9)
          {
     		  crc1=getCRC16(ucBuf,len-2);
     		  crc2=getCRC16(ucBuf,len-1);
     		  if((ucBuf[len-2]==crc1)&&(ucBuf[len-1]==crc2))
     		 {
     			  if(ucBuf[0]==0x01)
     			  {
//     			     f=ucBuf[5]>>4;
//     			     if(f==0xF)
//     			     {
//     			    	 Temp=ucBuf[3]*256+ucBuf[4];
//     			    	 Temp=65535-Temp;
//						 t=Temp*0.1;
//						 Humi=ucBuf[5]*256+ucBuf[6];
//						 t1=Humi*0.1;
//						 sprintf((char*)Temp_str,"\"Temp1\":\"%0.1f\";\"Humi1\":\"%0.1f\";",t,t1);
//						 printf("%s",Temp_str);
//
//     			     }else
//     			     {
						 Temp=ucBuf[3]*256+ucBuf[4];
						 t=Temp*0.1;
						 if((t>=templevelH) || (t<=templevelL))
				 		{
				 			Alarm=1;	//±®æØ
				 			tempBeep=1;
							infor_type=0;	//–≈œ¢¿‡–Õ£¨±®æØ¥•∑¢
							temrecover=1;	//µ»¥˝ª÷∏¥
				 		}
						 else
						 	{
						 		tempBeep=0;
						 	}
						  if((temrecover==1)&&((t<templevelH)||(t>templevelL)))
				 		{	
				 			Alarm=1;	//±®æØ
				 			temrecover=0;
				 		}
						 Humi=ucBuf[5]*256+ucBuf[6];
						 t1=Humi*0.1;
						  if((t1>=humilevelH) || (t1<=humilevelL))
				 		{
				 			Alarm=1;	//±®æØ
				 			humiBeep=1;
							infor_type=0;	//–≈œ¢¿‡–Õ£¨±®æØ¥•∑¢
							humrecover=1;	//µ»¥˝ª÷∏¥
				 		}
						  else
						 	{
						 		humiBeep=0;
						 	}
						 if((humrecover==1)&&((t1<humilevelH)||(t1>humilevelL)))
				 		{	
				 			Alarm=1;	//±®æØ
				 			humrecover=0;
				 		}
						 sprintf((char*)Temp_str,"\"Temp\":\"%0.1f\",\"Humi\":\"%0.1f\",",t,t1);
						 sprintf((char*)temp_buf,"DCV24(70,10,'%0.1f°Ê',3);DCV24(70,40,'%0.1f\%RH',3)",t,t1);
						 
						 printf("%s",Temp_str);

						 Flag_temp=0;
						 Flag_smok=1;
						 Flag_Nb=0;
						 flag1=0;
     			   //  }
//     			     Humi=ucBuf[5]*256+ucBuf[6];
//					 t=Humi*0.1;
//					 sprintf((char*)Humi_str,"Humi:%0.1f",t);


     			  }
     			    if(ucBuf[0]==0x05)
				   {
//					 f=ucBuf[5]>>4;
//					 if(f==0xF)
//					 {
//						 Temp=ucBuf[3]*256+ucBuf[4];
//						 Temp=65535-Temp;
//						 t=Temp*0.1;
//						 Humi=ucBuf[5]*256+ucBuf[6];
//						 t1=Humi*0.1;
//						 sprintf((char*)Temp_str1,"\"Temp2\":\"%0.1f\";\"Humi2\":\"%0.1f\";",t,t1);
//						 printf("%s",Temp_str);
//
//					 }else
//					 {
						 Temp=ucBuf[3]*256+ucBuf[4];
						 t=Temp*0.1;

						   if((t>=templevelH) || (t<=templevelL))
				 		{
				 			Alarm=1;	//±®æØ
				 			tempBeep=1;
							infor_type=0;	//–≈œ¢¿‡–Õ£¨±®æØ¥•∑¢
							tem1recover=1;	//µ»¥˝ª÷∏¥
				 		}
						   else
						 	{
						 		tempBeep=0;
						 	}
						 if((temrecover==1)&&((t<templevelH)||(t>templevelL)))
				 		{	
				 			Alarm=1;	//±®æØ
				 			tem1recover=0;
				 		}
						 Humi=ucBuf[5]*256+ucBuf[6];
						 t1=Humi*0.1;
						 if((t1>=humilevelH) || (t1<=humilevelL))
				 		{
				 			Alarm=1;	//±®æØ
				 			humiBeep=1;
							infor_type=0;	//–≈œ¢¿‡–Õ£¨±®æØ¥•∑¢
							hum1recover=1;	//µ»¥˝ª÷∏¥
				 		}
						 else
						 	{
						 		humiBeep=0;
						 	}
						  if((humrecover==1)&&((t1<humilevelH)||(t1>humilevelL)))
				 		{	
				 			Alarm=1;	//±®æØ
				 			hum1recover=0;
				 		}
						 sprintf((char*)Temp_str1,"\"Temp1\":\"%0.1f\",\"Humi1\":\"%0.1f\",",t,t1);
						 sprintf((char*)temp_buf1,"DCV24(70,10,'%0.1f°Ê',3);DCV24(70,40,'%0.1f\%RH',3)",t,t1);
						 printf("%s",Temp_str1);

						 Flag_temp1=0;
						 Flag_smok1=1;
						 Flag_Nb=0;
						 flag4=0;
					// }
     			  }		
     		 }
          }
     memset(ucBuf,0,100);
  }
}

void  *serial_send(void *threadid)
{
	    unsigned char Temp[8]={0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0B};
		unsigned char Smok[8]={0x02,0x03,0x00,0x00,0x00,0x01,0x84,0x39};
		unsigned char Water[8]={0x03,0x03,0x00,0x02,0x00,0x01,0x24,0x28};

		unsigned char Temp1[8]={0x05,0x03,0x00,0x00,0x00,0x02,0xC5,0x8F};
		unsigned char Smok1[8]={0x06,0x03,0x00,0x00,0x00,0x01,0x85,0xBD};
		unsigned char Water1[8]={0x04,0x03,0x00,0x02,0x00,0x01,0x25,0x9F};
		
	    struct termios opt;

	    iFd = open(DEV_NAME, O_RDWR | O_NOCTTY);
		if(iFd < 0) {
	            perror(DEV_NAME);
		}
	    tcgetattr(iFd, &opt);
	    if (tcgetattr(iFd,   &opt)<0) {
	             return   -1;
	    }
	    opt.c_lflag	&= ~(ECHO | ICANON | IEXTEN | ISIG);
	    opt.c_iflag	&= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	    opt.c_oflag 	&= ~(OPOST);
	    opt.c_cflag 	&= ~(CSIZE | PARENB);
	    opt.c_cflag 	|=  CS8;

	    opt.c_cc[VMIN] 	= 100;
	    opt.c_cc[VTIME]	= 1;

	    cfsetispeed(&opt, B9600);
	    cfsetospeed(&opt, B9600);

	    if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
	        	return   -1;
	    }
	    tcflush(iFd,TCIOFLUSH);
	    while(1)
	    {
	    	if(Flag_temp==1)
			 {
				 write(iFd, Temp, 8);
				 Flag_Nb++;
				 if(Flag_Nb>2)
				 {
					 sprintf((char*)Temp_str,"\"Temp\":\"--\",\"Humi\":\"--\",");
					 sprintf((char*)temp_buf,"DCV24(70,10,'Œ¥¡¨Ω” ',1);DCV24(70,40,'Œ¥¡¨Ω” ',1)");
					 
					 printf("sendcomand1");
					 Flag_smok=1;
					 Flag_temp=0;
					 Flag_Nb=0;
					 flag1=1;
				 }
			 }
			 if(Flag_smok==1)
			 {
				 write(iFd, Smok, 8);
				 Flag_Nb++;
				 if(Flag_Nb>2)
				 {
					 sprintf((char*)Smok_str,"\"Smok\":\"--\",");
					 sprintf((char*)smok_buf,"%s","DCV16(70,70,'Œ¥¡¨Ω”',1)");
					 printf("sendcomand2");
					 Flag_smok=0;
					 Flag_water=1;
					 Flag_Nb=0;
					 flag2=1;
				 }
			 }
			 if(Flag_water==1)
			 {
				 write(iFd, Water, 8);
				 Flag_Nb++;
				 if(Flag_Nb>2)
				 {
					 sprintf((char*)Water_str,"\"Water\":\"--\",");
					 sprintf((char*)water_buf,"%s","DCV16(70,90,'Œ¥¡¨Ω”',1)");
					 printf("sendcomand3");
					 Flag_water=0;
					 Flag_temp1=1;
					 Flag_Nb=0;
					 flag3=1;
				 }
			 }
			 if(Flag_temp1==1)
			 {
				 write(iFd, Temp1, 8);
				 Flag_Nb++;
				 if(Flag_Nb>2)                                                                                     
				 {
					 sprintf((char*)Temp_str1,"\"Temp1\":\"--\",\"Humi1\":\"--\",");
					 sprintf((char*)temp_buf1,"DCV24(70,10,'Œ¥¡¨Ω” ',1);DCV24(70,40,'Œ¥¡¨Ω” ',1)");
					 printf("sendcomand5");
					 Flag_smok1=1;
					 Flag_temp1=0;
					 Flag_Nb=0;
					 flag4=1;
				 }
			 }
			 if(Flag_smok1==1)
			 {
				 write(iFd, Smok1, 8);
				 Flag_Nb++;
				 if(Flag_Nb>2)
				 {
					 sprintf((char*)Smok_str1,"\"Smok1\":\"--\",");
					 sprintf((char*)smok_buf1,"%s","DCV16(70,70,'Œ¥¡¨Ω”',1)");
					 printf("sendcomand6");
					 Flag_smok1=0;
					 Flag_water1=1;
					 Flag_Nb=0;
					 flag5=1;
				 }
			 }
			 if(Flag_water1==1)
			 {
				 write(iFd, Water1, 8);
				 Flag_Nb++;
				 if(Flag_Nb>2)
				 {
					 sprintf((char*)Water_str1,"\"Water1\":\"--\",");
					 sprintf((char*)water_buf1,"%s","DCV16(70,90,'Œ¥¡¨Ω”',1)");
					 printf("sendcomand4");
					 Flag_water1=0;
					 Flag_temp=1;
					 Flag_Nb=0;
					 flag6=1;
				 }
			 }
			 sleep(1);
	    }
}
int substr(char dst[],char src[],int start,int len)
{
    int i;
    for(i=0;i<len;i++)
    {
        dst[i]=src[start+i];

    }
    dst[i]='\0';
    return i;
}
float strToFloat(char *p,unsigned char length)//Â∞èÊï∞ÁÇπÂêé‰∏Ä‰Ωç,Ââç3‰Ωç
{
   	char *bufa;
   	float end;
   	int leng;
   	bufa=strstr(p,".");
   	leng=length-2;
   	if(leng==3){
   		end=(*(bufa-3)-48)*100+(*(bufa-2)-48)*10+(*(bufa-1)-48)+(*(bufa+1)-48)*0.1;
   	}
   	if(leng==2){
   	   		end=(*(bufa-2)-48)*10+(*(bufa-1)-48)+(*(bufa+1)-48)*0.1;
   	   	}
   	if(leng==1){
   	   	   		end=(*(bufa-1)-48)+(*(bufa+1)-48)*0.1;
   	   	   	}
   	return end;
}
int main(int argc, char *argv[])
{
	int packet_length;
	pthread_t threads[5];
	int timeout;
	int wdt_fd = -1;
    int rc,ret;
    int tt;

    time_t t;
    struct tm *p;
    char timebuf[50],timestampbuf[50],Powerbuf[50],Powerbuf1[50],Networkbuf[50];
    char Sendokbuf[50]={0}; 
    char mon[10],day[10],hour[10],min[10],sec[10];

    //char dev[]="/media/mmcblk0p1/a.txt";
    //char dev_1[]="/media/mmcblk0p1/rec.log";
    char dev[50];
    FILE *fp=NULL;
    int num;
    int size;
    int qq;
    unsigned char log;
    char buf_rec[150];
    char buf_wr[5];
    //char Dev_Time[]="/opt/Time_flag.log";
    char Dev_Time[]="/opt/setting.txt";
    //char direct[]="/opt/remember.txt";
    char Trans_freq[5];
    char *ptr,*str;
    char buf[20];
    unsigned char IO1,IO2;


    int flag_net,times;
    unsigned char net;
   // char topic[20];
    //unsigned char light;
   // char light_buf[60];
    unsigned char red,len;
    char End_Data1[10];
            fd_export=open(EXPORT_PATH,O_WRONLY);
    		if(fd_export<0)
    		{
    			perror(EXPORT_PATH);
    		}
    			write(fd_export,GPIO3,strlen(GPIO3));
				fd_dir3=open(DIRECT_PATH3,O_RDWR);
				if(fd_dir3<1)
				{
					perror(DIRECT_PATH3);
				}
				write(fd_dir3,"in",sizeof("in"));
				fd_dev3=open(DEV_PATH3,O_RDWR);
				if(fd_dev3<0)
				{
					perror(DEV_PATH3);
				}

				write(fd_export,GPIO4,strlen(GPIO4));
				fd_dir4=open(DIRECT_PATH4,O_RDWR);
				if(fd_dir4<1)
				{
					perror(DIRECT_PATH4);
				}
				write(fd_dir4,"in",sizeof("in"));
				fd_dev4=open(DEV_PATH4,O_RDWR);
				if(fd_dev4<0)
				{
					perror(DEV_PATH4);
				}


//ÈÖçÁΩÆÊñá‰ª∂ËÆæÂÆö
    	 fp=fopen(Dev_Time,"r");//ÈÖçÁΩÆÊñá‰ª∂ÁõÆÂΩï
		 size=fread(buf_rec,1,sizeof(buf_rec),fp);	
		 printf("size:%d  %s\n",size,buf_rec);

		 ptr = strtok(buf_rec, ";");
		 printf("ptr:%s\n",ptr);
		 while(ptr != NULL)
		 {
			  if(strstr(ptr,"ip=")!=NULL)
			  {
				 len=strlen(ptr);
				 len=len-3;
				 substr(buf, ptr,3, len)  ;
				 sprintf((char*)ip,"%s",buf);
		//		 ip=buf;
				 printf("ip:%s\n",ip);
			  }
//			  if(strstr(ptr,"port=")!=NULL)//ÊúÄÈ´ò5‰Ωç
//			  {
//				 len=strlen(ptr);
//				 len=len-5;
//				 substr(buf, ptr,5, len)  ;
//				 sprintf((char*)Port,"%s",buf);
//				 printf("Port:%s\n",Port);
//			  }
			 if(strstr(ptr,"frequency=")!=NULL)
			 {
				 len=strlen(ptr);
				 len=len-10;
				 substr(buf, ptr,10, len)  ;
				 frequence=(buf[0]-48)*1000+(buf[1]-48)*100+(buf[2]-48)*10+buf[3]-48;
                 printf("frequence:%d\n",frequence);
			}

			 if(strstr(ptr,"mn=")!=NULL)
			 {
				 len=strlen(ptr);
				 len=len-3;
				 substr(buf, ptr,3, len);
				 sprintf((char*)mn,"%s",buf);
				 printf("mn:%s\n",mn);
			 }

			 if(strstr(ptr,"topic=")!=NULL)
			 {
				 len=strlen(ptr);
				 len=len-6;
				 substr(buf, ptr,6, len);
				 sprintf((char*)topic,"%s",buf);
				 printf("topic:%s\n",topic);
			 }

			 if(strstr(ptr,"temp=")!=NULL)
			 {
				 len=strlen(ptr);
				 len=len-5;
				 substr(buf, ptr,5, len);
				 templevelH=(buf[0]-48)*1000+(buf[1]-48)*100+(buf[2]-48)*10+buf[3]-48;
                 		printf("templevel:%d\n",templevelH);
			 }
			 
			if(strstr(ptr,"humi=")!=NULL)
			 {
				 len=strlen(ptr);
				 len=len-5;
				 substr(buf, ptr,5, len);
				 humilevelH=(buf[0]-48)*1000+(buf[1]-48)*100+(buf[2]-48)*10+buf[3]-48;
                 		printf("humilevel:%d\n",humilevelH);
			 }

			if(strstr(ptr,"smok=")!=NULL)
			 {
				 len=strlen(ptr);
				 len=len-5;
				 substr(buf, ptr,5, len);
				 smoklevel=(buf[0]-48)*1000+(buf[1]-48)*100+(buf[2]-48)*10+buf[3]-48;
                 		 printf("smoklevel:%d\n",smoklevel);
			 }
			
			 ptr = strtok(NULL, ";");
			 printf("ptr1:%s\n",ptr);
		 }
		 fclose(fp);


		 //ÁúãÈó®Áãó
		 wdt_fd = open(WDT, O_WRONLY);
		if (wdt_fd == -1)
		{
			printf("fail to open "WDT "!\n");
		}
		printf(WDT " is opened!!\n");
		timeout = 50;
		ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);
		//ioctl(wdt_fd, WDIOC_GETTIMEOUT, &timeout);
		printf("The timeout was is %d seconds\n", timeout);

		printf("111111111111111\n");
		system("udhcpc");
		printf("2222222222222222\n");

    	//mqtt_init(&broker, "Epc287_JF");
		 mqtt_init(&broker, topic);
		//mqtt_init(&broker, "agent");
    	mqtt_init_auth(&broker, "thhy", "thhy550550");
    	printf("3333333333");
    	//mqtt_init_auth(&broker, "admin", "password");
    	//init_socket(&broker, "61.181.255.77", 61613);

    	init_socket(&broker, ip, 61613);
    	// >>>>> CONNECT
    	mqtt_connect(&broker);
    	// <<<<< CONNACK
    	packet_length = read_packet(1);
    	if(packet_length < 0)
    	{
    		fprintf(stderr, "Error(%d) on read packet!\n", packet_length);
    		return -1;
    	}

    	if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_CONNACK)
    	{
    		fprintf(stderr, "CONNACK expected!\n");
    		return -2;
    	}

    	if(packet_buffer[3] != 0x00)
    	{
     	    fprintf(stderr, "CONNACK failed!\n");
    		return -2;
    	}	
	
	//¥Úø™œﬂ≥Ã
    for(qq=0;qq<5;qq++)
    {
    	rc=pthread_create(&threads[0],NULL,serial,NULL);
    	if(rc==0)
    	{
    		printf("thread-485:%d\n",rc);
        	break;
    	}
    	if(rc)
		{
			printf("ERROR:pthread_creat() is %d\n",rc);
			//exit(-1);
		}
    }

     for(qq=0;qq<5;qq++)
    {
    	rc=pthread_create(&threads[1],NULL,display,NULL);
    	if(rc==0)
    	{
    		printf("display:%d\n",rc);
        	break;
    	}
    	if(rc)
		{
			printf("ERROR:pthread_creat() is %d\n",rc);
			//exit(-1);
		}
    }


    for(qq=0;qq<5;qq++)
	{
    	rc=pthread_create(&threads[2],NULL,threadWork,NULL);
		if(rc==0)
		{
			printf("mqtt_rec:%d\n",rc);
			break;
		}
		if(rc)
		{
			printf("ERROR:pthread_creat() is %d\n",rc);
			//exit(-1);
		}
	}
        for(qq=0;qq<5;qq++)
    	{
    	    rc=pthread_create(&threads[3],NULL,serial_send,NULL);
    		if(rc==0)
    		{
    			printf("serial_send:%d\n",rc);
    			break;
    		}
    		if(rc)
    		{
    			printf("ERROR:pthread_creat() is %d\n",rc);
    			//exit(-1);
    		}
    	}

	 for(qq=0;qq<5;qq++)
    	{
    	    rc=pthread_create(&threads[4],NULL,beep_warning,NULL);
    		if(rc==0)
    		{
    			printf("beep_warning:%d\n",rc);
    			break;
    		}
    		if(rc)
    		{
    			printf("ERROR:pthread_creat() is %d\n",rc);
    			//exit(-1);
    		}
    	}


while(1)
    {
	    printf("qwqwqwqwq\r\n");
	    write(wdt_fd, "\0", 1);
	     //ÊµãËØïÂ§ßÁ´ØÂ∞èÁ´ØÈóÆÈ¢ò
	     time(&t);
	     sprintf((char*) timestampbuf,"\"Timestamp\":\"%d\"",t);
	     p=localtime(&t);
	     if((1+p->tm_mon)<10)
	     {
	    	 sprintf((char*)mon,"0%d",1+p->tm_mon);
	     }else{
	    	 sprintf((char*)mon,"%d",1+p->tm_mon);
	     }
	     if((p->tm_mday)<10)
		 {
			 sprintf((char*)day,"0%d",p->tm_mday);
		 }else{
			 sprintf((char*)day,"%d",p->tm_mday);
		 }
	     if((p->tm_hour)<10)
		 {
			 sprintf((char*)hour,"0%d",p->tm_hour);
		 }else{
			 sprintf((char*)hour,"%d",p->tm_hour);
		 }
	     if((p->tm_min)<10)
		 {
			 sprintf((char*)min,"0%d",p->tm_min);
		 }else{
			 sprintf((char*)min,"%d",p->tm_min);
		 }
	     if((p->tm_sec)<10)
		 {
			 sprintf((char*)sec,"0%d",p->tm_sec);
		 }else{
			 sprintf((char*)sec,"%d",p->tm_sec);
		 }

	     sprintf((char*)timebuf,"\"Time\":\"%d-%s-%s %s:%s:%s\"",(1900+p->tm_year),mon,day,hour,min,sec);
		// sprintf((char*)timebuf,"%d%d%d%d%d%d",(1900+p->tm_year),(1+p->tm_mon),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
		 ///ioÂè£ËØªÂèñËæìÂÖ•ÂÄº
		 lseek(fd_dev3,0,SEEK_SET);
		 read(fd_dev3,&buf,1);
		 lseek(fd_dev4,0,SEEK_SET);
		 read(fd_dev4,&buf[1],1);
		 printf("end: %c  %c\n",buf[0],buf[1]);
		 IO1=buf[0]-48;
		 IO2=buf[1]-48;
		 //sprintf((char*)Powerbuf,"\"Power\":\"%c\";\"Power\":\"%c\";",buf[0],buf[1]);
        	if((flag1==1)&&(flag2==1)&&(flag3==1)&&(flag4==1)&&(flag5==1)&&(flag6==1))
        	{
        		//write(iFd2, sensorerror, strlen(sensorerror));
			
        		system("reboot");
        	}
		if(IO1==0)
			{
				IO1Beep=0;
				sprintf((char*)Powerbuf,"\"Power\":\"%d\"",0);
				sprintf((char*)power_buf,"%s","DCV16(70,110,'’˝≥£',11)");
			}
		else
			{
				IO1Beep=1;
				sprintf((char*)Powerbuf,"\"Power\":\"%d\"",1);
				sprintf((char*)power_buf,"%s","DCV16(70,110,'“Ï≥£',1)");
			}
		 if(IO2==0)
			{
				IO2Beep=0;
				 sprintf((char*)Powerbuf1,"\"Electric\":\"%d\"",0);
				 sprintf((char*)power_buf1,"%s","DCV16(70,110,'’˝≥£',11)");
			}
		else
			{
				IO2Beep=1;
				sprintf((char*)Powerbuf1,"\"Electric\":\"%d\"",1);
				sprintf((char*)power_buf1,"%s","DCV16(70,110,'“Ï≥£',1)");
			}
		 sprintf((char*)End_Data1,"{}");
		 yy++;
		 if(yy==5)
		 {
			mqtt_publish(&broker, topic, End_Data1, 0);
			yy=0;
		 }
	     qq++;
	    if(Alarm)
	    	{
	    		printf("alarm wring\r\n");
				//‘≠∞Ê◊÷∂Œ
			//sprintf((char*)End_Data,"{\"SensorList\":[{%s%s%s%s},{%s%s%s%s}],\"DevID\":\"%s\",\"Network\":\"1\",\"SendOK\":\"1\",\"Alarm\":\"1\",%s,%s}",Temp_str,Smok_str,Water_str,Powerbuf,Temp_str1,Smok_str1,Water_str1,Powerbuf1,mn,timebuf,timestampbuf);
				//–¬∞Ê“™«Û◊÷∂Œ
			sprintf((char*)End_Data,"{%s%s%s%s,%s%s%s%s,\"DevID\":\"%s\",\"Network\":\"1\",\"SendOK\":\"1\",\"Alarm\":\"1\",%s,%s}",Temp_str,Smok_str,Water_str,Powerbuf,Temp_str1,Smok_str1,Water_str1,Powerbuf1,mn,timebuf,timestampbuf);
			printf("End_Data=%s",End_Data);
			tt=mqtt_publish(&broker, topic, End_Data, 0);
			printf("tt:%d\r\n",tt);
			Alarm=0;
		}
	    if(qq==frequence)
	    {
	    	 printf("/////////////////");
			 //‘≠∞Ê◊÷∂Œ
	    	 //sprintf((char*)End_Data,"{\"SensorList\":[{%s%s%s%s},{%s%s%s%s}],\"DevID\":\"%s\",\"Network\":\"1\",\"SendOK\":\"1\",\"Alarm\":\"0\",%s,%s}",Temp_str,Smok_str,Water_str,Powerbuf,Temp_str1,Smok_str1,Water_str1,Powerbuf1,mn,timebuf,timestampbuf);	
			//–¬∞Ê“™«Û◊÷∂Œ
		 sprintf((char*)End_Data,"{%s%s%s%s,%s%s%s%s,\"DevID\":\"%s\",\"Network\":\"1\",\"SendOK\":\"1\",\"Alarm\":\"0\",%s,%s}",Temp_str,Smok_str,Water_str,Powerbuf,Temp_str1,Smok_str1,Water_str1,Powerbuf1,mn,timebuf,timestampbuf);	
		 printf("End_Data=%s",End_Data);
		 tt=mqtt_publish(&broker, topic, End_Data, 0);
			// tt=mqtt_publish(&broker, "agent", End_Data, 0);
		 printf("tt:%d\r\n",tt);
		 qq=0;
	    }
	    tt=0;
//Êü•ÁúãÁΩëÁªúÊòØÂê¶Ê≠£Â∏∏
	    times++;
	   		 printf("times:%d\r\n",times);
	   		 if(times==60)
	   		 {
	   			      times=0;
	   		          net=system("ping www.baidu.com -c 2");
	   		          if(net!=0)
	   		          {
	   		          		//write(iFd2, neterror, strlen(neterror));
	   		          
	   		         	     system("reboot");
	   		          }
	   		 }
		
	  //  write(wdt_fd, "\0", 1);
		printf(".....................");
	    sleep(1);
    }
    close(iFd);
	close(iFd2);
    return 0;
}
