#include "iic.h"
#include "delay.h"

/************************************************************************************
* 函数名称：IIC_init
* 功能描述：IIC接口管脚初始化
* 入口参数：无
* 出口参数：无
* 返 回 值：无
* 说    明：初始化后SCL=1、SDA=1，总线处于空闲状态
************************************************************************************/
void IIC_init(void)
{
	RCC->APB2ENR |= (1 << 3);				//使能GPIOB时钟
	GPIOB->CRL &= 0x00FFFFFF; 				//SCL(PB6)与SDA(PB7)为推挽输出50M
	GPIOB->CRL |= 0x33000000;	   		
	GPIOB->ODR |= 3 << 6;              	//SCL(PB6)与SDA(PB7)输出高,IIC总线空闲状态
}


/************************************************************************************
* 函数名称：IIC_Start
* 功能描述：起始信号
* 入口参数：无
* 出口参数：无
* 返 回 值：无
* 说    明：起始信号后SCL=0、SDA=0
************************************************************************************/
void IIC_Start(void)
{
	IIC_SDA_OPUT;							//SDA为输出模式
	IIC_SDA_SET;							//SDA拉高
	IIC_SCL_SET;							//SCL拉高
	delay_us(TSTA_SU);						//延时TSTA_SU微秒
	IIC_SDA_CLR;							//SDA拉低
	delay_us(TSTA_HD);						//延时TSTA_HD微秒
	IIC_SCL_CLR;							//SCL拉低
}


/************************************************************************************
* 函数名称：IIC_Stop
* 功能描述：终止信号
* 入口参数：无
* 出口参数：无
* 返 回 值：无
* 说    明：终止信号后SCL=1、SDA=1,总线处于空闲状态
************************************************************************************/
void IIC_Stop(void)
{
	IIC_SDA_OPUT;							//SDA为输出模式
	IIC_SDA_CLR;							//SDA拉低
	IIC_SCL_SET;							//SCL拉高
	delay_us(TSTO_SU);						//延时TSTO_SU微秒
	IIC_SDA_SET;							//SDA拉高
	delay_us(TSTO_HD);						//延时TSTO_HD微秒
}


/************************************************************************************
* 函数名称：IIC_Ack
* 功能描述：应答信号
* 入口参数：无
* 出口参数：无
* 返 回 值：无
* 说    明：应答位后SCL=0、SDA=0
************************************************************************************/
void IIC_Ack(void)  //回复应答信号
{
	IIC_SDA_OPUT;							//SDA为输出模式
	IIC_SDA_CLR;							//SDA拉低
	delay_us(TDAT_SU);						//延时TDAT_SU微秒
	IIC_SCL_SET;							//SCL拉高
	delay_us(THIGH);						//延时THIGH微秒
	IIC_SCL_CLR;							//SCL拉低
	delay_us(TDAT_HD);						//延时TDAT_HD微秒
}


/************************************************************************************
* 函数名称：IIC_NAck
* 功能描述：非应答信号
* 入口参数：无
* 出口参数：无
* 返 回 值：无
* 说    明：非应答位后SCL=0、SDA=1
************************************************************************************/
void IIC_NAck(void)
{
	IIC_SDA_OPUT;							//SDA为输出模式
	IIC_SDA_SET;							//SDA拉高
	delay_us(TDAT_SU);						//延时TDAT_SU微秒
	IIC_SCL_SET;							//SCL拉高
	delay_us(THIGH);						//延时THIGH微秒
	IIC_SCL_CLR;							//SCL拉低
	delay_us(TDAT_HD);						//延时TDAT_HD微秒
}


/************************************************************************************
* 函数名称：IIC_CheckAck
* 功能描述：应答信号的检测
* 入口参数：无
* 出口参数：无
* 返 回 值：1:检测到非应答信号(NAck); 0:检测到应答信号(Ack)
* 说    明：应答位检测完后SCL=0、SDA=1(接收器发送Ack后会释放总线SDA)
************************************************************************************/
u8 IIC_CheckAck(void) //发送完数据包后
{
	u8 count = 0;
	IIC_SDA_IPUT;		  					//SDA为输入模式,总线电阻也会拉高SDA
	delay_us(TDAT_SU);						//延时TDAT_SU微秒
	IIC_SCL_SET;							//SCL拉高	
	while(IIC_SDA_RED)						//检测SDA是否被接收器拉低
	{
		count++;
		delay_us(1);
		if(count > TCHECK)					//判断检测是否超时,防止死循环
		{
			IIC_Stop();						//未检测到应答信号,即检测到了非应答信号
			return 1;	
		}		
	}
	IIC_SCL_CLR;							//SCL拉低
	return 0;
}


/************************************************************************************
* 函数名称：IIC_TxByte
* 功能描述：发送一个字节函数,包含应答/非应答位的检测,字节发送为先高位后低位
* 入口参数：-data:地址包时为器件地址或字节地址+读写控制位,数据包时为数据
* 出口参数：无
* 返 回 值：1:检测到非应答信号(NAck); 0:检测到应答信号(Ack)
* 说    明：发送完后SCL=0、SDA=1(接收器发送Ack后会释放总线SDA)
************************************************************************************/
u8 IIC_TxByte(u8 data) 
{
	u8 i;
	IIC_SDA_OPUT;							//SDA为输出模式
	for(i=0;i<8;i++)						//发送字节的8bit       	
	{
		if(data & 0x80)						//确定此位的值是0还是1
			IIC_SDA_SET; 					//SDA拉高,发送1
		else
			IIC_SDA_CLR;					//SDA拉低,发送0
		delay_us(TDAT_SU);					//延时TDAT_SU微秒
		IIC_SCL_SET;						//SCL拉高
		delay_us(THIGH);					//延时THIGH微秒
		IIC_SCL_CLR;						//SCL拉低
		delay_us(TDAT_HD);					//延时TDAT_HD微秒
		data <<= 1;							//下一位移至最高位
	}
	return IIC_CheckAck();			   		//应答/非应答位检测
}


/************************************************************************************
* 函数名称：IIC_RxByte
* 功能描述：接收一个字节函数,包含应答/非应答位的回复,接收的字节先高位后低位
* 入口参数：-ack:接收完字节回复的应答/非应答位,0为应答,1为非应答 			
* 出口参数：无
* 返 回 值：无
* 说    明：接收完后SCL=0、SDA=1或0
************************************************************************************/
u8 IIC_RxByte(u8 ack)
{
	u8 i,data = 0;							
	IIC_SDA_IPUT;							//SDA为输出模式,总线上拉电阻也会拉高SDA
	for(i=0;i<8;i++)						//接收字节的8bit  
	{
		data <<= 1;							//将接收的上一个位往左移动	
		IIC_SCL_SET;						//SCL拉高
		delay_us(THIGH);					//延时THIGH微秒
		data |= IIC_SDA_RED;				//读SDA的值并存入data的一个位里
		IIC_SCL_CLR;						//SCL拉低
		delay_us(TLOW);						//延时TLOW微秒		
	}
	ack ? IIC_NAck() : IIC_Ack(); 			//发送应答或非应答信号
	return data;	
}											//返回接收到的字节数据
