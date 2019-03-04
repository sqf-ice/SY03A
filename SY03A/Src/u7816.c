#include "ex_var.h"
#include "ex_func.h"
//#include <intrins.h>
#include <string.h>
#include "cpu_card.h"

//====================================================================
extern unsigned char RF_TxRxFrame( uint08 Lc, uint08 Le );
extern unsigned char RF_Init(void);
//====================================================================
//卡片或交互终端数据备份区
//3个参数依次为：
//卡片文件号,偏移地址，参数长度+MAC
//====================================================================
const uint08 code BACKBUF[42] = 
{
	0x81, 4,  6+4,		//00参数文件1+MAC（保留1+参数更新标志1+保留4）
	0x81, 15, 15+4,		//01参数文件2+MAC（保留1+报1+报2+变比1+变比2）
	0x83, 0,  52+4,		//02费率文件1+MAC（文件头4+费率12*4）
	0x84, 0,  52+4,		//03费率文件2+MAC（文件头4+费率12*4）
	0x81, 10, 5+4,		//04费率切换时间5+MAC
	
	0x83, 52, 52+4,		//05当前套阶梯文件1:阶梯值24+阶梯电价28+MAC
	0x83, 104,18+4,		//06当前套阶梯文件1:年结算日*6+MAC
	0x83, 122,52+4,		//07当前套阶梯文件2:阶梯值24+阶梯电价28+MAC
	0x83, 174,18+4,		//08当前套阶梯文件2:年结算日*6+MAC
	
	0x84, 52, 52+4,		//09备用套阶梯文件1:阶梯值24+阶梯电价28+MAC
	0x84, 104,18+4,		//10备用套阶梯文件1:年结算日*6+MAC
	0x84, 122,52+4,		//11备用套阶梯文件2:阶梯值24+阶梯电价28+MAC
	0x84, 174,23+4,		//12备用套阶梯文件2:年结算日*6+切换时间5+MAC
	0x81, 36, 6+4,		//13客户编号+MAC	
};

//====================================================================
//带MAC读写参数时，RAM中带MAC备份数据的偏移地址，顺序同BACKBUF函数内容
//表号 6
//客户编号 6
//费率切换时间 5
//阶梯切换时间 5
//费率文件1 16
//费率文件2 16
//当前套阶梯文件 52
//当前年结算日 12
//备用套阶梯文件 52
//备用年结算日 12
//====================================================================
const uint16 code BACKADDR[10] =
	{ 0, 6, 12, 17, 22, 38, 54, 106, 118, 170};	
//====================================================================


void Delay_Netu( unsigned char Num )	//Num*1etu
{
	uint16 i;
	
	for(i=0; i<Num*50; i++);		//1etu=93us(4M),或者1etu=186us(2M)
}

//复位响应数据时间为400/f~40000/f即100us~10ms
//返回值: 1为下降沿; 0为无变化
unsigned char Judge_Edge( void )
{
	uint16 i;

	Delay_Netu(1);

	for( i=0x00; i<3330; i++ ) 	//约10ms(4M),或者20ms(2M)
	{
//		if( (U7816SECONDSTATUS&B0000_0001) == 1 ) //正在接收
		{
			return 1;
		}
	}
		
	return 0;
}

//最长时间应为9600etu,目前为9600etu,1etu=93us
unsigned char Get_7816_Char( unsigned char CH, unsigned char *Data )
{
	unsigned long int i;

	if( CH == ESAM )
	{
//		U7816CTRL0 = B1110_0000;	//U7816_0
	}
	else
	{
//		U7816CTRL0 = B1111_0000;	//U7816_1
	}
	
//	U7816FRMCTRL0 &= B1011_1111;

	for( i=0x00; i<60000; i++ ) 	//约900ms(4M),或者1800ms(2M)
	{
		Clear_Wdt();

//		if( U7816PRIMARYSTATUS&B0000_0001 ) //RX_FLAG已经置位
		{
//			*Data = U7816RXBUF;

//			return 1; //成功
		}

//		if( U7816PRIMARYSTATUS&B0000_0100 ) //ERROR_FLAG已经置位
		{
//			switch( U7816ERRSTATUS ) //所有标志位: 硬件置位，写0清零
//			{
//				case B0000_0001: //接收溢出错误标志位
//					break;
//
//				case B0000_0010: //接收帧格式错误标志位
//					break;
//
//				case B0000_0100: //接收数据奇偶校验错误标志位
//					break;
//
//				case B0000_1000: //发送数据奇偶校验错误标志位
//					break;
//
//				default:
//					break;
//			}

//			U7816ERRSTATUS = B0000_0000; //clear error flag.

			return 2; //error
		}
		
//		if( EA == 0 ) return 3;
	}
	
	return 3;
}

unsigned char Snd_7816_Byte( unsigned char CH, unsigned char Data )
{
	uint16 i;

	if( CH == ESAM )
	{
//		U7816CTRL0 = B1010_0000;	//U7816_0
	}
	else
	{
//		U7816CTRL0 = B1011_0000;	//U7816_1
	}
	
	Delay_Netu(1);				//延时1个etu,保证从设备稳定工作

//	U7816FRMCTRL0 &= B1011_1111; 		//max one time.

//	U7816TXBUF = Data;
	
	Clear_Wdt();

	for( i=0x00; i<7000; i++ ) 	//约39ms(4M),或者78ms(2M)
	{
//		if( U7816PRIMARYSTATUS&B0000_0010 )	//TX_FLAG置位
		{
			return 0; //成功
		}

//		if( U7816PRIMARYSTATUS&B0000_0100)	//ERROR_FLAG置位
		{
//			switch( U7816ERRSTATUS ) //所有标志位: 硬件置位，写0清零
//			{
//				case B0000_0001: //接收溢出错误标志位
//					break;
//
//				case B0000_0010: //接收帧格式错误标志位
//					break;
//
//				case B0000_0100: //接收数据奇偶校验错误标志位
//					break;
//
//				case B0000_1000: //发送数据奇偶校验错误标志位
//					break;
//
//				default:
//					break;
//			}

//			U7816ERRSTATUS = B0000_0000; //clear error flag.

//			return 1; //error
		}
		
//		if( EA == 0 ) return 1;
	}
	
	return 1;
}

void Esam_Err_Report(void)
{
	if( esam_error==0 )
	{
		Set_Reportdata(0*8+1);//置esam错误上报标志
	}
	
	esam_error = 1;
}

//command-response pairs
unsigned char CardTxRxFrame( unsigned char CH, uint08 Lc, uint08 Le )
{
	uint08 k, result, Temp_Data, Temp_SW1, Temp_SW2, Proc_Byte;

	Ic_Comm_RxPtr = 0x00;
	memset( Ic_Comm_RxBuf, 0x00, u7816_buf_size );
	Delay_Netu(10);	//再延时10个etu
	Err_Sw12[0] = 0;
	Err_Sw12[1] = 0;
	
	if(CH == CARD)
	{
		result = RF_TxRxFrame(Lc, Le);
		return (result);
	}
	
	for( k=0; k<5; k++ )	//send command header (5bytes)
	{
		result = Snd_7816_Byte( CH, Ic_Comm_TxBuf[ k ] );	//;取待发送数据
		if( result == 1 ) { return 1; } //send failure.
	}

	for( ;; )
	{
		result = Get_7816_Char( CH, &Proc_Byte ); //receive procedure byte.
		if( result != 1 ) { return 2; } //receive failure.

		if( Proc_Byte == 0x60 ) //Null byte
		{
			//do nothing. continue to receive.
		}
		else if( Proc_Byte == Ic_Comm_TxBuf[1] ) //procedure byte = INS
		{
			if( k < (Lc+5) )	//send remaining data, then wait for procedure byte.
			{
				if( CH == ESAM ) Delay_Netu(16);	//接收到INS后与发送数据之间至少延时16个etu
				
				for( ; k<(Lc+5); )
				{
					result = Snd_7816_Byte( CH, Ic_Comm_TxBuf[ k++ ] ); //;取待发送数据
					if( result == 1 ) { return 3; }	//send failure.
				}
			}
			else
			{
				//;过程字节在之前已接收，只增加2个状态字节即可。
				for( Ic_Comm_RxPtr=0x00; Ic_Comm_RxPtr<Le; Ic_Comm_RxPtr++ )
				{
					result = Get_7816_Char( CH, &Temp_Data );
					if( result != 1 )	{ return 4; }	//;Receive fail
			
					Ic_Comm_RxBuf[ Ic_Comm_RxPtr ] = Temp_Data;
				}
			}
		}
//		else if( Proc_Byte == (Ic_Comm_TxBuf[1]^0xFF) ) //procedure byte = (INS XOR FFH)
//		{
//			if( k < (Lc+5) )	//send next one data, then wait for procedure byte.
//			{
//				result = Snd_7816_Byte( CH, Ic_Comm_TxBuf[ k++ ] ); //;取待发送数据
//				if( result == 1 ) { return 5;}	//send failure.
//			}
//			else
//			{
//				//;过程字节在之前已接收，只增加2个状态字节即可。
//				for( Ic_Comm_RxPtr=0x00; Ic_Comm_RxPtr<( Le+2 ); Ic_Comm_RxPtr++ )
//				{
//					result = Get_7816_Char( CH, &Temp_Data );
//					if( result != 1 )	{	return 6;	}	//;Receive fail
//			
//					Ic_Comm_RxBuf[ Ic_Comm_RxPtr ] = Temp_Data;
//				}
//
//				return 0;
//			}
//		}
		else	//other proc byte.
		{
			Temp_SW1 = ( Proc_Byte & 0xF0 );
			
			if( (Temp_SW1 == 0x60) || (Temp_SW1 == 0x90) ) //procedure byte = 6X or 9X (ex. 60H)	
			{
				result = Get_7816_Char( CH, &Temp_SW2 ); //receive sw2.
				if( result != 1 ) { return 7; } //receive failure.
		
				// 61H: send get response command, P3<=XX
				// 6CH: send last command again, P3 = XX;
				// other 6X or 9X, this communication is over.

				//'9000' command normally completed
				//'6E00' CLA not supported
				//'6D00' CLA supported, but INS not programmed or invalid
				//'6B00' CLA INS supported, but P1 P2 incorrect
				//'6700' CLA INS P1 P2 supported, but P3 incorrect
				//'6F00' command not supported and no precise diagnosis given
		
				Ic_Comm_RxBuf[ Ic_Comm_RxPtr++ ] = Proc_Byte;
				Ic_Comm_RxBuf[ Ic_Comm_RxPtr++ ] = Temp_SW2;
				Err_Sw12[0] = Proc_Byte;
				Err_Sw12[1] = Temp_SW2;
				
				if( (Proc_Byte == 0x6A) && (Temp_SW2 == 0x81)&& (CH == ESAM) ) Esam_Err_Report();
				
				if( (Proc_Byte == 0x61) || ((Proc_Byte == 0x90)&&(Temp_SW2 == 0x00)) )
					return 0;
				else
					return 8;
			}
			else
			{
				return 8;
			}
		}
	}

//	return 9;
}

