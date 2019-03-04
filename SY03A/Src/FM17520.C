#include "ex_var.h"
#include "ex_func.h"
//#include <intrins.h>
#include <string.h>

const uint08 code Reg_Addr_Int[IntRegLen] = {COMMAND, STATUS2, TXMODE, RXMODE, TXCONTROL, MODWIDTH, RXTRESHOLD, RFCFG, WATERLEVEL, TXAUTO, BITFRAMING, COLL, COMMIEN};
const uint08 code Reg_Data_Int[IntRegLen] = {0x20, 0x00, 0x08, 0x08, 0x0b, 0x26, 0x55, 0x48, 0x20, 0x40, 0x00, 0x00, 0x00};

uint08	near Pcb_Flag;

//==================== 底层读、写寄存器 ==================
//********************************************************
unsigned char RF_Spi_Com( unsigned char Reg_Addr ) //读/写一字节寄存器
{
	unsigned char i, Temp08;
	
	Temp08 = 0x00;
	for(i=0; i<8; i++)	//读取寄存器内容放入RF_Rx_Buf
	{
		RFSPICLK0;
		if( Reg_Addr&0x80 ) RFSPIMO1;		//发送下一个地址
		else RFSPIMO0;
		Reg_Addr <<= 1;

		RFSPICLK1;
		Temp08 <<= 1;
		if(RFSPIMIGPIO & BIT12)	Temp08 |= 1;
	}
	
	return Temp08;
}


//*********************************************************
void RF_Reg_RW( unsigned char OpMode, unsigned char Reg_Addr, unsigned char *Reg_Data, unsigned char Reg_Len )
{	
	unsigned char k, Addr_Temp;
 	
	Clear_Wdt();
 	RFSPICS0;
 	
 	Addr_Temp = (Reg_Addr<<1)|OpMode;
	
	RF_Spi_Com(Addr_Temp);	//先写入寄存器地址

	for( k=0; k<Reg_Len; k++ )
	{		
		if( FLAG_EA==0 ) break;
			
		if( OpMode == RD )
		{
			if( k == (Reg_Len - 1) ) Addr_Temp = 0;	//读取最后一字节，地址=0
			Reg_Data[k] = RF_Spi_Com( Addr_Temp );
		}
		else
			RF_Spi_Com( Reg_Data[k] );
	}

	RFSPICS1;
	RFSPICLK0;
}

//===================== 射频读写1个字节===================
unsigned char RF_Get_Reg( unsigned char Reg_Addr )
{
	unsigned char Reg_Data;
	
	RF_Reg_RW(RD, Reg_Addr, &Reg_Data, 1);
	
	return (Reg_Data);
}

void RF_Set_Reg( unsigned char Reg_Addr, unsigned char Reg_Data )
{
	RF_Reg_RW(WR, Reg_Addr, &Reg_Data, 1);
}

/*********************************************************************************************************
** 函数名称:	FM175xx_ReadFIFO
** 函数功能:	读FIFO
** 输入参数:	*Buf: 读取的数据存放地址
** 输出参数:	
** 返回值:		Len: FIFO读取的数据长度
*********************************************************************************************************/
unsigned char RF_Read_FIFO( unsigned char CH )
{
	unsigned char i, Temp, Len;
	
	Len = 0;

	i = 2; //最大延时: 0.6ms@1次
	if( CH == CARD ) i = 30; //最大延时：5.9ms@10次
	
	for( ; i>0; i-- ) //0.6ms@one loop, 符合帧间最小时间0.610ms
	{
		if( FLAG_EA == 0 ) break;

		Temp = RF_Get_Reg( FIFOLEVEL );	//读取FIFO的数据长度
		if( Temp > 64 ) break;	//最大读取64字节
	
		if( Temp )
		{
			if( (Len+Temp) > u7816_buf_size ) break;
			RF_Reg_RW(RD, FIFODATA, Ic_Comm_RxBuf+Len, Temp);
			Len += Temp;
		}
	}
	
	return (Len);
}

