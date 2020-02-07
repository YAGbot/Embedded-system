#include "oled.h"
#include "font.h"
#include "spi.h"
#include "delay.h"
#include <string.h>

		   
static u8 oled_buf[128][8];	 

static void oled_write(u8 dat,u8 cmd);

//更新显存到LCD		 
void oled_update(void)
{
	int i = 0, n = 0;

	for(i=0;i<8;i++)  
	{  
		oled_write (0xb0+i, OLED_CMD);    //设置页地址（0~7）
		oled_write (0x00, OLED_CMD);      //设置显示位置—列低地址
		oled_write (0x10, OLED_CMD);      //设置显示位置—列高地址   
		
		for(n=0;n<128;n++)
		{
			oled_write(oled_buf[n][i],OLED_DATA); 
		}
	} 		
}
	  	 
/**
  * @brief 开启OLED显示
  * @retval 无
  */
void oled_on(void)
{
	oled_write(0X8D,OLED_CMD);  //SET DCDC命令
	oled_write(0X14,OLED_CMD);  //DCDC ON
	oled_write(0XAF,OLED_CMD);  //DISPLAY ON
}

/**
  * @brief 关闭OLED显示
  * @retval 无
  */
void oled_off(void)
{
	oled_write(0X8D,OLED_CMD);  //SET DCDC命令
	oled_write(0X10,OLED_CMD);  //DCDC OFF
	oled_write(0XAE,OLED_CMD);  //DISPLAY OFF
}		 

/**
  * @brief 清空OLED显示。
  * @retval 无
  */
void oled_clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)
	{
		oled_buf[n][i]=0X00;  
	}
	oled_update();
}


//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
/**
  * @brief
  * @param  ? 
  * @retval ?
  */
void oled_write(u8 dat,u8 cmd)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* 推挽输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		/* IO口速度为50MHz */
	GPIO_Init(GPIOA, &GPIO_InitStructure);				    /* 初始化端口 */

    /* 写命令 */
	if(cmd)OLED_RS_SET;
	else OLED_RS_CLR;

	OLED_CS_CLR;		  
	spi1_read_write(dat);
    OLED_CS_SET;
    OLED_RS_SET;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		/* 推挽输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			/* IO口速度为50MHz */
	GPIO_Init(GPIOA, &GPIO_InitStructure);						/* 初始化端口 */
} 


//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void oled_point(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)oled_buf[x][pos]|=temp;
	else oled_buf[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void oled_fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)oled_point(x,y,dot);
	}													    
	oled_update();//更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 12/16/24
void oled_char(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)oled_point(x,y, mode ?1 :0);
			else oled_point(x,y,mode ?0 :1);
			
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 
/**
  * @brief
  * @param  ? 
  * @retval ?
  */
void oled_num(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				oled_char(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	oled_char(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

/**
  * @brief 显示一个字符串
  * @param  x,y显示的位置 
  * @param  size,显示字符串字体大小
  * @param   *p, 字符串起始地址
  * @retval 无
  */
void oled_str(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;oled_clear();}
        oled_char(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	 
/**
  * @brief
  * @retval ?
  */


void oled_str_mid(u16 x,u16 y,u8*str,u8 size,u8 len)
{
	u16 strlenth = 0;
   	strlenth = strlen((const char*)str);
	strlenth *= size / 2;
	if(strlenth>len)oled_str(x,y,str,size);
	else
	{
		strlenth=(len-strlenth)/2;
	    oled_str(strlenth+x,y,str,size);
	}
}   

/**
  * @brief OLED初始化
  * @retval 无
  */
void oled_init(void)
{ 	
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN;	    
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
 	GPIO_Init(OLED_RST_PORT, &GPIO_InitStructure);	                    

	GPIO_InitStructure.GPIO_Pin =  OLED_CS_PIN;	    
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           
 	GPIO_Init(OLED_CS_PORT, &GPIO_InitStructure);	                   
		  
	OLED_CS_SET;
	OLED_RST_CLR;
	delay_ms(100);
	OLED_RST_SET;
					  
	oled_write(0xAE,OLED_CMD); //关闭显示
	oled_write(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	oled_write(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	oled_write(0xA8,OLED_CMD); //设置驱动路数
	oled_write(0X3F,OLED_CMD); //默认0X3F(1/64) 
	oled_write(0xD3,OLED_CMD); //设置显示偏移
	oled_write(0X00,OLED_CMD); //默认为0

	oled_write(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	oled_write(0x8D,OLED_CMD); //电荷泵设置
	oled_write(0x14,OLED_CMD); //bit2，开启/关闭
	oled_write(0x20,OLED_CMD); //设置内存地址模式
	oled_write(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	oled_write(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	oled_write(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	oled_write(0xDA,OLED_CMD); //设置COM硬件引脚配置
	oled_write(0x12,OLED_CMD); //[5:4]配置
		 
	oled_write(0x81,OLED_CMD); //对比度设置
	oled_write(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	oled_write(0xD9,OLED_CMD); //设置预充电周期
	oled_write(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	oled_write(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	oled_write(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_write(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	oled_write(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	oled_write(0xAF,OLED_CMD); //开启显示	 
	oled_clear();
}  



