//触点激活&冷复位
//检测到卡插入后, RST置低电平, VCC上电, 延时一段时间, IO设置为输入态, CLK输出时钟信号(T0时刻)
//在T0后40000~45000个CLK内, 将RST置为高电平(T1时刻), 在T1后的400~40000个CLK内, 复位应答应该开始, 
//若IC卡的复位应答没有在此时间段内开始, 终端将启动释放时序.若收到复位应答, 但不符合协议规范, 则需要热复位
unsigned char ColdReset( unsigned char CH )
{
	unsigned char n, Result, Temp_ATR;	// T0, TD1, TD2;
	
	if( CH == ESAM )
	{
//		GPIO41CFG = B0110_0010; 	//SAMIO,
		RST_ESAM_L;	
	}
	ESAMPOWERON;
//	GPIO40CFG = B0100_0010; 		//MCUCLK
	Delay_Initial(20);			//延时100ms
	
	if( CH == 0x00 )
	{
//		U7816CTRL0 = B0110_0000;	//U7816_0
	}
	else
	{
//		U7816CTRL0 = B0111_0000;	//U7816_1
	}
	
	//----------------------------------------------------------------------------------------------
//	Result = U7816RXBUF; //RX启动后, 会产生rx_flag=1, 必须通过读取RXBUF来清除此标志.

//	U7816ERRSTATUS = 0x00;    //这三个寄存器的清零顺序很重要, 不能颠倒
//	U7816SECONDSTATUS = 0x00;
//	U7816PRIMARYSTATUS = 0x00;

	if( CH == 0x00 )
	{
		RST_ESAM_L;		//clr PGIO4.3 to reset U7816_0
	}
	else
	{
		RST_CARD_L;		//clr PGIO4.4 to reset U7816_1
	}

	I2c_Wait_5ms();			//等待RST时间,至少400周期,即100us
	
	//-------- T1时刻, RST输出高电平 --------
	if( CH == 0x00 )
	{
		RST_ESAM_H;		//set PGIO4.3
	}
	else
	{
		RST_CARD_H;		//set PGIO4.4
	}

	Result = Judge_Edge(); 		//40000个CLK内查看IO口的电平沿变化
	if( Result == 0 ) //IO口没有出现下降沿
	{
		//if(CH == ESAM) Esam_Err_Report();
		return 2;	//异常: 40000个CLK中没有收到起始位
	}
	
	for(n=0; n<13; n++)
	{
		Result = Get_7816_Char( CH, &Temp_ATR ); //receive TS.
		if( Result != 1 )
		{ 
			//if(CH == ESAM) Esam_Err_Report();
			return 3; 
		}
		Ic_Comm_RxBuf[ n ] = Temp_ATR;
	}
	
	if((Ic_Comm_RxBuf[0] != 0x3b) || (Ic_Comm_RxBuf[1] != 0x69))
	{
		//if(CH == ESAM)	Esam_Err_Report();
		return 3; 
	}
//	if(CH == ESAM) esam_error = 0;	
	return 0;
}

//ESAM复位
unsigned char EsamReset( void )
{
	unsigned char i;
	
	for(i=0; i<3; i++)
	{
		if( FLAG_EA == 0 ) return(1);		//掉电不处理,程序直接返回
		
		if(ColdReset( ESAM )) 
		{
//			CloseEsam();		//释放ESAM
			Delay_Initial(40);	//延时200ms		
		}
		else
		{
			esam_error = 0;
			memcpy( EsamRst8B, Ic_Comm_RxBuf+5, 8 ); //ESAM序列号, 8bytes.
			return(0);
		}
	}
	Esam_Err_Report();
	return(1);
}

//禁止ESAM工作
//void CloseEsam( void )
//{
//	RST_ESAM_L;
////	U7816CTRL0 &= B1101_1111;	//关闭CLK
//	ESAMPOWEROFF;
//	
////	GPIO40CFG = B0100_0000;	 	//MCUCLK作为输出
////	GPIO41CFG = B0100_0000; 	//SAMIO作为输出
////	GPIO4 &= ( B1111_1100);		//都输出低电平	
//}

//复位ESAM
unsigned char Clear_Rand_Gene( void )
{
	unsigned char result=0;
	
	Identitytimer.B32 = 0x00;
	identity_prog_flag = 0;	//清除 安全认证编程标志
	terminal_auth_flag = 0;	//清除 交互终端认证标志
	first_prog_flag = 0;//清首次编程标志

	memset( PcRand,  0x00, 8 );
	memset( PcRst8B, 0x00, 8 );

//	result = EsamReset();

	return (result);
}

//底层通讯
void RxTxCommhead( uint08 inc, uint08 com, uint08 P1, uint08 P2, uint08 len)
{
	Ic_Comm_TxBuf[ 0] = inc;
	Ic_Comm_TxBuf[ 1] = com;
	Ic_Comm_TxBuf[ 2] = P1;
	Ic_Comm_TxBuf[ 3] = P2;
	Ic_Comm_TxBuf[ 4] = len;
}

unsigned char ReadBinFile( unsigned char CH, unsigned char P1, unsigned char P2, unsigned char Len )
{return 0;

}
//读二进制文件
unsigned char ReadM1File( unsigned char  section,unsigned char Block )
{
	unsigned char Result;	
	
	Ic_Comm_TxBuf[0] = 0x30;   			//防冲突等级0x93,0x95,0x97(最多三重防冲突)
	Ic_Comm_TxBuf[1] = section+Block;
	
	Result = RF_Command_Send(FM17520, 2);	//发送命令
	
	if(Result != 0x10) return 1;	
	return 0;
}
unsigned char WriteM1File( unsigned char  section,unsigned char Block, unsigned char *BufA, unsigned char Len)
{
	unsigned char Result;	
	
	Ic_Comm_TxBuf[0] = 0xa0;   			//
	Ic_Comm_TxBuf[1] = section+Block;
	
	Result = RF_Command_Send(FM17520, 2);	//发送命令
	
	if((Ic_Comm_RxBuf[0]!=0x0a)&&(Result != 1) ) return 1;	
	
	memcpy(&Ic_Comm_TxBuf[0],BufA, Len);
	
	Result = RF_Command_Send(FM17520, Len);	//发送命令
	
	if((Ic_Comm_RxBuf[0]!=0x0a)&&(Result != 1) ) return 1;	
	return 0;
}


//更新二进制文件
unsigned char UpdateBinFile( uint08 CH, uint08 P1, uint08 P2, uint08 Len )
{
	unsigned char Result;

	RxTxCommhead(0, 0xD6, P1, P2, Len);
	
	memcpy(&Ic_Comm_TxBuf[5], Ic_Comm_RxBuf, Len);
				
	Result = CardTxRxFrame( CH, Len, 0x00 ); //CARD Com
	
	return (Result);
}

//读记录文件
unsigned char ReadRecFile( unsigned char P1 )
{
	unsigned char Result;

	RxTxCommhead(0, 0xb2, P1, 0x14, 0x04);
	
	Result = CardTxRxFrame( ESAM, 0x00, 0x04 );

	return (Result);
}

//取响应数据
unsigned char Read_Kx( unsigned char CH, unsigned char Len )
{
	unsigned char result;

	RxTxCommhead(0, 0xC0, 0, 0, Len);
	
	result = CardTxRxFrame( CH, 0x00, Len ); //Esam Com

	return (result);
}

//取随机数
unsigned char ReadRandom( unsigned char CH, unsigned char Len )
{
	unsigned char result;
	
	RxTxCommhead(0, 0x84, 0, 0, Len);
	
	result = CardTxRxFrame( CH, 0x00, Len );
	
	if( result == 0 )
	{
		if(CH == ESAM)	//取ESAM随机数
		{
			memcpy(EsamRand, Ic_Comm_RxBuf, Len);
		}
		else		//取卡片随机数
		{
			memcpy(CardRand, Ic_Comm_RxBuf, Len);
		}
	}
	
	return (result);
}

//加密，内部和外部认证命令
unsigned char Auth_In_Out( unsigned char INS, unsigned char CH, unsigned char P2, unsigned char *BufA )
{
	unsigned char Result;

	if(INS == 0xfa)
	{
		RxTxCommhead(0x80, INS, 0, P2, 0x08);
	}
	else
	{
		RxTxCommhead(0, INS, 0, P2, 0x08);
	}

	memcpy(&Ic_Comm_TxBuf[5],BufA, 8);	

	Result = CardTxRxFrame( CH, 0x08, 0x00 ); //CARD Com

	return (Result);
}

unsigned char Cal_Mac_Pro(unsigned char P2, unsigned char *BufA , unsigned char Len)
{
	unsigned char Result;

	RxTxCommhead(0x80, 0xfa, 1, P2, Len);		//程序对比：MAC计算
	memcpy(&Ic_Comm_TxBuf[5],BufA, Len);	

	Result = CardTxRxFrame( ESAM, Len, 0x00 ); 	//CARD Com
	if(Result) return 1;
	
	Result = Read_Kx( ESAM, 64 );			//取ESAM加密结果
	return (Result);
}


//由带MAC抄读命令中的M2选组
const uint08 code ConstIns[12] =
	{ 
		0x04, 0xd6, 0x86,	//正常带MAC读卡(0x86为带MAC读ESAM用)
		0x84, 0x32, 0x01,	//用户卡读钱包文件
		0x04, 0xdc, 0x01,	//预制卡读剩余金额
		0x04, 0xdc, 0x03,	//预制卡读购电次数
	};


//=====带MAC读取二进制文件
//Le：带MAC读命令的数据长度
unsigned char ReadBinWithMac( uint08 CH, uint08 P1, uint08 P2, uint08 Le )
{
	unsigned char result, Temp, Type;
	
	RxTxCommhead(0x04, 0xB0, P1, P2, 0x11);
	Clear_Wdt();
	
	Type = (CH&B1111_0000)>>4;
	if( (CH&B0000_1111) == CARD )	//对CARD操作
	{
		Ic_Comm_TxBuf[4] = 9;
		memcpy(Ic_Comm_TxBuf+5, EsamRand, 4);//对卡片操作，取ESAM随机数
		
		memcpy( Ic_Comm_TxBuf+9, ConstIns+Type*3, 3 );
		if( Type == 0 )
		{
			Ic_Comm_TxBuf[11] = P1;	//带MAC读卡片参数，文件号和偏移地址同ESAM
			if( P1==0x85 ) Ic_Comm_TxBuf[11] = 0x87;//合闸复电命令
			Ic_Comm_TxBuf[12] = P2;
		}
		else
		{
			Ic_Comm_TxBuf[12] = 0x14;
		}
		Temp = Le;	//卡片直接返回数据，不需取数
	}
	else//对ESAM操作
	{
		if( Comm_Auth_Judge() )//远程带mac读
			memcpy(Ic_Comm_TxBuf+5, PcRand, 4);
		else
			memcpy(Ic_Comm_TxBuf+5, CardRand, 4);

		memcpy( Ic_Comm_TxBuf+9, ConstIns, 3 );
		
		Ic_Comm_TxBuf[12] = P2;
		if( P1<0x04 )
		{
			Ic_Comm_TxBuf[ 1] = 0xB2;	//带MAC读钱包，命令字0xB2
			Ic_Comm_TxBuf[12] = 0x00;
		}
		
		if(CH == ESAM)
			memcpy(Ic_Comm_TxBuf+14, CardRst8B, 8);
		else
			memcpy(Ic_Comm_TxBuf+14, PcRst8B, 8);
		
		Temp = 0;
	}
	Ic_Comm_TxBuf[13] = Le;
	if(CH == ESAM+0x10) Ic_Comm_TxBuf[13] += 8;	//数据回抄和状态查询需+8字节
	
	CH &= B0000_0001;
	result = CardTxRxFrame( CH, Ic_Comm_TxBuf[4], Temp );//返回data+MAC
	if( result ) return (result);
	
	if(CH == ESAM)
	{
		result = Read_Kx(ESAM, Le ); //返回data+MAC
	}

	return ( result );
}

//======带MAC更新二进制文件
unsigned char WriteBinWithMac( uint08 CH, unsigned char P1, unsigned char P2, unsigned char Len )
{
	unsigned char  Result;

	RxTxCommhead(0x04, 0xd6, P1, P2, Len);
	if(CH>1) Ic_Comm_TxBuf[1] = 0xdc;		//带MAC写钱包文件
	if(Len == 4) Ic_Comm_TxBuf[ 0 ] = 0;//CLA 不带MAC
	CH &= B0000_0001;

	memcpy(&Ic_Comm_TxBuf[5], Ic_Comm_RxBuf, Len);

	Result = CardTxRxFrame( CH, Len, 0x00 ); //CARD Com

	if( Result )
	{
		if( (Err_Sw12[0] == 0x69) && (Err_Sw12[1] == 0x88) ) return(IC_MAC_ERR);//MAC校验错误(6988)
		else if(CH == ESAM) return(IC_ESAM_RDWR_ERR);
		else return(IC_NOIN_ERR);
	}
	return(Result);
}


