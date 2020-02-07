#ifndef __DIWEN__H
#define __DIWEN__H

#include"stm32f10x.h"

extern u8 ip1_modfiy_flag;
extern u8 ip2_modfiy_flag;
extern u8 UBB_modfiy_flag;
extern u8 IBB_modfiy_flag;
extern u8 UBB2_modfiy_flag;
extern u8 IBB2_modfiy_flag;


void diwen_init(void);
void check_IMI_cmd(void);
void Data_Scan(void);
void diwen_send_data(u16 _reg,u32 _data);
void diwen_view(void);

#endif
