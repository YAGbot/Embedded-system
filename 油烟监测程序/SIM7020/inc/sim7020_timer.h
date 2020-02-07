#ifndef __SIM7020_TIMER__H
#define __SIM7020_TIMER__H

#include "sys.h"                

extern u8 Flag_Rec_MQTT;	//收到短信标示
extern u8 USART2_RX_REC_ATCOMMAD;	//at命令时间内
extern u8 rec_ip_config;		//处理ip完成标志
/*函数声明*/
void TIM7_Init(void);
void time7_start(void);
void time7_stop(void);
void time7_set_timeout(u16 time);
void time7_reset(void);

void split(char str[],char delims[]);

#endif