unsigned char Authorization( unsigned char *Buf, unsigned char Len )
{
	unsigned char Result;

	//电能表对ESAM操作命令详解：
	//a）密钥分散命令：80fa000108+8字节分散因子；
	//;Esam encryp random data.
	Result = Auth_In_Out(0xfa, ESAM, 0x03, Buf+16 );
	if( Result != 0x00 )	return 1;
	
	//b）加密随机数命令：80fa000008+随机数；
	Result = Auth_In_Out(0xfa, ESAM, 0x00, Buf+8 );
	if( Result != 0x00 )	return 1;
	
	//c）取密文命令：00c0000008；
	//比较密文1和密文2, 两者不相等则报警, 相等则继续下面操作
	//;Read K2 from Esam
	Result = Read_Kx( ESAM, 0x08 );
	if( Result != 0x00 )	return 1;
	
	if( Len == 4 )
	{
		if( Cmp_Data(Buf, Ic_Comm_RxBuf, 8) != 3 ) return 1;//Reset failure.
	}
	else
	{
		memcpy( Buf, Ic_Comm_RxBuf, 8 );
	}

	//取随机数2：0084000004
	//;Read random in ESAM
	Result = ReadRandom( ESAM, Len );
	return ( Result );
}


//选择文件
unsigned char SelectFile( unsigned char CH, unsigned char F1, unsigned char F2 )
{
	unsigned char Result;
	
	RxTxCommhead(0,0xa4, 0, 0, 2);	
	Ic_Comm_TxBuf[ 5 ] = F1;	//DATA0
	Ic_Comm_TxBuf[ 6 ] = F2;	//DATA1
	
	Result = CardTxRxFrame( CH, 0x02, 8 );	//返回8字节
	
	return (Result);
}


//判断卡片数据格式
unsigned char JudgeFile(unsigned char Len)
{
	unsigned char Csdata;

	Csdata = Cal_ChkSum(Ic_Comm_RxBuf+1, Len+3);
	if( (Ic_Comm_RxBuf[0] != 0x68) || (Ic_Comm_RxBuf[Len+4] != Csdata) || (Ic_Comm_RxBuf[Len+5] != 0x16) ) return 1;//帧格式错误: 68H/CS/16H
	return 0;
}

//卡片内部认证
unsigned char Auth_CARD_IN(unsigned char CH)
{
	unsigned char Result;
	
	RxTxCommhead(0, 0x83, 1, 2, 0x10);
	
	if(CH == ESAM)
	{
		memcpy(&Ic_Comm_TxBuf[5], CardRst8B, 8);	
		memcpy(&Ic_Comm_TxBuf[5+8], CardRand, 8);
		Result = CardTxRxFrame( CH, 0x10, 0x00 );
	}
	else
	{
		Ic_Comm_TxBuf[ 1] = 0x86;
		memcpy(&Ic_Comm_TxBuf[5], Ic_Comm_RxBuf, 16);
		Result = CardTxRxFrame( CH, 0x10, 0x08 );
	}
	return (Result);
}

//系统内部认证
unsigned char Inter_Auth(void)
{
	unsigned char Result;
	unsigned char idata  K1[8];
	
	
	Result = ReadRandom( CARD, 8 );			//从CARD取8字节随机数
	if(Result) return(IC_NOIN_ERR);
		
	Result = Auth_CARD_IN(ESAM);
	if(Result) return(IC_ESAM_RDWR_ERR);
	
	Result = Read_Kx(ESAM, 0x18 );			//取ESAM加密结果K1
	if(Result) return(IC_ESAM_RDWR_ERR);
		
	memcpy(K1, Ic_Comm_RxBuf+16, 8);
	
	Result = Auth_CARD_IN(CARD);
	if(Result) return(IC_NOIN_ERR);
	
	if ( Cmp_Data( K1, Ic_Comm_RxBuf, 8) != 3 ) return(IC_AUTH_ERR);//身份认证错误(通信成功但密文不匹配)
	
	return 0;
}

//卡片外部认证
unsigned char Card_Ext_Auth( unsigned char P2 )
{
	unsigned char Result;

	Result = ReadRandom( CARD, 8 );			//从卡片取8字节随机数
	if(Result) return(IC_NOIN_ERR);
	
	Result = Auth_In_Out(0xfa, ESAM, 0x04, CardRst8B ); //ESAM分散外部认证密钥
	if(Result) return(IC_ESAM_RDWR_ERR);
	
	Result = Auth_In_Out(0xfa, ESAM, 0x00, CardRand );//ESAM加密随机数
	if(Result) return(IC_ESAM_RDWR_ERR);
	
	Result = Read_Kx( ESAM, 0x08 );			//取加密结果
	if(Result) return(IC_ESAM_RDWR_ERR);

	Result = Auth_In_Out(0x82, CARD, P2, Ic_Comm_RxBuf );//卡片做外部认证
	if( Result ) return(IC_EXAUTH_ERR);

	return 0;
}


//======带MAC存款操作
unsigned char IncreseWithMac( void )
{
	unsigned char  Result;

	RxTxCommhead(0x84, 0x32, 0x01, 0x14, 8+4);

	memcpy(&Ic_Comm_TxBuf[5], Ic_Comm_RxBuf, 8+4);
			
	Result = CardTxRxFrame( ESAM, 8+4, 0x00 ); //CARD Com
		
	if( Result )
	{
		if( (Err_Sw12[0] == 0x69) && (Err_Sw12[1] == 0x88) ) return(IC_MAC_ERR);//MAC校验错误(6988)
		else return(IC_ESAM_RDWR_ERR);	//操作ESAM错误
	}
	return (Result);
}


//======带MAC扣款操作
unsigned char DecreaseWithMac( unsigned char *InBuf )
{
	unsigned char Result;
	
	//04D68C0014+密文+MAC
	//00B08C0008
	//8430020C08+4字节退费金额+4字节MAC
	Result = Update_ESAM_Bin_Mac( 0x99, 0x00, InBuf, 16 );
	if( Result ) return 0x04;

	Result = ReadBinFile( ESAM, 0x99, 0x00, 8 );
	if( Result ) return 0x01;

	RxTxCommhead( 0x84, 0x30, 0x02, 0x14, 0x08 );

	memcpy(&Ic_Comm_TxBuf[5], Ic_Comm_RxBuf, 8);
	memcpy(InBuf, &Ic_Comm_TxBuf[5], 8);
	
	Result = CardTxRxFrame( ESAM, 8, 0x00 ); //CARD Com
	if( Result ) return 0x04;
	
	return 0x00;
}

//检测卡片
unsigned char Check_Card( void )
{
	if( (CardCheck == 0) && (CardKeySamp==0xff) )
	{
		CardCheck = 1;			//有新卡片插入
		Beep_Timer = 0;
		return 0;
	}
	
	if( (CardCheck == 1) && (CardKeySamp==0x00) )
	{
		CardCheck = 0;			//卡片已经拔走
	}
	return 1; 				//没有新卡片插入
}

//判断剩余和允许合闸金额
void Judge_Closerelay_Money()
{			
	uint08 Result;
	Result = Cmp_Data( Money.Remain, Money_Para.Closerelay.B08, 4 );
	if( Result > 1)//剩余金额<=合闸限额
	{
		if( remainder_maxtick_flag && relay_status_flag ) //透支拉闸才判断合闸允许门限		
		{
			charge_relay_open_flag = 1;
		}
	}
	else
	{
		charge_relay_open_flag = 0;
		remainder_maxtick_flag = 0;
	}
}


void Get_Usercard_Serial(void)
{
	unsigned char Buf[16];
	
	I2C_Read_Eeprom44(ee_usercard_serial_page, Buf, ee_usercard_serial_lenth);
	if(Buf[4] == 0x55) card_open_flag = 1;	//本地已开户
	else card_open_flag = 0;
	if(Buf[5] == 0x55) comm_open_flag = 1;	//远程已开户
	else comm_open_flag = 0;
	UserCardSerialBit = 0;
	if ( Cmp_Data( UID, Buf, 4) != 3 ) UserCardSerialBit = 1;	//卡序号不相等
}

//保存用户卡序列号和开户标志
//Type=0: 保存本地开户标志和用户卡序列号
//Type=1: 保存远程开户标志
void Updata_Usercard_Serial( unsigned char Type )
{
	unsigned char Buf[16];
	
	I2C_Read_Eeprom44(ee_usercard_serial_page, Buf, ee_usercard_serial_lenth);
	if(Type == 0)
	{
		if(UserCardFlag == 0x01) Buf[4] = 0x55;	//只有开户卡置本地开户标志
		memcpy(Buf, UID, 4);
	}
	if(Type == 1)	Buf[5] = 0x55;	//远程开户标志
	I2C_Write_Eeprom44(ee_usercard_serial_page, Buf, ee_usercard_serial_lenth);
}

//保存esam参数数据到ee
void Store_Esampara(void)
{
	WriteE2WithBackup(ee_esampara_page, ee_esampara_inpage, Esam_Para.Alarm_Limit1.B08, ee_esampara_lenth);
	I2C_Read_Eeprom(ee_esampara_page, ee_esampara_inpage, Esam_Para.Alarm_Limit1.B08, ee_esampara_lenth);
}


//保存剩余金额数据到ee
void Store_Remainmoney(void)
{
	WriteE2WithBackup( ee_remainmoney_page, ee_remainmoney_inpage, Remain_back, ee_remainmoney_lenth );
	I2C_Read_Eeprom(ee_remainmoney_page, ee_remainmoney_inpage, Money.Remain, ee_remainmoney_lenth);
}

//准备钱包数据
//数据备份区：剩余金额BCD6(字节)+透支金额BCD(6字节)+购电次数HEX(4字节)+购电前剩余金额BCD(4字节)
unsigned char Ready_Remainback(void)
{
//	unsigned char result;
	
//	result = ReadRecFile(3);		//读钱包购电次数
//	if(result) return 1;
	
	memcpy( Remain_back, Money.Remain, 12 );				//剩余透支BCD放到备份区
	memset( Remain_back+12, 0, 4 );			//购电次数以esam为准
	memcpy( Remain_back+16, Money.Remain+2, 4); 			//充值前剩余金额
	if(remainder_tick_flag)
	{
		memcpy( Remain_back+20, Money.Tick+2, 4); 	//充值前透支金额
		disp_over_flag = 1;
	}
	return 0;
}

//钱包处理
unsigned char Moneybag_Process(void)		
{
	unsigned char i, result;
	union B32_B08 Temp32_Ram, Temp32_Esam;
	unsigned char BufA[64];

	for(i=0; i<4; i++)
	{
		result = ReadRecFile((i&B0000_0010)+1);		//读钱包剩余金额(1)和购电次数(3)
		if(result) return(result);
		memcpy( BufA+4*i, Ic_Comm_RxBuf, 4 );
	}
	
	if( (Cmp_Data( BufA, BufA+4, 4) != 3) || (Cmp_Data( BufA+8, BufA+12, 4) != 3)  ) return 0;//二次读不一致则退出程序
	
	Remainmoney_Verify();	//剩余金额参数校验
	i = Cmp_Data( BufA+8, Money.Buy_Times.B08, 4);		//比较购电次数是否相等,i=3表示相等
	memcpy( Temp32_Esam.B08, BufA, 4 );			//Temp32_Esam 暂存ESAM 剩余金额
	Bcd2Hex(&Money.Remain[0], Temp32_Ram.B08, 4);		//Temp32_Ram 暂存剩余金额HEX
	
	if( (i==3) && (Temp32_Esam.B32 > Temp32_Ram.B32) )	//购电次数相等同时剩余ram<钱包,以ram为准
	{		
		Temp32_Esam.B32 -= Temp32_Ram.B32;		//钱包 - 剩余
		
		memcpy(&Ic_Comm_TxBuf[5], Temp32_Esam.B08, 4);
	
		RxTxCommhead( 0x80, 0x30, 0x01, 0x14, 0x05 );
		
		Ic_Comm_TxBuf[ 9 ] = Cal_ChkSum(Ic_Comm_TxBuf, 9);
	
		CardTxRxFrame( ESAM, 0x05, 0x00 );
		
		if((Decimal.Status & B0000_0001) == 0) WriteE2WithBackup( ee_remainmoney_page, ee_remainmoney_inpage, Money.Remain, ee_remainmoney_lenth );//剩余金额写数据ee(三备份同步)	
	}
	else if( (i!=3) || (Temp32_Esam.B32 < Temp32_Ram.B32) )	//购电次数不等或者剩余ram>钱包,以esam为准
	{
		//钱包金额恢复数据ee金额
		memset( Remain_back ,0 ,ee_remainmoney_lenth );
		Hex2Bcd( Temp32_Esam.B08, Remain_back, 4 );	//钱包剩余金额转换成ram剩余金额
		memcpy( Remain_back+12, BufA+8, 4 );		//购电次数以esam为准
		
		memcpy( BufA+16, Sclock.Clockbackup, 7 );	//备份覆盖时间
		memcpy( BufA+23, Temp32_Ram.B08, 4 );		//备份覆盖前ram剩余金额
		memcpy( BufA+27, Money.Buy_Times.B08, 4 );	//备份覆盖前ram购电次数
		I2C_Write_Eeprom( ee_esam_money_page, ee_esam_money_inpage, BufA+16, ee_esam_money_lenth );//恢复金额备份
		run_alarm_flag = 1;
		
		Store_Remainmoney();	//保存剩余金额数据到ee	
	}
	
	Decimal.Status &= B1111_1101;		//清金额变化标志
	
	return 0;	
}

