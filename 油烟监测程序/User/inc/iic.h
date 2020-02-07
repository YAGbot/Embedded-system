#ifndef __IIC__H
#define __IIC__H
#include "sys.h"

 
/*IIC管脚方向与操作宏定义*/
#define IIC_SDA_IPUT  	{GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= (u32)0x8 << 4*7;}  //上下拉输出入
#define IIC_SDA_OPUT 		{GPIOB->CRL	&= 0X0FFFFFFF; GPIOB->CRL |= (u32)0x3 << 4*7;}  //推挽输出50M	 
#define	IIC_SCL_SET			PB_SET(6)    	 		 							  //SCL输出1
#define IIC_SCL_CLR    	PB_CLR(6) 	 										  //SCL输出0
#define	IIC_SDA_SET			PB_SET(7)    	 		 							  //SDA输出1
#define IIC_SDA_CLR    	PB_CLR(7) 	 										  //SDA输出0
#define IIC_SDA_RED  		PB_RED(7)  										  //读SDA的值

/*IIC总线时序参数宏定义*/
#define TSTA_SU		5						//起始信号SDA下降沿之前SCL高电平保持时间(us)	  	
#define TSTA_HD		5						//起始信号SDA下降沿之后SCL高电平保持时间(us)
#define TSTO_SU		5						//终止信号SDA上升沿之前SCL高电平保持时间(us)
#define TSTO_HD		5						//终止信号SDA上升沿之后SCL高电平保持时间(us)
#define TDAT_SU		5						//数据位SCL拉高之前SDA保持时间(us)
#define TDAT_HD		5						//数据位SCL拉低之后SDA保持时间(us)
#define TLOW		5						//数据位SCL低电平时间(us)
#define THIGH		5						//数据位SCL高电平时间(us)
#define TCHECK		20						//检测应答/非应答信号周期

/*函数声明*/
void IIC_init(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_TxByte(u8 data);
u8 IIC_RxByte(u8 ack);

#endif
