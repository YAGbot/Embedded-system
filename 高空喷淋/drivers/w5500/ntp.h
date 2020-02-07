#ifndef	__NTP_H__
#define	__NTP_H__

#include <string.h>
#include "stm32f10x.h"

typedef unsigned long long tstamp;


#define NTP_START_INDEX 40 //last 8-byte of data_buf[size is 48 byte] is xmt, so the startindex should be 40



#define TIME_1970            0x83aa7e80

struct s_fixedpt 
{
    uint16_t    intpart;
    uint16_t    fracpart;
};

struct l_fixedpt 
{
    uint32_t    intpart;
    uint32_t    fracpart;
};


typedef struct _ntpformat
{
    uint8  dstaddr[4];        /* destination (local) address */

	/* LI（Leap Indicator）：长度为2比特，值为"11"时表示告警状态，时钟未被同步。为其他值时NTP本身不做处理。 */
	u8     leap 		:2;   

	/* VN（Version Number）：长度为3比特，表示NTP的版本号，目前的最新版本为3。 */	
	u8     version       :3;   

	/* Mode：长度为3比特，表示NTP的工作模式。不同的值所表示的含义分别是：
		0未定义、
		1表示主动对等体模式、
		2表示被动对等体模式、
		3表示客户模式、
		4表示服务器模式、
		5表示广播模式或组播模式、
		6表示此报文为NTP控制报文、
		7预留给内部使用。 */
    u8     mode        :3;   

	 /* Stratum：系统时钟的层数，取值范围为1～16，它定义了时钟的准确度。层数为1的时钟准确度最高，
	 准确度从1到16依次递减，层数为16的时钟处于未同步状态，不能作为参考时钟。 */
	u8     stratum;       

	/* 轮询时间，即两个连续NTP报文之间的时间间隔。l */
	u8    poll;           

	/* 时钟的精度,精确到秒的平分级  */
    u8  precision;    

	/* 主参考源之间往返的总共延时，  */
    struct s_fixedpt   rootdelay;      /* root delay */
    struct s_fixedpt   rootdisp;       /* root dispersion */

	/* Reference Identifier：参考时钟源的标识。 */
    u32    refid;          

	/*
	Originate Timestamp 	T1		客户端发送时间请求的时间
	Receive Timestamp		T2		服务器收到时间请求的时间
	Transmit Timestamp		T3		服务器发送时间回复的时间
	Destination Timestamp	T4		客户端收到时间回复的时间

	d = (T4 - T1) - (T2 - T3)
	t = ((T2 - T1) + (T3 - T4)) / 2

	*/

	/* Reference Timestamp：系统时钟最后一次被设定或更新的时间。 */
	struct l_fixedpt  reftime;       

	/* Originate Timestamp：NTP请求报文离开发送端时发送端的本地时间。 */
	struct l_fixedpt  org;            

	/* Receive Timestamp：NTP请求报文到达接收端时接收端的本地时间 */
	struct l_fixedpt  rec;            

	/* transmit timestamp 应答报文离开应答者时应答者的本地时间 */
    struct l_fixedpt  xmt;           
      
} ntpformat;



/* 函数声明 */
void ntpclient_init(void);



#endif