//================== 射频Command_Send命令=================
unsigned char Test_Command_Send( unsigned char CH, unsigned char *TxData, unsigned char RF_Len )
{
	unsigned char i, Result;
	
	RF_Set_Reg(COMMAND, 0x00);
	RF_Set_Reg(COMMIRQ, 0x00);				//清除TXI中断
	RF_Set_Reg(BITFRAMING, 0x00);	//最后1字节的所有位都发送
	RF_Set_Reg(FIFOLEVEL, 0x80);			//清FIFO
	
	if(Ic_Comm_TxBuf[0]==0x60)
	{			
		RF_Reg_RW( WR, FIFODATA, TxData, RF_Len ); //数据先写入FIFO	  
		RF_Set_Reg(COMMAND, 0x0e);	
		I2c_Wait_5ms(  ) ;	
	}
	else		
	  RF_Set_Reg(COMMAND, 0x0c);				//发射FIFO缓冲器中的数据到天线，发射后自动激活接收器
	
	if(CH == CARD)
	{
		RF_Set_Reg(FIFODATA, Pcb_Flag);	//对卡片操作先写Pcb_Flag
	}
	
	
	if(Ic_Comm_TxBuf[0]!=0x60) RF_Reg_RW( WR, FIFODATA, TxData, RF_Len ); //数据先写入FIFO

	if(Ic_Comm_TxBuf[0]!=0x60)
	{
		if((Ic_Comm_TxBuf[0]==0x52) && (RF_Len==1))
		  RF_Set_Reg(BITFRAMING, 0x87);	//发射数据开启，最后1字节发送7bit(Request)		
	  else 
		  RF_Set_Reg(BITFRAMING, 0x80);	//发射数据开启，最后1字节的所有位都发送
  }
	for(i=0; i<20; i++)
  {		
		if(Ic_Comm_TxBuf[0]==0x60)
		{			   
			  Result = RF_Get_Reg(STATUS2);	
		    if((Result & 0x08)) break; 				
	  }
		else	
		{	
			  Result = RF_Get_Reg(COMMIRQ);
		    if((Result & 0x02) ) return 0xff;		//接收错误，退出
	  }
	}
	if((Ic_Comm_TxBuf[0]==0x60)&&(i==20))	return 0xff;		//接收错误，退出
	
	return 0;
}

//================== 射频Command_Send命令=================
unsigned char RF_Command_Send( unsigned char CH, unsigned char RF_Len )
{
	unsigned char i, Result;
	
	Result = Test_Command_Send(CH, Ic_Comm_TxBuf, RF_Len);
	if( Result ) return 0;

	Result = RF_Read_FIFO(CH);	//读取FIFO数据
	if( CH == CARD )
	{
		for(i=0; i<10; i++)
		{
			if( Result && (Result != 2) ) return(Result);
			if(Result == 2) 
			{
				Result = Test_Command_Send(FM17520, Ic_Comm_RxBuf, 2);		//Transceive = 93 20
				if( Result ) return 0;
			}
			Result = RF_Read_FIFO(CH);			//读取FIFO数据
		}
	}
	
	return (Result);
}

//================== 射频卡接收、发送数据 =================
unsigned char RF_TxRxFrame( uint08 Lc, uint08 Le )
{
	unsigned char Result;

	Pcb_Flag ^= 0x01;

	Result = RF_Command_Send(CARD, Lc+5);	//Transceive = 52
	if( Result != Le+3 ) 
	{
		Err_Sw12[0] = Ic_Comm_RxBuf[1];
		Err_Sw12[1] = Ic_Comm_RxBuf[2];
		return(1); 	//判断返回的数据长度
	}
	
	memcpy( Ic_Comm_RxBuf, Ic_Comm_RxBuf+1, Le+2 );
	Ic_Comm_RxPtr = Le + 2;
	
	if( ((Ic_Comm_RxBuf[Le] == 0x90)&&(Ic_Comm_RxBuf[Le+1] == 0)) || (Ic_Comm_RxBuf[Le] == 61) ) return 0;
	else return 1;
}

//***********************************************
//函数名称：FM175xx_Initial
//函数功能：FM175xx芯片初始化
//入口参数：NA
//出口参数：uchar  0：成功   others:失败
/***********************************************
COMMAND =0x20:复位所有寄存器
STATUS2 =0x00:非密文传输
TXMODE =0x00:发射数据时，不使能CRC校验
RXMODE =0x08:接收数据时，不使能CRC校验，接收到的无效数据流会被忽略
RXTRESHOLD =0x55:接收译码器阈值选择，能接收到的最小信号强度
GSN =0xf4:低四位改变调制深度，数值越小，调制深度越小；
RFCFG=0x48:接收器放大倍数和接收灵敏度
WaterLevel =0x20:收到一半数据时候，起中断
TXAUTO =0x00:Force100ASK不会强制产生100%ASK调制
************************************************/
void RF_Fm17xx_Init(void)
{
	unsigned char i;
	
	Pcb_Flag = 0x03;

	RF_Set_Reg(GSN, Def_GSN);	//GSN和CWGSP 保护区可配置
	RF_Set_Reg(CWGSP, Def_CWGSP);
//	i=RF_Get_Reg(GSN);
//	i=RF_Get_Reg(CWGSP);
	for(i=0; i<IntRegLen; i++)
	{
		RF_Set_Reg(Reg_Addr_Int[i], Reg_Data_Int[i]);
//		TEMP=RF_Get_Reg(Reg_Addr_Int[i]);
	}
}

//***********************************************
//函数名称：RF_Request
//函数功能：ReqA
//入口参数：Type：ReqA：0x26 WupA：0x52
//出口参数：uchar  0：成功   others:失败
//          CardA_Sel_Res.ATQA[0]和[1]
//***********************************************
unsigned char RF_Request(void)
{
	unsigned char result;

	Ic_Comm_TxBuf[0] = 0x52; 
	result = RF_Command_Send(FM17520, 1);
	if(result != 2) return 1;
	
	return 0;
}