//钱包处理1(计量和同步专用)
void Moneybag_Process1(void)
{
	if( Moneybag_Process() ) 
	{
		EsamReset();		//复位
		Moneybag_Process();	//再次钱包处理
	}
}

//钱包处理2(通讯专用)
void Moneybag_Process2(void)
{
	if( (Decimal.Status & B0000_0010) == 0 ) return;	//金额未发生变化不更新钱包
	
	if( UpdateMoneytimer <= default_updatemoneytimer )
	{
		Moneybag_Process();//更新ESAM钱包
		UpdateMoneytimer = default_updatemoneytimer+15;//国网要求15min内的连续查询金额命令,只要第一次扣减
	}
}


void GetBuyTimes2Byte( unsigned char Offset )
{
	union B32_B08 Temp32;
	
	Hex2Bcd( Money.Buy_Times.B08, Temp32.B08, 4 );
	Eeprom.Buffer[Offset+0] = 0;	//购电次数(bcd)
	Eeprom.Buffer[Offset+1] = 0;
}

//比较表号是否相同
unsigned char Cmp_Meter_Id(unsigned char *Buf)
{
	if ( Cmp_Data( Esam_Para.Meter_Id, Buf, 6) != 3 ) return 1;
	return 0;
}

//异常插卡记录
void Card_Err_Record(void)
{
	if( (Keyswitch_Valid == 0x55)  && (Ic_Err_Reg == IC_NOIN_ERR) && (Find_Card != 0x55)) Ic_Err_Reg = IC_PREIC_ERR;	//提前拔卡
	
	//------ 异常插卡 记录------
	memcpy( Eeprom.Buffer+6, UID, 4);				//卡序列号
	memset(Eeprom.Buffer+10, 0x00, 4);
	Eeprom.Buffer[14] = Ic_Err_Reg;				//错误信息字
	
	if((Ic_Err_Reg==IC_ESAM_RDWR_ERR)||(Ic_Err_Reg==IC_NOFX_ERR)||(Ic_Err_Reg==IC_MAC_ERR) 
		  ||(Ic_Err_Reg == IC_NOIN_ERR)||(Ic_Err_Reg == IC_EXAUTH_ERR))
	{
		Exchange( Eeprom.Buffer+15, Ic_Comm_TxBuf, 5 );	//操作命令头
		Eeprom.Buffer[20] = 0xFF;	//错误响应状态SW2
		Eeprom.Buffer[21] = 0xFF;//错误响应状态SW1
	}
	else
	{
		memset(Eeprom.Buffer+15, 0x00, 7);
	}
	
	GetBuyTimes2Byte(22);	//购电次数(bcd)
	
	memcpy( Eeprom.Buffer+24, Money.Remain+2, 4 );	//剩余金额
	
	DltoEepromBuf( 28 );	//正反向有功总电量
	
	Event_Record( ee_errcardn_inpage );
	//------------- end ----------------
	
	//记录非法插卡次数（身份认证错误，MAC校验错误，用户卡外部认证错误）
	if((Ic_Err_Reg == IC_AUTH_ERR) || (Ic_Err_Reg == IC_MAC_ERR) || (Ic_Err_Reg == IC_EXAUTH_ERR) || (Ic_Err_Reg == IC_NOFX_ERR))
	{
		Keeptime.Times[0].B32++; //总次数+1
	}
}

//购电/退费记录
void Buy_Record(unsigned char Inpage)
{
	unsigned char result;
	
	if( Inpage == ee_buymoneyn_inpage )
	{
		result = Read_Event_Point( Inpage );	//购电事件记录指针
		I2C_Read_Eeprom( ee_buydl_note_page+result, ee_buydlnote_inpage, Eeprom.Buffer, ee_buydlnote_lenth );
	}
	
	GetBuyTimes2Byte(5); //购电次数(bcd)
	memcpy( Eeprom.Buffer+7, Buy_Money_Bcd, 4 );		//购电金额
	memcpy( Eeprom.Buffer+11, Remain_back+16, 4 );	//购电前剩余金额
	
	memcpy( Eeprom.Buffer+15, Money.Remain+2, 4 );		//购电后剩余金额
	
	if( Inpage == ee_buymoneyn_inpage )
	{
		if( RdRecTimes( Inpage ) == 0 )//判断有效指针,如果为0从头开始计算次数
		{
			memcpy( Eeprom.Buffer+19, Buy_Money_Bcd, 4 );	//购电后累购金额
		}
		else
		{
		//	Inverse( Eeprom.Buffer+19, 4 );		//转换成高字节在前
			Add_Bcd( Eeprom.Buffer+19, Buy_Money_Bcd, 4);	//购电后累购金额
			//Inverse( Eeprom.Buffer+19, 4 );	//转换成低字节在前
		}
		
		memset( Eeprom.Buffer, 0xff, 4 );	//操作者代码设置成0xff
		I2C_Write_Eeprom( ee_operatorcode_page, ee_operatorcode_inpage, Eeprom.Buffer, 4 );
	}
	
	Event_Record( Inpage );
}

//==================================================
//P1 != 0：带MAC从卡片读数据到备份区
//P1 == 0：将备份数据+MAC写入ESAM
//==================================================
unsigned char Backup_With_Mac(uint08 Type)
{
	unsigned char Result, Ramaddr;
	
	Ramaddr = Type*3;
	Result = ReadBinWithMac( CARD, BACKBUF[Ramaddr], BACKBUF[Ramaddr+1], BACKBUF[Ramaddr+2] );	//带MAC读卡片相关参数
	memcpy(&Refers_File_Mac[BACKADDR[Type]], Ic_Comm_RxBuf, BACKBUF[Ramaddr+2]);//保存参数+MAC到相应备份区
	return( Result );
}

unsigned char BackupW_With_Mac(uint08 Type)
{
	unsigned char Result, Ramaddr;
	
	Ramaddr = Type*3;
	memcpy(Ic_Comm_RxBuf, &Refers_File_Mac[BACKADDR[Type]], BACKBUF[Ramaddr+2]);											//复制RAM备份区数据
	Result = WriteBinWithMac( ESAM, BACKBUF[Ramaddr], BACKBUF[Ramaddr+1], BACKBUF[Ramaddr+2] );//带MAC写入ESAM相关参数
	return( Result );
}

void Updata_Feilv_Page(unsigned char CH)
{
	Clear_Wdt();
	I2C_Write_Eeprom(ee_tariffprice1_page+CH, 0, &Refers_File_Mac[BACKADDR[CH+4]], ee_tariffprice_lenth);//第1套费率电价
	Price.Crc16c.B16 += 1;	
}

//更新当前(0/1)/备用套(2/3)阶梯电量、阶梯电价、年结算日
void Updata_Ladder_Page(unsigned char CH)
{
	unsigned char Temp08;
	
	Clear_Wdt();
	Temp08 = CH;
	if(Temp08 > 1) Temp08 += 2; 
//	LoopInverse(&Refers_File_Mac[BACKADDR[6+CH*2]], 6, 3 ); //年结算日字节交换顺序
//	memcpy(&Refers_File_Mac[BACKADDR[6+CH*2]]-4, &Refers_File_Mac[BACKADDR[6+CH*2]], 18);
	
	//阶梯电量及电价
	WriteE2WithJieti(Temp08, &Refers_File_Mac[BACKADDR[6+CH*2]]);
	RefreshPrice();
}

//==================================================
//带MAC更新相关参数信息到EEPROM
//Fefer1_File_Mac： 用户类型+参数更新标志+预留4字节+MAC
//Fefer2_File_Mac： 报警金额1、2+电压、电流互感器变比+MAC
//Feilv_File1_Mac： 费率文件1+MAC
//Feilv_File2_Mac： 费率文件2+MAC
//Feilv_Change_Mac：费率文件切换时间+MAC
//Ladder_File1_Mac：第1套阶梯值+阶梯电价+MAC
//Ladder_File2_Mac：第2套阶梯值+阶梯电价+启动时间+MAC
//==================================================
unsigned char UpdataRefer_ToEe_WithMac(void)
{
	unsigned char Buf[10];
	
	Exchange( Buf, UID, 4 );	//操作者代码（卡序号后4字节）
	
//	//根据参数更新标志位bit7读写参数+MAC
//	if((UpdataFlag & B1000_0000)!=0)
//	{
//		memcpy(Esam_Para.Alarm_Limit1.B08, &Refers_File_Mac[BACKADDR[1]+1], 14);//更新EE数据		
//		Store_Esampara();//保存esam参数数据到ee
//	}
	
	//根据参数更新标志位bit0读卡片当前套费率电价文件，更新表内当前套费率电价参数
	if((UpdataFlag & B0000_0001)!=0)
	{
		if( CardType == UserCardCommand ) FLRecord( Buf );	//费率编程记录
		Updata_Feilv_Page(0);//当前套费率电价
	}
			
	//根据参数更新标志位bit1读卡片备用费率电价文件，更新表内备用套费率电价参数
	if((UpdataFlag & B0000_0010)!=0)
	{
 		if( CardType == UserCardCommand ) FLRecord( Buf );	//费率编程记录
 		Updata_Feilv_Page(1);//备用套费率电价
		memcpy( Starttime.Rate, &Refers_File_Mac[BACKADDR[2]], 5 );	//更新启动参数
	}
	
	//根据参数更新标志位bit2读卡片当前套阶梯文件，更新表内当前套阶梯电量+阶梯电价+年结算日参数
	if((UpdataFlag & B0000_0100)!=0)
	{
		if(CardType == UserCardCommand ) JTRecord( Buf );	//阶梯编程记录
		
		Updata_Ladder_Page(0);	//当前套阶梯1数据
		Updata_Ladder_Page(1);	//当前套阶梯2数据
	}
	
	//根据参数更新标志位bit3读卡片备用套阶梯文件，更新表内备用套阶梯电量+阶梯电价+年结算日+启动时间参数
	if((UpdataFlag & B0000_1000)!=0)
	{
		if(CardType == UserCardCommand ) JTRecord( Buf );	//阶梯编程记录
		
		Updata_Ladder_Page(2);	//备用套阶梯1数据
		Updata_Ladder_Page(3);	//备用套阶梯2数据
		memcpy( Starttime.Ladder, &Refers_File_Mac[BACKADDR[3]], 5 );//更新启动参数
	}
	
	//及时更新RAM数据，重新计算当前电价
	if((UpdataFlag & B0000_1111)!=0)
	{
		run_price_flag = 1;	//重新查找当前时段及电价
		if((UpdataFlag & B0000_1010)!=0)//更新费率切换时间或阶梯电价切换时间
		{
			WriteE2WithBackup(ee_starttimer_page, ee_starttimer_inpage, Starttime.Shiqu, ee_starttimer_lenth);
			Starttime.Crc16.B08[0] ++;
			
			if( (CardType == PresetCardCommand) //预置卡两套费率切换时间及两套阶梯切换时间判断(清约定冻结)
			&& (((Cmp_Clock( Starttime.Ladder, 5 ) != 0x01) && ChkBufZero( Starttime.Ladder, 5 ))
			|| ((Cmp_Clock( Starttime.Rate, 5 ) != 0x01) && ChkBufZero( Starttime.Rate, 5 ))) )
			{
				Year_Ladder_Judge();	//年阶梯启动判断和数据合法性判断
				Price_Judge();		//费率阶梯电价判断
			}
		}
	}
	
	if( terminal_auth_flag ) return 0; //交互终端不在此做编程记录
	
	//编程记录
	if((CardType == UserCardCommand) && (UpdataFlag & B1000_1111))//用户卡
	{
		Buf[0] = UpdataFlag;			//更新标志
		Buf[1] = CardType;			//电卡类型
		Buf[2] = 0x98;				//私钥
		Buf[3] = 0x99;				//标识
		Exchange( Buf+4, UID, 4 );	//操作者代码（卡序号后4字节）
		ProgramRecord( Buf, Buf+4 );
	}
	
	return 0;
}

