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

	/* LI��Leap Indicator��������Ϊ2���أ�ֵΪ"11"ʱ��ʾ�澯״̬��ʱ��δ��ͬ����Ϊ����ֵʱNTP���������� */
	u8     leap 		:2;   

	/* VN��Version Number��������Ϊ3���أ���ʾNTP�İ汾�ţ�Ŀǰ�����°汾Ϊ3�� */	
	u8     version       :3;   

	/* Mode������Ϊ3���أ���ʾNTP�Ĺ���ģʽ����ͬ��ֵ����ʾ�ĺ���ֱ��ǣ�
		0δ���塢
		1��ʾ�����Ե���ģʽ��
		2��ʾ�����Ե���ģʽ��
		3��ʾ�ͻ�ģʽ��
		4��ʾ������ģʽ��
		5��ʾ�㲥ģʽ���鲥ģʽ��
		6��ʾ�˱���ΪNTP���Ʊ��ġ�
		7Ԥ�����ڲ�ʹ�á� */
    u8     mode        :3;   

	 /* Stratum��ϵͳʱ�ӵĲ�����ȡֵ��ΧΪ1��16����������ʱ�ӵ�׼ȷ�ȡ�����Ϊ1��ʱ��׼ȷ����ߣ�
	 ׼ȷ�ȴ�1��16���εݼ�������Ϊ16��ʱ�Ӵ���δͬ��״̬��������Ϊ�ο�ʱ�ӡ� */
	u8     stratum;       

	/* ��ѯʱ�䣬����������NTP����֮���ʱ������l */
	u8    poll;           

	/* ʱ�ӵľ���,��ȷ�����ƽ�ּ�  */
    u8  precision;    

	/* ���ο�Դ֮���������ܹ���ʱ��  */
    struct s_fixedpt   rootdelay;      /* root delay */
    struct s_fixedpt   rootdisp;       /* root dispersion */

	/* Reference Identifier���ο�ʱ��Դ�ı�ʶ�� */
    u32    refid;          

	/*
	Originate Timestamp 	T1		�ͻ��˷���ʱ�������ʱ��
	Receive Timestamp		T2		�������յ�ʱ�������ʱ��
	Transmit Timestamp		T3		����������ʱ��ظ���ʱ��
	Destination Timestamp	T4		�ͻ����յ�ʱ��ظ���ʱ��

	d = (T4 - T1) - (T2 - T3)
	t = ((T2 - T1) + (T3 - T4)) / 2

	*/

	/* Reference Timestamp��ϵͳʱ�����һ�α��趨����µ�ʱ�䡣 */
	struct l_fixedpt  reftime;       

	/* Originate Timestamp��NTP�������뿪���Ͷ�ʱ���Ͷ˵ı���ʱ�䡣 */
	struct l_fixedpt  org;            

	/* Receive Timestamp��NTP�����ĵ�����ն�ʱ���ն˵ı���ʱ�� */
	struct l_fixedpt  rec;            

	/* transmit timestamp Ӧ�����뿪Ӧ����ʱӦ���ߵı���ʱ�� */
    struct l_fixedpt  xmt;           
      
} ntpformat;



/* �������� */
void ntpclient_init(void);



#endif

