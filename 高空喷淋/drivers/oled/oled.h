#ifndef _OLED_H
#define _OLED_H

#include "stm32f10x.h"
		 
#define OLED_RS_PORT GPIOA
#define OLED_RS_PIN  GPIO_Pin_6
#define OLED_RS_SET  GPIO_SetBits(OLED_RS_PORT, OLED_RS_PIN)
#define OLED_RS_CLR  GPIO_ResetBits(OLED_RS_PORT, OLED_RS_PIN)


#define OLED_CS_PORT GPIOB
#define OLED_CS_PIN  GPIO_Pin_14
#define OLED_CS_SET  GPIO_SetBits(OLED_CS_PORT, OLED_CS_PIN)
#define OLED_CS_CLR  GPIO_ResetBits(OLED_CS_PORT, OLED_CS_PIN)

#define OLED_RST_PORT GPIOA
#define OLED_RST_PIN  GPIO_Pin_8
#define OLED_RST_SET GPIO_SetBits(OLED_RST_PORT, OLED_RST_PIN)
#define OLED_RST_CLR GPIO_ResetBits(OLED_RST_PORT, OLED_RST_PIN)


#define GUI_TEXTMODE_NORMAL      (0)
#define GUI_TEXTMODE_REVERSE     (1<<0)
#define GUI_TEXTMODE_UNDERLINE   (1<<1)
#define GUI_TEXTMODE_XOR         (1<<2)
//
#define GUI_TA_LEFT              (0<<4)
#define GUI_TA_HCENTER           (1<<4)
#define GUI_TA_RIGHT             (2<<4)
#define GUI_JUST_TEXT            (1<<7)
 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//OLED控制用函数
void oled_init(void);
void oled_clear(void);
void oled_update(void);  
void oled_point(u8 x,u8 y,u8 t);
void oled_fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void oled_char(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void oled_num(u8 x,u8 y,u32 num,u8 len,u8 size);
void oled_str(u8 x,u8 y,const u8 *p,u8 size);	 
void oled_str_mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void oled_show_font(u8 x,u8 y,u8 *font,u8 size, u8 mode);
void oled_add_str(void * p);
void oled_ico(u8 x, u8 y, u8 whith, u8 hight, u8 * pData, u8 mode, u8 is_black);




#endif