const uint08 code WRNUM[8] =	{ 1, 2, 4, 4, 0, 0, 0, 1};//对应参数修改时bit0~7需要读写的操作次数
//==================================================
//带MAC读写参数数据
//分别读写UpdataFlag的bit0~7
//Type：0读；1写
//==================================================
unsigned char UpdataRefer_WithMac(unsigned char Type)
{
	unsigned char Result;
	union B16_B08 CRCtemp[2];
	
	if( (UpdataFlag&B0000_0001))	
	{	
		Result = Inter_Authrf08( SECTION2 );	    
		if(Result) return(IC_AUTH_ERR);
		
		Result = ReadM1File( SECTION2,BLOCK0 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[4]], Ic_Comm_RxBuf, 16);//		
	
		Result = ReadM1File( SECTION2,BLOCK2 );
	  if( Result )return (IC_NOIN_ERR);	
		
		memcpy(CRCtemp[1].B08, Ic_Comm_RxBuf, 2);		
		if( CRCtemp[1].B16 != Crc_16Bits( &Refers_File_Mac[BACKADDR[4]], 16 ) ) return IC_FORMAT_ERR;	
	}
	Clear_Wdt();
	if( (UpdataFlag&B0000_0010))	
	{	
		Result = Inter_Authrf08( SECTION2 );	    
		if(Result) return(IC_AUTH_ERR);
		
		Result = ReadM1File( SECTION2,BLOCK1 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[5]], Ic_Comm_RxBuf, 16);//
		
		Result = ReadM1File( SECTION2,BLOCK2 );
	  if( Result )return (IC_NOIN_ERR);		
		memcpy(CRCtemp[1].B08, &Ic_Comm_RxBuf[2], 2);
		if( CRCtemp[1].B16 != Crc_16Bits( &Refers_File_Mac[BACKADDR[5]], 16 ) ) return IC_FORMAT_ERR;	
		
		memcpy(CRCtemp[1].B08, &Ic_Comm_RxBuf[9], 2);
		if( CRCtemp[1].B16 != Crc_16Bits( &Ic_Comm_RxBuf[9], 2 ) ) return IC_FORMAT_ERR;	
	}
	Clear_Wdt();
	if( (UpdataFlag&B0000_0100))	
	{	
		Result = Inter_Authrf08( SECTION3 );	    
		if(Result) return(IC_AUTH_ERR);
		
		Result = ReadM1File( SECTION3,BLOCK0 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[6]], Ic_Comm_RxBuf, 16);//		
	
		Result = ReadM1File( SECTION3,BLOCK1 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[6]+16], Ic_Comm_RxBuf, 8);//		
	  memcpy(CRCtemp[1].B08, &Ic_Comm_RxBuf[2], 2);
		if( CRCtemp[1].B16 != Crc_16Bits( &Refers_File_Mac[BACKADDR[6]], 24 ) ) return IC_FORMAT_ERR;		
		Clear_Wdt();
		Result = Inter_Authrf08( SECTION5 );	    
		if(Result) return(IC_AUTH_ERR);
		
		Result = ReadM1File( SECTION5,BLOCK0 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[6]+24], Ic_Comm_RxBuf, 16);//		
	
		Result = ReadM1File( SECTION5,BLOCK1 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[6]+40], Ic_Comm_RxBuf, 12);//		
	  memcpy(CRCtemp[1].B08, &Ic_Comm_RxBuf[12], 2);
		if( CRCtemp[1].B16 != Crc_16Bits( &Refers_File_Mac[BACKADDR[6]+24], 28 ) ) return IC_FORMAT_ERR;		
		
		Result = ReadM1File( SECTION5,BLOCK2 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[7]], Ic_Comm_RxBuf, 12);//		
		memcpy(CRCtemp[1].B08, &Ic_Comm_RxBuf[12], 2);
		if( CRCtemp[1].B16 != Crc_16Bits( &Refers_File_Mac[BACKADDR[7]], 12 ) ) return IC_FORMAT_ERR;		
		
	}
	Clear_Wdt();
	if( (UpdataFlag&B0000_1000))	
	{	
		Result = Inter_Authrf08( SECTION4 );	    
		if(Result) return(IC_AUTH_ERR);
		
		Result = ReadM1File( SECTION4,BLOCK0 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[8]], Ic_Comm_RxBuf, 16);//		
	
		Result = ReadM1File( SECTION4,BLOCK1 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[8]+16], Ic_Comm_RxBuf, 8);//		
	  memcpy(CRCtemp[1].B08, &Ic_Comm_RxBuf[2], 2);
		if( CRCtemp[1].B16 != Crc_16Bits( &Refers_File_Mac[BACKADDR[8]], 24 ) ) return IC_FORMAT_ERR;		
		
		Result = ReadM1File( SECTION4,BLOCK2 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[3]], Ic_Comm_RxBuf, 5);//		
	  memcpy(CRCtemp[1].B08, &Ic_Comm_RxBuf[5], 2);
		if( CRCtemp[1].B16 != Crc_16Bits( &Refers_File_Mac[BACKADDR[3]], 5 ) ) return IC_FORMAT_ERR;	
	  Clear_Wdt();	
		Result = Inter_Authrf08( SECTION6 );	    
		if(Result) return(IC_AUTH_ERR);
		
		Result = ReadM1File( SECTION6,BLOCK0 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[8]+24], Ic_Comm_RxBuf, 16);//		
	
		Result = ReadM1File( SECTION6,BLOCK1 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[8]+40], Ic_Comm_RxBuf, 12);//		
	  memcpy(CRCtemp[1].B08, &Ic_Comm_RxBuf[12], 2);
		if( CRCtemp[1].B16 != Crc_16Bits( &Refers_File_Mac[BACKADDR[8]+24], 28 ) ) return IC_FORMAT_ERR;		
		
		Result = ReadM1File( SECTION6,BLOCK2 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&Refers_File_Mac[BACKADDR[9]], Ic_Comm_RxBuf, 12);//		
		memcpy(CRCtemp[1].B08, &Ic_Comm_RxBuf[12], 2);
		if( CRCtemp[1].B16 != Crc_16Bits( &Refers_File_Mac[BACKADDR[9]], 12 ) ) return IC_FORMAT_ERR;		
		
	}
	Clear_Wdt();
	return 0;
}

//==================================================
//初始化钱包文件
//Buy_Money_Mac：充值金额+MAC
//Buy_Times_Mac：购电次数+MAC
//Buy_Money_Bcd：本次购电金额BCD
//==================================================
unsigned char Init_Money(void)
{
//	unsigned char Result;
	
//	memcpy(Ic_Comm_RxBuf, Buy_Money_Mac, 4+4);							//充值金额+MAC
//	Result = WriteBinWithMac(ESAM+0x10, 0x01, 0x14, 4+4 );	//更新钱包金额
//	if( Result ) return (Result);
//	
//	memcpy(Ic_Comm_RxBuf, Buy_Times_Mac, 4+4);				//购电次数+MAC
//	Result = WriteBinWithMac(ESAM+0x10, 0x03, 0x14, 4+4 );	//更新钱包购电次数
//	if( Result ) return (Result);
	
	Hex2Bcd(Buy_Money_Mac[0].B08, Buy_Money_Bcd, 4);			//购电金额HEX转换成BCD
	memset(Remain_back, 0, 20);				//清透支金额，购电次数,购电前剩余金额
	memcpy(Remain_back, Buy_Money_Bcd, 4);	//购电金额为剩余金额
	
	//更新RAM和EE：剩余，透支，次数
	Store_Remainmoney();		//保存剩余金额数据到ee	
	
	//判断剩余和允许合闸金额
	charge_relay_open_flag = 0;//清小于合闸允许门限
	remainder_maxtick_flag = 0;
	card_closerelay_flag   = 0;
	card_closerelay_flag1  = 0;
	Store_Meterstatus();

	return 0;
}

//==================================================
//判断购电次数、囤积金额、透支金额
//入口：
//		  Buy_Money_Bcd：	 本次充值金额BCD
//			Buy_Times：本次购电次数
//			Remain_back：	 表内原剩余金额+透支金额+购电次数
//出口：
//00：购电次数相等
//01：购电次数差1
//02：购电次数差值>=2，购电次数错误
//55：购电次数差1，但超囤积或溢出
//FF: 购电次数 < 表内购电次数
//Remain_back：充值后剩余金额BCD
//==================================================
unsigned char JudgeBuyTimes( void )
{
	unsigned char Result;
//	union B32_B08 Temp32;
	
//	memcpy(Temp32.B08, Remain_back+12, 4);			//复制表内购电次数
//	
////	if(Buy_Times.B32 < Temp32.B32) return (0xff);	//卡片购电次数<表内值

//	Temp32.B32 = Buy_Times.B32-Temp32.B32;			//卡片购电次数 - 表内值
//	
//	if(Temp32.B32 == 0) return (0);	//购电次数相等
//	if(Temp32.B32 == 1) 			//购电次数差1
//	{
		if(Buy_Money_Mac[0].B32 > 99999999)  return(0x55); 	//溢出不允许购电
		
		Add_Bcd(Remain_back+2, Buy_Money_Bcd, 4);			//剩余= 剩余+充值
		Result = Cmp_Bcd_Inv( Remain_back, Money.Remain, 6);
		if(Result == 2) return(0x55); 						//溢出不允许购电
		
		//判断透支金额	
		Result = Cmp_Bcd_Inv( Remain_back+2 , &Remain_back[8], 4 );
		if(Result == 2)	//剩余 < 透支
		{
			Sub_Bcd( &Remain_back[6], &Remain_back[0], 6 );	//透支=透支-剩余
			memset( &Remain_back[0], 0x00, 6 );		   			//剩余=0
		}
		else						//剩余 >=透支  
		{
			Sub_Bcd( &Remain_back[2], &Remain_back[8], 4 );	//剩余=剩余-透支
			memset( &Remain_back[8], 0x00, 4 );		  			//透支=0
		}
	
		//判断囤积金额
		if(Money_Para.Regrate.B32)
		{
			if( Cmp_DataLH( Remain_back+2, Money_Para.Regrate.B08, 4) == 1 ) return(0x55);//剩余>囤积
		}
		
		memset( &Remain_back[12], 0, 4 );	//新购电次数
		
		return (0);	//购电次数差 1
//	}
//	else return(2);	//购电次数差>1
}

//==================================================
//钱包文件操作
//入口：Buy_Money_Mac：购电金额+购电次数+MAC
//==================================================
unsigned char MoneyIn()
{
//	unsigned char Result;
	
//	memcpy(Ic_Comm_RxBuf, Buy_Money_Mac, 8+4);	//剩余金额+MAC
//	Result = IncreseWithMac();
//	if( Result ) return (Result);
//		
	//更新RAM和EE：剩余，透支，次数
	Store_Remainmoney();	//保存剩余金额数据到ee
	
//	Moneybag_Process();	//更新ESAM钱包
//	
	//购电记录、编程记录
	Buy_Record(ee_buymoneyn_inpage);
	
	//判断剩余和允许合闸金额
	Judge_Closerelay_Money();
	card_closerelay_flag   = 0;
	card_closerelay_flag1  = 0;
	Store_Meterstatus();
	
	return 0;
}


