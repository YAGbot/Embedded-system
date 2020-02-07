#include "at24cxx.h"
#include "delay.h"
#include "iic.h"


/************************************************************************************
* 函数名称：AT24Cxx_init
* 功能描述：AT24Cxx初始化
* 入口参数：无
* 出口参数：无
* 返 回 值：无
* 说    明：无
************************************************************************************/
void AT24Cxx_init(void)
{
	IIC_init();
}


/************************************************************************************
* 函数名称：AT24Cxx_WriteByte
* 功能描述：AT24Cxx写一个字节函数
* 入口参数：-SlaAddr：器件地址(0-7)
*			-ByteAddr：字节地址(不同容量芯片地址范围不一样)
*			-data：要写的数据 				
* 出口参数：无						   
* 返 回 值：无
* 说    明：器件地址取值范围：AT24C01/02/04/08/16：SlaAddr=0~7
************************************************************************************/
void AT24Cxx_WriteByte(u8 SlaAddr, u16 ByteAddr, u8 data)
{
	IIC_Start();								//起始信号
	IIC_TxByte(AT24CXX_SLA_W(SlaAddr));			//器件地址+写控制
	IIC_TxByte(ByteAddr);						//字节地址
	IIC_TxByte(data);							//发送字节数据
	IIC_Stop();									//停止信号
	delay_ms(AT24CXX_TWR);						//等待擦除
}

void AT24Cxx_WriteByte1(u8 SlaAddr, u16 ByteAddr, u8 *data)
{
	 u16 i;
	IIC_Start();								//起始信号
	IIC_TxByte(AT24CXX_SLA_W(SlaAddr));			//器件地址+写控制
	IIC_TxByte(ByteAddr);						//字节地址
//	IIC_TxByte(ByteAddr1);						//字节地址
//	IIC_TxByte(data);							//发送字节数据
//	IIC_TxByte(data1);							//发送字节数据
	
	 for(i=0; i<8; i++)									//要写Len个字节
	 {
	 IIC_TxByte( *(data + i));	
		
	 }	 
	IIC_Stop();									//停止信号
	delay_ms(AT24CXX_TWR);						//等待擦除
}


/************************************************************************************
* 函数名称：AT24Cxx_WriteBuf
* 功能描述：AT24Cxx写多个字节
* 入口参数：-SlaAddr：器件地址(0-7)
*			-ByteAddr：要写多个字节的首地址(不同容量芯片地址范围不一样)
*			-pBuf：待写入的数据缓冲区(数组缓冲区)的首地址
*           -Len：要写的字节个数 				
* 出口参数：无
* 返 回 值：无
* 说    明：器件地址取值范围：AT24C01/02/04/08/16：SlaAddr=0~7
************************************************************************************/
void AT24Cxx_WriteBuf(u8 SlaAddr, u16 ByteAddr, u8 *pBuf, u16 Len)
{
	 u16 i;
	 for(i=0; i<Len; i++)									//要写Len个字节
	 {
	 	AT24Cxx_WriteByte(SlaAddr, ByteAddr, *(pBuf + i));	//写第i个字节
		ByteAddr++;											//求出下一个字节的地址
	 }	 
}


/************************************************************************************
* 函数名称：AT24Cxx_ReadBuf
* 功能描述：AT24Cxx连续读函数
* 入口参数：-SlaAddr：器件地址(0-7)
*			-ByteAddr：要读的多个字节的首地址(不同容量芯片地址范围不一样)
*			-pBuf：读出数据要存入的缓冲区(数组缓冲区)的首地址
*           -Len：要读的字节个数 				
* 出口参数：无
* 返 回 值：无
* 说    明：器件地址取值范围：AT24C01/02/04/08/16：SlaAddr=0~7
************************************************************************************/
void AT24Cxx_ReadBuf(u8 SlaAddr, u16 ByteAddr, u8 *pBuf, u16 Len)
{
	u16 i;
	IIC_Start();								//起始信号
	IIC_TxByte(AT24CXX_SLA_W(SlaAddr));			//器件地址+写控制
	IIC_TxByte(ByteAddr);						//字节地址
	IIC_Start();								//起始信号
	IIC_TxByte(AT24CXX_SLA_R(SlaAddr));			//器件地址+读控制	
	for(i=1; i<=Len; i++)						//连续读Len个字节
	{
		if(i < Len)								//不是第Len个字节
			*pBuf = IIC_RxByte(0);				//读一个字节并回复应答信号
		else									//是第Len个字节
			*pBuf = IIC_RxByte(1);				//读第Len个字节并回复非应答信号
		pBuf++;									//指向下一个位置
	}
	IIC_Stop();								    //停止信号
}
