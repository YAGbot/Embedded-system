#ifndef _SMTPUTIL_H_
#define _SMTPUTIL_H_

#include "config.h"


//SMTP_STATE

#define waitfor220 					0
#define waitforHELO250 				1
#define waitforAUTH334 				2
#define waitforuser334 				3
#define waitforpassword235 			4
#define waitforsend250 				5
#define waitforrcpt250 				6
#define waitfordate354 				7
#define waitformime250 				8

extern uint8 SMTP_STATE;


void mailmessage(void);
void base64encode(char *s, char *r);
void str_insert(char *s1,char *s2,int pos);
void do_smtp(void);
s8 send_mail(u8 ch , char * pbuf);



#endif