void Clear_MeterMoney()
{	
  	memset(Remain_back,0, 24);			//复制表内购电次数
    Store_Remainmoney();
	  card_closerelay_flag   = 0;
	  card_closerelay_flag1  = 0;
	  Store_Meterstatus();
}
//输入参数说明
/*
Type: 通讯方式及卡片类型
Bit7: 0本地插卡, 1远程或交互终端; 
Bit4: 0远程, 1卡片或交互终端;
Bit1~0: 01开户, 02充值, 03补卡;
*/
unsigned char OpenAddMoney( unsigned char Type )
{
	unsigned char Result, Ch = 0;
	
	if( Type & 0x80 ) Ch = 1;	//1为远程或交互终端，0为本地
	Type &= B0001_1111;	
	
	if( IsMultiBcd( &Refers_File_Mac[BACKADDR[1]], 6 ) )	//客户编号
	{
		if( Ch == 0 ) //本地插卡
			return (IC_FORMAT_ERR);	//客户编号错
		else  //远程命令
			return 0x01;	//客户编号不匹配
	}
	
	
	//========== 卡片序号及开户判断 ===========
	Get_Usercard_Serial();
	
	if( card_open_flag || comm_open_flag )	//本地已开户 or 远程已开户
	{
		Result = Cmp_Data( &Refers_File_Mac[BACKADDR[1]], Esam_Para.User_Id, 6 );
		if( Result != 3 ) //已开户的表，客户编号不相等
		{
			if( Ch == 0 ) //本地插卡
				return (IC_HUHAO_ERR);	//客户编号错
			else  //远程命令
				return 0x10;	//客户编号不匹配
		}
		
		if( comm_open_flag )
		{
			if(Type == 0x01 ) return (0x01);	//远程已经开户不允许重复开户		
		}	
		if( card_open_flag )
		{
			if(Type == 0x11 ) return (0x0A);	//远程已经开户不允许重复开户		
		}	
		//对于远程开户的电能表，只接受开户卡和补卡操作，不接受购电卡操作.
		if( (Type == 0x12) && UserCardSerialBit ) 
		{
			if( Ch == 0 ) return (IC_SERIAL_ERR);	//购电卡卡序号错
			else return 0x01;
		}
	}
	else	//未开户
	{
		switch( Type )
		{
			case 0x02:	return 0x01;	//充值次数错误
			case 0x12:	
				if( Ch == 0 ) return (IC_NOOPEN_ERR ); //购电卡插入未开户表
				else return 0x01;	//充值次数错误
			case 0x13:	
				if( Ch == 0 ) return (IC_NOOPEN2_ERR); //补卡插入未开户表
				else return 0x01;	//充值次数错误
			default: break;
		}
	}	
	Clear_Wdt();
  
	KeySTS_OK_flag = 0; //密钥更新标志清零
	
  if(( Type == 0x11 )||( Type == 0x01 ))
	{
		Result=api_Handle_key_sts_code(&KeyH_Token[0]);
	  if( Result != 0x28  ) return (Result);
		Clear_Wdt();
    Result=api_Handle_key_sts_code(&KeyL_Token[0]);
	  if( Result) return (Result);
    
		KeySTS_OK_flag = 1;		//密钥更新标志
		
	}	
	Clear_Wdt();
	Result=api_Handle_key_sts_code(&Charge_Token[0]);
	KeySTS_OK_flag = 0;			
	if( Result) return (Result);	
	
	if((gs_APDU_analyze_var.sub_class != STS0_SUB_CLASS_PURCHASE_KWH )&&(gs_APDU_analyze_var.sub_class != STS0_SUB_CLASS_PURCHASE_MONEY ) )
	{
		return(STS_TOKEN_REJRCT_2);	//子类型错误，不支持的TOKEN子类//		
	}
//	Hex2Bcd( Buy_Money_Mac[0].B08+0, Buy_Money_Bcd, 4 );	//购电金额HEX转换成BCD
  memcpy(  Buy_Money_Bcd,Buy_Money_Mac[0].B08+0, 4 );

	//======== 判断购电次数和临时充值 ==========
	Ready_Remainback();		//准备钱包数据	
	
	Result = JudgeBuyTimes();	
	if(Result) return(IC_OVERMAX_ERR);		//购电超囤积
	
	MoneyIn();//钱包充值
	
	if(( Type == 0x11 )||( Type == 0x01 )) 	
	{
			api_save_APDU_key_var();	//	保存正式密钥到EEPROM
		  KeySTS_OK_flag = 0;		
		
			memcpy( Esam_Para.User_Id, &Refers_File_Mac[BACKADDR[1]], 6 );	//更新EE客户编号	
      
		  if( Type == 0x11 ) 	Updata_Usercard_Serial(0);
      else 	Updata_Usercard_Serial(1);		
			Store_Esampara();	//保存esam参数数据到ee
	}
	if( (Type == 0x13) && UserCardSerialBit )  Updata_Usercard_Serial(0);
	
	
	//本地插卡显示剩余金额(先前和当前)
	if( Ch == 0 ) 
	{	
		disp_remain_flag = 1;    
		
	//	Result = UpdataRefer_WithMac(0);//更新ESAM和EEPROM参数
	//	if( Result ) return( Result);
	
		UpdataRefer_ToEe_WithMac();	//更新Eeprom参数
  }
	
	return 0x00;
}


//卡片处理
unsigned char Card_Esam_Proc( void )
{
	unsigned char Result;
//	unsigned char Buf[64];
	unsigned char i;
	union B16_B08 Crc16,TempCrc16;	
	Clear_Wdt();
	
	Ic_Type_Reg = 0x00;
	Ic_Err_Reg  = 0x00;
	UpdataFlag = 0x00;		
  
//	Delay_Initial(150);	
	
//	for(i = 0; i<10; i++)
//  {
//	  Result = RF_Anticoll();					//防冲突
//	  if(Result==0) break;
//		Clear_Wdt();
//	}	
//	if(Result) return(IC_NOIN_ERR);
//	
//	for(i = 0; i<3; i++)
//  {
//	  Result = RF_Select_Tag();				//选卡
//	  if(Result==0) break;
//	}		
//	if(Result) return(IC_NOIN_ERR);
	
	for(i = 0; i<3; i++)
  {
	  Result = Inter_Authrf08( SECTION7 );
	  if(Result==0) break;
	}		
	if(Result) return(IC_AUTH_ERR);
	
//=========== 判断卡片类型 ================	
	Result = ReadM1File( SECTION7,BLOCK0 );
	if( Result )
	{
		 return (IC_NOIN_ERR);			//卡片操作错误
	}	
	
	Crc16.B16=Crc_16Bits( Ic_Comm_RxBuf, 14 );
	TempCrc16.B08[0]=Ic_Comm_RxBuf[15];
	TempCrc16.B08[1]=Ic_Comm_RxBuf[14];
	if( Crc16.B16 != TempCrc16.B16)
	{
		 return (IC_FORMAT_ERR);			//卡片文件不合法
	}	
	
	UserCardFlag = Ic_Comm_RxBuf[0];
	if( UserCardFlag >0x04 )
	{
		 return (IC_FORMAT_ERR);			//卡片文件不合法
	}	
	UpdataFlag = Ic_Comm_RxBuf[1];	
	
	if( UserCardFlag ==0x04 )
	{
		 return (0);			//清零信用卡   
	}	
	
  Result =	UpdataRefer_WithMac(0);	
  if( Result )
	{
		 return (Result);			//卡片操作错误
	}
	
	memcpy( &Refers_File_Mac[BACKADDR[1]], &Ic_Comm_RxBuf[2], 6 ); //客户编号

	if((UserCardFlag == UserOpenCardType)||(UserCardFlag == UserKeyCardType))
	{
		Result = Inter_Authrf08( SECTION1 );
	  if(Result) return(IC_AUTH_ERR);		
		
		Result = ReadM1File( SECTION1,BLOCK0 );
	  if( Result )return (IC_NOIN_ERR);			//卡片操作错误	  
		memcpy(&KeyH_Token[0], Ic_Comm_RxBuf, 16);//key token1
		
		Result = ReadM1File( SECTION1,BLOCK1 );
	  if( Result )return (IC_NOIN_ERR);			//卡片操作错误
	  memcpy(&KeyH_Token[16], Ic_Comm_RxBuf, 4);//key token1
		memcpy(&KeyL_Token[0], &Ic_Comm_RxBuf[4], 12);//key token2
		
		Result = ReadM1File( SECTION1,BLOCK2 );
	  if( Result )return (IC_NOIN_ERR);	
		memcpy(&KeyL_Token[12], &Ic_Comm_RxBuf[0],8);//key token2		
	}
	Clear_Wdt();
	Result = Inter_Authrf08( SECTION0 );
	if(Result) return(IC_AUTH_ERR);	
	
	Result = ReadM1File( SECTION0,BLOCK1 );
	if( Result )return (IC_NOIN_ERR);			//卡片操作错误	  
	memcpy(&Charge_Token[0], Ic_Comm_RxBuf, 16);//Charge_Token
		
	Result = ReadM1File( SECTION0,BLOCK2 );
	if( Result )return (IC_NOIN_ERR);			//卡片操作错误
	memcpy(&Charge_Token[16], Ic_Comm_RxBuf, 4);//Charge_Token
	
  //各类型卡片的处理
	//所有卡判断表号
	//充值卡需要判断系列号以及客户编号
	//补卡需要判断客户编号
//	if((CardType == UserchargeCardType)
//	{
//	if( Cmp_Meter_Id(&Ic_Comm_RxBuf[8]) ) 
//	}
//	else if	(CardType == UserRecuitCardType))
//	else 
	Result = OpenAddMoney( UserCardFlag|0x10 );
	if(Result) return(Result);
	//更新Eeprom数据
	//UpdataRefer_ToEe_WithMac();	//更新KeyToken到Eeprom数据
		
	//================ 返写卡片 =============
	//UpdataRunFile();	//更新运行文件
			
	return OK_CARD_PROC;
}


