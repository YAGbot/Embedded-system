#ifndef __USR_4G_TIMER__H
#define __USR_4G_TIMER__H

#include "sys.h"                

extern u8 Flag_Rec_MQTT;	//收到短信标示
extern u8 reboot_mqtt;	//收到重启mqtt
extern u8 error_count;	//重启错误计数
extern u8 USART1_RX_REC_ATCOMMAD;
/*函数声明*/
void TIM7_Init(void);
void time7_start(void);
void time7_stop(void);
void time7_set_timeout(u16 time);
void time7_reset(void);

#endif