//*************************************
//函数  名：FM175xx_AntiCol
//入口参数：size:防冲突等级，包括0、1、2，只用到第1级
//出口参数：uchar:0:OK  others：错误
//*************************************
unsigned char RF_Anticoll(void)
{
	unsigned char i, result;
	
	Ic_Comm_TxBuf[0] = 0x93;   					//防冲突等级0x93,0x95,0x97
	Ic_Comm_TxBuf[1] = 0x20;
	result = RF_Command_Send(FM17520, 2);//发送 93 20（0级冲突）
	if(result != 0x05) return 1;	//应返回5字节ID数据(Buf[3]应为接收字节数)
	
	result = 0;
	for(i=0; i<4; i++)
	{
		result ^= Ic_Comm_RxBuf[i];
	}
	if(result != Ic_Comm_RxBuf[4])	return 1;				//BCC 数据校验, UID的异或校验值错
	memcpy( UID, Ic_Comm_RxBuf, 5);
	return 0;
}

//*************************************
//函数  名：FM175xx_Select
//入口参数：size:防冲突等级，包括0、1、2
//出口参数：uchar:0:OK  others：错误
//*************************************
unsigned char RF_Select_Tag(void)
{
	unsigned char result;
	
	RF_Set_Reg(TXMODE, 0x88);//发送数据CRC校验
	RF_Set_Reg(RXMODE, 0x88);//接收数据CRC校验
		
	Ic_Comm_TxBuf[0] = 0x93;   			//防冲突等级0x93,0x95,0x97(最多三重防冲突)
	Ic_Comm_TxBuf[1] = 0x70;
	memcpy(Ic_Comm_TxBuf+2, Ic_Comm_RxBuf, 5);//93 70 +4字节UID+1字节校验
	result = RF_Command_Send(FM17520, 7);	//发送命令
	if(result != 1) return 1;

	if( Ic_Comm_RxBuf[0] == 0x08 ) return(0);//RETURN容量
	else return(1);
}

//*************************************
//函数  名：FM175xx_Rats
//入口参数：FSDI：Reader的帧长度最大值：2^x；CID:0~14,Card的逻辑地址；
//出口参数：uchar:0:OK  others：错误
//*************************************
unsigned char RF_Send_Rats(void)
{
	unsigned char Result;
		
	Ic_Comm_TxBuf[0] = 0xE0;
	Ic_Comm_TxBuf[1] = 0x50;   					//协议激活，最大64字节
	Result = RF_Command_Send(FM17520, 2);
	if( Result == 0x0e ) 
	{
		memcpy(CardRst8B, Ic_Comm_RxBuf+6, 8);
		return(0);
	}
	
	else return 1;
}


//系统内部认证
unsigned char Inter_Authrf08(unsigned char  section)
{
	unsigned char result;
	unsigned char idata  K1[8];
	
	RF_Set_Reg(TXMODE, 0x88);//发送数据CRC校验
	RF_Set_Reg(RXMODE, 0x88);//接收数据CRC校验
	memset(K1, 0xff, 8);
	Ic_Comm_TxBuf[0] = 0x60;   			//防冲突等级0x93,0x95,0x97(最多三重防冲突)
	Ic_Comm_TxBuf[1] = section;
	memcpy(Ic_Comm_TxBuf+2, K1, 6);//93 70 +4字节UID+1字节校验
	memcpy(Ic_Comm_TxBuf+8, UID, 4);
	result = RF_Command_Send(FM17520, 12);	//发送命令
	if(result != 0) return 1;

	return(0);
}

//================ 射频卡寻卡、防冲突、锁定 ==============
unsigned char RF_Init(void)
{
	uint16 i;
	unsigned char Result;
	
	RFSPIRST1;
	for(i=0; i<330; i++)		//延时3ms
	{
		Clear_Wdt();
	}

	RF_Set_Reg(COMMAND, 0x0f);			//复位寄存器
	
	RF_Fm17xx_Init();								//初始化17520相关寄存器
	
	Result = RF_Request();					//寻卡
	if(Result) 
	{
		if(RFTESTCNT) RFTESTCNT=0;
		return(0xff);
	}	
	
	Result = RF_Anticoll();					//防冲突
	if(Result) 
	{
		if(RFTESTCNT) RFTESTCNT=0;
		return(0xff);
	}	
	
	Result = RF_Select_Tag();				//选卡
	
	if(Result) 
	{
		if(RFTESTCNT) RFTESTCNT=0;
		return(0xff);
	}	
	
	RFTESTCNT++;
	if(RFTESTCNT<2) return(0xff);
	
	RFTESTCNT=0;
	return 0;
}

void Release_Rf(void)
{
	RFSPIRST0;				//RST脚：上升沿:正常工作；0:复位
}