//插卡处理程序
void Card_Proc( void )
{
	unsigned char result;
//	
//	Release_Rf();		return;	
	
	
	if( FLAG_EA == 0 ) return;							//掉电不处理,程序直接返回
	if( Keyswitch_Valid == 0xaa ) return;//无卡槽
//	if( Find_Card == 0xaa ) 					//禁止寻卡
//	{
//		CardCheck = 0;
//		CardCheck1 = 1;							//保证禁止寻卡跨日后，不会立即寻卡
//		return;	
//	}
	
	if( Beep_Timer ) return;	//避免刚寻卡结束又寻卡，降低功耗

//========= 判断是否寻卡 ==============
  if( Keyswitch_Valid == 0x55 )	//卡槽有触点
	{
		if( Find_Card == 0x55 )
		{
			CardCheck = 0;
//			if( rf_card_flag == 0 ) return;	//自动寻卡模式下，不判断卡槽
		}
		else
		{
			CardCheck1 = 0;
			if( Check_Card() != 0 ) return;	//正常触点寻卡模式
			memset(CardRst8B, 0xff, 8);			//序号默认0xff
		}
	}
  else	//卡槽无触点
	{
		CardCheck = 0;
		if( rf_card_flag == 0 ) return;		//无触点，1s寻卡1次
	}
		
	rf_card_flag = 0;

//=========== 卡片复位 ================
	result = RF_Init();
	if( ((result == 0) && (CardCheck1 == 0)) || CardCheck )//有卡且未操作过，或，卡槽有新卡插入
	{
		Lcdled.B16 = lcdled_timer_delay;		//启动60s背光定时器
		LCDLEDON;
		if((Real.U[0] > 0x1450)||(Real.U[1] > 0x1450)||(Real.U[2] > 0x1450))
		{
			if((Display.Number==0x00)||(Display.Number == 0xff)) Display.Number = 1;//防止显示异常
			ic_display = 0;
			disp_readic_flag = 1;
			Lcd_Display();	//显示“读卡中”
			
			Ic_Err_Reg = Card_Esam_Proc();		//卡片处理
					
			Release_Rf();							//立即释放射频模块，降低功耗
			
			if( BDMODE && (Ic_Err_Reg == 0) && (UserCardFlag == PresetCardCommand)) //调试时使用，步骤往前提，正常需要在报警判断后调用
			{
//				Clear_Meter(3, CardRst8B);	//电表清零,保存清零记录
				  Clear_MeterMoney();
			}
			Alarmstatus_Judge();			//电表报警状态判断
			Enable_Closerelay();			//插卡闭合继电器
//			
//			if( BDMODE && (Ic_Err_Reg == 0) && (UserCardFlag == PresetCardCommand))
//			{
//				Clear_Meter(3, CardRst8B);	//电表清零,保存清零记录
//				 
//			}
		}
		else Ic_Err_Reg = IC_VLOW_ERR;			//电压过低
		
		if( Ic_Err_Reg == 0 )
		{
			InsertCardSta = 0x01;			//插卡成功
			
			Beep_Timer = 156;			//成功蜂鸣器定时鸣叫625ms
		}
		else
		{
			InsertCardSta = 0x02;			//插卡失败
			
			Beep_Timer = 156;			//失败蜂鸣器定时鸣叫秒125*5

			if( !result ) Card_Err_Record();//当插入的为电能表不能识别的介质（如铁片或反插卡）时，电能表无需进行非法插卡和异常插卡记录。
			
			disp_remain_flag = 0;			//防止充值成功但是其他操作失败的情况
			
			// Ic_Err_Reg = 1;卡片通讯错误
			// Ic_Err_Reg = 2;卡片认证失败
			// Ic_Err_Reg = 3;客户编号不一致
			// Ic_Err_Reg = 4;文件格式不正确
			// Ic_Err_Reg = 5;卡片系列号不一致
			// Ic_Err_Reg = 6;重复开户
			// Ic_Err_Reg = 7;购电卡插入未开户电表
			// Ic_Err_Reg = 8;补卡插入未开户电表
			// Ic_Err_Reg = 9;剩余金额超囤积
			// Ic_Err_Reg = 10;表计电压过低176V(80%Un)
			// Ic_Err_Reg = 11;解密失败，可能是秘钥错误//	
			// Ic_Err_Reg = 12;不支持的TOKEN类//	
			// Ic_Err_Reg = 13;解密成功，但是TOKEN内的数据超出范围//	
			// Ic_Err_Reg = 14;/TID超出秘钥有效期//	
			// Ic_Err_Reg = 15;TOKEN已经被使用过//	
			// Ic_Err_Reg = 16;TOKEN已过期	
			// Ic_Err_Reg = 17;充值解析正确但是超出囤积上限
			// Ic_Err_Reg = 18;其他错
			// Ic_Err_Reg = 19;其他错
			// Ic_Err_Reg = 20;其他错	
			if(Ic_Err_Reg < 2)Ic_Err_Reg = 10;
			else if((Ic_Err_Reg < 5)||(Ic_Err_Reg == 19)) Ic_Err_Reg = 1;
			else if(Ic_Err_Reg < 10) Ic_Err_Reg = 2;
			else if(Ic_Err_Reg == 10) Ic_Err_Reg = 6;
			else if(Ic_Err_Reg < 13) Ic_Err_Reg = 3;
			else if(Ic_Err_Reg < 14) Ic_Err_Reg = 5;
			else if(Ic_Err_Reg < 15) Ic_Err_Reg = 4;
			else if(Ic_Err_Reg < 16) Ic_Err_Reg = 7;
			else if(Ic_Err_Reg < 17) Ic_Err_Reg = 8;			
			else if(Ic_Err_Reg == 21) Ic_Err_Reg =9 ;
			else if(Ic_Err_Reg == 31) Ic_Err_Reg = 11;
			else if(Ic_Err_Reg == 32) Ic_Err_Reg =12 ;
			else if(Ic_Err_Reg == 34) Ic_Err_Reg = 13;
			else if(Ic_Err_Reg == 36) Ic_Err_Reg = 14;
			else if(Ic_Err_Reg == 37) Ic_Err_Reg = 15;
			else if(Ic_Err_Reg == 38) Ic_Err_Reg =16 ;
			else Ic_Err_Reg = 18;
		}
		
		Clear_Wdt();
		
	  if(Keyswitch_Valid == 0x55)
	  {
	  	if(Find_Card == 0x55) CardCheck1 = 1;//卡槽有触点，且在寻卡模式，卡片已处理
	  }
		else CardCheck1 = 1;	//卡片已处理
		ic_display = 1;
		disp_readic_flag = 0;
		Display.Timer = Display_Para.Loop_Time;
		Display.Number = 0;
		Display.Status &= B1111_1110;
		if( (CardType == UserCardCommand) && (Ic_Err_Reg == 0) && BDMODE )
		{
			if(disp_remain_flag) Display.Timer += 2;//购电成功的用户卡,显示先前剩余金额2s
			else	disp_remain_flag = 1;		//返写成功的用户卡,显示当前剩余金额
		}
		Lcd_Display();	//刷新显示
	}
	else if(( result )&&( RFTESTCNT ==0 ))
	{
		
		CardCheck1 = 0;
	}
	
	Release_Rf();			//释放射频模块
}


//下面是远程费控的程序
//MAC校验结果处理函数
//累计MAC失败100次挂起
void Mac_Err_Proc( void )
{
	if( terminal_auth_flag ) return;
	
	if(Mac_Err_Times < 200) Mac_Err_Times++;
	if(Mac_Err_Times >= 200)
	{
		Mac_Err_Times = 0x00;	//清零
		hangup_comm_flag = 1;	//要求电能表远程设置参数功能挂起
	}
}

unsigned char Update_ESAM_Bin_Mac( unsigned char P1, unsigned char P2, unsigned char *Buf, unsigned char Len )
{
	unsigned char result;
	
	Exchange( Ic_Comm_RxBuf, Buf, Len );
	Exchange( Ic_Comm_RxBuf+Len, Buf+Len, 4 );

	result = WriteBinWithMac( ESAM, P1, P2, Len+4 );
	if( result )
	{
		Mac_Err_Proc();	//mac错误次数加1
	}
	
	return (result);
}


//同步更新esam运行信息文件
void UpdataRunFile(void)
{
	unsigned char Buf[60];
	
	ReadBinFile( ESAM, 0x86, 0, 50 );	//读全部06文件
	memcpy( Buf, Ic_Comm_RxBuf, 50 );	//06文件数据备份
	
	Ic_Comm_RxBuf[0] = 0x68;
	Ic_Comm_RxBuf[1] = 0x11;							//用户卡返写
	Ic_Comm_RxBuf[2] = 0x00;
	Ic_Comm_RxBuf[3] = 0x2c;
	
	Bcd2Hex( Money.Remain, Ic_Comm_RxBuf+23, 4);		//剩余金额
	memcpy( Ic_Comm_RxBuf+27, Money.Buy_Times.B08, 4 );	//购电次数
	memcpy( Ic_Comm_RxBuf+31, Money.Tick, 4 );					//透支(BCD)	
	
	Save_Keep_Timer(0);
	Hex2Bcd( Eeprom.Buffer+1, Ic_Comm_RxBuf+39, 3 ); 	//非法卡交互次数 3 BCD
		
	memcpy( Ic_Comm_RxBuf+5, Esam_Para.Current_Ratio,18 );//变比1+变比2+表号+客户编号
	Ic_Comm_RxBuf[47] = Display_Para.MeterCtrl;	//电表模式字
	
	if( (Cmp_Data( Ic_Comm_RxBuf, Buf, 48) != 3) || (CardType == UserCardCommand))//判断07文件数据是否发生变化,如果变化了才返写07文件
	{
		Exchange( Ic_Comm_RxBuf+42, Sclock.Clockbackup+1, 5 );//分时日月年
		//更新CS
		Ic_Comm_RxBuf[48] = Cal_ChkSum(Ic_Comm_RxBuf+1, 47);
		Ic_Comm_RxBuf[49] = 0x16;
		
		UpdateBinFile(ESAM, 0x86, 0, 50 );
	}
}

unsigned char GetPswBit( unsigned char *InBuf )
{
	unsigned char i, n, Temp08;

	Temp08 = Feik.Num[7];
	if( Temp08 > MAX_PSW_NUM ) Temp08 = MAX_PSW_NUM;	//密钥总条数非法默认

	for( i=0; i<Temp08; i++ )	//表内允许设置的密钥总条数.
	{
		if( (i%8) == 0 ) n = 1;
		else n <<= 1;
		
		if( InBuf[0]&1 )
		{
			if( (InBuf[i/8]&n) == 0x00 ) return 0xFF;
		}
		else
		{
			if( InBuf[i/8]&n ) return 0xFF;
		}
	}
	
	return (InBuf[0]&1);
}

//读密钥状态字
void GetKey( unsigned char *KeyStatus )
{
	I2C_Read_Eeprom44( ee_keystatus_page, KeyStatus, ee_keystatus_lenth );
}

//写密钥状态字
void SetKey( unsigned char *KeyStatus )
{
	I2C_Write_Eeprom44( ee_keystatus_page, KeyStatus, ee_keystatus_lenth );
	GetKeyStatus();
}

//读当前密钥状态
void GetKeyStatus()
{
	unsigned char Buf[32];
	
	GetKey( Buf ); //读密钥状态
	Key_Status = GetPswBit( Buf );
}


unsigned char GetSetBitNum( unsigned char *InBuf )
{
	unsigned char i, k, n;

	for( i=0, k=0; i<MAX_PSW_NUM; i++ )	//统计已下载的密钥总条数
	{
		if( (i%8) == 0 ) n = 1;
		else n <<= 1;
		if( InBuf[i/8] & n ) k++;
	}
	
	return (k);
}


unsigned char UpdateEepromKey( unsigned char Ch )
{
	unsigned char i, k, result;
	unsigned char Buf[64], ChangeBuf[32];
	unsigned char FramePswNum, MeterPswNum, SettedPswNum;

	if( Uart[Ch].Buf[9] <= 12 ) return 1;
	
	//FramePswNum:  命令帧中包含的密钥条数.
	//MeterPswNum:  表内允许设置的密钥总条数.
	//SettedPswNum: 已下载的密钥总条数(不包含重复更新的密钥)
	FramePswNum = (Uart[Ch].Buf[9]-8-4)/36;
	if( (FramePswNum==0) || (FramePswNum>4) || ((Uart[Ch].Buf[9]-8-4)%36) ) return 1;

	MeterPswNum = Feik.Num[7];
	if( MeterPswNum > MAX_PSW_NUM ) MeterPswNum = MAX_PSW_NUM;	//密钥总条数非法默认

	//ee_keystatus_page: 
	//密钥状态字(4)  : 符合国网规定的“密钥状态字”。
	//密钥状态(1)    : (预留)
	//更新状态(1)    : 密钥更新的过程状态 (00启动写E2PROM, 55启动写ESAM, AA更新完成).
	//更新到E2PROM(4): 按照国网规定的“密钥状态字”格式，每更新一条到E2PROM 对应bit置1.
	//更新到ESAM(4)  : 按照国网规定的“密钥状态字”格式，每更新一条到ESAM 对应bit置1.(预留)
	//新密钥状态字(4): 按照国网规定的“密钥状态字”格式，0为公钥，1为私钥。
	//ee_keystatus1_page:
	//操作者代码(4)  : 更新记录中的操作者代码是电表收到第一条密钥更新命令中的操作者代码.
	//密钥更新的总条数(1) HH
	//更新前密钥状态字(4):
	GetKey( ChangeBuf );
	if( ChangeBuf[5] == 0x55 )return 0; //更新状态: 00已启动更新E2PROM; 55已启动更新ESAM; AA已更新结算.
	if( ChangeBuf[5] != 0x00 ) //结束更新密钥
	{
//		ChangeBuf[4] &= 0x3F;	//清除Bit7和Bit6。
		ChangeBuf[5] = 0x00;	//只有更新结束才允许新一轮的更新。
		memset( ChangeBuf+6, 0x00, 12 ); //清空更新的标志位。
		
		memcpy( Eeprom.Buffer, Uart[Ch].Buf+14, 4 );	//操作者代码
		Eeprom.Buffer[4] = MeterPswNum;	//密钥更新的总条数(1) HH
		memcpy( Eeprom.Buffer+5, ChangeBuf, 4 );	//更新前的密钥状态字
		I2C_Write_Eeprom( ee_keystatus1_page, ee_keystatus1_inpage, Eeprom.Buffer, ee_keystatus1_lenth ); 
	}

	memcpy( Buf, Uart[Ch].Buf+18, 36 );
//	for( i=1; i<FramePswNum; i++ ) Xor_Data( Buf, Uart[Ch].Buf+18+i*36, 36 );

	//有效的密钥条数
	for( i=0, k=0; i<FramePswNum; i++, k+=36 )
	{
		if(i) Xor_Data( Buf, Uart[Ch].Buf+18+k, 36 );
		
		//密钥总条数+预留+密钥编号+密钥状态
		if((Uart[Ch].Buf[18+32+k] > 0x01) //密钥状态标志位（00-测试状态，01-正式状态）
		 ||(Uart[Ch].Buf[18+32+k] != Uart[Ch].Buf[18+32+0]) ) return 1; //错误应答: 报文出现密钥状态标志位不一致
		if( Uart[Ch].Buf[18+33+k] >= MeterPswNum ) return 1;	//密钥编号超限.
		if( Uart[Ch].Buf[18+35+k] != MeterPswNum ) return 1;	//错误应答: 报文中的密钥总条数与参数表中密钥总条数不一致.

		ChangeBuf[6+Uart[Ch].Buf[18+33+k]/8] |= (1<<(Uart[Ch].Buf[18+33+k]%8));	//记录下载密钥情况.
		
		if( Uart[Ch].Buf[18+32+k] )
			ChangeBuf[14+Uart[Ch].Buf[18+33+k]/8] |= (1<<(Uart[Ch].Buf[18+33+k]%8));
		else
			ChangeBuf[14+Uart[Ch].Buf[18+33+k]/8] &= ~(1<<(Uart[Ch].Buf[18+33+k]%8));
	}

//	memset( Buf+36, 0x00, 8 );
//	if( (Uart[Ch].Buf[18+32+0]&0x7F) == 0x00 )	Buf[36] = 0x01; //密钥恢复时固定为01
//	else	Exchange( Buf+36, Esam_Para.Meter_Id, 6 );	//密钥下装时取表号
	Exchange( Buf+36, Esam_Para.Meter_Id, 6 );	//密钥下装时取表号
	Buf[42] = 0x00;
	Buf[43] = 0x00;
	memcpy( Buf+44, Uart[Ch].Buf+10+Uart[Ch].Buf[9]-4, 4 ); //MAC
	
	result = Update_ESAM_Bin_Mac( 0x95, 0x00, Buf, 44 );	//
	if( result != 0x00 ) return 4;	//当传输的数据MAC 校验错是采用错误信息字ERR 中bit2（密码错/未授权）置位，并按照异常应答帧响应。
	
	SettedPswNum = GetSetBitNum( ChangeBuf+6 ); //统计已下载的密钥总条数
	if( SettedPswNum == MeterPswNum ) //更新的密钥条数达到总条数
	{
		result = GetPswBit( ChangeBuf+14 );
		if( result != 0xFF ) 	//判断密钥状态是否一致
		{
			ChangeBuf[5] = 0x55; //设置启动更新ESAM标志
		}
	}
	else if( SettedPswNum > MeterPswNum ) return 1;	//错误应答：下载的有效密钥条数>表内密钥总条数

	for( i=0, k=0; i<FramePswNum; i++, k+=36 )
	{
		Inverse( Uart[Ch].Buf+18+k, 32 );	//只倒序密钥
		I2C_Write_Eeprom( ee_keydata_1_page+Uart[Ch].Buf[18+33+k]/2, 32*(Uart[Ch].Buf[18+33+k]%2), Uart[Ch].Buf+18+k, 32 );
	}

	SetKey( ChangeBuf ); //首先记录下密钥写入E2PROM的情况.

	return 0;
}


//检测是否有密钥需更新到ESAM，有则更新。
unsigned char UpdateEsamKey( void )
//函数返回: 当前电表密钥状态
{
	unsigned char i, k, n, result, key, Bkmain;
	unsigned char TmpBuf[32];
	
	GetKey( TmpBuf );
	if( TmpBuf[5] == 0x55 ) //更新状态: 00已启动更新E2PROM; 55已启动更新ESAM; AA已更新结算.
	{
		key = GetPswBit( TmpBuf+14 );
		for( i=0; i<MAX_PSW_NUM; i++ )
		{
			
			if( (i%8) == 0 ) k = 1;
			else k <<= 1;
			
			if( (TmpBuf[6+i/8]&k)  )
			{
				//启动更新ESAM指令
				for( n=0; n<3; n++ )
				{
					if( i == 0x00 )
						RxTxCommhead(0x84, 0xd4, 1, 0x00, 0x20); //主控密钥密文
					else
						RxTxCommhead(0x84, 0xd4, 1, 0xff, 0x20); //其他密钥密文

					result = I2C_Read_Eeprom( ee_keydata_1_page+i/2, 32*(i%2), Ic_Comm_TxBuf+5, 0x20 );
					if( result ) continue;
					
					result = CardTxRxFrame( ESAM, 0x20, 0x00 );
					if( result == 0x00 ) break;
				}
				
				if( ( i == 0 )||( result == 0 ) )
				{
					if( i == 0 ) Bkmain = TmpBuf[0]; 
					
					if( key == 0 ) TmpBuf[i/8] &= (~k);
					else TmpBuf[i/8] |= k;
					
					if( ( i == 0 )&&( result == 0 ) ) Bkmain = TmpBuf[0]; 	//成功更新主控密钥
				}
				else
				{
					if( FLAG_EA == 0 ) return 0x00; 	//更新过程中出现停电,上电从密钥编号为0的密钥开始对ESAM重新进行密钥更新
					break;				//更新失败,不再对本条密钥之后的密钥进行更新
				}
			}
		}
		
//		if( i == MAX_PSW_NUM )
//		{			
			key = GetPswBit( TmpBuf);
			
			if( key == 0xFF )	
			{
				if( (Bkmain & 0x01) == 0 ) TmpBuf[0] &= 0xFE;		//更新失败,恢复主控状态
				else TmpBuf[0] |= 1;
			}
			
			TmpBuf[5] = 0xAA;	//全部更新完毕
//		}

		for( n=0; n<3; n++ )
		{
			Exchange( Ic_Comm_RxBuf, TmpBuf, 4 );//复制4字节密钥状态
			result = UpdateBinFile( ESAM, 0x85, 0x00, 4 );	//写esam的05文件
			if( result == 0x00 ) break;
			if( FLAG_EA == 0 ) return 0x00; 
		}
		
		SetKey( TmpBuf ); //首先记录下密钥写入E2PROM的情况.
		
		//--- 密钥更新记录 ---
		I2C_Read_Eeprom( ee_keystatus1_page, ee_keystatus1_inpage, Eeprom.Buffer+6, ee_keystatus1_lenth ); 
		Event_Record( ee_keysetn_inpage );
		//--- 密钥更新记录 end---
	}

	return 0x00;
}


void FailKey( void )
{
	unsigned char Buf[64];

	GetKey( Buf );
	if( Buf[5] != 0xAA )
	{
		memset( Buf+4, 0x00, ee_keystatus_lenth-4 ); //清空更新的标志位。
		Buf[5] = 0xAA;
		SetKey( Buf );
	}
}
//void Clear_MeterMoney(void)
//{
//	
//	memset( Remain_back, 0x00, 16 );
//	Store_Remainmoney();	//保存剩余金额数据到ee
//	
////	Moneybag_Process();	//更新ESAM钱包
////	
//	//购电记录、编程记录
//	Buy_Record(ee_buymoneyn_inpage);
//	
//	//判断剩余和允许合闸金额
//	Judge_Closerelay_Money();
//	card_closerelay_flag   = 0;
//	card_closerelay_flag1  = 0;
//	Store_Meterstatus();
//	
//	return 0;	
//	;	
//}
//电表清零
void Clear_Meter( unsigned char Ch, unsigned char *Opt )
{
	unsigned char Buf[72];
	Clear_Wdt();
		
	ClrRecTimes( 0, 0xaa );//清冻结和事件有效指针(除清零记录)
	
	//上报相关,密码相关和负荷记录指针清零
	memset( &System_St_Bk, 0x00, ee_reportdata_lenth+18 );
	
	//清零点清零数据
	Clr_Zero_Data();
	
	//电表清零时要清除远程拉闸、远程报警状态，不清除保电状态
	if( Relay_Mode == 0x55 )//外置继电器
	{
		if( (!BDMODE) || ((remainder_maxtick_flag == 0) || relay_commu_close_flag) )//插完预置卡处于合闸状态才置继电器成功标志
		{
			relay_cleardl_flag = 1;	//置外置表清零标志
		}
	}
	relay_commu_alarm_flag = 0;
	
	Clr_Commu_Relaydata();
//	Store_Meterstatus();
	ClrSelfClose();	//清自保电状态(因为拉闸状态被清掉了)
	
	//清结算用电量/上月用电量/上1~4年用电量
	Process_Usedl(3);
	Process_Usedl(4);
	Process_Usedl(5);

	//记录清电表事件
	//清零事件记录：发生时刻(6)+操作者代码(4)+电表清零前正向有功(4)+电表清零前反向(4)	
	//存储各项数据时倒序，低字节在前，高字节在后
	if(Ch != 3)
	{			
		memcpy( Eeprom.Buffer+6, Opt, 4 );	//操作者代码(4),远程清零
	}
	else
	{
		Exchange( Eeprom.Buffer+6, Opt+4, 4 );	//操作者代码（卡序号后4字节）
	}
	
	DltoEepromBuf( 10 );
	Event_Record( ee_clearn_inpage );

	//清 XRAM和E2电量
	Veri_Etmr1_Pcount(1);		//清脉冲尾数
	
	memset( Fwdl.Fwdl_T.B08, 0x00, ee_fwdl_lenth+2 );		//清正向电量
	WriteE2WithBackup( ee_fwdl_page, ee_fwdl_inpage, Fwdl.Fwdl_T.B08, ee_fwdl_lenth );

	memset( Bkdl.Bkdl_T.B08, 0x00, ee_bkdl_lenth+2 );		//清反向电量
	WriteE2WithBackup( ee_bkdl_page, ee_bkdl_inpage, Bkdl.Bkdl_T.B08, ee_bkdl_lenth );
	
	Clear_Wdt();
	if( BDMODE ) //本地模式
	{
		run_price_flag = 1;		//电量清零重新判断阶梯值
	
		//初始化卡序号
		memset( Eeprom.Buffer, 0xff, ee_usercard_serial_lenth );
		I2C_Write_Eeprom44( ee_usercard_serial_page, Eeprom.Buffer, ee_usercard_serial_lenth );//crc是否正确来判断电表开户
		
		//保存本次购电记录
		Buy_Record(ee_buymoneyn_inpage);
		
	
	memset( Buf, 0xff, 72 );			
	I2C_Write_Eeprom( ee_TID_page, ee_TID_inpage, Buf, ee_TID_lenth );
	
	memset( Buf, 0x00, 72 );
  I2C_Write_Eeprom( ee_TIDCNT_page, ee_TIDCNT_inpage, Buf, ee_TIDCNT_lenth );	
	I2C_Write_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Buf, ee_TOKEN_lenth );
	I2C_Write_Eeprom( ee_Pre7DayMon_page, ee_TOKEN_inpage, Buf, ee_Pre7DayMon_lenth );
	I2C_Write_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, Buf, ee_Pre12monthMon_lenth );
	I2C_Write_Eeprom( ee_DayUserINFO_page, ee_UserINFO_inpage, Buf, 16 );
	
	}
	
	Mt_Alarmstatus_Judge();
	if( relay_status_flag ) ClrRecTimes( ee_openrelayn_inpage, 0x01 );//清拉闸记录
	else ClrRecTimes( ee_closerelayn_inpage, 0x01 );//清合闸记录
	
	Wr_Powerdown_Data(0);//写掉电相关数据到数据ee
	Wr_Pretime_Dayfreeze();	//写上一次日冻结时间
	
	
}
