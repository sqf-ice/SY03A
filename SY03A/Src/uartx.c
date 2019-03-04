#include "ex_var.h"
#include "ex_func.h"
#include <string.h>
#include <absacc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "cpu_card.h"

#define	MAXREGION	14		//年时区数最大值, 可设
#define	MAXSIDUANTB	8		//时段表数最大值, 可设
#define	MAXSIDUAN	max_shiduancount//时段数最大值, 可设
#define	MAXHOLIDAY	max_maxholiday	//公共假日数最大值, 可设
#define	MAXFLPRICE	12		//允许设置最大费率电价数
#define SCREENNUM	99		//本地费控表
//地址范围:PROGRAMSTA1~PROGRAMEND1~PROGRAMSTA2~PROGRAMEND2
#define PROGRAMSTA1	0x800000	//程序空间1起始地址
#define PROGRAMEND1	0x80FFFF	//程序空间1结束地址
#define PROGRAMSTA2	0xFF0000	//程序空间2起始地址
#define PROGRAMEND2	0xFFFFFF	//程序空间2起始地址
#define PROGRAMPROZ	0xFFF000	//程序保护区起始地址(keil工程定义?EC?FACTORY(0xFFF000))

#define	KEYTYPE1	0x97
#define	KEYTYPE2	0x96


extern unsigned char UpdataRefer_ToEe_WithMac(void);
extern unsigned char ReadBinWithMac( uint08 CH, uint08 P1, uint08 P2, uint08 Len );
extern unsigned char Authorization( unsigned char *Buf, unsigned char Len );
extern unsigned char Auth_In_Out( unsigned char INS, unsigned char CH, unsigned char P2, unsigned char *BufA );
extern unsigned char DecreaseWithMac( unsigned char *InBuf );
extern unsigned char CardTxRxFrame( unsigned char CH, uint08 Lc, uint08 Le );
extern void RxTxCommhead( uint08 inc, uint08 com, uint08 P1, uint08 P2, uint08 len);
extern uint16 CalcAbsDay(unsigned char * psTm);
extern uint16 CalcAbsMin(unsigned char * psTm);

void ClrSelfClose( void )
{
	self_close_flag = 0; 
	Self_Cnt = 0;
	Self_Timer3 = 0;
	Self_Timer24 = 0;

	Store_Meterstatus();
}

void LoopExchange( unsigned char *Drc, unsigned char *Src, unsigned char Times, unsigned char Spec )
{
	unsigned char i, k;
	
	for( i=0,k=0; i<Times; i++, k+=Spec ) Exchange( Drc+k, Src+k, Spec );
}

void LoopInverse( unsigned char *Inv, unsigned char Times, unsigned char Spec )
{
	unsigned char i;

	for( i=0; i<Times; i++ ) Inverse( Inv+i*Spec, Spec );
}

uint08 IsBCD( uint08 Value )
{
	if( ((Value&0xF0)<=0x90) && ((Value&0x0F)<=0x09) )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

unsigned char IsMultiBcd( unsigned char *Buf, unsigned char Len )
//return: 0: BCD; 1: no BCD
{
	unsigned char i;
	
	for( i=0; i<Len; i++ )
	{
		if( IsBCD(Buf[i]) != 0x00 )		return 1;
	}
	
	return 0;
}


void Xor_Data( unsigned char *BufA, unsigned char *BufB, unsigned char Num )
{
	unsigned char i;

	for( i=0; i<Num; i++ )
	{
		BufA[i] ^= BufB[i];
	}
}


void Calculate_Program_Cs( unsigned char *Buf )//计算程序累加和	NNNNNNNN	4
{
//	unsigned long int ecode *Code_Flash_Char;
	union B32_B08 Temp32;
		
	Temp32.B32 = 0;
	
	if( progenable )	//厂内模式才会计算
	{
//		Clear_Wdt();
//		for( Code_Flash_Char=0x800000; Code_Flash_Char<=0x80ffff; Code_Flash_Char++ )	//4M下执行时间约280ms
//		{
//			Temp32.B32 += *Code_Flash_Char;
//			if( Code_Flash_Char == 0x80fffc ) break;
//		}
//		
//		for( Code_Flash_Char=0xff0000; Code_Flash_Char<=0xffffff; Code_Flash_Char++ )	//4M下执行时间约280ms
//		{
//			Temp32.B32 += *Code_Flash_Char;
//			if( Code_Flash_Char == 0xfffffc ) break;
//		}
		Clear_Wdt();
	}
	
	Hex2Bcd( Temp32.B08, Buf, 4 );	//转换成bcd码,方便显示
}


//Offset从0~3,分别为拉闸延时,大电流拉闸延时,红外认证时间,上报自动复位时间
/*BCD码的分钟定时，转换到hex的秒定时计算
offset：
		0：通讯拉闸延时			2字节，min
		1：大电流拉闸延时		2字节，min（南网无）
		2：红外认证时间			1字节，min
		3：上报自动复位时间	1字节，min
 0xff：远程身份认证时效 2字节，min
*/
unsigned long int Calculate_Timer(unsigned char Offset)
{
	unsigned char len;
	union B32_B08 Temp32;
	
	Temp32.B08[0] = 0;
	Temp32.B08[1] = 0;
	Temp32.B08[2] = 0;
	Temp32.B08[3] = 0;
	
	if( Offset == 0xff )
	{
		Bcd2Hex( Esam_Para.Identity_Delay.B08, Temp32.B08+2, 2 );	//计算身份认证时效
	}
	else
	{
		if( Offset < 2)
		{
			Offset = Offset*2;	//0,2
			len = 2;
		}
		else
		{
			Offset = Offset+2;	//4,5
			len = 1;
		}	
		Bcd2Hex( Mode.Exp_Timer+Offset, Temp32.B08, len );
	}
	
	Temp32.B32 *= 60;	//延时时间分钟转换成秒
	return( Temp32.B32 );
}

	
void Clr_Commu_Relaydata()
{
	Opendelaytimer.B32 = 0;
	relay_commu_open_flag = 0;	//清通讯拉闸标志
	relay_commu_disable_close_flag = 0;
	relay_commu_close2_flag = 0;	//清保电解除标志
}


void Clr_Reportnum( unsigned char i ) //清零上报新增次数
{
	unsigned char Temp08;
	
	for( Temp08=0; Temp08<8; Temp08++ )//根据上报状态字清零上报新增次数
	{
		if( (Report_Data[i]&(1<<Temp08))==0 ) Report_Num[i*8+Temp08] = 0;
	}
}

void Clr_Reportdata( unsigned char i ) //清上报状态字和新增次数(有事件记录类)
{
	unsigned char Temp08;
	
	Temp08 = EventTable[i-ee_clearn_inpage].offset2;
	if( Temp08 != 0xff )
	{	
		Report_Data[Temp08/8] &= ~(1<<(Temp08%8));	//清上报状态字
		Report_Num[Temp08] = 0;	//清新增次数
	}
}

//置位上报状态字和新增次数加1
void Set_Reportdata( unsigned char Offset ) 
{
	unsigned char byt,bt,i,j;
	
	if( Offset == 0xff ) return;
	
	byt = Offset/8;
	bt = Offset%8;
	
	//修正上报模式字
	if( (byt==2)&&(bt==7) )
	{
		i = 4; j = 5; //掉电临时借用存A相断相
	}
	else
	{
		if( byt<3 )   i = byt;	//修正模式字
		else  i = byt+3;
		j = bt;
	}
	
	if( Mode.Report[i]&(1<<j) )	//判断模式字是否置位
	{
		Report_Data[byt] |= (1<<bt);	//置位上报状态字
		i = byt*8+bt;
		if( ((i>=1)&&(i<=9)) || (i==22) ) Report_Num[i] = 0xff;	//无发生次数数据标识的事件传FF
		else if( Report_Num[i] < 0xff ) Report_Num[i]++;	//新增次数加1
	}
}

//准备主动上报状态字和新增次数
unsigned char Ready_Reportdata( unsigned char *Buf )
{
	unsigned char i,n;
	
	Reset_Rep_Timer = Calculate_Timer(3);		//启动复位上报状态字定时器
	memcpy( Report_Data2, Report_Data, 5 );		//备份上报状态字
	
	Buf[0] = 0x00;
	memcpy( Buf+1, Report_Data, 3 );
	memset( Buf+4, 0x00, 7 );
	Buf[11] = Report_Data[3];
	Buf[12] = Report_Data[4];
	Buf[13] = 0xAA;
	
	//修正应答的上报状态字
	if( Buf[3]&B1000_0000 ) {Buf[3]&=B0111_1111; Buf[9]|=B0010_0000;}//掉电临时借用存A相断相 
		
	for( i=0,n=0; i<40; i++ )
	{
		Buf[14+n] = Report_Num[i];
		if( Report_Num[i] ) n++;
	}
	Buf[14+n] = 0xAA;
	return(15+n);
}


//清零点清零数据
void Clr_Zero_Data()
{
	already_setclock_flag = 0;//清广播校时标志
	hangup_comm_flag = 0;//清挂起标志
	Mac_Err_Times = 0;//mac错误次数清零
	Password_Err[0] = 0;//清密码错误次数
	Password_Err[1] = 0;
}


//type=0清非法插卡次数,type=1过流持续时间
void Clr_Otherdata( unsigned char Type )
{
//	非法插卡次数(4)+过流持续时间(4)+电池欠压总累计时间(4)+掉电总累计时间(4)+电池工作时间(4) 
	if( Type > 4 ) return;
	
	Save_Keep_Timer(0);
	
	Keeptime.Times[Type].B32 = 0x00; //过流持续时间(4)
	memset(Eeprom.Buffer+Type*4, 0x00, 4);
	
	I2C_Write_Eeprom( ee_illegalcardnum_page, ee_illegalcardnum_inpage, Eeprom.Buffer, ee_illegalcardnum_len );
}


unsigned char ClockAddOne( unsigned char *Clock, unsigned char Max, unsigned char Def )
{
	unsigned char Temp08;
	
	Temp08 = Bcd_To_Hex( *Clock ); //时
	if( Temp08 < Max )
	{
		*Clock = Hex_To_Bcd( Temp08+1 ); //
		return 0;
	}
	else
	{
		*Clock = Def;
		return 1;
	}
}

unsigned char Add_Min_Clock( unsigned char *Clock, unsigned char Min )
//Clock: ssmmhhDDMMYY
{
	unsigned char Temp08, MaxDay;
	uint16  Year;
	unsigned char code Days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	Temp08 = Bcd_To_Hex(Clock[1]); //mm
	if( (Temp08+Min) < 60 )
	{
		Clock[1] = Hex_To_Bcd( Temp08+Min );
	}
	else
	{
		Clock[1] = Hex_To_Bcd( (Temp08+Min)-60 );
		Temp08 = ClockAddOne( Clock+2, 23, 0 );	//hour
		if( Temp08 )
		{
			Temp08 = Bcd_To_Hex(Clock[4]);
			MaxDay = Days[ Temp08-1 ];
			if( Temp08 == 2 )
			{
				Year = Bcd_To_Hex( Clock[5] );
				if( (Year&0x03) == 0 )	MaxDay++;	//闰年
			}

			Temp08 = ClockAddOne( Clock+3, MaxDay, 1 );	//day
			if( Temp08 )
			{
				Temp08 = ClockAddOne( Clock+4, 12, 1 );	//month
				if( Temp08 ) ClockAddOne( Clock+5, 99, 0 );	//year
			}
		}
	}
	
	return 0;
}

unsigned char Get_RTCTop_Proc( short Temp16 )	//顶点调校值折算
{
	unsigned char i;
	float Rtcadj;
		
	Rtcadj = Temp16/131.76 + Rtcadj_Offset;		//0.0864*1.525*1000
	
	if( Rtcadj >= 0 )
	{
		i = Rtcadj+0.5;
		return i;
	}
	else
	{
		i = (Rtcadj-0.5)*(-1.0);
		return (i|0x80);
	}
}

//密码判断
unsigned char Veri_Psw( uint08 *Buf )
//Buf[0]:权限[1]~[3]:倒序密码
//返回值: 0:正确; 1/2:错误
{
	unsigned char i, I2C_Buf[6];

	i = Buf[0];
	
	if( (i != 3) && (i != 4) ) return (1);
	
	i -= 3; //0,1
	
	if( Password_Err[i] >= Defpassworderrcnt ) return (1);
	
	ReadE2WithBackup( ee_password_page, ee_password_inpage+i*6, I2C_Buf, ee_password_lenth );

	if( Cmp_Bcd_Inv( I2C_Buf, Buf, 4 ) == 0 )	//0-A=B; 1:A>B; 2:A<B
	{
		Password_Err[i] = 0;
		return (0);
	}
	else
	{
		Password_Err[i]++;
		return (1);
	}
}


//不同的冻结对应固定的模式字，冻结模式字是可设置的，用户在冻结模式字中未选择的数据类型，
//单相抄读时应返回异常应答，错误信息字Bit1 位置“1”“无数据请求”。
//数据块抄读，传输时以两个连续的AAH 略过。
void ReadFreezedRec( uint08 Type, uint08 Sn, uint08 No, uint08 *AckBuf )
//Type: 记录类型; No: 上几次; Sn:数据项
//return: 0:读取成功; 1:读取失败
{
	unsigned char k, n, ptrmax, offset, len, style, fill=0xff;
	unsigned char ptr, num, mode;
	uint16 page;
	unsigned char Buf[64];
	
	AckBuf[0] = 0x00;
	
	if( (!BDMODE) && ((Type == 5) || (Type == 7) || (Type == 8) || (Type == 0x0a)) ) return; //远程模式

	switch( Type )
	{
		case 0x00:	//定时冻结, 60次
			ptr = ee_timingf_inpage;
			page = ee_timingfreeze_dl_page;
			num = max_timingfreeze_p;
			mode = Mode.Freeze[1];
			break;

		case 0x01:	//瞬时冻结, 3次
			ptr = ee_immedf_inpage;
			page = ee_realtimefreeze1_page;
			num = 3;
			mode = Mode.Freeze[2];
			break;
		
		case 0x02:	//约定冻结, 时区表切换, 2次
			ptr = ee_agreesqf_inpage;
			page = ee_appointfreeze1_page;
			num = 2;
			mode = Mode.Freeze[3];
			break;
		
		case 0x03:	//约定冻结, 日时段表切换, 2次
			ptr = ee_agreesdf_inpage;
			page = ee_appointfreeze2_page;
			num = 2;
			mode = Mode.Freeze[3];
			break;

		case 0x04:	//整点冻结: 冻结时间+正向有功总电能+反向有功总电能, 254次, 整点冻结时间间隔默认为60分钟。
			ptr = ee_hourf_inpage;
			page = ee_hourfreezedl_page;
			num = max_hourfreeze_p;
			mode = Mode.Freeze[4];
			break;
			
		case 0x06:	//日冻结, 62次
			ptr = ee_dayf_inpage;
			page = ee_dayfreezedl_page;
			num = max_dayfreeze_p;
			mode = Mode.Freeze[5];
			break;	

		case 0x05:	//约定冻结, 费率电价切换, 2次
			ptr = ee_agreeflf_inpage;
			page = ee_appointfreeze3_page;
			num = 2;
			mode = Mode.Freeze[3];
			break;
		
		case 0x07:	//约定冻结, 阶梯切换, 2次
			ptr = ee_agreejtf_inpage;
			page = ee_appointfreeze4_page;
			num = 2;
			mode = Mode.Freeze[3];
			break;

		case 0x08:	//日冻结总购电次数+剩余金额+透支金额
			ptr = ee_dayf_inpage;
			num = max_dayfreeze_p;
			break;	

		case 0x09:	//约定冻结, 费率模式切换, 2次
			ptr = ee_modeswith_inpage;
			page = ee_modeswith_note_page;
			num = 2;
			mode = Mode.Freeze[3];
			break;
			
		case 0x0A:	//约定冻结, 阶梯时区切换, 2次
			ptr = ee_jtsqswith_inpage;
			page = ee_jtsqswith_note_page;
			num = 2;
			mode = Mode.Freeze[3];
			break;
			
		default:
			return;
	}

	if( (No==0) || (No > num) ) return;	//次数超限
	
	ptrmax = RdRecTimes( ptr );//返回有效指针
	ptr = Read_Event_Point( ptr );
	if( ptr < (No-1) ) ptr += num;
	ptr -= (No-1);

	if( Type == 0x08 )	//日冻结总购电次数+剩余金额+透支金额
	{
		if( Sn != 0x02 ) return;
		
		if( ptrmax < No )	memset( AckBuf+1, 0x00, 10 );
		else I2C_Read_Eeprom( ee_dayfreezemoney_page+ptr, ee_dayfreezemoney_inpage, AckBuf+1, 10 );

		AckBuf[0] = 10;	//设置返回数据的长度
		return;
	}
	
	if( Type == 0x04 ) //整点冻结
	{
		page += (ptr>>2);
		offset = (ptr%4)*13;
		len = ee_hourfreezedl_lenth;
		style = 4;	//偏移地址和长度
	}
	else
	{
		page += ptr*2;
		offset = 0x00;
		len = ee_freezedl_lenth/2;
		style = 20;	//偏移地址和长度
	}

	if( ptrmax < No )	memset( Buf, 0x00, len*2 );
	else 
	{	
		I2C_Read_Eeprom( page, offset, Buf, len );
		if( Type != 0x04 )	I2C_Read_Eeprom( page+1, offset, Buf+len, len );
	}
		
	k = 0x01;
	switch( Sn )
	{
		case 0xFF:
		case 0x00: //冻结时间:YYMMDDhhmm
			memcpy( AckBuf+k, Buf, 5 );
			k += 5;
			if( Sn == 0x00 ) break;
			AckBuf[k++] = 0xAA;
			
		default: 
			for( n=0; n<2; n++ )
			{
				if( (Sn == (n+1) || (Sn == 0xFF)) )	//正向有功总电能, 反向有功总电能
				{
					if( mode & (n+1) )
					{
						memcpy( AckBuf+k, Buf+5+n*style, style );
						k += style;
					}

					if( Sn == (n+1) ) break;
					AckBuf[k++] = 0xAA;
				}
			}
			
			if( Type == 0x04 ) break;	//整点冻结
			
			//组合无功1~2电能数据, 第一~四象限无功电能数据, 正向有功最大需量, 反向有功最大需量
			if( Sn == 0xFF )
			{
				memset( AckBuf+k, 0xAA, 8 );
				k += 8;
			}
			
			//变量数据: 总有功功率
			if( (Sn == 0x10) || (Sn == 0xFF) )
			{
				if( mode & 0x80 )
				{
					memcpy( AckBuf+k+0, Buf+45, 3 );	//总有功功率
					memcpy( AckBuf+k+3, Buf+45, 3 );	//A 相有功功率
					if( ptrmax < No ) fill = 0x00;
					memset( AckBuf+k+6, fill, 18 );		//其他功率
					k += 24;
				}

				if( Sn == 0x10 ) break;
				AckBuf[k++] = 0xAA;
			}
			break;
	}

	AckBuf[0] = k-1;
	return;
}


uint08 Find_EventTable( unsigned char *DI )
{
	unsigned char  i;
	union B16_B08 Temp16;
	
	Temp16.B08[0] = DI[0];
	Temp16.B08[1] = DI[1];
	
	for( i=0; i<24; i++ )
	{
		
		if( Temp16.B16 == EventTable[i].DI21 )	break;
	}

	if( (!BDMODE) && (i > ee_batterlow_inpage-ee_clearn_inpage) ) return 24;	//远程模式
	if( i >= 24 ) return 24;
	
	return i;
}


unsigned char BitValue( unsigned char *InBuf, unsigned char Pos )
{
	return ( (InBuf[Pos/8]>>(Pos%8))&1 );
}

unsigned char ReadMaxDemand( unsigned char *DIx, unsigned char *ReturnBuf )
{
	unsigned char result,ptrmax;
	unsigned char xdata Buf[64];
	unsigned int  Page;
	unsigned char Inpage;
	ReturnBuf[0] = 0x00;
	
	if( DIx[0] == 0x00 )	//当前
	{
		Page = ee_p_demand_page;
		Inpage = (DIx[2]-0x01)*42;
	}
	else if( DIx[0] <= 12 )	//1~12结算日
	{
		ptrmax = RdRecTimes(ee_histroyDM_inpage);//返回有效指针
		result = Read_Event_Point(ee_histroyDM_inpage);
		
		if( result < (DIx[0]-1) ) result += max_histroydm_p;
		result -= (DIx[0]-1);
		
		Page = ee_histroyp_demand_page+result;
		Inpage = (DIx[2]-0x01)*42;
	}
	
	I2C_Read_Eeprom( Page, Inpage, Buf, ee_pdemand_lenth );

	if( DIx[1] <= 4 )	//总, 1~4费率
	{
		ReturnBuf[0] = 8;
		memcpy( ReturnBuf+1, Buf+8*DIx[1], 8 );
		if( ptrmax < DIx[0] )	memset( ReturnBuf+1, 0x00, 8 );
	}
	else
	{
		ReturnBuf[0] = 40;
		memcpy( ReturnBuf+1, Buf, 40 );
		if( ptrmax < DIx[0] )	memset( ReturnBuf+1, 0x00, 40 );
	}

	return 0;
}


void Rd_Relay_Buy_Rec( unsigned char *DI, unsigned char *AckBuf )
{
	unsigned char i, k, n, result, inpage, offset=0x00, ptr, len=0, total_len, rec_len, ptrmax, Temp08;
	uint16  page;
	union B16_B08 Temp16;
	unsigned char Buf[64];
	unsigned char code Tab_Buy[] = {5,2,4,4,4,4};
	unsigned char code Tab_Reley[]={6,4,4,4};
	unsigned char code Tab_Maxi[]= {6,4,4,4,4,4,4,4,4,2,3,3,3,2,4,4,4,4,2,3,3,3,2,4,4,4,4,2,3,3,3,2, 
				       				6,4,4,4,4,4,4};
	unsigned char code Tab_PwrDn[]={6,4,4,4,4,4,4,4,4,          4,4,4,4,          4,4,4,4,
									6,4,4,4,4,4,4};
	const uint08 code MaxiOffset1[8] = { 0,  6, 22, 38, 49, 109, 109+6, 109+22 };
	const uint08 code MaxiOffset2[8] = { 0, 12, 12, 20, 28, 6, 30, 30 };
	const uint08 code MaxiLenth[8] = { 6,  8,  0,  8,  2, 6,  8,  0 };
	
	unsigned char code PwrDnOffset1[6] = { 0, 6,22,70,70+6,70+22 };
	unsigned char code PwrDnOffset2[6] = { 0,12,12, 6, 20, 20 };
	unsigned char code PwrDnLen[6]     = { 6, 8, 0, 6, 8,  0 };
	unsigned char code MaxiSup [7] = { B0000_0111, B0010_1110, B0000_0000, B0000_0000, B0000_0111, B0000_0000, B0000_0000 };
	unsigned char code PwrDnSup[5] = { B0000_0111, B0000_0000, B0000_1110, B0000_0000, B0000_0000 };

	AckBuf[0] = 0x00;	//缺省返回数据长度是0x00

	if( (DI[1] == 0xFF) && (DI[0] == 0xFF) ) return;

	i = Find_EventTable(DI+2);		//判断是否找到合法数据标识
	if( i == 23 )	return;

	page = EventTable[i].page;
	inpage = EventTable[i].offset;
	rec_len = EventTable[i].len;
	ptr = i+ee_clearn_inpage;
	
	switch( ptr )
	{
		case ee_powerdownn_inpage: //掉电记录
			total_len = 140;
			len = 140;

			if( DI[1] != 0xFF )
			{
				if( DI[1] == 0 )
				{
					if( DI[0] == 0x0c )//读掉电总累计时间
					{
						Get_Keep_Timer(3, AckBuf);
						return;
					}
				}
				else
				{
					if( (DI[1] > 0x22) || (BitValue(PwrDnSup, DI[1]-1) == 0) ) return;
	
					len = Tab_PwrDn[DI[1]-1];
					offset += Cal_ChkSum( Tab_PwrDn, (DI[1]-1) );
				}
			}
			break;
			
		case ee_maxi_inpage:	//过流
			total_len = 179;
			len = 179;

			if( DI[1] != 0xFF )
			{
				if( DI[1] == 0 )
				{
					if( DI[0] == 2 )//读过流累计时间
					{
						Get_Keep_Timer(1, AckBuf);
						return;
					}
				}
				else
				{
					if( (DI[1] > 0x31) || (BitValue(MaxiSup, DI[1]-1) == 0) ) return;
	
					len = Tab_Maxi[DI[1]-1];
					offset += Cal_ChkSum( Tab_Maxi, (DI[1]-1) );
				}
			}
			break;

		case ee_batterlow_inpage:	//电池欠压记录
			if( DI[1] == 0 )
			{
				if( DI[0] == 0 ) //电池欠压总累计时间
				{
					Get_Keep_Timer(2, AckBuf);
					return;
				}
				else if( DI[0] == 1 ) //电池欠压记录
				{
					total_len = 70;
					len = 70;
				}
			}
			break;

		case ee_buymoneyn_inpage: //购电记录
			total_len = 23;
			len = 23;

			if( DI[1] != 0xFF )
			{
				if( (DI[1]==0x00) || (DI[1]>6) ) return;
				
				if( DI[1] )
				{
					len = Tab_Buy[DI[1]-1];
					offset += Cal_ChkSum( Tab_Buy, (DI[1]-1) );
				}
			}
			break;

		case ee_openrelayn_inpage:	//跳闸次数
		case ee_closerelayn_inpage:	//合闸次数
			total_len = 34;
			len = 34;

			if( DI[1] != 0xFF )
			{
				if( DI[1] > 4 ) return;

				if( DI[1] )
				{
					len = Tab_Reley[DI[1]-1];
					offset += Cal_ChkSum( Tab_Reley, (DI[1]-1) );
				}
			}
			break;

		default:
			return;
	}

	if( len == 0 ) return;

	if( (ptr != ee_batterlow_inpage) && (DI[1] == 0) )
	{
		if( ((ptr == ee_powerdownn_inpage) && (DI[0] == 0)) || //掉电记录
			((ptr != ee_powerdownn_inpage) && (DI[0] == 1)))	 //其他记录
		{
			Read_Event_Number( AckBuf+1, ptr );
			AckBuf[0] = 3;
		}

		return;		
	}
					
	if( (DI[0]) && (DI[0]<=max_eventrecord_p) )	//上1~10次
	{
		Temp16.B08[0] = DI[0]-1;
		Temp16.B08[1] = DI[0];
	}
	else if( DI[0] == 0xFF )
	{
		Temp16.B08[0] = 0;
		Temp16.B08[1] = max_eventrecord_p;
	}
	else
	{
		return;
	}

	ptrmax = RdRecTimes(ptr);//返回有效指针
	Temp08 = Read_Event_Point( ptr );
	
	k = 0x01;
	for( n=Temp16.B08[0]; n<Temp16.B08[1]; n++ )
	{
		result = Temp08;
		if( result < n ) result += max_eventrecord_p;
		result -= n;

		if( ptrmax < n+1 ) memset( Ic_Comm_TxBuf, 0x00, total_len );
		else
		{
			memset( Ic_Comm_TxBuf, 0xFF, total_len );
			I2C_Read_Eeprom( page+result, inpage, Buf, rec_len );//读存储在数据ee里的有效数据
			if( ptr == ee_maxi_inpage )	//过流则重新组帧数据
			{
				for( i=0; i<8; i++ ) memcpy( Ic_Comm_TxBuf+MaxiOffset1[i], Buf+MaxiOffset2[i], MaxiLenth[i] );//发生时刻
			}
			else if( ptr == ee_powerdownn_inpage )	//掉电则重新组帧数据
			{
				for( i=0; i<6; i++ ) memcpy( Ic_Comm_TxBuf+PwrDnOffset1[i], Buf+PwrDnOffset2[i], PwrDnLen[i] );//发生时刻
			}
			else if( ptr == ee_batterlow_inpage )	//电池欠压
			{
				memcpy( Ic_Comm_TxBuf+ 0, Buf+0, 14 );//发生时刻+正反向电量
				memcpy( Ic_Comm_TxBuf+22, Buf+6,  8 );//正反向电量
			}
			else 
			{
				memcpy( Ic_Comm_TxBuf, Buf, rec_len );
			}
		}
		
		memcpy( AckBuf+k, Ic_Comm_TxBuf+offset, len );
		k += len;
	}
	
	AckBuf[0] = k-1;
}


uint08 RdHisEvent( unsigned char *DI, unsigned char SN, uint08 *AckBuf )
//return 0: success; other: failure.
{
	unsigned char  i, k, n, TmpPtr, ptrmax, MaxEvent = max_eventrecord_p;
	unsigned char offset, ptr, len;
	uint16  page, TmpP1, TmpP2;
	unsigned char Buf[64], TmpBuf[5], ReadPtr[16];
	union B32_B08 Temp32;

	AckBuf[0] = 0x00;

	i = Find_EventTable(DI+1);		//判断是否找到合法数据标识
	if( i == 24 )	return 1;

	page = EventTable[i].page;
	offset = EventTable[i].offset;
	len = EventTable[i].len;
	ptr = i+ee_clearn_inpage;

	if( ptr == ee_powerdownn_inpage || 
		ptr == ee_maxi_inpage		||
		ptr == ee_openrelayn_inpage ||
		ptr == ee_closerelayn_inpage||
		ptr == ee_batterlow_inpage	||
		ptr == ee_buymoneyn_inpage ) return 1;	//非法数据标识
	
	if( len > 0x00 )
	{	
		if( DI[0] == 0x00 )	//总次数
		{
			if( ptr == ee_dmoneyn_inpage )  return 2;	//退费记录无总次数
			
			Read_Event_Number( AckBuf+1, ptr );
			AckBuf[0] = 3;
		}
		else	//事件记录
		{
			if( ptr == ee_keysetn_inpage ) MaxEvent = 2;	//密钥更新记录只有2条
			
			if( DI[0] > MaxEvent ) return 4;	//次数超限
			
			ptrmax = RdRecTimes(ptr);//返回有效指针
			ptr = Read_Event_Point( ptr );
			
			TmpPtr = ptr;	//备份上一次事件记录指针
			
			if( ptr < (DI[0]-1) )
			{
				ptr += MaxEvent;
			}
			
			ptr -= (DI[0]-1);
	
			I2C_Read_Eeprom( page+ptr, offset, Buf, len );
			memcpy( AckBuf+1, Buf, len );

			switch( DI[1] )
			{
				case 0x01:	//电表清零
					AckBuf[0] = 106;	//设置返回数据的长度
					memset( AckBuf+19, 0xFF, 88 );
					break;
				case 0x02:	//需量清零记录
					switch( SN )
					{
						case 0x00:	//发生时间, 操作者代码, 正反向, 第一~四象限
							AckBuf[0] = 58;	//设置返回数据的长度
							
							memset( AckBuf+27, 0xFF,  32 );
							break;
						
						case 0x01:	//A,B,C三相: 正反, 第一~四象限
							AckBuf[0] = 145;	//设置返回数据的长度
							memset( AckBuf+1, 0xFF, 144 );
							AckBuf[145] = SN;
							break;
						
						default:
							break;
					}
					break;
					
				case 0x05:	//时段表编程
				case 0x06:	//时区表编程
					//以(Buf中第几个半字节的值)为"指针所指向的ReadPtr的对应字节"中写入几。
					I2C_Read_Eeprom( page+TmpPtr, 0, TmpBuf, 5 );	//读取上1次日时段表编程记录页内历史记录
					
					memset( ReadPtr, 0xFF, 16 );
	
					for( n=0; n<DI[0]; n++ )
					{
						if( TmpPtr%2 )	ReadPtr[ (TmpBuf[TmpPtr/2]&0xF0)>>4 ] = TmpPtr;
						else			ReadPtr[ (TmpBuf[TmpPtr/2]&0x0F)    ] = TmpPtr;
	
						if( TmpPtr ) TmpPtr--;
						else	TmpPtr = MaxEvent-1;
					}

					if( DI[1] == 0x05 )
					{
						TmpP1 = ee_shiduantable1_1_page;
						TmpP2 = ee_shiduanrecord_page;
					}
					else
					{
						TmpP1 = ee_shiqu1_page;
						TmpP2 = ee_shiqurecord_page;
						
						if( SN ) return 0x00;
					}
					
					switch( SN )
					{
						case 0x00:
							if( DI[1] == 0x05 )
							{
								Temp32.B08[0] = 196;//从E2中抄读字节总数
								Temp32.B08[1] = 0;	//起始页
								Temp32.B08[2] = 4;	//结束页
								Temp32.B08[3] = 0;	//偏移量
							}
							else
							{
								Temp32.B08[0] = 94;	//从E2中抄读字节总数
								Temp32.B08[1] = 0;	//起始页
								Temp32.B08[2] = 1;	//结束页
								Temp32.B08[3] = 0;	//偏移量
							}

							memcpy( AckBuf+1, Buf+5, 10 );	//发生时间+操作者代码
							k = 10;	//返回缓冲器地址指针
							break;
							
						case 0x01:
							Temp32.B08[0] = 195;//从E2中抄读字节总数
							Temp32.B08[1] = 4;	//起始页
							Temp32.B08[2] = 9;	//结束页
							Temp32.B08[3] = 18;	//偏移量
							k = 0;	//返回缓冲器地址指针
							break;

						case 0x02:
							Temp32.B08[0] = 195;//从E2中抄读字节总数
							Temp32.B08[1] = 9;	//起始页
							Temp32.B08[2] = 13;	//结束页
							Temp32.B08[3] = 3;	//偏移量
							k = 0;	//返回缓冲器地址指针
							break;
							
						case 0x03:
							Temp32.B08[0] = 96;//从E2中抄读字节总数
							Temp32.B08[1] = 13;	//起始页
							Temp32.B08[2] = 15;	//结束页
							Temp32.B08[3] = 30;	//偏移量
							k = 0;	//返回缓冲器地址指针
							break;
							
						default:
							return 0x00;
					}

					AckBuf[0] = Temp32.B08[0];	//设置返回数据的长度

				for( n=Temp32.B08[1]; n<=Temp32.B08[2]; n++, k+=len )	//
				{
					if( ReadPtr[n] == 0xFF )
					{
						page = TmpP1+n;
						offset = 0;
					}
					else
					{
						page = TmpP2+ReadPtr[n];
						offset = 15;
					}

					len = 42;	//默认读取的长度
					if( n == Temp32.B08[1] ) //第4页被分成两次读取。上一帧读取了18字节, 本次读取18字节之后。
					{
						len    -= Temp32.B08[3];
						offset += Temp32.B08[3];
					}
					
					if( k+len > Temp32.B08[0] ) len = Temp32.B08[0]-k;

					I2C_Read_Eeprom( page, offset, AckBuf+1+k, len );	//当前时段表。
				}
				break;

				case 0x0D:	//开表盖
					AckBuf[0] = 60;	//设置返回数据的长度
					memset( AckBuf+21, 0xFF, 40 );
					memcpy( AckBuf+37, Buf+20, 8 );
					break;
					
				case 0x10:	//阶梯编程
				switch( SN )
				{
					case 0x00:
						Temp32.B08[0] = 196;//从E2中抄读字节总数
						Temp32.B08[1] = 0;	//起始页
						Temp32.B08[2] = 2;	//结束页
						Temp32.B08[3] = 0;	//偏移量
						k = 10;	//返回缓冲器地址指针
						break;
							
					case 0x01:
						Temp32.B08[0] = 195;//从E2中抄读字节总数
						Temp32.B08[1] = 2;	//起始页
						Temp32.B08[2] = 5;	//结束页
						Temp32.B08[3] = 46;	//偏移量
						k = 0;	//返回缓冲器地址指针
						break;

					case 0x02:
						Temp32.B08[0] = 179;//从E2中抄读字节总数
						Temp32.B08[1] = 5;	//起始页
						Temp32.B08[2] = 7;	//结束页
						Temp32.B08[3] = 31;	//偏移量
						k = 0;	//返回缓冲器地址指针
						break;
							
					default:
						return 0x00;
				}
					
				for( n=Temp32.B08[1]; n<=Temp32.B08[2]; n++ )
				{
					I2C_Read_Eeprom( ee_jtpricerecord2_page+ptr*8+n, Temp32.B08[3], AckBuf+1+k, ee_jtpricerecord2_len-Temp32.B08[3] );
					k += (ee_jtpricerecord2_len-Temp32.B08[3]);
					Temp32.B08[3] = 0x00;
				}

				AckBuf[0] = Temp32.B08[0];	//设置返回数据的长度
				break;
					
				default:
					AckBuf[0] = len;	//设置返回数据的长度
					break;
			}
			
			if( ptrmax < DI[0] )	memset( AckBuf+1, 0x00, AckBuf[0] );
			if( ((DI[1] == 0x05) || (DI[1] == 0x10)) && SN  )	AckBuf[ ++AckBuf[0] ] = SN;
		}
	}
	
	return 0x00;	//读取成功
}

//组合电量计算函数, 每次计算1组
void CalZhuhe( const unsigned char *ZhuBuf, unsigned char *RtBuf, unsigned char Mode, unsigned char Offset )
//ZhuBuf要求必须包含正向和反向的总尖峰平谷电量。
//RtBuf是返回组合电量，4字节。
{
	signed long int Zhen;
	union B32_B08 Temp32;
	
	Zhen = 0x00;

	switch( Mode&B0000_0011 )
	{
		case 0x01:
		case 0x02:
			Bcd2Hex( (uint08*)ZhuBuf, Temp32.B08, 4 );
			if( (Mode&B0000_0011) == 0x01 )
			{
				Zhen += Temp32.B32;
			}
			else
			{
				Zhen -= Temp32.B32;
			}
			break;
			
		default:
			break;
	}

	switch( Mode&B0000_1100 )
	{
		case 0x04:
		case 0x08:
			if( Offset == 0 )
			{
				Bcd2Hex( (uint08*)ZhuBuf+20, Temp32.B08, 4 );
			}
			else
			{
				Bcd2Hex( (uint08*)ZhuBuf+4, Temp32.B08, 4 );
			}
			if( (Mode&B0000_1100) == 0x04 )
			{
				Zhen += Temp32.B32;
			}
			else
			{
				Zhen -= Temp32.B32;
			}
			break;
			
		default:
			break;
	}

	Temp32.B32 = labs(Zhen);
	if( Temp32.B32 > 79999999 )	//组合: 组合有功、组合无功的数值范围变为：0.00～799999.99
	{
		Temp32.B32 -= 80000000;	//对此，要求在到达极限值时进行归零处理。
		if( Temp32.B32 > 79999999 )	Temp32.B32 -= 80000000;
	}

	Hex2Bcd( Temp32.B08, RtBuf, 4 );

	if( Zhen < 0x00 )	RtBuf[0] |= 0x80;	//组合电量<0，最高位置1。
}


unsigned char Cal_Comb( unsigned char DI2, unsigned char DI1, unsigned char DI0, unsigned char *ReturnBuf )
//DI2: 0:组合; 1:正向; 2:反向; 3:结算时间
//DI1: 0:总(结算时间); 1~4:1~4费率; FF:所有
//DI0: 0:当前; 1~12:1~12结算日; FF:所有(除13外)
//ReturnBuf: 数据均为反序
//当前电量,用电量,结算电量均以高字节在前存数据ee
{
	unsigned char i, k, n, result, Temp08, ptrmax;
	unsigned char Buf[64];
	union B32_B08 Temp32;

	if( DI2 > 0x03 ) return 0x00;
	
	if( (DI1 == 0xFF) && (DI0 == 0xFF) ) return 0x00;	//两者不能同时为FF。
	
	if( (DI1 > 0x04) && (DI1 != 0xFF) ) return 0x00;
	
	if( DI0 == 0xFF ) 
	{
		Temp32.B08[0] = 0;
		Temp32.B08[1] = 13;
	}
	else
	{
		Temp32.B08[0] = DI0;
		Temp32.B08[1] = DI0+1;
	}
	
	if( DI1 == 0xFF ) 
	{
		Temp32.B08[2] = 0;
		Temp32.B08[3] = 5;
	}
	else
	{
		Temp32.B08[2] = DI1;
		Temp32.B08[3] = DI1+1;
	}
		
	if( DI0 ) //非当前数据
	{
		ptrmax = RdRecTimes(ee_histroyf_inpage);//返回有效指针
		Temp08 = Read_Event_Point(ee_histroyf_inpage);
	}

	k = 0x00;
	for( n=Temp32.B08[0]; n<Temp32.B08[1]; n++ )	//DI0: 0~12,FF
	{
		if( n == 0x00 )	//当前
		{
			if( DI2 != 0x03 )
			{
				memcpy( Buf, Fwdl.Fwdl_T.B08, 20 );
				memcpy( Buf+20, Bkdl.Bkdl_T.B08, 20 );
			}
		}
		else if( n <= 12 )	//1~12结算日
		{
			result = Temp08;
			if( result < (n-1) ) result += max_histroy_p;
			result -= (n-1);

			if( DI2 != 0x03 )
			{
				I2C_Read_Eeprom( ee_histroy_dl_page+result*2, 0x05, Buf, ee_histroydl_lenth/2-5 );
				I2C_Read_Eeprom( ee_histroy_dl_page+result*2+1, 0x00, Buf+ee_histroydl_lenth/2-5, ee_histroydl_lenth/2 );
				if( ptrmax < n )	memset( Buf, 0x00, ee_histroydl_lenth-5 );
			}
			else	//结算时间
			{
				I2C_Read_Eeprom( ee_histroy_dl_page+result*2, 0x01, ReturnBuf+k, 4 );
				if( ptrmax < n )	memset( ReturnBuf+k, 0x00, 4 );
				
				k += 4;
			}
		}
		else return 0x00;

		if( DI2 != 0x03 )
		{
			for( i=Temp32.B08[2]; i<Temp32.B08[3]; i++, k+= 4 )
			{
				if( DI2 == 0x00 )	//组合
				{
					if( n == 0x00 )	//当前
						result = Mode.Comp_B;
					else
						result = Buf[42];
					
					CalZhuhe( Buf+i*4, ReturnBuf+k, result, 0 );
					
//					Inverse( ReturnBuf+k, 4 );
				}
				else	//正向电量 or 反向电量
				{
					memcpy( ReturnBuf+k, Buf+(DI2-1)*20+i*4, 4 );
				}
			}
		}
	}

	return (k);
}



void Cal_Comb_Usedl( unsigned char DI2, unsigned char DI0, unsigned char *ReturnBuf )
//DI2: 0:结算周期用电量;1:月用电量;2:年用电量
//DI0: 0:当前; 1:上1
//ReturnBuf: 数据均为反序
//当前电量,用电量,结算电量均以高字节在前存数据ee
{
	unsigned char result;
	unsigned char Buf[ee_yhistroyusedl_lenth+8];
	
	//准备数据
	memcpy( Buf, Sclock.Clockbackup+2, 4 );//复制时间
	Cal_Comb( 0, 0, 0, Buf+4 );	//计算当前组合电量	
	Inverse( Buf+4, 4 );

	I2C_Read_Eeprom( ee_pre1_histroyusedl_page+DI2, 0, Buf+8, ee_yhistroyusedl_lenth );

	memcpy( ReturnBuf+4, Buf+8*DI0+4, 4);
	memcpy( ReturnBuf+8, Buf+8*DI0+4+8, 4);
	
	//计算用电量
	if( ReturnBuf[4]&0x80 )	result = 0x02;
	else result = 0x01;
	ReturnBuf[4] &= (~0x80);

	if( ReturnBuf[8]&0x80 )	result |= 0x04;
	else result |= 0x08;
	ReturnBuf[8] &= (~0x80);
	
	CalZhuhe( ReturnBuf+4, ReturnBuf+4, result, 0x01 );
	
	//应答数据
	Inverse( ReturnBuf+4, 4 );		//转换成低字节在前
	memcpy( ReturnBuf, Buf+8*DI0, 4 );	//结算时间

	if( DI2 == 0x02 ) //年用电量
	{
		if( (Price.Ladder_Mode == 0x00) && (DI0 == 0x00) )
		{
			memset( ReturnBuf, 0x00, 8 );//月阶梯,当前年结算电量为0	
		}
	}
}


//编程记录的说明：从按下编程开关到弹起编程开关，且对电能表进行编程操作，
//记录为一次编程记录，记录编程的最后10 个数据标识及编程结束时刻。
//编程记录结束时刻：1.按键退出编程模式; 2.编程有效时间结束; 3.身份认证失效; 4.身份认证有效时间结束
//以上4种情况下需要设置New_Program_Rec = 0;
void ProgramRecord( unsigned char *Dix, unsigned char *Opt )
{
	if( first_prog_flag == 0 )	//需要重新增加一条编程记录
	{
		first_prog_flag = 1;	//继续本次编程记录
		memcpy( Eeprom.Buffer+6, Opt, 4 );			//操作者代码(4)
		memcpy( Eeprom.Buffer+10, Dix, 4 );			//数据标识(4)
		memset( Eeprom.Buffer+14, 0xFF, 36 );			//不足的补0xFF(4*9)
		Event_Record(ee_programn_inpage);
	}
	else
	{
		memcpy( Eeprom.Buffer+50, Dix, 4 );			//数据标识(4)
		Event_Record(ee_programn_inpage|0x80);
	}
}


unsigned char UpdateSysPara( unsigned char Start, unsigned char *Para, unsigned char Len )
{
	unsigned char result, Temp08, Inpage;
	uint16 Page;
	unsigned char Buf[64];
	unsigned char  *ptr;
	
	memset( Buf,0, 64 );	//更新RAM\
	
	switch( Start&0xE0 )
	{
		case 0x00:
			Page = ee_displaypara_page;
			Inpage = ee_displaypara_inpage;
			Temp08 = ee_displaypara_lenth;
			ptr =  &Display_Para.S_Number;
			disppm_veri_flag = 1;
			break;
			
		case 0x40:
			Page = ee_esampara_page;
			Inpage = ee_esampara_inpage;
			Temp08 = ee_esampara_lenth;
			ptr = Esam_Para.Alarm_Limit1.B08;
			break;

		case 0x60:
			Page = ee_weekholiday_page;
			Inpage = ee_weekholiday_inpage;
			Temp08 = ee_weekholiday_lenth;
			ptr = &Week_Holiday.Status;
			run_timezone_flag = 1;
			Tariff.Crc16.B16 += 1;
			break;
			
		case 0x80:
			Page = ee_featureword_page;
			Inpage = ee_featureword_inpage;
			Temp08 = ee_featureword_lenth;
			ptr =  Mode.Bps;
			mode_veri_flag = 1;
			break;
			
		case 0xA0:
			Page = ee_eventthreshold_page;
			Inpage = ee_eventthreshold_inpage;
			Temp08 = ee_eventthreshold_lenth;
			ptr =  Threshold;
			mode_veri_flag = 1;
			break;
			
		case 0xC0:
			Page = ee_starttimer_page;
			Inpage = ee_starttimer_inpage;
			Temp08 = ee_starttimer_lenth;
			ptr = Starttime.Shiqu;
			run_timezone_flag = 1;
			break;
			
		default:
			return 1;
	}
	
	//更新RAM和E2中的客户编号
	ReadE2WithBackup( Page, Inpage, Buf, Temp08 );
	Exchange( Buf+(Start&0x1F), Para, Len );	//数据倒序
	result = WriteE2WithBackup( Page, Inpage, Buf, Temp08 );
	if( result ) return 1;
	//ReadE2WithBackup( Page, Inpage, Buf, Temp08 );
	memcpy( ptr, Buf, Temp08+2 );	//更新RAM
	return 0;
}


unsigned char JudgeSwitch( unsigned char *Buf, unsigned char Inv )
{
	unsigned char i, result;

	if( ChkBufZero(Buf,5)==0 ) return 2;//合法特殊时间(全0x00)
	
	for( i=0; i<5; i++ )
	{
		if( Buf[i] != 0x99 ) break;
	}
	
	if( i < 5 )
	{
		if( Inv ==1 )	Inverse( Buf, 5 );
		result = JudgeClockNoWeek( 1, Buf, 5 );
		if( Inv ==1 )	Inverse( Buf, 5 );
		return (result); //错误
	}
	else
	{
		return 3;		//合法特殊时间(全0x99)
	}
}

void JudgeSwitch2( unsigned char offset )//判断启动时间是否为全99,如果是则默认第一套
{
	I2C_Read_Eeprom44( ee_changecontrol_page, Eeprom.Buffer, ee_changecontrol_lenth);//读启动控制字节
	if( Eeprom.Buffer[ offset ] != 0xFF )
	{
		Eeprom.Buffer[ offset ] = 0xFF;
		I2C_Write_Eeprom44( ee_changecontrol_page, Eeprom.Buffer, ee_changecontrol_lenth ); //写启动控制字节
		Tariff.Crc16.B16 += 1;
		
		//进行约定冻结
		Freeze_Dl(ee_appointfreeze1_page+4*offset,5+offset);
	}
}

//时区/时段表编程记录
uint16 ShiquRecord( unsigned char *InDi, unsigned char *InOpt )
{
	unsigned char i, n, Temp08, Ptr;
	union B16_B08 AddrA, AddrB;
	unsigned char Buf[64];
	
	if( InDi[0] == 0x00 )	//时区表
	{
		Ptr = ee_shiqun_inpage;
		AddrA.B16 = ee_shiqurecord_page;
		AddrB.B16 = ee_shiqu1_page;
	}
	else	//日时段表
	{
		Ptr = ee_shiduann_inpage;
		AddrA.B16 = ee_shiduanrecord_page;
		AddrB.B16 = ee_shiduantable1_1_page;
	}
		
	//历史记录(5,每字节代表2次)+发生时刻（6）+操作者代码（4）+ 时区表编程前第几套第1~14 时区表数据（3*14）
	//历史记录(5,每字节代表2次)+发生时刻（6）+操作者代码（4）+ 时段表编程前第几套第几 日时段表数据（3*14）
	Number_Add1( Ptr );	//时区/时段修改总次数+1(3)
	
	Temp08 = Read_Event_Point( Ptr );
	I2C_Read_Eeprom( AddrA.B16+Temp08, 0x00, Buf, 5 );	//历史记录(5,每字节代表2次)
	
	//i范围时区0～1,时段0～15
	if( InDi[2] == 1 )	i = 0;
	else
	{
		if( InDi[0] == 0 )	i = 1;	//时区表
		else i = 8;	//日时段表
	}
	if( InDi[0] ) i += (InDi[0]-1);			//计算得到第i个表被修改

	Temp08 = Write_Event_Point( Ptr );	//计算指针+1

	n = Temp08/2;
	if( Temp08%2 )	//历史记录(5,每字节代表2次)
		Buf[n] = (Buf[n]&0x0F)+(i<<4);		//记录对应字节的高4位
	else
		Buf[n] = (Buf[n]&0xF0)+i;			//记录对应字节的低4位
	
	memcpy( Buf+5, Sclock.Clockbackup, 6 );	//发生时刻(6)
	memcpy( Buf+11, InOpt, 4 );				//操作者代码(4)
	I2C_Read_Eeprom( AddrB.B16+i, 0x00, Buf+15, 42 );	//编程前 时区表or时段第i表(42)
	
	I2C_Write_Eeprom( AddrA.B16+Temp08, 0x00, Buf, ee_shiduanrecord_lenth );	//时区表
	
	return (AddrB.B16+i);
}


void FLRecord( unsigned char *InOpt )
{
	//--- 费率编程记录 ---
	memcpy( Eeprom.Buffer+6, InOpt, 4 );			//操作者代码(4)
	I2C_Read_Eeprom( ee_tariffprice1_page, 0x00, Eeprom.Buffer+10, ee_tariffprice_lenth );
	I2C_Read_Eeprom( ee_tariffprice2_page, 0x00, Eeprom.Buffer+26, ee_tariffprice_lenth );
	LoopInverse( Eeprom.Buffer+10, 8, 4 );
	Event_Record( ee_flpricen_inpage );
	//--- 费率编程记录 end---
}


void JTRecord( unsigned char *InOpt )
{
	unsigned char i, Temp08;
	unsigned char Buf[ee_ladder_lenth+2];
	
	//--- 阶梯编程记录 ---
	memcpy( Eeprom.Buffer+6, InOpt, 4 );			//操作者代码(4)
	Temp08 = Event_Record( ee_jtpricen_inpage );

	for( i=0; i<8; i++ )
	{
		I2C_Read_Eeprom( ee_ladder1_page+i, 0, Buf, ee_ladder_lenth);
		LoopInverse( Buf, 13, 4 );
		I2C_Write_Eeprom( ee_jtpricerecord2_page+8*Temp08+i, 0x00, Buf, ee_jtpricerecord2_len );
	}
	//--- 阶梯编程记录 end ---
}


unsigned char Set_Comm_Rate( unsigned char Ch, unsigned char Rate )
{
	switch( Ch )
	{
		case 0x01:	//调制型红外光口波特率特征字	NN  1
			if( Rate == 0x04 )	//1200bps
			{
				return 0;
			}
			break;
						
//		case 0x02:	//接触式红外光口波特率特征字    NN  1
//			Temp08 = 0x00;
//			break;
			
		case 0x03:	//通信口1 波特率特征字          NN  1
			if( (Rate == 0x04) || (Rate == 0x08) || (Rate == 0x10) || (Rate == 0x20) )	//1200, 2400, 4800, 9600
			{
				Uart[2].Flag |= B0001_0000;	//波特率改变标志(规约处理置位)
				return(UpdateSysPara( 0x80+0, &Rate, 1 ));
			}
			break;
			
//		case 0x04:	//通信口2 波特率特征字          NN  1
//			Temp08 = 0x00;
//			break;											

		case 0x05:	//通信口3 波特率特征字          NN  1
			if( (Rate == 0x04) || (Rate == 0x08) || (Rate == 0x10) || (Rate == 0x20) )	//1200, 2400, 4800, 9600
			{
				Uart[0].Flag |= B0001_0000;	//波特率改变标志(规约处理置位)
				return(UpdateSysPara( 0x80+29, &Rate, 1 ));
			}
			break;									

		default:
			break;
	}
	
	return 1;
}


//通过显示的数据标识和序号确定对应的屏编码
unsigned char GetLcdNo( unsigned char *LcdSn )
{
	unsigned char n, Temp08, result, Sn;
	union B32_B08 Temp32;
	
	memcpy( Temp32.B08, LcdSn, 4 );
	Sn = LcdSn[4];

	//------ 组合 (当前，上1~上12结算日),序号从1开始 ------
	//------ 正向 (当前，上1~上12结算日),序号从66开始 ------
	//------ 反向 (当前，上1~上12结算日),序号从131开始 ------
	if( (Temp32.B08[3] == 0x00) 
	 && (Temp32.B08[2] <= 0x02) 
	 && (Temp32.B08[1] <= 0x04) 
	 && (Temp32.B08[0] <= 0x0C) )
	{
		Temp08 = 0;		//最大序号是0
		result = (Temp32.B08[2]*65+1)+Temp32.B08[1]+Temp32.B08[0]*5;
	}	
	//------ 其他显示项目，序号从200开始--------
	else
	{
		Exchange( Temp32.B08, LcdSn, 4 );
		for( n=0; n<disp_num; n++ )
		{
			if( Temp32.B32 == disp_table[n] ) break;
		}
		
		if( n >= disp_num ) return 0x00;	//显示项目错误
			
		if( (!BDMODE) && ((n>=disp_num-6)||(n==15)) && (!progenable) ) return 0x00;	//远程模式,不支持本地显示项目(厂内模式支持),西力远程不支持客户编号显示

		switch( n )	//确定最大序号
		{
			case 15:	//户号
			case 17:	//表号
			case 19:	//地址
			case 23:	//编程日期和时间
			case 25:	//清零日期和时间
				Temp08 = 1;		//最大序号是1
				break;
				
			case 0:	//全屏
				if( Sn != 0xFF ) 	return 0x00;	//显示项目错误

				Sn = 0x00;
				Temp08 = 0;		//最大序号是0
				break;
				
			default:
				Temp08 = 0;		//最大序号是0
				break;
		}
		
		result = 200+n+Sn;		//显示序号为HEX
	}

	if( Sn > Temp08 )		
		return 0x00;	//显示序号错
	else
		return (result);
}

unsigned char JudgeEndClock()	//截止时间合法性和有效性判断
{
	if( JudgeClockNoWeek( 0, Ic_Comm_RxBuf+2, 6 ) || (Cmp_Bcd_Inv( Ic_Comm_RxBuf+2, Sclock.Clockbackup, 6 )==2) )return 0x04; //错误
	else return 0x00; //成功
}

unsigned char Decrypt( unsigned char Ch, unsigned char File, unsigned char Len )
{
	unsigned char result, TmpLen;

	if( Ch < 3 )
	{
		if( Len != 0x02 )	
		{
			TmpLen = 16;
			memcpy( Uart[Ch].Buf+22+16, Uart[Ch].Buf+18,  4 );
		}
		else TmpLen = Uart[Ch].Buf[9]-12-4;
	
		result = Update_ESAM_Bin_Mac( File, 0x00, Uart[Ch].Buf+22, TmpLen );
	}
	else
	{
		result = WriteBinWithMac( ESAM, File, 0x00, 16+4 );
	}
	if( result ) return IC_MAC_ERR;	//

	result = ReadBinFile( ESAM, File, 0x00, Len );
	if( result ) return IC_ESAM_RDWR_ERR;
	
	if( Len == 0x02 )
	{
		result = ReadBinFile( ESAM, File, 0x01, Ic_Comm_RxBuf[1]+1 );
		if( result ) return IC_ESAM_RDWR_ERR;
	}
	else
	{
		//1A00 091104100301 9000
		Inverse( Ic_Comm_RxBuf+2, 6 );
		
		result = JudgeEndClock();	//判断截至时间
		if( result ) return IC_FORMAT_ERR; //错误
	}

	return 0x00;
}


void CommSetRtc_Adj()
{	
	unsigned char Buf[8];
	
	memcpy( Buf, Sclock.Clockbackup, 6 );	//备份校时前时钟
	
	Read_Rtc( Sclock.Clockmain );//读时钟
	Syn_Clock();	//同步时钟
	
	if( Cmp_Bcd_Inv( Buf, Sclock.Clockbackup, 6 ) == 1 )
	{
		ClearPloadRec( Sclock.Clockbackup+1 );
	}
	
	run_timezone_flag = 1;
	Tariff.Crc16.B16 += 1;
	RefreshPrice();
	run_readrtc_flag = 1;
	
	Wr_Powerdown_Data(0);//写掉电相关数据到数据ee
	Wr_Pretime_Dayfreeze();	//写上一次日冻结时间
}


unsigned char CommSetRtc( unsigned char Ch, unsigned char Start, unsigned char Len )
{
	unsigned char result;
	unsigned char Buf[8];
	
	result = Judge_Clock( Start, &Uart[Ch].Buf[22], Len ); //判断数据是否合法
	if( result != 0 ) return 1; //设置值错误
	
	if( clock_error )
	{
		Buf[0] = 0x00;	//ss
		Buf[1] = 0x00;	//mm
		Buf[2] = 0x00;	//hh
		Buf[3] = 0x05;	//WW
		Buf[4] = 0x01;	//DD
		Buf[5] = 0x01;	//MM
		Buf[6] = 0x10;	//YY
	}
	else
	{
		Read_Rtc( Buf );//读时钟
	}
	
	memcpy( Buf+Start, Uart[Ch].Buf+22, Len );
	
	Buf[3] = (CalcAbsDay(Buf+2)+6)%7;	//计算周,格式为秒+分+时+周+日+月+年,起始地址周,从日开始计算

	result = Set_Rtc( 0, Buf, 7 ); //
	if( result != 0 ) return 2; //设置RTC失败

	memcpy( Eeprom.Buffer, Uart[Ch].Buf+18, 4 );		//操作者代码(4)
	memcpy( Eeprom.Buffer+4, Sclock.Clockbackup, 6 );	//校时前时间(6)
	
	CommSetRtc_Adj();	//设置时钟调整相关数据
	
	memcpy( Eeprom.Buffer+10, Sclock.Clockbackup, 6 );	//校时后时间(6)
	Event_Record( ee_setclockn_inpage );
	
	if( Cmp_Data( Eeprom.Buffer+7, Sclock.Clockbackup+3, 3 ) != 3 )	//日期发生变化
	{
		Clr_Zero_Data();	//跨天清零相关数据
//		Find_Card = 0x00;	//结束寻卡模式
	}
	
	return 0;
}



unsigned char Judge_Min_Space( unsigned char *bufA )
{				
	unsigned char result,Temp08;
	unsigned char Buf[16];
	
	memcpy( Buf+0, bufA, 6 ); //需要判断的时间
	memcpy( Buf+6, Sclock.Clockbackup, 6 ); //当前时间
	
	Temp08 = Cmp_Bcd_Inv( Buf+0, Buf+6, 6 ); 
	switch( Temp08 )
	{
		case 0x01: //时间 > 当前时间
			Add_Min_Clock( Buf+6, 5 ); //对当前时间做加5分钟处理
			break;
			
		case 0x02: //时间 < 当前时间
			Add_Min_Clock( Buf+0, 5 ); //对时间做加5分钟处理
			break;
			
		default:
			break;
	}
	
	if( Temp08 )
	{
		result = Cmp_Bcd_Inv( Buf+0, Buf+6, 6 );
		if( result == Temp08 ) //超过5min限制
		{
			return 1;	//超过5min限制
		}
	}
	
	return 0;
}


unsigned char  Cal_ChkSum(unsigned char *Buf,unsigned char Len)
{
	unsigned char i,csdata=0;
	
	for(i=0;i<Len;i++)
	{
		csdata+= Buf[i];
	}
	return(csdata);
}

//模式切换时，刷新显示列表
//void ModeChangeDisp(unsigned char NewMode)
//{
//	unsigned char i;
//	unsigned char Buf[64];
//	
//	Clear_Wdt();
//	WriteE2WithBackup( ee_displaypara_page, ee_displaypara_inpage, Def_DisplayPara_table+8*NewMode, ee_displaypara_lenth );
//	memcpy( &Display_Para.S_Number, Def_DisplayPara_table+8*NewMode, ee_displaypara_lenth );
//
//	memset( Buf, 0x01, ee_displayitem_lenth );
//	I2C_Write_Eeprom( ee_loop_displayitem_page+1, 0x00, Buf, ee_displayitem_lenth );
//	I2C_Write_Eeprom( ee_key_displayitem_page+1, 0x00, Buf, ee_displayitem_lenth );
//
//	memcpy( Buf, Def_Autodisp_table+16*NewMode, sizeof(Def_Autodisp_table)/2 );
//	I2C_Write_Eeprom( ee_loop_displayitem_page, 0x00, Buf, ee_displayitem_lenth );
//	memset( Buf, 0x01, ee_displayitem_lenth );
//	memcpy( Buf, Def_Keydisp_table+32*NewMode, sizeof(Def_Keydisp_table)/2 );
//	I2C_Write_Eeprom( ee_key_displayitem_page, 0x00, Buf, ee_displayitem_lenth );
//
//	for( i=0; i<4; i++ ) Item[i].Crc16.B16 += 1;
//	dispitem_veri_flag = 1;
//}

unsigned char ModeChange( unsigned char NewMode )
{
	unsigned char Result,Buf[ee_curyearjs_len];
	
	run_timezone_flag = 1;	//刷新时区和时段表, 根据模式置位刷新电价或刷新继电器.
	
	if( UpdateSysPara( 6, &NewMode, 1 ) ) return 1;

	remainder_tick_flag = 0;//清赊欠标志
	Meter_Status = 0x00;
	Meter_Status1 &= B1000_0001;
	Opendelaytimer.B32 = 0;
	ClrSelfClose();

	if( NewMode == 0 ) //设置为本地模式
	{
		RefreshPrice();
		
		Result = Year_Ladder_Judge();
		if( Result == 0x00 )
		{
			Freeze_Usedl(2);//月结算,月用电量转存和清零
			Freeze_Dl(ee_histroy_dl_page,ee_histroyf_inpage);
		}

		Clr_Reportdata(ee_clearn_inpage);	//清单项上报状态字和新增次数(有事件记录类)
	}
	else if( NewMode == 1 ) //设置为远程模式
	{
		//费控电能表切换为远程模式后：停止剩余金额扣减；切换为远程模式显示列表；
		//清除本地报警、拉闸等费控状态，置为合闸状态；接受远程拉合闸控制命令。
		Report_Data[1] &= B1111_1101;
		Report_Num[9] = 0x00;  //透支状态
		Clr_Reportdata(ee_flpricen_inpage);	//清单项上报状态字和新增次数(有事件记录类)
		Clr_Reportdata(ee_jtpricen_inpage);	//清单项上报状态字和新增次数(有事件记录类)
		
		//清当前电价参数
		memset(Price.Current_L.B08, 0x00, 16);
		
		//远程表应该始终处于月结算
		if( Price.Ladder_Mode == 0x55 )
		{
			//年结算到月结算
			Freeze_Usedl(0);//年用电量转存和清零
			Price.Ladder_Mode = 0x00;//月结算模式
			Process_Usedl(1);//月用电量从0开始
			Freeze_Dl(ee_histroy_dl_page,ee_histroyf_inpage);
			
			//当前第1~6年结算日无效
			memset( Buf, 0x99, ee_curyearjs_len);	
			WriteE2WithBackup( ee_curyearjs_page, ee_curyearjs_inpage, Buf, ee_curyearjs_len );
		}
		else
		{
			Freeze_Usedl(2);//月结算,月用电量转存和清零
			Freeze_Dl(ee_histroy_dl_page,ee_histroyf_inpage);
		}
	}
	else return 1;
	
//	ModeChangeDisp(NewMode);
	
	RdMeterMode();
	
	//费控模式切换如果产生拉合闸则不上报
//	Result = relay_status_flag;
	Alarmstatus_Judge();		//电表报警状态判断
	Enable_Closerelay();		//直接闭合继电器
	
	if( Relay_Mode == 0x55 )//外置继电器
	{
		if( (!BDMODE) || ((remainder_maxtick_flag == 0) || relay_commu_close_flag) )//插完预置卡处于合闸状态才置继电器成功标志
		{
			relay_cleardl_flag = 1;	//置外置表清零标志
		}
	}
	
	Mt_Alarmstatus_Judge();
//	if( ((Result == 1) && (relay_status_flag == 0))||((Result == 0) && (relay_status_flag == 1)) ) Relay_modechange_flag = 1;//继电器动作则无上报
//	else Relay_modechange_flag = 0;
	Relay_modechange_flag = 1;
	Store_Meterstatus();
	
	return 0;
}

unsigned char Yc_Auth(unsigned char Ch, unsigned char *OutBuf)
{
	if( Clear_Rand_Gene() ) return 1;	//Esam Reset

	LoopInverse( Uart[Ch].Buf+18, 3, 8 ); //密文1+随机数1+分散因子

	if( Uart[Ch].Buf[34] || Uart[Ch].Buf[35] ) return 1;
	if( Cmp_Meter_Id(Uart[Ch].Buf+36) ) return 1;

	if( Authorization( Uart[Ch].Buf+18, 4 ) ) return 8; //身份认证失败

	memcpy( PcRand,  Uart[Ch].Buf+26, 8 );	//随机数, 8bytes 
	memcpy( PcRst8B, Uart[Ch].Buf+34, 8 );	//分散因子, 8bytes

	first_prog_flag = 0;	//清首次编程标志

	//返回信息: 随机数2+ESAM序列号
	Exchange( OutBuf+1, EsamRand, 4 );	//随机数2, 4bytes
	Exchange( OutBuf+5, EsamRst8B, 8 );	//ESAM序列号, 8bytes
	
	CardType = 0;
	UpdataFlag = 0;
	UserCardFlag = 0xff;
	return 0;
}

//验证通过后按照明文定义的指令执行相应操作
/*Ch:
	0x14：用户卡远程模式下的合闸命令，无操作者代码，需解密
	0x24~0x26：远程终端的合闸命令，有操作者代码，需解密
	0x20~0x22：96级远程控制继电器命令，有操作者代码，需解密
	0x00~0x02：03级远程控制继电器命令，有操作者代码，无解密
*/
unsigned char Relay_Ctrl(unsigned char Ch)
{
	unsigned char Temp08;
	
	if(Ch >= 0x10) //除03级密码，其它都需要解密
	{
		Temp08 = Decrypt( Ch&0x0f, 0x87, 0x08 );
		if( Temp08 ) return(Temp08);
	}
	
	//验证通过后按照明文定义的指令执行相应操作
	Temp08 = 0x00;
	switch( Ic_Comm_RxBuf[0] )	//N1
	{
		case 0x1A:	//代表跳闸
			if(relay_commu_close_flag == 1) {Temp08 = 0x04;break;}
			relay_commu_open_flag = 1;
			
			if( relay_status_flag || progenable )
			{
				Opendelaytimer.B32 = 0;	//已经跳闸了就不要再等延时
			}
			else Opendelaytimer.B32 = Calculate_Timer(0);
			
			if( over_xxx_flag && (CurrentTimer.B32 || (relay_status_flag==0)) ) CurrentTimer.B32 = Calculate_Timer(1); //处于大电流延时拉闸则重新启动
			
			relay_commu_disable_close_flag = 0;
			relay_commu_close2_flag = 0;	//清保电解除标志
					
			break;

		case 0x1B:	//代表合闸允许
			if(relay_commu_close_flag == 1) break;
			if(relay_commu_open_flag == 0)	break;

			Clr_Commu_Relaydata();
			if(Relay_Mode != 0x55)
			{
				if( relay_status_flag == 1 )
				{
					if( (!BDMODE) || ((remainder_maxtick_flag == 0) && (charge_relay_open_flag == 0)) )
					{
						relay_commu_disable_close_flag = 1;
					}
				}
			}
			break;
			
		case 0x1C:	//直接合闸，
			Clr_Commu_Relaydata();
			Enable_Closerelay();
			break;

		case 0x2A:	//代表报警，
			relay_commu_alarm_flag = 1;
			Temp08 = 0xff;
			break;

		case 0x2B:	//代表报警解除，
			relay_commu_alarm_flag = 0;
			Temp08 = 0xff;
			break;

		case 0x3A:	//代表保电，
			relay_commu_close_flag = 1;
			Clr_Commu_Relaydata();
			if(Relay_Mode != 0x55)//内置继电器
			{
				if( relay_status_flag == 1 )
				{
					relay_commu_disable_close_flag = 1;//插卡才能闭合继电器
				}
			}
			break;

		case 0x3B:	//代表保电解除。
			if( self_close_flag && (!BDMODE) ) {Temp08 = 0x04;break;}
			if( (Relay_Mode != 0x55) && relay_commu_close_flag && (BDMODE&&( charge_relay_open_flag || remainder_maxtick_flag)) )//内置表
			{
				relay_commu_disable_close_flag = 0;//保电解除,解除保电后如果电表透支则拉闸
			}
			
			if( relay_commu_close_flag ) relay_commu_close2_flag = 1;//置保电解除标志
			relay_commu_close_flag = 0;
			Temp08 = 0xff;
			break;

		default:
			Temp08 = 0x04;
			break;
	}

	if( Temp08 == 0x04 ) return(IC_FORMAT_ERR);
	else
	{
		if( (Temp08 != 0xff) && (Ch != 0x14) ) I2C_Write_Eeprom(ee_operatorcode_page, ee_operatorcode_inpage, &Uart[Ch&0x03].Buf[14], 4);//操作者代码写数据ee
		
		if( (Temp08 != 0xff) && ((Ch&0x0f) == 0x00) ) Zb_Relayctrl_Timer = Relaytimer_Delay_ZB;//载波拉合闸延时
		
		if( (!BDMODE) && (SELFCLOSE == 0x55) )	//远程拉闸，直接合闸，保电时，取消自保状态
		{
			if( Temp08 == 0x00 ) ClrSelfClose();
		}
		
		Store_Meterstatus();	//保存电表状态字节
		
		return 0;
	}
}

unsigned char Yc_Money_In( unsigned char Type )
{
	unsigned char result;
					
	result = OpenAddMoney( Type|0x80 );
	if( result ) return (result);
	
	Alarmstatus_Judge();			//电表报警状态判断
	if(Relay_Mode != 0x55)//内置继电器
	{
		if( (relay_status_flag == 0) || relay_commu_open_flag )
		{
			Enable_Closerelay();		//插卡闭合继电器
		}
		else if( Type&B0001_0000 )//交互终端直接合闸,远程充值手动合闸
		{
			Enable_Closerelay();		//插卡闭合继电器
		}
		else if( (remainder_maxtick_flag == 0)&&(charge_relay_open_flag == 0) )
		{
			relay_commu_disable_close_flag = 1;//远程充值后仍然拉闸
			Store_Meterstatus();	//保存电表状态字节
		}
	}
	else	Enable_Closerelay();		//直接闭合继电器
	
	return 0;
}


unsigned char Yc_Preset_Pro(void)
{
	unsigned char result;

	LoopInverse( Buy_Money_Mac[0].B08, 2, 4 ); //读取钱包购电次数+MAC
	LoopInverse( Buy_Times_Mac[0].B08, 2, 4 ); //读取钱包购电次数+MAC
	
	if( Buy_Times_Mac[0].B32 ) return 0x20; //购电次数须恒等于 0，不为零出错处理
	
	result = Init_Money();
	if( result != 0x00 ) 
	{
		Mac_Err_Proc();	//mac错误次数加1
		return 0x04;
	}
	else
	{
		Alarmstatus_Judge();		//电表报警状态判断
		Enable_Closerelay();		//插卡闭合继电器
		return 0;
	}
}


//读取钱包文件：
//Type = 1：读记录文件
//Type = 2：带MAC读取钱包文件
unsigned char Get_Money_File(unsigned char Type, unsigned char *OutBuf)
{
	unsigned char i, K;
	
	Moneybag_Process2();//通讯更新钱包
		
	for(i=0; i<2; i++)
	{
		K = i*2+1;
		if(Type == 1) { if( ReadRecFile(K) ) return 1; }
		else { if( ReadBinWithMac( ESAM+0x10, K, 0x14, 4+4) ) return 1; }
		LoopExchange( OutBuf+i*(Type*4), Ic_Comm_RxBuf, Type, 4 );
	}

	return 0;
}

//返回信息: 返写文件状态(1)+返写信息文件(50)+MAC3(4)
unsigned char Terminal_Moneyin_Judge(unsigned char *OutBuf)
{
	unsigned char Temp08=0;
	
	if( (UpdataFlag & B0011_0000) != 0 )
	{
		OutBuf[1] = 0;	//无返写信息
		return 2;
	}
	
	if( CardType == UserCardCommand )//用户卡
	{
//		result = Yc_Money_In( UserCardFlag+0x10 );	//处理流程同卡片
//		if( result == 0 )
		{
			//更新Eeprom数据
			if( UserCardFlag != 0x01 ) UpdataFlag &= B0111_1010;	//购电卡和补卡，只允许更新备用套; 
			UpdataRefer_ToEe_WithMac();	//更新Eeprom数据

			UpdataRunFile();
				
			ReadBinWithMac( ESAM, 0x86, 0, 50+4 );
		
			OutBuf[1] = 1;	//有返写信息
			MacExchange( OutBuf+2, Ic_Comm_RxBuf, 50 );
			Temp08 = 56;
		}
	}
	else
	{
		Yc_Preset_Pro();

		UpdataRefer_ToEe_WithMac();	//更新Eeprom数据
		
		OutBuf[1] = 0;	//无返写信息
		Temp08 = 1;
	}
	
	terminal_auth_flag = 0;	//数据操作结束，身份认证失效
	Identitytimer.B32 = 0;
	
	return (Temp08);
}

//远程身份认证/交互终端身份认证有效判断
unsigned char Comm_Auth_Judge( void )
{
	if(identity_prog_flag || terminal_auth_flag) return 1;
	else return 0;
}

void MacExchange( unsigned char *Drc, unsigned char *Src, unsigned char Len )
{
	Exchange( Drc, Src, Len );
	Exchange( Drc+Len, Src+Len, 4 );
}

void Dmoney_Record(unsigned char *Buf)
{
	Hex2Bcd( Buf, Buy_Money_Bcd, 4 );//退费金额
	memcpy( Remain_back+16, Money.Remain, 4 );	//备份退费前 剩余金额和购电次数
	Remainmoney_Process(Buf, 0x55);//直接退费
	Buy_Record( ee_dmoneyn_inpage );
}

void Uart_Proc( unsigned char Ch )
{
	unsigned char i, k, n=0, Temp08=0, result, Err_DI=0, Auth=0, Remain = 0x00;
	uint16  MeterClear = 0;
	unsigned char OutBuf[256];
	unsigned char Buf[64],ChangeBuf[32];
	unsigned char ecode *Code_Flash_Char;
	unsigned char *Ram_Ptr;
	unsigned char *Ptr;
	union B16_B08 Temp16,TempCap16;
	union B32_B08 Temp32,DI07;
	union B32_B08 OptID;
	unsigned long Temp;
	
	if( FLAG_EA == 0 ) return;//掉电不处理,程序直接返回
	
	Uart[Ch].Flag &= ~(B0000_0100);
	
	Adj_Clk_Timer = Adjclk_timer_delay;	//重新启动定时8M时钟调校功能
	
	Ram_Ptr = Uart[Ch].Buf;
	if( Uart[Ch].Buf[8]&B1000_0000 )	//通讯bit7=1的命令码电表均不响应
	{
		Uart[Ch].Overtime_Timer = 2;
		return;
	}

	result=Cal_ChkSum(Uart[Ch].Buf,Uart[Ch].Buf[9]+10);
		
	for( i=0; i<Uart[Ch].Buf[9]; i++ )
	{
		Uart[Ch].Buf[ 10+i ] -= 0x33;
	} 	
	
	if( Uart[Ch].Buf[10+i] != result )
	{
		Uart[Ch].Overtime_Timer = 2;
		return;
	}
	
	memset( Buf, 0xaa, 6 );
	if( (Cmp_Data( Buf, Uart[Ch].Buf+1, 6 ) ==3) &&			//全aa命令不支持写
	    ((Uart[Ch].Buf[8]==0x14)||(Uart[Ch].Buf[8]==0x16)||(Uart[Ch].Buf[8]==0x1a)||(Uart[Ch].Buf[8]==0x1b)||(Uart[Ch].Buf[8]==0x1c)||(Uart[Ch].Buf[8]==0x1d)) )
	{
		Uart[Ch].Overtime_Timer = 2;
		return;
	}
	
	memcpy( DI07.B08, Uart[Ch].Buf+10 ,4 );
	
	memcpy( OptID.B08, Uart[Ch].Buf+14 ,4 );
	
	if( (Ch == 0x01) && (progenable==0) )	//红外通信口
	{
		//红外认证不通过或红外认证失效后，
		//电能表应仅支持读表号、通信地址、备案号、当前日期、当前时间、
		//当前电能、当前剩余金额、
		//红外认证查询命令，其它信息不允许读出，所有信息均不允许设置
		if( IR_EnTimer == 0 )	//红外认证失效
		{
			Temp08 = 0xFF;	//默认不允许抄读
			
			if( (Uart[Ch].Buf[8] == 0x03) && 
				 ((DI07.B32 == 0xFF038007) || (DI07.B32 == 0xFF030007) || (DI07.B32 == 0xFF028007) || (DI07.B32 == 0xFF028107)) )	//红外认证请求、红外认证、数据对比、状态查询除外
			{	
				Temp08 = 0x00;
			}
			else if( (Uart[Ch].Buf[8] == 0x09) || (Uart[Ch].Buf[8] >= 0x11 && Uart[Ch].Buf[8] <= 0x13) || (Uart[Ch].Buf[8] == 0x1f) )
			{
				Temp08 = 0x00;
			}
			else if( Uart[Ch].Buf[8] == 0x1c )
			{
				Temp08 = 0x00;
			}
			
			if( Temp08 )	//不允许抄读
			{
				if( Uart[Ch].Buf[8] == 0x03 ) DI07.B08[3] = 0xff;//返回2字节错误
				else Uart[Ch].Buf[8] |= 0x40; //返回1字节04错误
			}
		}
	}
	
	Successled.Timer = 5+1;	//点灯5s

	k = 0x00;
	OutBuf[0] = 0x00;
	switch( Uart[Ch].Buf[8] ) //cmd
	{
		case 0x03:	//安全认证
			if( (Uart[Ch].Flag&B0000_0001) ||
			((Uart[Ch].Flag&B0000_0010)&&(DI07.B32!=0xFF018007)&&(DI07.B32!=0xFF028007)&&(DI07.B32!=0xFF028107)&&(DI07.B32!=0xFF038007)) )
			{
				OutBuf[ k++ ] = 0xFF;	//Operation result.
				OutBuf[ k++ ] = 0x01;
				OutBuf[ k++ ] = 0x00;
				break;
			}
			
			//注3：密码权限04代表通过明文的方式进行数据传输，需要进行密码验证，同时要有编程键配合。
			//注4：密码权限99代表通过明文+MAC的方式进行数据传输，不需要进行密码验证，也不需要编程键配合使用。
			//注5：密码权限98代表通过密文+MAC的方式进行数据传输，不需要进行密码验证，也不需要编程键配合使用。

			//--------------------------- 安全认证错误信息字 SERR ---------------------------------------------
			//Bit7 | Bit6       | Bit5         | Bit4           | Bit3         | Bit2          | Bit1     | Bit0     |
			//保留 | 购电超囤积 | 充值次数错误 | 客户编号不匹配 | 身份认证失败 | ESAM 验证失败 | 重复充值 | 其它错误 |
			//Bit15 Bit14 Bit13 Bit12 Bit11 Bit10 Bit9 Bit8
			//保留 保留 保留 保留 保留保留保留保留	
			//注 1：0 代表无相应错误发生，1 代表相应错误发生。
			//注2：当表内已充值次数比充值指令中充值次数小1 时，为正常充值。充值完成后表内已充值次数加1；当
			//表内已充值次数和充值指令中充值次数相等时，该充值已完成，充值错误为重复充值。除以上两种情
			//况外，为充值次数错误。
			//注3：身份认证失败和身份认证超时统称为身份认证失败，需要重新进行身份认证。

			//68H A0 … A5 68H 03H L DIO … DI3 C0 … C3 N1 … Nm CS 16H
			switch( DI07.B08[3] )
			{
				case 0x07:	//
					switch( DI07.B08[2] )
					{
						case 0x00:	//身份认证指令
							switch( DI07.B08[1] )
							{
								case 0x00:	//身份认证
									switch( DI07.B08[0] )
									{
//										case 0x01:	//密文1, 8bytes	[18]~[25]
//										case 0x02:  //随机数1, 8bytes	[26]~[33]
//										case 0x03:  //分散因子, 8bytes	[34]~[41]
										case 0xFF:  //身份认证指令: 密文1+随机数1+分散因子
											//68+表号+68+03+20+FF000007+操作者代码+密文1(8)+随机数1(8)+分散因子(8)+CS+16
											if( terminal_auth_flag )
											{
												result = 0x01;	//交互终端身份认证优先
											}
											else result = Yc_Auth(Ch, OutBuf);
											if( result )
											{
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = result;	//身份认证失败
												OutBuf[ k++ ] = 0x00;
												break;
											}
											
											Identitytimer.B32 = Calculate_Timer(0xff);//计算身份认证时效
											identity_prog_flag = 1;	//置位 安全认证编程标志
																			
											k = 13;
											break;

										default:
											break;
									}
									break;
									
								case 0x01:	//身份认证时效设置
									switch( DI07.B08[0] )
									{
//										case 0x01:	//身份认证有效时长(分钟), 2bytes, BCD
//										case 0x02:	//MAC, 4bytes
										case 0xFF:
											//68+表号+68+03+0E+FF010007+操作者代码+2字节认证时效时间+MAC+CS+16
											//1.在没有设置安全认证时效参数前，默认为5分钟。
										
											//密钥信息文件1 0005 自由 身份认证+MAC
											//参数修改命令：04d68200+LC+分散因子+参数+MAC；LC 为分散因子＋参数＋MAC 的总长度，它是1字节的十六进制数。
											if( hangup_comm_flag )
											{
												k = 0x00;  
												OutBuf[ k++ ] = 0xFF;	//Operation result.
												OutBuf[ k++ ] = 0x00;
												OutBuf[ k++ ] = 0x01;	//远程安全认证锁住状态位
												break;
											}
											
											if( !identity_prog_flag )
											{		
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = 0x08;	//身份认证失败
												OutBuf[ k++ ] = 0x00;
												break;
											}
											
											if( (Uart[Ch].Buf[18]==0x00) && (Uart[Ch].Buf[19]== 0x00) )	//安全认证时效不能写入0，若写入0则返回失败。
											{
												break;
											}
											
											if( IsMultiBcd( Uart[Ch].Buf+18, 2 ) ) break;

											result = Update_ESAM_Bin_Mac( 0x81, 0x2B, &Uart[Ch].Buf[18], 2 );
											if( result != 0x00 ) 
											{
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = 0x04;	//ESAM验证失败
												OutBuf[ k++ ] = 0x00;
												break;
											}
											
											result = UpdateSysPara( 0x40+27, Uart[Ch].Buf+18, 2 ); //更新XRAM, E2PROM
											if( result ) break;
											
											Identitytimer.B32 = Calculate_Timer(0xff);//计算身份认证时效

											ProgramRecord( DI07.B08, OptID.B08 );
											
											//正确：68+表号+68+83+04+FF010007+CS+16
											k = 1;
											break;
											
										default:
											break;
									}
									break;
									
								case 0x02:	//身份认证失效
									switch( DI07.B08[0] )
									{
										case 0xFF:
											if( !identity_prog_flag )
											{		
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = 0x08;	//身份认证失败
												OutBuf[ k++ ] = 0x00;
												break;
											}
											
											//客户编号
											result = ReadBinFile( ESAM, 0x81, 36, 6 );
											if( result != 0x00 ) break;

											Exchange( OutBuf+1, Ic_Comm_RxBuf, 6 );
											
											result = Get_Money_File(1, OutBuf+7);	//读取钱包文件
											if( result != 0x00 ) break;
						
											//密钥信息
											GetKey( OutBuf+15 );
										
											//身份认证失效
											Clear_Rand_Gene();
											
											k = 19;
											break;
											
										default:
											break;
									}
									break;
									
								case 0x03:	//红外认证命令, 红外认证成功后30分钟内有效, 该先于身份认证完成
									switch( DI07.B08[0] )
									{
										case 0xFF:
											if( Ch &&(progenable==0) ) break; //只允许红外通信口
											if( identity_ir_flag == 0 ) break; //没有红外请求
											
											//随机数2密文
											//红外认证成功后30分钟内有效。
											Inverse( Uart[Ch].Buf+18, 8 );	//密文2

											result = Auth_In_Out(0x82, ESAM, 0x10, Uart[Ch].Buf+18 );//卡片做外部认证
											if( result )
											{
												{IR_EnTimer = 0;identity_ir_flag = 0;}
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = 0x04;	//ESAM验证失败
												OutBuf[ k++ ] = 0x00;
												break;
											}				
											
											IR_EnTimer = Calculate_Timer(2);
											
											k = 1;
											break;
											
										default:
											break;
									}
									break;
									
								case 0x04:	//明文写信息文件
								case 0x05:	//密文写信息文件
									//68+表号+68+03+L+FF050007+操作者代码+文件编号+偏移地址+明文(密文)+MAC+CS+16
									if(DI07.B08[0] != 0xff) break;
									if( Uart[Ch].Buf[9] <= 15 ) break;//数据长度必须大于15
										
									if( hangup_comm_flag )
									{
										k = 0x00;  
										OutBuf[ k++ ] = 0xFF;	//Operation result.
										OutBuf[ k++ ] = 0x00;
										OutBuf[ k++ ] = 0x01;	//远程安全认证锁住状态位
										break;
									}
									
									if( !identity_prog_flag )
									{		
										k = 0x00;
										OutBuf[ k++ ] = 0xFF;
										OutBuf[ k++ ] = 0x08;	//身份认证失败
										OutBuf[ k++ ] = 0x00;
										break;
									}
																		
									result = SelectFile( ESAM, 0x00, Uart[Ch].Buf[18] );//选择文件(明文：18，1A,1B,1C; 密文:0B,15)
									if(result) break;
										
									result = Update_ESAM_Bin_Mac(Uart[Ch].Buf[20], Uart[Ch].Buf[19], Uart[Ch].Buf+21, Uart[Ch].Buf[9]-15 );
									if(result)
									{
										k = 0x00;
										OutBuf[ k++ ] = 0xFF;
										OutBuf[ k++ ] = 0x04;	//ESAM验证失败
										OutBuf[ k++ ] = 0x00;
										break;
									}
									
									ProgramRecord( DI07.B08, OptID.B08 );
									
									k = 1;
									break;	
									
								default:
									break;
							}
							break;
						
						case 0x01:	//开户/充值/钱包初始化
							if( !BDMODE ) break; //远程模式
							
							//购电金额, 4bytes.	[18~21]
							//购电次数, 4bytes.	[22~25]
							//MAC1, 4bytes.		[26~29]
							//客户编号, 6bytes.	[30~35]
							//MAC2, 4bytes.		[36~39]
							//设客户编号：04D682240A+户号+MAC
							//电能表充值：8432010C0C+购电金额+次数+MAC

							//68+表号+68+03+1E+FF010107+操作者代码+金额+次数+MAC+户号+MAC+CS+16	//开户
							//开户时，不需要验证客户编号，直接将客户编号写到ESAM 的对应文件中；再进行充值操作。

							//68+表号+68+03+1E+FF020107+操作者代码+金额+次数+MAC+户号+MAC+CS+16	//充值
							//充值时，先比对客户编号是否相同，相同再将客户编号写到ESAM 的相应区，进行MAC 校验，如果验证通过再进行充值操作。
							if( DI07.B08[0] != 0xFF )
							{
								break;
							}
							
//							if( hangup_comm_flag )
//							{
//								k = 0x00;  
//								OutBuf[ k++ ] = 0xFF;	//Operation result.
//								OutBuf[ k++ ] = 0x00;
//								OutBuf[ k++ ] = 0x01;	//远程安全认证锁住状态位
//								break;
//							}  
////							
//							if( !identity_prog_flag )
//							{		
//								k = 0x00;
//								OutBuf[ k++ ] = 0xFF;
//								OutBuf[ k++ ] = 0x08;	//身份认证失败
//								OutBuf[ k++ ] = 0x00;
//								break;
//							}
							if( DI07.B08[1] == 0x01) 
							{
								Exchange(&KeyH_Token[0], Uart[Ch].Buf+38, 20);//key token1
		            Exchange(&KeyL_Token[0], Uart[Ch].Buf+58, 20);//key token2
							  						
							}
							if( (DI07.B08[1] == 0x01) || (DI07.B08[1] == 0x02) )
							{
//								if( Key_Status != 1 ) break; //公钥不允许开户/充值
								Exchange(Charge_Token, Uart[Ch].Buf+18, 20);	
								if( DI07.B08[1] == 0x01) n =78;
							  else  n =38;
								Exchange( &Refers_File_Mac[BACKADDR[1]], Uart[Ch].Buf+n, 6 ); //备份客户编号+MAC
								result = Yc_Money_In( DI07.B08[1] );
								if( result )
								{
									k = 0x00;
									OutBuf[ k++ ] = 0xFF;
									OutBuf[ k++ ] = result;	//错误类型
									OutBuf[ k++ ] = 0x00;
									break;
								}
								
								k = 1;
							}
							else if( DI07.B08[1] == 0x03 )	//钱包初始化，等效于不带参数的预置卡
							{
								Exchange(Charge_Token, Uart[Ch].Buf+18, 20);
								result=api_Handle_key_sts_code(&Charge_Token[0]);
	              if( result)
								{
									k = 0x00;
									OutBuf[ k++ ] = 0xFF;
									OutBuf[ k++ ] = result;	//ESAM校验失败
									OutBuf[ k++ ] = 0x00;
								} 
								result = Yc_Preset_Pro();
								if( result )
								{
									k = 0x00;
									OutBuf[ k++ ] = 0xFF;
									OutBuf[ k++ ] = result;	//ESAM校验失败
									OutBuf[ k++ ] = 0x00;
								}
								else
								{
									//电表清零,保存清零记录
									MeterClear = 0x55aa;		//钱包初始化清零
									
									k = 1;
								}
							}
							break;
							
						case 0x03:	//密钥更新命令
							if( (DI07.B08[1] != 0x01) || (DI07.B08[0] != 0xFF) )
							{
								break;
							}

							if( !identity_prog_flag )
							{		
								k = 0x00;
								OutBuf[ k++ ] = 0xFF;
								OutBuf[ k++ ] = 0x08;	//身份认证失败
								OutBuf[ k++ ] = 0x00;
								break;
							}
							
							result = UpdateEepromKey( Ch );
							if( result ) //电表应错误应答，主站应启动密钥重装流程
							{
//								FailKey();	//更新密钥失败处理
								k = 0x00;
								OutBuf[ k++ ] = 0xFF;
								OutBuf[ k++ ] = result;
								OutBuf[ k++ ] = 0x00;
							}
							else
							{
								MeterClear = 0xaa55;
								
								k = 1;
							}
							break;
							
						case 0x80:	//数据回抄
							switch( DI07.B08[1] )
							{
								case 0x01:	//
									switch( DI07.B08[0] )
									{
										case 0xFF:	//数据回抄标识, 8字节,
											//返回的数据: 8+Le+4字节, [7:0]数据回抄标识; [7+Le:8]返回的数据; [8+Le+3:8+Le]MAC
						
											//数据回抄命令：采取文件名的方式：
											//81,读取剩余金额和购电次数：
											//回抄剩余金额：04b2010C11+4 字节随机数+04d681000A+8 字节分散因子；
											//回抄购电次数：04b2030C11+4 字节随机数+04d681040A+8 字节分散因子。
											//82—85，读取相应ESAM 二进制文件数据：
											//04b082（83，84，85）+ p2(偏移地址)＋11+4 字节随机数1+04d683+00+LC+8 字节分散因子。
											//LC 是所要读取的明文数据＋MAC+分散因子的总长度，它是1 字节的十六进制数。

											if( (!identity_prog_flag)&&(!progenable) )
											{		
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = 0x08;	//身份认证失败
												OutBuf[ k++ ] = 0x00;
												break;
											}
											
											//发送目录选择到ESAM
											if( (Uart[Ch].Buf[25] != 0xDF) || (Uart[Ch].Buf[24] != 0x01) ) break;

//											result = SelectFile( ESAM, 0xDF, 0x01 );
//											if( result != 0x00 ) break;	//

											if( Uart[Ch].Buf[22] != 0x02 )	//除钱包文件以外的其它文件
											{
												if( Uart[Ch].Buf[18] > 200 ) break;	//645规定读取字符不超过200个
												
												//发送：04b082(83,84,86)+p2+11+S4+04D686+P2+LC+8字节分散因子
												//返回：61 XX
												//发送：00C0000008
												//返回：Data+MAC
												
												if( Uart[Ch].Buf[22] == 0x06 ) UpdataRunFile();//更新运行文件
												
												result = ReadBinWithMac( ESAM+0x10, (Uart[Ch].Buf[22]|0x80), Uart[Ch].Buf[20], Uart[Ch].Buf[18]+4 );
												if( result != 0x00 ) break;
												
												MacExchange( OutBuf+9, Ic_Comm_RxBuf, Uart[Ch].Buf[18] );
												
												k = Uart[Ch].Buf[18]+13;
												memcpy( OutBuf+1, Uart[Ch].Buf+18, 8 );
												
											}
											
											else	//读取剩余金额和购电次数
											{
												//剩余金额(ESAM 内), 4bytes.
												//购电次数（ESAM 内）, 4bytes.
												result = Get_Money_File(2, OutBuf+9);
												if( result != 0x00 ) break;
	
												//返回状态信息:
												k = 25;
												memcpy( OutBuf+1, Uart[Ch].Buf+18, 8 );
											}
											break;
											
										default:
											break;
									}
									break;
									
								case 0x02:	//数据比对
									switch( DI07.B08[0] )
									{
										case 0xFF:
											if( !identity_prog_flag )
											{		
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = 0x08;	//身份认证失败
												OutBuf[ k++ ] = 0x00;
												break;
											}
											
											//判断cpu编号
											if( Uart[Ch].Buf[18] != 0x00 ) break;
											
											//判断地址是否异常和准备数据
											for( i=0; i<2; i++ )
											{
												if( i== 0 ) {Temp16.B16 = 16; Ptr = ChangeBuf;}	//比对因子
												else {Temp16.B16 = 256; Ptr = OutBuf;}	//比对数据
												
												Temp32.B32 = 0;
												Exchange( Temp32.B08, Uart[Ch].Buf+20+4*i, 4);	//比对因子起始地址
												Code_Flash_Char = (unsigned char ecode *)Temp32.B32;
												if( (Temp32.B32 < PROGRAMSTA1) || (Temp32.B32 > PROGRAMEND2) 
													|| ((Temp32.B32 > PROGRAMEND1)&&(Temp32.B32 < PROGRAMSTA2)) 
													|| Uart[Ch].Buf[23] || Uart[Ch].Buf[27] )
												{
													break;		//地址异常
												}
												else if( (Temp32.B32 < (PROGRAMEND1+1)) && (Temp32.B32+Temp16.B16 > (PROGRAMEND1+1)) )//断开地址补充
												{
//												   	memcpy( Ptr, Code_Flash_Char, PROGRAMEND1+1-Temp32.B32 );	//断开地址补充
//												  	memcpy( Ptr+(PROGRAMEND1+1-Temp32.B32), (unsigned char ecode *)0xff0000, Temp16.B16-(PROGRAMEND1+1-Temp32.B32) );
												  	memset( Ptr, 0x00, Temp16.B16);	
												  	memcpy( Ptr, Code_Flash_Char, PROGRAMEND1+1-Temp32.B32 );	//到保留区地址补0x80
												   	Ptr[PROGRAMEND1+1-Temp32.B32] = 0x80;
												}
												else if( (Temp32.B32+Temp16.B16 > (PROGRAMEND2+1)) )	//结束地址补0x80
												{
												  	memset( Ptr, 0x00, Temp16.B16);
												  	memcpy( Ptr, Code_Flash_Char, PROGRAMEND2+1-Temp32.B32 );
												   	Ptr[PROGRAMEND2+1-Temp32.B32] = 0x80;
												}
												else if( (Temp32.B32 < (PROGRAMPROZ)) && (Temp32.B32+Temp16.B16 > (PROGRAMPROZ)) )	//到保护区地址补0x80
												{
												   	memset( Ptr, 0x00, Temp16.B16);
												  	memcpy( Ptr, Code_Flash_Char, PROGRAMPROZ-Temp32.B32 );
												   	Ptr[PROGRAMPROZ-Temp32.B32] = 0x80;
												}
												else
												{
													memcpy( Ptr, Code_Flash_Char, Temp16.B16 );
												}
											}
											
											if( i != 2 )
											{
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = 0x80;	//地址异常
												OutBuf[ k++ ] = 0x00;
												break;
											}
                									
											//读ROM数据,计算密文
											Xor_Data( ChangeBuf, ChangeBuf+8, 8 );//得到比对因子
											result = Auth_In_Out(0xfa, ESAM, Uart[Ch].Buf[19], ChangeBuf );//生产比对密钥
											if( result ) break;
											
											for( i=1; i<4; i++ )
											{
												Xor_Data( OutBuf, OutBuf+64*i, 64 );//得到data1
											}
											result = Cal_Mac_Pro(0x00, OutBuf, 64); //ESAM加密data1(可能需要输入长度)
											if( result ) break;
											
											Exchange(OutBuf+1, Ic_Comm_RxBuf, 64);	//数据取反,返回加密数据
											
											k = 65;
											OutBuf[0] = 0x00;
											break;
											
										default:
											break;
									}
									break;

								case 0x03:	//红外认证请求
									switch( DI07.B08[0] )
									{
										case 0xFF:	//随机数1, 8字节,
											if( Ch &&(progenable==0) ) break; //只允许红外通信口
										
											//返回信息: 表号(8)+ESAM序列号(8)+随机数1密文(8)+随机数2(8)
											result = Clear_Rand_Gene(); //Esam Reset
											if( result != 0x00 ) break;	

											Buf[16] = 0x00;
											Buf[17] = 0x00;
											memcpy( Buf+18, Esam_Para.Meter_Id, 6 );
											Exchange( Buf+8, Uart[Ch].Buf+18, 8 );	//随机数1
											result = Authorization( Buf, 8 );
											if( result ) break;
											
											//返回信息: 表号 ESAM序列号 随机数1密文 随机数2
											Exchange( OutBuf+1, Esam_Para.Meter_Id, 6 );//表号, 6bytes
											Exchange( OutBuf+7, EsamRst8B, 8 );	//ESAM序列号, 8bytes
											Exchange( OutBuf+15, Buf, 8 );	//随机数1密文, 8bytes
											Exchange( OutBuf+23, EsamRand, 8 );	//随机数2, 8bytes
											
											identity_ir_flag = 1;	//置红外请求标志

											k = 31;
											break;
											
										default:
											break;
									}
									break;									
									
								default:
									break;
							}
							break;
							
						case 0x81:	//状态查询
							switch( DI07.B08[1] )
							{
								case 0x02:
									switch( DI07.B08[0] )
									{
										case 0xFF:
											if( !identity_prog_flag )
											{		
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = 0x08;	//身份认证失败
												OutBuf[ k++ ] = 0x00;
												break;
											}
											
											//返回信息: 剩余金额（ESAM 内）+ MAC + 购电次数（ESAM 内）+ MAC + 客户编号 + 密钥信息
											result = Get_Money_File(2, OutBuf+1);
											if( result != 0x00 ) break;
						
											//客户编号,	6bytes
											result = ReadBinFile( ESAM, 0x81, 36, 6 );
											if( result != 0x00 ) break;

											Exchange( OutBuf+17, Ic_Comm_RxBuf, 6 );
						
											//密钥信息, 4bytes.							
											GetKey( OutBuf+23 );
											
											//返回状态信息:
											k = 27;
											break;
										
										default:
											break;
									}
									break;

								default:
									break;
							}
							break;
							
						case 0xA0:	//交互终端
							if( !terminal_auth_flag && (DI07.B08[1] != 0x01) && (DI07.B08[1] != 0x06) )
							{
								k = 3;
								OutBuf[0] = 0xFF;
								OutBuf[1] = 0x08;	//身份认证失败
								OutBuf[2] = 0x00;
								break;
							}

//							DIVSEL	= 0x00;		//系统频率提高到8M
//							
							switch( DI07.B08[1] )
							{
								case 0x01: //交互终端和表身份认证
									result = Yc_Auth(Ch, OutBuf);//返回信息: 随机数2+ESAM序列号+费控模式状态字
									if(result)
									{
										k = 0x00;
										OutBuf[ k++ ] = 0xFF;
										OutBuf[ k++ ] = result;	//身份认证失败
										OutBuf[ k++ ] = 0x00;
										break;
									}
									
									Identitytimer.B32 = Terminal_delay;//交互终端身份认证时效1分钟
									terminal_auth_flag = 1;	//交互终端身份认证标志
									identity_prog_flag = 0;
									
									if((Decimal.Status & B0000_0010) != 0) UpdateMoneytimer = 0;	//金额变化更新钱包,返回实时数据
									
									result = Get_Money_File(1, OutBuf+14);	//读取钱包文件
									if( result ) break;

									OutBuf[13] = Display_Para.MeterCtrl;	//费控模式状态字, 1byte
									
									k = 22;
									break;
									
								case 0x02: //交互终端下发参数文件
//								case 0x01:	//卡片序列号, 8bytes	[18]~[25]
//								case 0x02:  //随机数1, 4bytes	[26]~[29], 用来算返写信息文件的MAC
//								case 0x03:	//电卡类型，1byte[30]
//								case 0x03:  //购电金额, 4bytes	[31]~[34]
//								case 0x03：	//购电次数	4bytes	[35]~[38]
//								case 0x04:	//MAC1,MAC2, 8bytes [39]~[46]（用户卡的MAC2是4字节00）
//预置卡
//								case 0x05:	//参数信息文件, 32bytes,[47]~[78] 包含全部参数信息文件从起始码到结束码的全部内容
//								case 0x06:	//MAC3, 4bytes, [79]~[82]根据参数信息文件从两套分时费率切换时间到电压互感器变比数据进行计算
//								case 0X07:	//MAC4, 4bytes, [83]~[86]4字节00
//用户卡
//								case 0x05:	//参数信息文件, 48bytes,[47]~[91] 包含全部参数信息文件从起始码到结束码的全部内容
//								case 0x06:	//MAC3, 4bytes, [92]~[95]根据参数信息文件从两套分时费率切换时间到电压互感器变比数据进行计算
//								case 0X07:	//MAC4, 4bytes, [96]~[99]根据参数信息文件中的客户编号数据进行计算
									//68+表号+68+03+20+FF02A007+操作者代码+卡片序列号+随机数1(4)+....+MAC3(4)+CS+16
									//返回参数：返写文件状态(1)+返写文件(50)+MAC(4)
									//过报警金额2 跳闸后，电能表收到交互终端发来“交互终端下发参数文件（数据标识：07A002FF）”命令时，电能表合闸。
									Alarmstatus_Judge();			//电表报警状态判断
									Enable_Closerelay();			//插卡闭合继电器
									
									Exchange( PcRand,  Ram_Ptr+26, 4 );	//随机数, 4bytes 
									Exchange( PcRst8B, Ram_Ptr+18, 8 );	//卡片序列号, 8bytes
									
									CardType = Ram_Ptr[30];
									if(CardType == UserCardCommand)//用户卡
									{
										if( Key_Status != 1 ) break;	//只允许私钥
										
										Exchange(Buf, Ram_Ptr+56, 6);
										if( Cmp_Meter_Id(Buf) ) break;//表号不相等
										
										UserCardFlag = Ram_Ptr[49];	//暂存用户卡类型
										
										if( BDMODE ) //本地表
										{
											if( UserCardFlag && (UserCardFlag <4) )//用户卡类型为1，2，3
											{
												UpdataFlag = Ram_Ptr[86];	//参数更新标志字

												Exchange( CardRst8B, Ram_Ptr+18, 8 );	//卡片序列号
			
												memcpy( Refers_File_Mac+BACKADDR[13], Ram_Ptr+50, 6 ); //备份客户编号
												memcpy( Refers_File_Mac+BACKADDR[13]+6, Ram_Ptr+96, 4 ); //MAC4
												memcpy( Buy_Money_Mac, Ram_Ptr+31, 8+4 ); //备份购电金额，次数，MAC1
												
												result = Yc_Money_In(UserCardFlag+0xd0);	//判断购电次数和囤积金额												
												if(result == 0)
												{
													if( (BuyTimesFlag == 2) || (BuyTimesFlag == 0xff) )
													{
														UpdataRunFile();
					
														ReadBinWithMac( ESAM, 0x86, 0, 50+4 );
													
														if(BuyTimesFlag == 2)	//购电次数差>1，返写SERR+0xAA+返写信息
														{
															OutBuf[0] = 0xFF;
															OutBuf[1] = 0x20;	//充值次数错
															OutBuf[2] = 0x00;
															OutBuf[3] = 0xAA;
															MacExchange( OutBuf+4, Ic_Comm_RxBuf, 50 );
															k = 58;
														}
														else
														{
															OutBuf[1] = 1;	//有返写信息
															MacExchange( OutBuf+2, Ic_Comm_RxBuf, 50 );
															k = 56;
														}
														terminal_auth_flag = 0;	//数据操作结束，身份认证失效
														Identitytimer.B32 = 0;
														
														break;
													}
												}
												else
												{
													terminal_auth_flag = 0;	//数据操作结束，身份认证失效
													Identitytimer.B32 = 0;
													
													k = 3;
													OutBuf[0] = 0xFF;
													OutBuf[1] = result;	//错误类型
													OutBuf[2] = 0x00;
													break;
												}
												
												if(UpdataFlag & B0000_0010)	//用户卡费率切换时间修改MAC验证
												{
													memcpy( Ram_Ptr+77+5, Ram_Ptr+43, 4);//MAC2
													result = Update_ESAM_Bin_Mac( 0x81, 0x0a, Ram_Ptr+77, 5 );//带MAC更新参数1
													if(result)
													{
														k = 0x00;
														OutBuf[ k++ ] = 0xFF;
														OutBuf[ k++ ] = 0x04;	//ESAM验证失败
														OutBuf[ k++ ] = 0x00;
														break;
													}
													//备份数据
													Exchange( Refers_File_Mac+BACKADDR[4], Ram_Ptr+77, 5 );	//2套分时费率切换时间
												}

												memcpy(Ram_Ptr+82-6, Ram_Ptr+92, 4);//MAC3
												Temp08 = 62;	//参数偏移地址
											}
											else break;	//用户卡类型错
										}
										else	//远程表
										{
											if( UserCardFlag == 0 )//只接收合闸复电命令
											{
												k = 2;
												OutBuf[1] = 0;
												break;
											}
											else break;
										}
									}
									else if(CardType == PresetCardCommand)//预置卡
									{
										if( (Key_Status != 0) || (!BDMODE) ) break;	//只支持公钥、本地模式(西力要求)
										
										memcpy( Buy_Money_Mac[0].B08, Ram_Ptr+31, 4 ); 	//备份购电金额
										memcpy( Buy_Money_Mac[1].B08, Ram_Ptr+39, 4 ); 	//备份MAC1
										memcpy( Buy_Times_Mac[0].B08, Ram_Ptr+35, 4 ); 	//备份购电次数
										memcpy( Buy_Times_Mac[1].B08, Ram_Ptr+43, 4 ); 	//备份MAC2
									
										Exchange( Refers_File_Mac+BACKADDR[4], Ram_Ptr+64, 5 );	//2套分时费率切换时间
										
										UpdataFlag = Ram_Ptr[73];
										memcpy(Ram_Ptr+69-6, Ram_Ptr+79, 4);//MAC3
										Temp08 = 49;	//参数偏移地址
									}
									else break;	//卡片类型错
															
									if( (UpdataFlag & B1000_0000) && ((CardType == PresetCardCommand) || (UserCardFlag == 0x01)) )	//参数修改MAC验证
									{
										result = Update_ESAM_Bin_Mac( 0x81, 0x10, Ram_Ptr+Temp08, 14 );//带MAC更新参数1
										if(result)
										{
											k = 0x00;
											OutBuf[ k++ ] = 0xFF;
											OutBuf[ k++ ] = 0x04;	//ESAM验证失败
											OutBuf[ k++ ] = 0x00;
											break;
										}
										
										//备份数据
										Refers_File_Mac[BACKADDR[1]] = 0;
										Exchange( Refers_File_Mac+BACKADDR[1]+1, Ram_Ptr+Temp08, 14 );//参数1数据
										
										ProgramRecord( DI07.B08, OptID.B08 );
									}
									
									//返回信息: 返写文件状态(1)+返写信息文件(50)+MAC3(4)									
									UpdataFlag &= B1100_1111;
									if( UpdataFlag & B0000_0101 ) UpdataFlag |= B0001_0000;//置位bit4
									if( UpdataFlag & B0000_1010 ) UpdataFlag |= B0010_0000;//置位bit5
									
									k = Terminal_Moneyin_Judge(OutBuf);	//有返写信息
									if( k == 1 ) 
									{
										MeterClear = 0x55aa;	//预置卡清零
										k = 2;
									}
									
									break;

								case 0x03: //交互终端下发当前套电价文件
								case 0x04: //交互终端下发备用套电价文件
									//当前/备用套电价文件, 199bytes	[18]~[216]
									//MAC1, 4bytes	[217]~[220], 用来算返写信息文件的MAC
									//68+表号+68+03+20+FF03A007+操作者代码+当前套电价文件+MAC1(4)+CS+16
									if( ((DI07.B08[1] == 0x03) && ((UpdataFlag & B0000_0101) == 0x00)) ||
									    ((DI07.B08[1] == 0x04) && ((UpdataFlag & B0000_1010) == 0x00)) )
									{
										k = 2;
										OutBuf[1] = 0;
										break;		//不需更新参数，返回成功
									}
									
									if( (DI07.B08[1] == 0x04) || (UserCardFlag == 0x01) || (CardType == PresetCardCommand) ) //购电卡和补卡，不更新当前套，直接返回成功
									{
										Temp08 = DI07.B08[1]+0x80;
										result = Update_ESAM_Bin_Mac( Temp08, 0, Ram_Ptr+18, 199 );//带MAC更新备用套电价文件
										if( result ) 
										{
											k = 0x00;
											OutBuf[ k++ ] = 0xFF;
											OutBuf[ k++ ] = 0x04;	//ESAM验证失败
											OutBuf[ k++ ] = 0x00;
											break;
										}
										Inverse(Ram_Ptr+18, 199);
										
										//备份相关参数
										Temp08 = DI07.B08[1]-3;
										memcpy(&Refers_File_Mac[BACKADDR[2+Temp08]], Ram_Ptr+18, 52);//费率电价
										memcpy(&Refers_File_Mac[BACKADDR[5+Temp08*4]], Ram_Ptr+18+52, 52);//第1阶梯电价
										memcpy(&Refers_File_Mac[BACKADDR[6+Temp08*4]], Ram_Ptr+18+52+52, 18);//第1年结算日
										memcpy(&Refers_File_Mac[BACKADDR[7+Temp08*4]], Ram_Ptr+18+52+52+18, 52);//第2阶梯电价
										memcpy(&Refers_File_Mac[BACKADDR[8+Temp08*4]], Ram_Ptr+18+52+52+18+52, 18+5);//第2年结算日(备用套有切换时间5字节)
										
										ProgramRecord( DI07.B08, OptID.B08 );
									}
									if(DI07.B08[1] == 0x03) UpdataFlag &= B1110_1111;
									else UpdataFlag &= B1101_1111;
																		
									k = Terminal_Moneyin_Judge(OutBuf);	//有返写信息
									if( k == 1 ) 
									{
										MeterClear = 0x55aa;	//预置卡清零
										k = 2;
									}
									
									break;
											
								case 0x05: //合闸复电
									//68+表号+68+03+20+FF05A007+操作者代码+合闸复电密文+CS+16	
									if( BDMODE ) break;	//只有远程表才支持合闸复电命令
									
									MacExchange( Ic_Comm_RxBuf, Ram_Ptr+18, 16 );
									result = Relay_Ctrl(Ch+0x24);
									if( result )
									{
										k = 0x00;
										OutBuf[ k++ ] = 0xFF;
										OutBuf[ k++ ] = 0x00;
										OutBuf[ k++ ] = 0x02;	//合闸复电失败
										break;
									}
									
									terminal_auth_flag = 0;	//数据操作结束，身份认证失效
									Identitytimer.B32 = 0;
									
									k = 2;
									OutBuf[1] = 0;
									break;
											
								case 0x06: //费控模式切换命令
//								case 0x01:	//表号, 8bytes	[18]~[25]
//								case 0x02:  //费控模式状态字, 1byte[26]
//								case 0x03:	//MAC1, 4bytes[27]~[30]
//								case 0x03:  //钱包文件, 8bytes[31]~[38]
//								case 0x04:	//MAC2, 4bytes[39]~[42]
									if( hangup_comm_flag )
									{
										k = 0x00;  
										OutBuf[ k++ ] = 0xFF;	//Operation result.
										OutBuf[ k++ ] = 0x00;
										OutBuf[ k++ ] = 0x01;	//远程安全认证锁住状态位
										break;
									}
											
									if( !identity_prog_flag )
									{
										k = 0x00;
										OutBuf[ k++ ] = 0xFF;
										OutBuf[ k++ ] = 0x08;	//身份认证失败
										OutBuf[ k++ ] = 0x00;
										break;
									}
									
									if( (Ram_Ptr[26] != 0x00) && (Ram_Ptr[26] != 0x01) ) break;	//模式字错误
									if(Ram_Ptr[26] == Display_Para.MeterCtrl) break;	//模式相同

									Exchange( Buf, Ram_Ptr+18, 8 );	//表号
									if( Buf[0] || Buf[1] || Cmp_Meter_Id(Buf+2) ) break;
									
									if((!BDMODE) && (Key_Status != 0)) break;	//私钥不允许远程切本地
									
									if( Update_ESAM_Bin_Mac( 0x81, 0x2d, Ram_Ptr+26, 1 ) ) //更新费控模式字
									{
										k = 0x00;
										OutBuf[ k++ ] = 0xFF;
										OutBuf[ k++ ] = 0x04;	//ESAM验证失败
										OutBuf[ k++ ] = 0x00;
										break;
									}
									
									if(BDMODE)//本地——远程：清钱包文件
									{
										Moneybag_Process(); //钱包同步更新处理
										
										if( ChkBufZero( Money.Remain, 4 ) )//剩余金额大于0
										{
											Bcd2Hex(&Money.Remain[0], Ic_Comm_TxBuf+5, 4);//扣除表内剩余金额
                                                                                	
											RxTxCommhead( 0x80, 0x30, 0x00, 0x14, 0x04 );
											result = CardTxRxFrame( ESAM, 0x04, 0x08 );
											if( result )
											{
												k = 0x00;
												OutBuf[ k++ ] = 0xFF;
												OutBuf[ k++ ] = 0x04;	//ESAM验证失败
												OutBuf[ k++ ] = 0x00;
												break;
											}
											
											Dmoney_Record(Ic_Comm_TxBuf+5);	//退费及退费记录
										}
										else//剩余金额等于0
										{
											memset( Money.Remain, 0x00, 12 );//剩余金额和透支金额清零
											Money.Crc16.B16 = Crc_16Bits( Money.Remain, ee_remainmoney_lenth );	 //CRC计算
											WriteE2WithBackup( ee_remainmoney_page, ee_remainmoney_inpage, Money.Remain, ee_remainmoney_lenth );//剩余金额写数据ee(三备份同步)
										}
									}
									
									result = ModeChange( Ram_Ptr[26] ); //费控模式切换
									if( result ) break;
									
									MeterClear = 0x1122;			//缩短应答时间					
									
									k = 2;
									OutBuf[1] = Display_Para.MeterCtrl;	//费控模式状态字, 1byte
									break;

								default:
									break;
							}
							
							if( ((k == 0) || (k == 3)) && (DI07.B08[1] != 6)) //报错，交互终端身份认证失效
							{
								if( terminal_auth_flag ) Identitytimer.B32 = 0;
								terminal_auth_flag = 0;	
							}

							break;
							
						default:
							break;
					}
				
				default:
				break;
			}
			
			if( k == 0x00 )
			{
				//错误：68+表号+68+C3+02+SERR+CS+16
				OutBuf[ k++ ] = 0xFF;	//Operation result.
				OutBuf[ k++ ] = 0x01;
				OutBuf[ k++ ] = 0x00;
			}
			break;

		//====================================================================================
		case 0x14: //写参变量
			if( ((Uart[Ch].Flag & B0000_0011)==0)&&(DI07.B32 == 0x03150004)&&(Reset_Rep_Timer) )//复位主动上报状态字
 			{
 				if( Uart[Ch].Buf[9] != 24 ) break;
 				
 				//修正上报状态字
				if( (Uart[Ch].Buf[22+2]&B1000_0000)==0 ) Uart[Ch].Buf[22+2] |= B1000_0000;//掉电临时借用存A相断相
				if( (Uart[Ch].Buf[22+8]&B0010_0000)==0 ) Uart[Ch].Buf[22+2] &= B0111_1111;
 				
 				for( i=0,n=0; i<5; i++,n++ )
				{
					Temp08 = (Report_Data[i]^Report_Data2[i]);
					if( Temp08 && ((Temp08&Uart[Ch].Buf[22+n]) != Temp08) )  break;//存在30min内新的上报,并且通讯复位新增上报	
					if( i==2 ) n = 9;
				}
				if( i < 5 )
				{
					k = 2;
 					OutBuf[0] = 0xFF;	//Operation result.
 					OutBuf[1] = 0x04;	//Error: bit2密码错误/未授权
					break;//存在30min内新的上报,并且通讯复位新增上报
				}
 				
 				for( i=0,n=0; i<5; i++,n++ )
				{
					Report_Data[i] &= Uart[Ch].Buf[22+n];//复位上报状态字
					if( i==2 ) n = 9;
					
					Clr_Reportnum(i);	//清零上报新增次数
					Reset_Rep_Timer = 0;
				}
 				k = 1;
				break;
 			}
 			
 			if(  (Uart[Ch].Flag & B0000_0011) || //不支持通信地址为广播和|| (!identity_prog_flag)全AA
 				(( DI07.B08[3] != 0x04 )&&( DI07.B08[3] != 0xee )) ||	//用户对不支持写操作的数据项编程
 				( Uart[Ch].Buf[14] == 0x04 ) ||	//不支持04级密码
 				((Uart[Ch].Buf[14] == 0x03 ) && (!progenable)&&(Veri_Psw( Uart[Ch].Buf+14) ) ) || //03级：编程禁止
 				((Uart[Ch].Buf[14] != 0x03 ) && (hangup_comm_flag )) )	//98或99级：设置功能挂起, 安全认证失效
 			{
 				k = 2;
 				OutBuf[0] = 0xFF;	//Operation result.
 				OutBuf[1] = 0x04;	//Error: bit2密码错误/未授权
 				break;
 			}
			
			if( Uart[Ch].Buf[9] <= 12 ) break;//数据长度必须大于12

			memcpy( OptID.B08, Uart[Ch].Buf+18 ,4 );

			Auth = Uart[Ch].Buf[14];
			switch( Auth ) //密码权限
			{
				case KEYTYPE2:
					if( DI07.B32 == 0x06100004 ) 
					{
						if( !BDMODE ) { Err_DI = 1;	break; }
						
						Moneybag_Process1();//钱包同步更新处理
						
						result = DecreaseWithMac( Uart[Ch].Buf+22 );
						if( result )                                           
						{
							k = 0x00; 
							OutBuf[ k++ ] = 0xFF;
							OutBuf[ k++ ] = result;
							break;
						}
						
						Dmoney_Record(Ram_Ptr+22);   //退费记录 
						
						k = 1;
						break;
					}
				
					//通过密文+MAC的方式进行数据传输，不需要进行密码验证，也不需要编程键配合使用。
					//68+表号+68+14+L+DI0-DI3+99(98)000000+操作者代码+参数明文+MAC（带MAC的参数密文）+CS+16
					//带MAC写ESAM:		04D682(83,84)+P2+LC+参数+MAC
					//输入到ESAM的参数更新文件来解密，文件号=DI07.B08[2]%5;
					//将密文写入ESAM: 	04D689(90,91,92)+00+14+密文(带MAC)
					if( Uart[Ch].Buf[9] <= 16 ) break;//数据长度必须大于16

					Temp08 = Hex_To_Bcd(88+(DI07.B08[2]%5));
					result = Decrypt( Ch, Temp08, 0x02 );
					if( result )
					{
						k = 0x00; 
						OutBuf[ k++ ] = 0xFF;
						OutBuf[ k++ ] = 0x04;
						break;
					}
					
					//Ic_Comm_RxBuf[0]:数据长度; [1]~[4]:数据标识; [5]~[n]:
					//比对数据中数据标识
					for( i=0; i<4; i++ )
					{
						if( Ic_Comm_RxBuf[4-i] != DI07.B08[i] ) break;
					}
					
					if( i < 4 ) break;
				
					//剔除数据长度和DI后填入UART.Buf中.
					Uart[Ch].Buf[9] = 8+Ic_Comm_RxBuf[0];	//
					
					Exchange( Uart[Ch].Buf+22, Ic_Comm_RxBuf+5, Ic_Comm_RxBuf[0]-4 );
					
				case 0x03:	//厂内扩展命令
					if(DI07.B08[3] == 0xee)
					{
						switch( DI07.B32 )
					  {
							case 0xEE000500://校表参数数据块
              case 0xEE000501:
              case 0xEE000502:
                  Flag.PhOfset0 = 0;
                  Flag.PhOfset1 = 0;
                  Ch = (unsigned char)( DI07.B32 - 0xEE000500 );
                  if(Uart[Ch].Buf[22+14] == 0x00)
                  {
                    Uart[Ch].Buf[22+14]= 0x80;
                    Uart[Ch].Buf[22+15] = 0x80;
                    Uart[Ch].Buf[22+16] = 0x80;
                    Uart[Ch].Buf[22+17] = 0x80;
                   }            
            memcpy( Adj_Block[Ch].AdjBuff, &Uart[Ch].Buf[22], 30 );
						WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;
        case 0xEE000503://电流增益
        case 0xEE000504:
        case 0xEE000505:
            Ch = (unsigned char)( DI07.B32 - 0xEE000503 );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.Igain, &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;
        case 0xEE000506://电压增益
        case 0xEE000507:
        case 0xEE000508:
            Ch = (unsigned char)( DI07.B32 - 0xEE000506 );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.Ugain, &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;
        case 0xEE000509://功率偏移
        case 0xEE00050A:
        case 0xEE00050B:
            Ch = (unsigned char)( DI07.B32  - 0xEE000509 );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.Pofst, &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;
        case 0xEE00050C://Ib电流相位
        case 0xEE00050D:
        case 0xEE00050E:
            Ch = (unsigned char)(  DI07.B32 - 0xEE00050C );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.PTofst[0], &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;            
        case 0xEE00050F://小电流相位
        case 0xEE000510:
        case 0xEE000511:
            Ch = (unsigned char)(  DI07.B32 - 0xEE00050F );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.PTofst[1], &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;            
            RN8302Init();
            break;            
        case 0xEE000512://大电流相位
        case 0xEE000513:
        case 0xEE000514:
            Ch = (unsigned char)(  DI07.B32 - 0xEE000512 );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.PTofst[2], &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;
        case 0xEE00052C://无功Ib电流相位
        case 0xEE00052D:
        case 0xEE00052E:
            Ch = (unsigned char)(  DI07.B32 - 0xEE00052C );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.QTofst[0], &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;            
        case 0xEE00052F://无功小电流相位
        case 0xEE000530:
        case 0xEE000531:
            Ch = (unsigned char)(  DI07.B32 - 0xEE00052F );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.QTofst[1], &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;            
        case 0xEE000532://无功大电流相位
        case 0xEE000533:
        case 0xEE000534:
            Ch = (unsigned char)(  DI07.B32 - 0xEE000532 );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.QTofst[2], &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;            
        case 0xEE000515://电流偏移
        case 0xEE000516:
        case 0xEE000517:
            Ch = (unsigned char)( DI07.B32 - 0xEE000515 );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.Iofst, &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;
        case 0xEE00051C://电压偏移
        case 0xEE00051D:
        case 0xEE00051E:
            Ch = (unsigned char)( DI07.B32 - 0xEE00051C );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.Uofst, &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;            
        case 0xEE000518://有功功率增益
        case 0xEE000519:
        case 0xEE00051A:
            Ch = (unsigned char)( DI07.B32 - 0xEE000518 );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.Pgain, &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;    
        case 0xEE000526://无功功率增益
        case 0xEE000527:
        case 0xEE000528:
            Ch = (unsigned char)( DI07.B32 - 0xEE000526 );
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.Qgain, &Uart[Ch].Buf[22], 2 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;            
        case 0xEE000520://采样通道U相位偏移
        case 0xEE000521:
        case 0xEE000522:
            Ch = (unsigned char)( DI07.B32 - 0xEE000520 );
            if( Uart[Ch].Buf[22] == 0x00 )
               Uart[Ch].Buf[22] = 0x80; 
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.UTofst, &Uart[Ch].Buf[22], 1 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;    
        case 0xEE000523://采样通道I相位偏移
        case 0xEE000524:
        case 0xEE000525:
            Ch = (unsigned char)( DI07.B32 - 0xEE000523 );
            if( Uart[Ch].Buf[22] == 0x00 )
            {
                Uart[Ch].Buf[22] = 0x80; 
                Uart[Ch].Buf[23] = 0x80;
                Uart[Ch].Buf[24] = 0x80;
            }
            memcpy( (unsigned char*)&Adj_Block[Ch].Adj.ITofst, &Uart[Ch].Buf[22], 3 );
            WriteE2WithBackup( ee_para1_RN8302_page, ee_para1_RN8302_inpage+30*Ch,(unsigned char*)&Adj_Block[Ch].AdjBuff[0], ee_para1_RN8302_lenth);			 
            Adj_Block[Ch].Adj.AdjChk = 0;
            RN8302Init();
            break;  
//        case 0xEE000535://NVM1模式下电流自动 offset 校正 
//            for( Ch = 0; Ch < 3; Ch++ )
//            {
//                RN8302_Read( (unsigned char*)&Adj_Block2[Ch].Adj2.Iofst2, (DCOS_IA+Ch), 2 );//读直流通道
//                MEM_Write( ADJ_DCOS_IA+2*Ch, (unsigned char*)&Adj_Block2[Ch].Adj2.Iofst2, 2, Mem_E2PROMA );
//            }
//            break;   
        case 0xEE000536://关闭高通 
            Temp = 0xE5;//写使能
            RN8302_Write( 0x0180, (unsigned char*)&Temp, 1 );
            Temp = 0x02;//关闭高通
            RN8302_Write( 0x0170, (unsigned char*)&Temp, 1 );
            Temp = 0x0095;//直流 OFFSET 自动校正使能寄存器：0x0010(不参与校验,但下次checksum会出错)
            RN8302_Write( 0x0191, (unsigned char*)&Temp, 2 );
            Temp = 0xDC;//写保护
            RN8302_Write( 0x0180, (unsigned char*)&Temp, 1 );
            break;				
			  default:
						Err_DI = 1;	//数据标识错
						break;
					}
								
					}
				  else
					{
						switch( DI07.B08[2] )
					  {
						case 0x00:
							switch( DI07.B08[1] )
							{
								case 0x01:
									switch( DI07.B08[0] )
									{
										case 0x01:	//日期及星期(其中0代表星期天)	YYMMDDWW  	4	年月日星期	
										case 0x02:	//时间                          hhmmss    	3	时分秒      
										case 0x0C:	//日期、星期(其中0代表星期天)及时间	YYMMDDWWhhmmss	7 年月日星期时分秒
											if( DI07.B08[0] == 0x01 )
											{
												Temp16.B08[0] = 3;
												Temp16.B08[1] = 4;
											}
											else if( DI07.B08[0] == 0x02 )
											{
												Temp16.B08[0] = 0;
												Temp16.B08[1] = 3;
											}
											else
											{
												Temp16.B08[0] = 0;
												Temp16.B08[1] = 7;
											}
																									
											result = CommSetRtc( Ch, Temp16.B08[0], Temp16.B08[1] );
											if( result ) break;
                      
											Festival_holiday_Judge(); 
											run_alarm_flag = 1;
											
											Init_Demand();	//当前需量清零
											
											k = 1;
											OutBuf[0] = 0x00;
											k = 1;
											break;
											
										case 0x03:	//最大需量周期                  NN        	1	分
											
											result = IsBCD( Uart[Ch].Buf[22] );
											if( result != 0x00 ) 
											{												
												break; //error.
											}
											
											//出厂默认值：需量周期15min、滑差时间1min。
											switch( Uart[Ch].Buf[22] )
											{
												case 0x05:
												case 0x10:
												case 0x15:
												case 0x30:
												case 0x60:
													ReadE2WithBackup( ee_Demand_Period_page, ee_Demand_Period_inpage,Buf, ee_Demand_Period_len );				
													
													Temp08 = Bcd_To_Hex(Uart[Ch].Buf[22]);
													if( Temp08 == Buf[0])
										      {													
														k = 1;
													  OutBuf[0] = 0x00;
														break;
													}				
													else
													  Buf[0] = Temp08;
													
													result = WriteE2WithBackup( ee_Demand_Period_page, ee_Demand_Period_inpage,Buf, ee_Demand_Period_len );
													if( result != 0x00 )
													{													
														break;
													}

													Demand_Para.Max_Demand_Period = Temp08;													
				                            	
													Init_Demand();	//当前需量清零
                          
													Dm_veri_flag = 1;
													k = 1;
													OutBuf[0] = 0x00;
													break;
													
												default:
													break;
											}
											break;

										case 0x04:	//滑差时间                      NN        	1	分          
											 
										  result = IsBCD( Uart[Ch].Buf[22] );
											if( result != 0x00 ) 
											{												
												break; //error.
											}  
											switch( Uart[Ch].Buf[22] )
											{
												case 0x01:
												case 0x02:
												case 0x03:
												
												  ReadE2WithBackup( ee_Demand_Period_page, ee_Demand_Period_inpage,Buf, ee_Demand_Period_len );				
													
													Temp08 = Bcd_To_Hex(Uart[Ch].Buf[22]);
													if( Temp08 == Buf[1])
										      {													
														k = 1;
													  OutBuf[0] = 0x00;
														break;
													}				
													else
													  Buf[1] = Temp08;
													
													result = WriteE2WithBackup( ee_Demand_Period_page, ee_Demand_Period_inpage,Buf, ee_Demand_Period_len );
													if( result != 0x00 )
													{													
														break;
													}

													Demand_Para.Sliding_Time = Temp08;													
				                            	
													Init_Demand();	//当前需量清零
                          Dm_veri_flag = 1;
													k = 1;
													OutBuf[0] = 0x00;
													break;												
													
												default:
													break;
											}
											break;
										
	
//										case 0x05:	//校表脉冲宽度                  XXXX      	2	毫秒        
//											break;

										case 0x0A:	//两套阶梯时区切换时间	YYMMDDhhmm 5
											if( !BDMODE ) { Err_DI = 1;	break; }
										case 0x06:	//两套时区表切换时间            YYMMDDhhmm	5	年月日时分
										case 0x07:	//两套日时段切换时间            YYMMDDhhmm	5	年月日时分
//										case 0x0B:	//两套费控模式切换时间	YYMMDDhhmm 5
											//两套时区表切换时间（5）＋两套日时段表切换时间（5）＋两套费率电价切换时间（5）＋两套阶梯切换时间（5）＋crc（2）
											Temp08 = JudgeSwitch( Uart[Ch].Buf+22, 0 );
											if( Temp08==1 ) break;
												
											result = UpdateSysPara( 0xC0+5*(DI07.B08[0]-6), Uart[Ch].Buf+22, 5 );
											if( result ) break;
											
											if( (Temp08==3)&&(DI07.B08[0]<=0x07) ) JudgeSwitch2(DI07.B08[0]-6);//翻转控制字节,清启动时间和进行约定冻结
											
											k = 1;
											break;
											
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;
									
								case 0x02:
									//年时区数（1）＋日时段表数（1）＋日时段数 （1）＋费率数（1）＋公共假日数（1）+谐波分析次数（1）+阶梯数（1）＋密钥数（1）＋crc（2）
									Temp32.B08[0] = 0x00;	//最小值
									Temp32.B08[1] = 0x00;	//最大值
									Temp32.B08[2] = 0x00;	//超过报错字
									switch( DI07.B08[0] )
									{
										case 0x01:	//年时区数p≤14            	NN      1	个
											Temp32.B08[0] = 1;			//最小值
											Temp32.B08[1] = MAXREGION;	//最大值
											Temp32.B08[2] = 0x10;		//超过报错字: 年时区数超(Bit4)
											break;

										case 0x02:	//日时段表数q≤8           	NN      1	个	
											Temp32.B08[0] = 1;			//最小值
											Temp32.B08[1] = MAXSIDUANTB;//最大值
											break;

										case 0x03:	//日时段数(每日切换数)m≤14	NN      1	个
											Temp32.B08[0] = 1;			//最小值
											Temp32.B08[1] = MAXSIDUAN;	//最大值
											Temp32.B08[2] = 0x20;		//超过报错字: 日时段数超(Bit5)
											break;

										case 0x04:	//费率数k≤63              	NN      1	个	
											Temp32.B08[1] = 4;			//最大值
											Temp32.B08[2] = 0x40;		//超过报错字: 费率数超(Bit6)
											break;

										case 0x05:	//公共假日数n≤254         	NNNN	2	个
											if( Uart[Ch].Buf[23] ) break;
											Temp32.B08[1] = MAXHOLIDAY;	//最大值
											break;
//
//										case 0x06:	//谐波分析次数             	NN  	1	次	
//											break;

										case 0x07:	//阶梯数			NN	1	个
											if( !BDMODE ) { Err_DI = 1;	break; }
											
											Temp32.B08[1] = 6;			//最大值
											RefreshPrice();
											break;
											
										case 0x08:	//密钥总条数			HH 	1	个
											Temp32.B08[1] = MAX_PSW_NUM;	//最大值
											break;
										
										default:
											Err_DI = 1;	//数据标识错
											break;	
									}
									
									if( Temp32.B08[1] == 0x00 ) break;	//配置失败

									if( DI07.B08[0] != 0x08 )
									{
										if( IsBCD(Uart[Ch].Buf[22]) ) break;
										Temp08 = Bcd_To_Hex( Uart[Ch].Buf[22] );
									}
									else Temp08 = Uart[Ch].Buf[22];		//密钥条数设置为hex码

									if( (Temp08>=Temp32.B08[0]) && (Temp08 <= Temp32.B08[1]) )
									{
										ReadE2WithBackup( ee_feik_number_page, ee_feiknumber_inpage, Buf, ee_feiknumber_lenth );
										Buf[DI07.B08[0]-1] = Uart[Ch].Buf[22];
										result = WriteE2WithBackup( ee_feik_number_page, ee_feiknumber_inpage, Buf, ee_feiknumber_lenth );
										if( result != 0x00 ) break; //error
										
										run_timezone_flag = 1;
										Tariff.Crc16.B16 += 1;
										Feik.Crc16.B16 += 1;
									
										if( DI07.B08[0] == 8 )
										{
											Feik.Num[7] = Uart[Ch].Buf[22];
											GetKeyStatus();
										}

										k = 1;
									}
									else if( Temp32.B08[2] && (Temp08 > Temp32.B08[1]) )
									{
										k = 2;
										OutBuf[0] = 0xFF;
										OutBuf[1] = Temp32.B08[2]; 	//错误信息字ERR
									}
									break;
									
								case 0x03:	  
									//循显屏数(1)＋循显时间(1)＋电能小数位数(1)＋功率位数(1)＋键显屏数(1)＋波特率(1)+ 跳闸延时时间(2)＋crc(2)
									Temp32.B08[0] = 0x00;	//最小值
									Temp32.B08[1] = 0x00;	//最大值
									switch( DI07.B08[0] )
									{
										case 0x01:	//自动循环显示屏数            	NN      1   个
										case 0x05:	//按键循环显示屏数            	NN      1   个  	
											Temp32.B08[0] = 1;	//最小值
											Temp32.B08[1] = SCREENNUM;	//最大值
											break;
											
										case 0x02:	//每屏显示时间                	NN      1   秒
											Temp32.B08[0] = 5;	//最小值
											Temp32.B08[1] = 20;	//最大值
											break;
                                	
										case 0x03:	//显示电能小数位数            	NN      1   位  
											Temp32.B08[1] = 2;	//最大值
											break;
                                	
										case 0x04:	//显示功率（最大需量）小数位数	NN      1   位  
											Temp32.B08[1] = 4;	//最大值
											break;
										
										case 0x08:	//上电全显时间	NN	1	位
											Temp32.B08[0] = 5;	//最小值
											Temp32.B08[1] = 30;	//最大值
											break;
											
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									
									if( Temp32.B08[1] == 0x00 ) break;

									if( IsBCD(Uart[Ch].Buf[22]) ) break;
									Temp08 = Bcd_To_Hex( Uart[Ch].Buf[22] );
									
									if( (Temp08 >= Temp32.B08[0]) && (Temp08 <= Temp32.B08[1]) )
									{
										result = UpdateSysPara( DI07.B08[0]-1, &Temp08, 1 );
										if( result != 0x00 ) break;
	                                		                                	
										k = 1;
									}
									break;

								case 0x04:	  
									switch( DI07.B08[0] )
									{
										case 0x01:	//通信地址                     	NNNNNNNNNNNN	6
											memset( Buf, 0x99, 6 );
											if( Cmp_Data( Buf, Uart[Ch].Buf+22, 6 ) ==3 ) break;	//禁止设置全9
											if( IsMultiBcd( Uart[Ch].Buf+22, 6 ) )	break;
											
											Exchange( Buf, Uart[Ch].Buf+22, 6 );
																						
											result = WriteE2WithBackup( ee_commaddr_page, ee_commaddr_inpage, Buf, ee_commaddr_lenth );
											if( result != 0x00 ) break;
												
											memcpy( Comm_Addr.Comm_Id, Buf, 8 );
	
											k = 1;
											break;
                    case 0x02:	//通信地址                     	NNNNNNNNNNNN	6
											memset( Buf, 0x99, 6 );
											if( Cmp_Data( Buf, Uart[Ch].Buf+22, 6 ) ==3 ) break;	//禁止设置全9
											if( IsMultiBcd( Uart[Ch].Buf+22, 6 ) )	break;
											
											Exchange( Buf, Uart[Ch].Buf+22, 6 );
											memcpy( Esam_Para.Meter_Id, Buf, 6 );//复制esam表号到ram
		                  
										  result = WriteE2WithBackup(ee_esampara_page, ee_esampara_inpage, Esam_Para.Alarm_Limit1.B08, ee_esampara_lenth);
												
											if( result != 0x00 ) break;											
	
											k = 1;
											break;
										case 0x05:	//额定电流/基本电流（ASCII 码）XXXXXXXXXXXX	6
										case 0x06:	//最大电流（ASCII 码）         	XXXXXXXXXXXX	6
										case 0x07:  	//精度等级（ASCII 码）         XXXXXXXX   	4
										case 0x0B:  	//电表型号（ASCII 码）         XX…XX      	10 
										case 0x0C:	//生产日期（ASCII 码）          XX…XX      	10 
										case 0x0D:	//协议版本号（ASCII 码）        XX…XX      	16 	
											if( Auth != 3 )
											{
												k = 2;
												OutBuf[0] = 0xFF; //结果
												OutBuf[1] = 0x04; //Error: bit4密码错误/未授权
												break;
											}
										case 0x03:	//资产管理编码（ASCII 码）     	NN…NN      	32
										case 0x0F:	//电能表位置信息				11
											if(DI07.B08[0] == 0x05)
											{
												Temp16.B08[0] = ee_ratedcurr_page;//额定电流
												Temp16.B08[1] = ee_ratedcurr_inpage;
												Temp08 = 6;//6字节
											}
											else if(DI07.B08[0] == 0x06)
											{
												Temp16.B08[0]=ee_maxcurr_page;//最大电流
												Temp16.B08[1] = ee_maxcurr_inpage;
												Temp08 = 6;//6字节
											}
											else if(DI07.B08[0] == 0x07)
											{
												Temp16.B08[0] = ee_activeclass_page; //有功精度等级
												Temp16.B08[1] = ee_activeclass_inpage;
												Temp08 = 4;
											}
											else if(DI07.B08[0] == 0x0B)
											{
												Temp16.B08[0] = ee_matertype_page; //电表型号
												Temp16.B08[1] = ee_matertype_inpage;
												Temp08 = 10;
											}
											else if(DI07.B08[0] == 0x0c) 
											{
												Temp16.B08[0] = ee_meterdate_page;//生产日期
												Temp16.B08[1] = ee_meterdate_inpage;
												Temp08 = 10;
											}
											else if(DI07.B08[0] == 0x0D)
											{
												Temp16.B08[0] = ee_protocolver_page;//协议版本号
												Temp16.B08[1] = ee_protocolver_inpage;
												Temp08 = 16;
											}
											else if(DI07.B08[0] == 0x03) 
											{
												Temp16.B08[0] = ee_zichannum_page;//资产管理编号
												Temp16.B08[1] = ee_zichannum_inpage;
												Temp08 = 32;
											}
											else
											{
												Temp16.B08[0] = ee_meterpos_page;//电能表位置信息
												Temp16.B08[1] = ee_meterpos_inpage;
												Temp08 = 11;
												
												if( Auth == KEYTYPE2 )
												{
													Inverse( Uart[Ch].Buf+22, 11 );
													Inverse( Uart[Ch].Buf+22+0, 4 );	//XXXX.XXXX	4	经度
													Inverse( Uart[Ch].Buf+22+4, 4 );	//XXXX.XXXX	4	纬度
													Inverse( Uart[Ch].Buf+22+8, 3 );	//XXXX.XX	3	高度
												}
											}
											if( Uart[Ch].Buf[9] != (Temp08+12) ) break;//数据长度错误
											I2C_Write_Eeprom( Temp16.B08[0], Temp16.B08[1], Uart[Ch].Buf+22, Temp08 );
											k = 1;
											break;
										
										default:
											Err_DI = 1;	//数据标识错
											break;                                      		
									}
									break;

								case 0x06:	  
									switch( DI07.B08[0] )
									{
										case 0x01:  //有功组合方式特征字  	NN	1
											//波特率特征字（5）＋有功组合方式特征字1(1)＋冻结数据模式字(6)＋电表运行特征字1(1) +上报模式字(6)+crc（2）
											if( Uart[Ch].Buf[22]&B1111_0000 ) break;	//非法格式
											
											i = Mode.Comp_B;	//暂存编程前数据
											
											result = UpdateSysPara( 0x80+1, Uart[Ch].Buf+22, 1 );
											if( result != 0x00 ) break;
												
											//--------- 有功组合方式编程 ------------
											memcpy( Eeprom.Buffer+6, OptID.B08, 4 );	//操作者代码(4)
											Eeprom.Buffer[10] = i;		//编程前采用的数据(1)
											Event_Record( ee_combn_inpage );
											
											k = 1;
											break;
											
//										case 0x02:  //无功组合方式1 特征字  NN	1	
//											break;
//											
//										case 0x03:	//无功组合方式2 特征字  NN	1	
//											break;
											
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;
									
								case 0x07:	  
									switch( DI07.B08[0] )
									{
										case 0x01:	//调制型红外光口波特率特征字	NN  1
//										case 0x02:	//接触式红外光口波特率特征字    NN  1
										case 0x03:	//通信口1 波特率特征字          NN  1
//										case 0x04:	//通信口2 波特率特征字          NN  1
										case 0x05:	//通信口3 波特率特征字          NN  1
											Temp08 = Set_Comm_Rate( DI07.B08[0], Uart[Ch].Buf[22] );
											if( Temp08 )
											{
												k = 2;
												OutBuf[0] = 0xff;
												OutBuf[1] = 0x08;
											}
											else
											{
												k = 1;
											}
											break;
	
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;
																		
								case 0x08:
									switch( DI07.B08[0] )
									{
										case 0x02:	//周休日采用的日时段表号    NN	1
											if( (Uart[Ch].Buf[22] == 0x00) || (Uart[Ch].Buf[22] > MAXSIDUANTB) )	break;	//只能是1~8的日时段表号
											i = Week_Holiday.Shiduan_N;	//暂存编程前数据
											
										case 0x01:	//周休日特征字		NN		1										
											result = UpdateSysPara( 0x60+DI07.B08[0]-1, Uart[Ch].Buf+22, 1 );
											if( result != 0x00 ) break;

											if( DI07.B08[0] == 0x02 )
											{
												//--------- 周休日编程 ------------
												memcpy( Eeprom.Buffer+6, OptID.B08, 4 );	//操作者代码(4)
												Eeprom.Buffer[10] = i;		//编程前采用的数据(1)
												Event_Record( ee_weekdayn_inpage );
											}
											
											k = 1;
											break;

										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;

								case 0x09:	
									switch( DI07.B08[0] )
									{
										case 0x01:	//负荷记录模式字		NN	1													
										case 0x02:	//冻结数据模式字		NN	1	定时冻结数据模式字
										case 0x03:	//瞬时冻结数据模式字	NN	1
										case 0x04:	//约定冻结数据模式字    NN	1
										case 0x05:	//整点冻结数据模式字    NN	1
										case 0x06:	//日冻结数据模式字      NN	1
											//波特率特征字（5）＋有功组合方式特征字1(1)＋冻结数据模式字(6)＋电表运行特征字1(1) +上报模式字(6)+crc（2）
											if( DI07.B08[0] == 0x05 )
											{
												if( Uart[Ch].Buf[22]&B1111_1100 ) break;
											}
											if( DI07.B08[0] == 0x01 )
											{
												if( Uart[Ch].Buf[22]&B1111_0000 ) break;
											}
											else
											{
												if( Uart[Ch].Buf[22]&B0111_1100 ) break;
											}
											
											result = UpdateSysPara( 0x80+2+DI07.B08[0]-1, Uart[Ch].Buf+22, 1 );
											if( result != 0x00 ) break;
	
											k = 1;
											break;
	
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;
									
								case 0x0A:
									switch( DI07.B08[0] )
									{
										case 0x01:	//负荷记录起始时间		MMDDhhmm	4
											result = JudgeClockNoWeek( 1, Uart[Ch].Buf+22, 4 );
											if( result != 0 ) break; //错误
											
											result = UpdateSysPara( 0x60+3, Uart[Ch].Buf+22, 4 );
											if( result != 0x00 ) break;
											
											k = 1;
											break;
												
										case 0x02:	//第1 类负荷记录间隔时间    NNNN	2
										case 0x03:	//第2 类负荷记录间隔时间    NNNN	2
										case 0x04:	//第3 类负荷记录间隔时间    NNNN	2
										case 0x05:	//第4 类负荷记录间隔时间    NNNN	2
											if( IsMultiBcd( Uart[Ch].Buf+22, 2 ) )	break;
											if( ((Uart[Ch].Buf[22] == 0) || (Uart[Ch].Buf[22]>0x60)) || Uart[Ch].Buf[23] ) break;
											
											result = UpdateSysPara( 0x60+7+2*(DI07.B08[0]-2), Uart[Ch].Buf+22, 2 );
											if( result != 0x00 ) break;
										
											k = 1;
											break;

										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;
																	
								case 0x0B:
									switch( DI07.B08[0] )
									{
										case 0x01:	//每月第1 结算日	DDhh	2	日时
										case 0x02:	//每月第2 结算日    DDhh	2	日时
										case 0x03:	//每月第3 结算日    DDhh	2	日时
											if( (Uart[Ch].Buf[22] != 0x99) || (Uart[Ch].Buf[23] != 0x99) )
											{
												result = JudgeClockNoWeek( 2, Uart[Ch].Buf+22, 2 );
												if( result != 0 ) break; //错误
	
												if( Uart[Ch].Buf[23] > 0x28 )	break;	//只能是1~28日的整点时刻
											}
											
											ReadE2WithBackup( ee_histroyparameter_page, ee_histroyparameter_inpage, Buf, ee_histroyparameter_lenth );
											
											memcpy( ChangeBuf, Buf, ee_histroyparameter_lenth );

											Buf[2*(DI07.B08[0]-1)+0] = Uart[Ch].Buf[22];
											Buf[2*(DI07.B08[0]-1)+1] = Uart[Ch].Buf[23];

											result = WriteE2WithBackup( ee_histroyparameter_page, ee_histroyparameter_inpage, Buf, ee_histroyparameter_lenth );
											if( result != 0x00 ) break;
											
											if( (DI07.B08[0] == 0x01)	//设置第一结算日
											 && ((ChangeBuf[0]!=Uart[Ch].Buf[22]) || (ChangeBuf[1]!=Uart[Ch].Buf[23])) )//第一结算日发生改变
											{
												if( Price.Ladder_Mode == 0x55 )
													Freeze_Usedl(1);//年结算,月转存,不清用电量
												else
													Freeze_Usedl(2);//月结算,月用电量转存和清零
												Freeze_Dl(ee_histroy_dl_page,ee_histroyf_inpage);
												
												run_price_flag = 1;
											}

											//---- 结算日编程记录 ----
											memcpy( Eeprom.Buffer+6, OptID.B08, 4 );	//操作者代码(4)
											memcpy( Eeprom.Buffer+10, ChangeBuf, 6 );	//结算日编程前每月第1~3结算日数据(6)
											
											Event_Record( ee_jieshuann_inpage );
											
											k = 1;
											break;

										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;
								
								case 0x0C:	//0级密钥~9级密钥：03级密码，04级密码		
									if( ((DI07.B08[0] == 0x04) || (DI07.B08[0] == 0x05)) && ((DI07.B08[0]-1) == Uart[Ch].Buf[22]) )
									{
										WriteE2WithBackup( ee_password_page, ee_password_inpage+(DI07.B08[0]-4)*6, Uart[Ch].Buf+22, ee_password_lenth );
										k = 1;
									}
									else
									{
										k = 2;
										OutBuf[0] = 0xFF; //结果
										OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
									}		
									break;
									
								case 0x10:
									if( !BDMODE ) { Err_DI = 1;	break; }
										
									switch( DI07.B08[0] )
									{
										case 0x01:  //报警金额1 限值	XXXXXX.XX	4	元											
										case 0x02:  //报警金额2 限值	XXXXXX.XX	4	元
										  Temp32.B08[2] = 0x40+(DI07.B08[0]-1)*4;	//E2中页内偏移量	
										  Temp32.B08[0] = 4;
										  if( IsMultiBcd( Uart[Ch].Buf+22, 4 ) )	break;
	 
										  result = UpdateSysPara( Temp32.B08[2], Uart[Ch].Buf+22, Temp32.B08[0] ); //更新XRAM, E2PROM
										  if( result ) break;	
										
										  run_price_flag = 1;
										  k = 1;									
									    break;
										
										case 0x03:	//透支金额限值	XXXXXX.XX	4	元
											if( remainder_maxtick_flag )	//防止剩余金额为0,透支门限有0变成非0自动闭合继电器的情况
											{
												card_closerelay_flag   = 0;
												card_closerelay_flag1  = 0;
												Store_Meterstatus();
											}
										case 0x04:	//囤积金额限值    NNNNNN.NN	4	元
										case 0x05:	//合闸允许金额限值  NNNNNN.NN   4	元
											//透支金额限值(4)＋囤积金额限值(4)＋合闸允许金额限值(4)＋Crc(2)
											Temp08 = (DI07.B08[0]-0x03)*4;
											
											if( IsMultiBcd( Uart[Ch].Buf+22, 4 ) )	break;

											ReadE2WithBackup( ee_limitmenory_page, ee_limitmenory_inpage, Buf, ee_limitmenory_lenth );

											Exchange( Buf+Temp08, Uart[Ch].Buf+22, 4 );

											result = WriteE2WithBackup( ee_limitmenory_page, ee_limitmenory_inpage, Buf, ee_limitmenory_lenth );

											if( result != 0x00 ) break;
																								
											memcpy( Money_Para.Tick.B08, Buf, ee_limitmenory_lenth+2 );

											run_alarm_flag = 1;

											k = 1;
											break;
											
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;
								
								case 0x11:
									switch( DI07.B08[0] )
									{
										case 0x01:	//电表运行特征字1	NN	1
											//波特率特征字（5）＋有功组合方式特征字1(1)＋冻结数据模式字(6)＋电表运行特征字1(1) +上报模式字(6)+crc（2）
											result = UpdateSysPara( 0x80+8, Uart[Ch].Buf+22, 1 );
											if( result != 0x00 ) break;
	
											k = 1;
											break;
											
										case 0x04:	//主动上报模式字	NNNNNNNNNNNNNNNN	8
											//波特率特征字（5）＋有功组合方式特征字1(1)＋冻结数据模式字(5)＋电表运行特征字1(1)＋上报模式字(6) + crc（2）
											if( Uart[Ch].Buf[9] != 20 ) break;//数据长度错误
											Inverse( Uart[Ch].Buf+22, 8 );	//数据倒序(低字节在前变成高字节在前,下面程序又变成低字节在前)
											result = UpdateSysPara( 0x80+9, Uart[Ch].Buf+22, 8 );
											if( result != 0x00 ) break;
	
											k = 1;
											break;
											
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;
																
								case 0x12:
									I2C_Read_Eeprom44( ee_freezeparameter_page, Buf, ee_freezeparameter_lenth );
									
									Temp08 = 0x00;	//默认失败
									switch( DI07.B08[0] )
									{
										case 0x01:	//整点冻结起始时间	YYMMDDhhmm	5	年月日时分，默认数值为全零，设置值应是整点值或半点值。
//											if( (Uart[Ch].Buf[22]!=0x00) && (Uart[Ch].Buf[22]!=0x30) )
//											{
//												break;	//错误
//											}

											result = JudgeClockNoWeek( 1, &Uart[Ch].Buf[22], 5 );
											if( result != 0 ) break; //错误
																						
											Exchange( Buf, Uart[Ch].Buf+22, 5 );
											Temp08 = 0x01;
											break;
											
										case 0x02:  //整点冻结时间间隔	NN          1	分钟, 设置为30、60分钟，默认为60分钟。
											if( (Uart[Ch].Buf[22]!=0x30)&&(Uart[Ch].Buf[22]!=0x60) ) break;
											
											Buf[5] = Uart[Ch].Buf[22];
											Temp08 = 0x01;
											break;
											
										case 0x03:  //日冻结时间      	hhmm        2	时分      
											result = JudgeClockNoWeek( 1, &Uart[Ch].Buf[22], 2 );
											if( result != 0 ) break; //错误
											
											Wr_Pretime_Dayfreeze();	//写上一次日冻结时间
											
											Buf[6] = Uart[Ch].Buf[23];
											Buf[7] = Uart[Ch].Buf[22];
											Temp08 = 0x01;
											break;
											
										default:
											Err_DI = 1;	//数据标识错
											break;
									}

									if( Temp08 )
									{
										result = I2C_Write_Eeprom44( ee_freezeparameter_page, Buf, ee_freezeparameter_lenth );
										if( result != 0x00 ) break;
										
										k = 1;
									}
									break;
									
//								case 0x13:	
//									switch( DI07.B08[0] )
//									{
//										case 0x01:	//无线通信在线及信号强弱指示	NN	1	
//											break;
//											
//										default:
//											break;
//									}
//									break;
									
								case 0x14:
									switch( DI07.B08[0] )
									{
										case 0x01:	//跳闸延时时间（NNNN 为跳闸前告警时间）	NNNN	2
										case 0x02:	//继电器拉闸控制电流门限值	XXX.XXX	3
										case 0x03:	//继电器延时拉闸时间	XXXX	2
										case 0x04:	//红外认证时效	XXXX	1
										case 0x05:	//上报复位时间	XXXX	1
											if( DI07.B08[0] == 0x02 )
											{
												Temp16.B08[0] = 0;	//偏移地址0
												Temp16.B08[1] = 3;	//字节数
											}
											else if( DI07.B08[0] >= 0x04 )
											{
												Temp16.B08[0] = DI07.B08[0]+0x03;//偏移地址7,8
												Temp16.B08[1] = 1;
												if( Uart[Ch].Buf[22] == 0 ) break;
											}
											else
											{
												Temp16.B08[0] = DI07.B08[0]+2;//偏移地址3,5	
												Temp16.B08[1] = 2;	
//												if( (DI07.B08[0] == 0x03) && (Uart[Ch].Buf[22] == 0) && (Uart[Ch].Buf[23] == 0) ) break;
											}
											
											if( IsMultiBcd( Uart[Ch].Buf+22, Temp16.B08[1] ) ) break;
											
											result = UpdateSysPara( 0x80+17+Temp16.B08[0], Uart[Ch].Buf+22, Temp16.B08[1] );
											if( result != 0x00 ) break;						
											
											if( Opendelaytimer.B32 && (DI07.B08[0]==0x01) ) Opendelaytimer.B32 = Calculate_Timer(0)+2;
//											if( Relay_Mode != 0x55 )//内置继电器
											{
												if( CurrentTimer.B32 && (DI07.B08[0]==0x03) ) CurrentTimer.B32 = Calculate_Timer(1);
											}
											if( IR_EnTimer && (DI07.B08[0]==0x04) ) IR_EnTimer = Calculate_Timer(2);
											if( Reset_Rep_Timer && (DI07.B08[0]==0x05) ) Reset_Rep_Timer = Calculate_Timer(3);
											
											k = 1;
											break;											
											
										case 0xee:	//继电器合闸允许方式	XX	1
											result = UpdateSysPara( 0x80+26, Uart[Ch].Buf+22, 1 );
											if( result != 0x00 ) break;
											
										case 0xf0:	//PLCEVTOUT脚状态  1	BCD	
											if( (Uart[Ch].Buf[22]!=0x55) && (Uart[Ch].Buf[22]!=0xaa) ) break;
											
											F_Plcevtout_Sta = Uart[Ch].Buf[22];//0x55置高（输出开漏）,0xaa置低（输出低）
											
											k = 1;
											break;
										
										case 0xF1:	//停电事件电压触发定值	1	BCD
										case 0xF2:	//停电事件电流触发定值	1	BCD
										case 0xF3:	//停电事件判定延时时间定值	1	BCD
										case 0xF4:	//开表盖事件判定延时时间定值	1	BCD
										case 0xF5:	//时钟电池欠压事件触发下限定值	1	BCD
										case 0xF6:	//时钟电池欠压事件判定延时时间定值	1	BCD
											if( IsBCD(Uart[Ch].Buf[22]) || (Uart[Ch].Buf[22]==0x00) ) break;
											
											result = UpdateSysPara( 0xA0+(DI07.B08[0]-0xF1), Uart[Ch].Buf+22, 1 );
											if( result != 0x00 ) break;				
											
											k = 1;
											break;
										
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;

								default:
									Err_DI = 1;	//数据标识错
									break;
							}
							break;
							
						case 0x07:	//当前套阶梯时区表，备用套阶梯时区表
							if( !BDMODE ) { Err_DI = 1;	break; }
						case 0x01:	//第一套时区表和日时段表数据
						case 0x02:	//第二套时区表和日时段表数据
							//时区表内时区数1~14个，可设，时区数不足14个时，用最后一个补足。
							//日时段表的个数是1~8个，可设，没有补足功能。
							//日时段表内时段数1~14个，不可设，
							//下发设置时，不判断设定的时区数和时段数，只管其中包含了多少个。
							//年时区数（1）＋日时段表数（1）＋日时段数 （1）＋费率数（1）＋公共假日数（1）+谐波分析次数（1）+阶梯数（1）＋密钥数（1）＋crc（2）
							if( (DI07.B08[1]!=0x00) || (DI07.B08[0]>MAXSIDUANTB) || 
							   ((DI07.B08[2]==0x07) && (DI07.B08[0]>0x01)) )//阶梯时区表当前和备用套均可以设置
							{
								Err_DI = 1;	//数据标识错
								break;
							}

							if( DI07.B08[2] == 0x07 ) DI07.B08[0] += 0x09;  //便于软件区分数据类型
							if( DI07.B08[0] == 0x00 )	//第一套时区表数据, 第1~14时区起始日期及日时段表号(14*3), MMDDNN
							{
								Temp32.B08[0] = 3;	//时间起始位置
								Temp32.B08[1] = MAXSIDUANTB;	//日时段表表号限值
								Temp32.B08[2] = MAXREGION;	//最大时区数目
								Temp32.B08[3] = ee_shiqu_lenth;
							}
							else if( DI07.B08[0] <= 0x08 )	//最多可设置8个日时段表, 每个日时段表固定为14个时段
							{
								Temp32.B08[0] = 1;	//时间起始位置
								Temp32.B08[1] = 4;	//费率号限值
								Temp32.B08[2] = MAXSIDUAN;	//最大日时段数目
								Temp32.B08[3] = ee_shiduantable_lenth;
							}
							else
							{
								Temp32.B08[0] = 3;	//时间起始位置
								Temp32.B08[1] = 4;	//阶梯表张数限值
								Temp32.B08[2] = 4;	//最大阶梯时区数目
								Temp32.B08[3] = ee_jtshiqu_len;
							}
						
							//时区时间顺序判断: 要求按照时间升序排列
							Temp08 = (Uart[Ch].Buf[9]-12)/3;	//
							if( ((Uart[Ch].Buf[9]-12)%3) || (Temp08==0) ) break; //数据长度错误, 退出 
								
							if( Temp08 > Temp32.B08[2] )
							{
								k = 2;
								OutBuf[0] = 0xFF;
								if( DI07.B08[0]==0 ) OutBuf[1] = 0x10;//时区数超(Bit4)
								else if( DI07.B08[0] <= 0x08 ) OutBuf[1] = 0x20;//时段数超(Bit5)
								else OutBuf[1] = 0x01;
								break;
							}							
							
							if( Auth == KEYTYPE2 )
							{
								Inverse( Uart[Ch].Buf+22, (Uart[Ch].Buf[9]-12) );
								
								for( i=0; i<Temp08; i++ )
								{
									Inverse( Uart[Ch].Buf+22+3*i, 3 );
								}
							}

							memcpy( Buf, Uart[Ch].Buf+22, Uart[Ch].Buf[9]-12 );
							Sequence( Buf, Temp08 );

							//时间顺序判断: 要求按照时间升序排列
							n = Temp08*3;	//为了判断日时段15分钟间隔方便, 将"第1组+固定值"存入"最后1组"的后面。
							Buf[n+0] = Buf[0];	//日时段表号 or 费率号
							Buf[n+1] = Buf[1];	//日 or 分
							Buf[n+2] = Hex_To_Bcd(Bcd_To_Hex(Buf[2])+24);	//月 or 时
							
							for( i=0, n=0; i<Temp08; i++, n+=3 ) //时间是升序或者一直相同
							{
								if( (Buf[0+n]==0) || (Buf[0+n]>Temp32.B08[1]) )	//日时段表号or费率号是否合法
								{
									if( DI07.B08[0] && (DI07.B08[0]<=0x08) )
									{
										k = 2;
										OutBuf[0] = 0xFF;
										OutBuf[1] = 0x40;//费率数超(Bit6)
									}
									break;
								}
							
								result = JudgeClockNoWeek( Temp32.B08[0], Buf+1+n, 2 );	//minute+hour
								if( result != 0 ) break; //错误
									
								result = Cmp_Bcd_Inv( Buf+1+n, Buf+4+n, 2 );
								if( result == 0x00 )	//A == B
								{
									if( Buf[3+n] != Buf[0+n] ) break;
								}
								else if( result == 0x02 ) //A < B
								{
									if( DI07.B08[0] && (DI07.B08[0]<=0x08))	//日时段表中最小时段间隔不应小于15分钟
									{
										if( (CalcAbsMin(Buf+4+n)-CalcAbsMin(Buf+1+n)) < 15 ) break;
									}
								}
								else //A > B
								{
									break;	//数据有误
								}
							}
							
							if( i < Temp08 )	break;	//数据有误

							for( i=(Temp08); i<Temp32.B08[2]; i++ )	//数据不足, 补最后一组数据
							{
								memcpy( Uart[Ch].Buf+22+3*i, Uart[Ch].Buf+22+3*(Temp08-1), 3 );
							}

							if( DI07.B08[0] <= 0x08 ) Temp16.B16 = ShiquRecord( DI07.B08, OptID.B08 );	//事件记录
							else Temp16.B16 = ee_jtshiqu1_page+DI07.B08[0]-0x09;
			
							I2C_Write_Eeprom( Temp16.B16, 0x00, Uart[Ch].Buf+22, Temp32.B08[3] );	//编程后 时区表or时段第i表(42)
			
							if( DI07.B08[2] == 0x07 ) DI07.B08[0] -= 0x09;  //便于软件区分数据类型
							
							run_timezone_flag = 1;
							Tariff.Crc16.B16 += 1;
							RefreshPrice();

							k = 1;												
							break;

						case 0x03:	//第n 公共假日日期及日时段表号
							switch( DI07.B08[1] ) 
							{
								case 0x00:
									if( (DI07.B08[0]>=1) && (DI07.B08[0]<=MAXHOLIDAY) )	//公共假日：1~14, YYMMDDNN, 4
									{
										if( (Uart[Ch].Buf[22]==0) || (Uart[Ch].Buf[22]>MAXSIDUANTB) ) break;	//日时段表号: 1~8

										result = Judge_Clock( 4, &Uart[Ch].Buf[23], 3 ); //判断数据是否合法
										if( result != 0 ) break;
										
										Temp16.B08[0] = (DI07.B08[0]-1)/15;
										Temp16.B08[1] = ((DI07.B08[0]-1)%15)*4;
										
										result = I2C_Write_Eeprom( ee_holiday_page+Temp16.B08[0], Temp16.B08[1], Uart[Ch].Buf+22, 4 );
										if( result != 0x00 ) break;
										
										run_timezone_flag = 1;
										Tariff.Crc16.B16 += 1;
										
										k = 1;
									}
									else
									{
										Err_DI = 1;	//数据标识错
									}
									break;
									
								default:
									Err_DI = 1;	//数据标识错
									break;
							}
							break;

						case 0x04:
							switch( DI07.B08[1] )
							{
								case 0x01:	//自动循显
								case 0x02:	//按键循显
									//循显屏数(1)＋循显时间(1)＋电能小数位数(1)＋功率位数(1)＋键显屏数(1)＋波特率(1)+ 跳闸延时时间(2)＋crc(2)
									if( Auth == KEYTYPE2 )
									{
										Temp08 = Uart[Ch].Buf[22];
										memcpy( Uart[Ch].Buf+22, Uart[Ch].Buf+23, 4);
										Uart[Ch].Buf[26] = Temp08;
									}
														
									if( (DI07.B08[0]) && (DI07.B08[0]<=SCREENNUM) )
									{
										result = GetLcdNo( Uart[Ch].Buf+22 );
										if( result == 0x00 ) break;	//显示项目错误 or 显示序号错
																				
										n = 2*(DI07.B08[1]-1)+((DI07.B08[0]-1)/62);
										Temp08 = ((DI07.B08[0]-1)%62);
										
										Item[n].Display[Temp08] = result;
										Item[n].Crc16.B16 = Crc_16Bits( Item[n].Display, ee_displayitem_lenth );
										result = I2C_Write_Eeprom( ee_loop_displayitem_page+n, 0x00, Item[n].Display, ee_displayitem_lenth );
										if( result != 0x00 )
										{
											break;
										}
										
										k = 1;
									}
									else
									{
										Err_DI = 1;	//数据标识错
									}
									break;
								
								default:
									Err_DI = 1;	//数据标识错
									break;
							}
							break;
							
						case 0x09:
							switch( DI07.B08[1] )
							{
								case 0x08:
									if( DI07.B08[0]==1 )//过流事件电流触发下限	NNN.N	2	A
									{		
										if( IsMultiBcd( Uart[Ch].Buf+22, 2) )	break;
										result = UpdateSysPara( 0x60+15, Uart[Ch].Buf+22, 2 );
										
										
										
									}
									else if( DI07.B08[0]==2 )//过流事件判定延时时间	NN	1	S
									{
											
										if( IsMultiBcd( Uart[Ch].Buf+22, 1) || (Uart[Ch].Buf[22] < 10) ) break;
										result = UpdateSysPara( 0x60+17, Uart[Ch].Buf+22, 1 );
									}
									else
									{
										Err_DI = 1;	//数据标识错
										break;
									}
										
									if( result != 0x00 ) break;
											
									k = 1;
									break;

								default:
									Err_DI = 1;	//数据标识错
									break;
							}
							break;	
							
						case 0x80:    // :p		
							switch( DI07.B08[1] )
							{
								case 0x00:
									switch( DI07.B08[0] )
									{
										case 0x01:	//厂家软件版本号(ASCII 码)	NN…NN	32
										case 0x02:	//厂家硬件版本号(ASCII 码)	NN…NN	32
										case 0x03:	//厂家编码(ASCII 码)		NN…NN	32
										case 0x04:	//软件备案号			NN…NN	8
											if( Auth != 3 )
											{
												k = 2;
												OutBuf[0] = 0xFF; //结果
												OutBuf[1] = 0x04; //Error: bit4密码错误/未授权
												break;
											}
											Temp08 = 32;//32字节
											if(DI07.B08[0] == 0x01)
											{
												Temp16.B08[0] = ee_softver_page;	//软件备案号
												Temp16.B08[1] = ee_softver_inpage;
											}
											else if(DI07.B08[0] == 0x02)
											{
												Temp16.B08[0] = ee_hardver_page;	//硬件备案号
												Temp16.B08[1] = ee_hardver_inpage;
											}
											else if(DI07.B08[0] == 0x03)
											{
												Temp16.B08[0] = ee_factorynum_page;	//厂家编号
												Temp16.B08[1] = ee_factorynum_inpage;
											}
											else
											{
												Temp16.B08[0] = ee_softbk_page; 	//软件备案号
												Temp16.B08[1] = ee_softbk_inpage;
												Temp08 = 8;//8字节
											}
											if( Uart[Ch].Buf[9] != (Temp08+12) ) break;//数据长度错误
											I2C_Write_Eeprom( Temp16.B08[0], Temp16.B08[1], Uart[Ch].Buf+22, Temp08 );
											k = 1;
											break;
																						
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
									break;
									
								default:
									Err_DI = 1;	//数据标识错
									break;
							}
							break;		
							
						default:
							Err_DI = 1;	//数据标识错
							break;
					}
					break;
				}

				case KEYTYPE1:	//通过明文+MAC的方式进行数据传输，不需要进行密码验证，也不需要编程键配合使用。
					if( (DI07.B32 != 0x02040004) && 
					   (((BDMODE)&&(DI07.B32 == 0x0E040004)) || 
					    ((!BDMODE)&&(DI07.B32 != 0x0E040004))) )//任何模式都支持表号设置, 客户编号区分模式
					{
						Err_DI = 1;	//Error: bit2密码错误/未授权
						break;
					}

					switch( DI07.B08[3] )
					{
						case 0x04:
							switch( DI07.B08[2] )
							{
								case 0x00:
									Temp32.B08[0] = 0x00;
									Temp32.B08[3] = 0x81;	//写入ESAM的文件号
									
									switch( DI07.B08[1])
									{
										case 0x01:
											switch( DI07.B08[0] )
											{
												case 0x08:	//两套费率电价切换时间			YYMMDDhhmm	5	年月日时分
													//两套时区表切换时间（5）＋两套日时段表切换时间（5）＋两套费率电价切换时间（5）＋两套阶梯切换时间（5）＋crc（2）
													result = JudgeSwitch( Uart[Ch].Buf+22, 0 );
													if( result==1 ) break;
													
													Temp32.B08[0] = 5;	//数据长度
													Temp32.B08[1] = 10;	//82文件中偏移量
													Temp32.B08[2] = 0xC0+10;	//E2中页内偏移量
													break;

												case 0x09:	//两套阶梯切换时间		YYMMDDhhmm	5	年月日时分
													//两套时区表切换时间（5）＋两套日时段表切换时间（5）＋两套费率电价切换时间（5）＋两套阶梯切换时间（5）＋crc（2）
													result = JudgeSwitch( Uart[Ch].Buf+22, 0 );
													if( result==1 ) break;

													Temp32.B08[0] = 5;	//数据长度
													Temp32.B08[1] = 192;//84文件中偏移量
													Temp32.B08[2] = 0xC0+15;	//E2中页内偏移量
													Temp32.B08[3] = 0x84;	//写入ESAM的文件号
													break;
												
												default:
													Err_DI = 1;	//数据标识错
													break;
											}
											break;

										//报警金额1限值(4)＋报警金额2限值(4)+互感器变比(6)＋表号(6)+客户编号(6)+电卡类型(1)+身份认证时效(2)
										case 0x03:
											switch( DI07.B08[0] )
											{
												case 0x06:	//电流互感器变比              	NNNNNN	3       break;
													Temp32.B08[0] = 3;	//数据长度
													Temp32.B08[1] = 24;	//82文件中偏移量
													Temp32.B08[2] = 0x40+8;	//E2中页内偏移量
													break;
																										
												case 0x07:	//电压互感器变比              	NNNNNN	3	    break;
													Temp32.B08[0] = 3;	//数据长度
													Temp32.B08[1] = 27;	//82文件中偏移量
													Temp32.B08[2] = 0x40+11;	//E2中页内偏移量
													break;
												
												default:
													Err_DI = 1;	//数据标识错
													break;
											}
											break;
		
										case 0x04:
											switch( DI07.B08[0] )
											{
												case 0x02:	//表号		NNNNNNNNNNNN	6
													if( Key_Status!=0 ){Err_DI = 1;break;}//公钥允许设置

													Temp32.B08[0] = 6;	//数据长度
													Temp32.B08[1] = 30;	//82文件中偏移量
													Temp32.B08[2] = 0x40+14;	//E2中页内偏移量
													break;
																										
												case 0x0E:	//客户编号	NNNNNNNNNNNN	6
													if( Key_Status!=0 ){Err_DI = 1;break;}//公钥允许设置
													
													Temp32.B08[0] = 6;	//数据长度
													Temp32.B08[1] = 36;	//82文件中偏移量
													Temp32.B08[2] = 0x40+20;	//E2中页内偏移量
													break;
																							
												default:
													Err_DI = 1;	//数据标识错
													break;
											}
											break;
											
										case 0x10:
											switch( DI07.B08[0] )
											{
												case 0x01:  //报警金额1 限值	XXXXXX.XX	4	元
													Temp32.B08[0] = 4;	//数据长度
													Temp32.B08[1] = 16;	//82文件中偏移量
													Temp32.B08[2] = 0x40+0;	//E2中页内偏移量
													break;
													
												case 0x02:  //报警金额2 限值	XXXXXX.XX	4	元
													Temp32.B08[0] = 4;	//数据长度
													Temp32.B08[1] = 20;	//82文件中偏移量
													Temp32.B08[2] = 0x40+4;	//E2中页内偏移量
													break;
		
												default:
													Err_DI = 1;	//数据标识错
													break;
											}
											break;
											
										default:
											Err_DI = 1;	//数据标识错
											break;
									}
					
									if( Temp32.B08[0] )
									{
										if( IsMultiBcd( Uart[Ch].Buf+22, Temp32.B08[0] ) )	break;
	
//										result = Update_ESAM_Bin_Mac( Temp32.B08[3], Temp32.B08[1], Uart[Ch].Buf+22, Temp32.B08[0] );
//										if( result != 0x00 )	//当传输的数据MAC 校验错是采用错误信息字ERR 中bit2（密码错/未授权）置位，并按照异常应答帧响应。
//										{
//											k = 0x00; 
//											OutBuf[ k++ ] = 0xFF;
//											OutBuf[ k++ ] = 0x04;	//密码错/未授权
//											break;
//										}
										
										result = UpdateSysPara( Temp32.B08[2], Uart[Ch].Buf+22, Temp32.B08[0] ); //更新XRAM, E2PROM
										if( result ) break;
										
										run_price_flag = 1;

										k = 1;
									}
									break;
									
								case 0x05:	//设置备用套费率电价
									if( (DI07.B08[1] != 0x02) || (DI07.B08[0] != 0xFF) ) { Err_DI = 1;	break; } //数据标识错

									Temp16.B08[0] = (Uart[Ch].Buf[9]-16);
									Temp16.B08[1] = Temp16.B08[0]/4;
									if( (Temp16.B08[0]%4) || (Temp16.B08[1]==0) || (Temp16.B08[1]>MAXFLPRICE) ) break; //数据长度错误, 退出 
									if( IsMultiBcd( Uart[Ch].Buf+22, Temp16.B08[0] ) )	break;

									for( i=0; i<Temp16.B08[1]; i++ ) Inverse( Uart[Ch].Buf+22+4*i, 4 );
							
									memcpy( Buf, Uart[Ch].Buf+22, ee_tariffprice_lenth );	//存储正序的数据

//									Inverse( Uart[Ch].Buf+22, Temp16.B08[0] );	//为了符合MAC校验函数的格式
//									result = Update_ESAM_Bin_Mac( 0x84, 0x04, Uart[Ch].Buf+22, Temp16.B08[0] );
//									if( result != 0x00 )	//当传输的数据MAC 校验错是采用错误信息字ERR 中bit2（密码错/未授权）置位，并按照异常应答帧响应。
//									{
//										k = 0x00; 
//										OutBuf[ k++ ] = 0xFF;
//										OutBuf[ k++ ] = 0x04;	//密码错/未授权
//										break;
//									}
									
									FLRecord( OptID.B08 );	//费率编程记录

									//E2PROM中只保留前4项费率电价
									for( i=Temp16.B08[1]; i<ee_tariffprice_lenth/4; i++ )	//数据不足, 补最后一组数据
									{
										memcpy( Buf+4*i, Buf+4*(Temp16.B08[1]-1), 4 );
									}

									result = I2C_Write_Eeprom( ee_tariffprice2_page, 0x00, Buf, ee_tariffprice_lenth );	//正序格式
									if( result != 0x00 ) break;
							   
//									I2C_Read_Eeprom( ee_tariffprice2_page, 0x00, Buf, ee_tariffprice_lenth );
									run_price_flag = 1;
									
									k = 1;
									break;

								case 0x06:	//设置备用套阶梯值+阶梯电价+年结算日
									if( (DI07.B08[1] < 0x0a) || (DI07.B08[1] > 0x0b) || (DI07.B08[0] != 0xFF) ) { Err_DI = 1;	break; } //数据标识错

									if( Uart[Ch].Buf[9] != 70+12 ) break;	//6*4+7*4+4*3=64
									if( IsMultiBcd( Uart[Ch].Buf+22, 70 ) )	break;

									for( i=0; i<13; i++ ) Inverse( Uart[Ch].Buf+22+4*i, 4 );
									for( i=0; i< 6; i++ ) Inverse( Uart[Ch].Buf+74+3*i, 3 );
									
									for( i=1; i< 6; i++ ) //阶梯电量升序判断(杭州西力特殊要求)
									{
										if( Cmp_Data( Uart[Ch].Buf+22+4*(i-1), Uart[Ch].Buf+22+4*i, 4 ) == 1 ) break;  //1,A>B
									}
									if( i != 6 ) break;

									memcpy( Buf, Uart[Ch].Buf+22, 70 );	//存储正序的数据
									
////									if( DI07.B08[1] < 0x0c )
////									{
////										Inverse( Uart[Ch].Buf+22, 70 );	//为了符合MAC校验函数的格式
////										
//////										result = Update_ESAM_Bin_Mac( 0x84, 0x04+12*4+(DI07.B08[1]-0x0a)*70, Uart[Ch].Buf+22, 70 );
//////										if( result != 0x00 )	//当传输的数据MAC 校验错是采用错误信息字ERR 中bit2（密码错/未授权）置位，并按照异常应答帧响应。
//////										{
//////											k = 0x00; 
//////											OutBuf[ k++ ] = 0xFF;
//////											OutBuf[ k++ ] = 0x04;	//密码错/未授权
//////											break;
////									}

									JTRecord( OptID.B08 );	//阶梯编程记录
							
									for( i=0; i< 6; i++ ) Inverse( Buf+52+3*i, 3 );

									WriteE2WithJieti( DI07.B08[1]-0x06, Buf );
									
									RefreshPrice();
									
									k = 1;
									break;	
									
								default:
									Err_DI = 1;	//数据标识错
									break;
							}
							break;
							
						default:
							Err_DI = 1;	//数据标识错
							break;
					}
					break;
				
				default:
					Err_DI = 1;	//Error: bit2密码错误/未授权
					break;
			}

			if( Err_DI )	//用户对不支持写操作的数据项编程，返回异常应答帧，将错误信息字Bit2置1，即“密码错/未授权”。
			{
				k = 2;
				OutBuf[0] = 0xFF;	//Operation result.
				OutBuf[1] = 0x04;	//Error: bit2密码错误/未授权
				break;
			}
						
			if( k && (OutBuf[0] == 0x00) )
			{
				ProgramRecord( DI07.B08, OptID.B08 );
			}
			break;

		//=====================================================================================
		case 0x11:	//读取
			//……不做编程记录……
			
			if( (Uart[Ch].Buf[9] == 0x09) && (DI07.B32 == 0x00030404) )//液晶查看
			{
				if(progenable&&(Uart[Ch].Buf[14]==0xbb)&&(Uart[Ch].Buf[15]==0xbb)&&(Uart[Ch].Buf[16]==0xbb)&&(Uart[Ch].Buf[17]==0xbb)) result = 234;
				else if(progenable&&(Uart[Ch].Buf[14]==0xee)&&(Uart[Ch].Buf[15]==0xee)&&(Uart[Ch].Buf[16]==0xee)&&(Uart[Ch].Buf[17]==0xee)) result = 235;
				else if(progenable&&(Uart[Ch].Buf[14]==0xdd)&&(Uart[Ch].Buf[15]==0xdd)&&(Uart[Ch].Buf[16]==0xdd)&&(Uart[Ch].Buf[17]==0xdd)) result = 236;
				else
				{
					result = GetLcdNo( Uart[Ch].Buf+14 );
					if( result == 0x00 )	break;//显示项目错误 or 显示序号错
				}
				
				Ext_Disp_Cnt = result;
				Display.Timer = 10;
				Display.Status &= B1111_1110;	//进入循显状态
				Ic_Type_Reg = 0;//清插卡显示相关标志
				
				Display.Number  = 0xfc;//液晶查看显示
				Lcdled.B16 = 10;//点亮背光
				if( (Ext_Disp_Cnt == 234) || (Ext_Disp_Cnt == 235) || (Ext_Disp_Cnt == 236) )
				{
//					GPIO27CFG = B0100_0000;//输出
//					GPIO2 &= B0111_1111;//输出低电平点亮脉冲灯
				}
				if( Ext_Disp_Cnt == 200 )Init_Led_Timer = 10;//全显显示点亮led灯
				
				k = 6;
				memcpy( OutBuf+1, Uart[Ch].Buf+14, 5 );
				break;
			}
			
			if( Uart[Ch].Flag&B0000_0001 ) break; //通讯地址全99   不支持
			
			if( ((Uart[Ch].Buf[9] == 0x0A) || (Uart[Ch].Buf[9] == 0x05)) && (DI07.B08[3] == 0x06) )
			{
				if( (DI07.B08[2] == 0x10) && (DI07.B08[1]) && (DI07.B08[1] <= 0x08) )
				{
					if( Uart[Ch].Buf[9] != 0x0A ) break;
					if( IsMultiBcd( Uart[Ch].Buf+14, Uart[Ch].Buf[9]-0x04 ) ) break;
					if( JudgeClockNoWeek( 1, Uart[Ch].Buf+15, 5 ) )break;
				
					k = ReadLoadRec( Ch, Uart[Ch].Buf+8, OutBuf );
					if( k == 0 ) {RdLoadPara[Ch][4] = 0; k++;}
					else if(RdLoadPara[Ch][4]) Remain = 1;
					break;
				}
			}
			else if( (Uart[Ch].Buf[9] == 0x11)||(Uart[Ch].Buf[9] == 0x07) )//校表操作
			{
				if(progenable)
				{
					if( DI07.B08[0] != 0x01)
					{							
						Inverse( Uart[Ch].Buf+12, 3 );
						Inverse( Uart[Ch].Buf+16, 3 );
						Inverse( Uart[Ch].Buf+19, 4 );
						Uart[Ch].Buf[11] = 0;
						Uart[Ch].Buf[15] = 0;
					}
					Uart[Ch].Buf[10] = 0xff;
				  Comm_Write_Reg(&Uart[Ch].Buf[10]);
			
					Uart[Ch].Overtime_Timer = 2;	//无需应答字节返回
					return;
				}
			}
			else if( Uart[Ch].Buf[9] != 0x04 ) break;	//长度错误
			else if( DI07.B32 == 0x5e000002 )
			{
				k = 2;
				OutBuf[0] = 0x00;
				if( progenable ) OutBuf[1] = 0x55;	//0x55表示厂内状态,0x00表示厂外状态
				else OutBuf[1] = 0;
				break;
			}
					
			switch( DI07.B08[3] )
			{
				case 0x00:	//当前和结算日 电量数据
					switch( DI07.B08[2] )
					{
						case 0x00:	//组合
						case 0x01:	//正向
						case 0x02:	//反向
							//结算电量: 定时冻结时间(5)+正向有功总电能(5*4)+反向有功总电能(5*4)+定时冻结变量数据(3)
							result = Cal_Comb( DI07.B08[2], DI07.B08[1], DI07.B08[0], OutBuf+1 );
							if( (result == 0x00) || (result > 13*4) ) break;	//错误的数据标识

							k = 0x01+result;
							break;
						
						case 0x90:
							switch( DI07.B08[1] )
							{
								case 0x02:
									switch( DI07.B08[0] )
									{
										case 0x00:	//（当前）剩余金额
										case 0x01:	//（当前）透支金额
											if( !BDMODE ) break;
												
											if( DI07.B08[0] == 0x00 )
											{
												memcpy( OutBuf+1, Money.Remain+2, 4);
											}
											else
											{
												memcpy( OutBuf+1, Money.Tick+2, 4);
											}
											
											k = 5;
											break;
										
										default:
											break;
									}								
									break;
									
								default:
									break;
							}
							break;
						
						case 0x0B:	//当前/上1结算周期用电量
						case 0x0C:	//当前/上1月度用电量
							if( DI07.B08[0] > 0x01 ) break;
						case 0x0D:	//当前/上1~6年度用电量
							if( (DI07.B08[1] != 0x00) || (DI07.B08[0] > 0x06) ) break;
							if( (!BDMODE) && (DI07.B08[2] == 0x0D) ) break;

							Cal_Comb_Usedl( DI07.B08[2]-0x0B, DI07.B08[0], OutBuf+1 );
							
							if( DI07.B08[2] == 0x0D )
							{
								k = 9;
							}
							else
							{
								k = 5;
								memcpy( OutBuf+1, OutBuf+5, 4 );//无需返回结算时间
							}
							break;
							
						case 0xFE:	//结算时间
							if( (DI07.B08[1] != 0x00) || (DI07.B08[0] == 0) || ((DI07.B08[0]>0x0C)&&(DI07.B08[0]!=0xFF)) ) break;
							
							result = Cal_Comb( 0x03, 0x00, DI07.B08[0], OutBuf+1 );	//返回结算时间
							
							k = 0x01+result;
							break;
						
						default:
							break;
					}
					break;
				
				case 0x01:	//当前和结算日 最大需量及发生时间
					switch( DI07.B08[2] )
					{
						case 0x01:	//正向
						case 0x02:	//反向
							if( (DI07.B08[1]>4) && (DI07.B08[1]!=0xFF) ) break;
								
							if( DI07.B08[0] <= 12 )
							{
								Temp16.B08[0] = DI07.B08[0];
								Temp16.B08[1] = DI07.B08[0]+1;
							}
							else if( DI07.B08[0] == 0xFF )
							{
								if( DI07.B08[1] == 0xFF ) break;
						
								Temp16.B08[0] = 0;
								Temp16.B08[1] = 13;
							}
						
							k = 0x01;
							OutBuf[0] = 0x00;
						
							Temp08 = DI07.B08[0];
							for( n=Temp16.B08[0]; n<Temp16.B08[1]; n++ )
							{
								DI07.B08[0] = n;
						
								ReadMaxDemand( DI07.B08, Buf );
								
								memcpy( OutBuf+k, Buf+1, Buf[0] );
								k += Buf[0];
							}
							DI07.B08[0] = Temp08;

							break;

						default:
							break;
					}
					break;
				
				case 0x02:	//变量数据标识
					if( DI07.B08[2] == 0x80 )
					{
						if( DI07.B08[1] != 0x00 ) break;

						if( (!BDMODE) && ( (DI07.B08[0] == 0x0b) || (DI07.B08[0]==0x20) || (DI07.B08[0]==0x21))) break;	//远程模式不支持抄读本地参数
							
						switch( DI07.B08[0] )
						{
							case 0x01:	//零线电流	XXX.XXX	3	A
								Temp08 = 3;
								Temp32.B32 = Emu_Data.I2.B32;
								if((dl_back_flag) && (Emu_Data.I2.B32)) Temp32.B08[1] |= 0x80;
								break;
							
							case 0x02:	//电网频率	XX.XX	2	Hz
								Temp08 = 2;
								Temp32.B32 = Emu_Data.Freq.B32;
								break;
								
							case 0x03:	//一分钟有功总平均功率	XX.XXXX	3	kW
								Temp08 = 3;
								Temp32.B32 = Emu_Data.Pav.B32;
								break;
					    
							case 0x04:	//当前有功需量	XX.XXXX 3	kW
								Temp08 = 3;
								Temp32.B32 = Current_Demand.B32;
								if( dl_back_flag )	Temp32.B08[2] |= 0x80;
								
								break;
					
							case 0x07:	//表内温度	XXX.X	2	℃
								Temp08 = 2;
								Temp32.B32 = Fun_Para.Temper_M.B16;
								break;
								
							case 0x08:	//时钟电池电压(内部)	XX.XX	2	V
							case 0x09:
								Temp08 = 2;
								Temp32.B32 = Fun_Para.Voltage_B.B16;
								break;
								
//							case 0x09:	//停电抄表电池电压 (外部)	XX.XX	2	V
//								break;
								
							case 0x0A:	//内部电池工作时间	XXXXXXXX	4	分
								Temp08 = 4;
								Save_Keep_Timer(0);
								Hex2Bcd( Eeprom.Buffer+16, Temp32.B08, 4 );
								break;
							
							case 0x0B:	//当前阶梯电价	XXXX.XXXX	4	元/kWh
								Temp08 = 4;
								Temp32.B32 = Price.Current_L.B32;
							  Exchange( Temp32.B08,Price.Current_L.B08, 4 );
								break;
							
						
							case 0x0E:	//火线电流		XXX.XXX 3 A
								Temp08 = 3;
								Temp32.B32 = Emu_Data.I1.B32;
								if( dl_back_flag )	Temp32.B08[1] |= 0x80;
								break;
								
							case 0x0F:	//火线有功功率	XX.XXXX 3 kW
								Temp08 = 3;
								Temp32.B32 = Emu_Data.P1.B32;
								if( dl_back_flag )	Temp32.B08[1] |= 0x80;
								break;
								
							case 0x10:	//火线功率因数	X.XXX 2
								Temp08 = 2;
								Temp32.B32 = Emu_Data.Cosa.B32;
								if( dl_back_flag )	Temp32.B08[2] |= 0x80;
								break;
							
							case 0x20:	//当前电价
								Temp08 = 4;
								Temp32.B32 = Price.Current.B32;								
								break;
								
							case 0x21:	//当前费率电价
								Temp08 = 4;
								Temp32.B32 = Price.Current_F.B32;
							  Exchange( Temp32.B08,Price.Current_F.B08, 4 );
								break;
								
							case 0x22:	//身份认证时效剩余时间	XXXX 	2	分钟
							case 0x23:	//红外认证时效剩余时间	XXXX 	1  	分钟
								if( DI07.B08[0] == 0x22 )
								{
									Temp32.B32 = Identitytimer.B32;
									Temp08 = 2;
								}
								else
								{
									Temp32.B32 = IR_EnTimer;
									Temp08 = 1;
								}
								
								Temp32.B32 = (Temp32.B32+59)/60;
								Hex2Bcd( Temp32.B08, Temp32.B08, 4 );
								break;
								
							default:
								Temp08 = 0x00;
								break;
						}
						
						if( Temp08 )
						{	
							memcpy( OutBuf+1, Temp32.B08, 4 );
							k = 1+Temp08;
						}
					}
					else
					{
						if( DI07.B08[0] != 0x00 ) break;
						
						Temp08 = 0x00;
						switch( DI07.B08[2] )
						{
							case 0x01:	//A相电压	XXX.X	2
							case 0x02:	//A相电流	XXX.XXX	3
								if( (DI07.B08[1] == 0x01) || (DI07.B08[1] == 0xFF) )
								{
									if( DI07.B08[2] == 0x01 ) Temp08 = 2;
									else Temp08 = 3;
								}
								break;

							case 0x03:	//瞬时总有功功率	XX.XXXX	3
							case 0x05:	//瞬时总视在功率	XX.XXXX	3
							case 0x06:	//总功率因数		X.XXX	2
								if( (DI07.B08[1] <= 0x01) || (DI07.B08[1] == 0xFF) )
								{
									if( DI07.B08[2] == 0x06 ) Temp08 = 2;
									else Temp08 = 3;
								}
								break;
								
							default:
								break;
						}
						
						if( Temp08 )
						{
							memcpy( OutBuf+1, Emu_Data.U.B08+(DI07.B08[2]-1)*4, 4 );
							
							if( dl_back_flag && (
							(DI07.B08[2]==0x02) ||
							(DI07.B08[2]==0x03) ||
							(DI07.B08[2]==0x06) ) )	OutBuf[Temp08] |= 0x80;
							
							k = 1+(Temp08);
							if( DI07.B08[1] == 0xFF )
							{
								if(DI07.B08[2] >= 0x03 )
								{
									memcpy( OutBuf+k, OutBuf+1, (Temp08) );
									k += Temp08;
								}

								memset( OutBuf+k, 0xFF, 2*(Temp08) );
								k += 2*(Temp08);
							}
						}
					}
					break;
					
				case 0x03:	//事件记录数据标识				
					RdHisEvent( DI07.B08, 0, OutBuf );
					if( OutBuf[0] )
					{			
						if( (DI07.B08[1] == 0x02) && DI07.B08[0] ) Remain = 1;
						if( (DI07.B08[1] == 0x05) && DI07.B08[0] ) Remain = 1;
						if( (DI07.B08[1] == 0x10) && DI07.B08[0] ) Remain = 1;
						
						k = OutBuf[0]+1;
						OutBuf[0] = 0x00;
						break;
					}
					else if( DI07.B32 == 0x00143003 )//非法插卡次数
					{		
						k = 4;
						Get_Keep_Timer( 0, OutBuf );
						OutBuf[0] = 0x00;
						break;
					}
					else if( (DI07.B08[2] != 0x11) && (DI07.B08[2] != 0x33) ) break;
					
				case 0x19:	//过流记录
				case 0x1D:	//拉闸记录，跳闸记录
				case 0x1E:	//合闸记录
				case 0x22:	//电池欠压记录
					Rd_Relay_Buy_Rec( DI07.B08, OutBuf );
					if( OutBuf[0] == 0x00 ) break;
					
					k = OutBuf[0]+1;
					OutBuf[0] = 0x00;	//结果
					break;

				case 0x04:	//参变量数据标识
					switch( DI07.B08[2] )
					{
						case 0x00:	
							switch( DI07.B08[1] ) 
							{
								case 0x01:	
									switch( DI07.B08[0] )
									{
										case 0x01:	//日期及星期(其中0代表星期天)	YYMMDDWW  	4	年月日星期	
											k = 5;
											OutBuf[1] = Sclock.Clockmain[3];
											OutBuf[2] = Sclock.Clockmain[4];
											OutBuf[3] = Sclock.Clockmain[5];
											OutBuf[4] = Sclock.Clockmain[6];
											break;
											
										case 0x02:	//时间                          hhmmss    	3	时分秒      
											k = 4;
											OutBuf[1] = Sclock.Clockmain[0];
											OutBuf[2] = Sclock.Clockmain[1];
											OutBuf[3] = Sclock.Clockmain[2];
											break;
										
										case 0x03:	//最大需量周期                  NN        	1	分
											k = 2;
											OutBuf[0] = 0x00;
											OutBuf[1] = Hex_To_Bcd(Demand_Para.Max_Demand_Period);
											break;
											
										case 0x04:	//滑差时间                      NN        	1	分
											k = 2;
											OutBuf[0] = 0x00;
											OutBuf[1] = Demand_Para.Sliding_Time;
											break;										
											
										case 0x05:	//校表脉冲宽度                  XXXX      	2	毫秒        
											k = 3;
											OutBuf[1] = Hex_To_Bcd(Def_Plusevalidtimer*4);
											OutBuf[2] = 0x00;
											break;
											
										case 0x08:	//两套费率电价切换时间			YYMMDDhhmm	5	年月日时分, 明文+MAC
										case 0x09:	//两套阶梯切换时间			YYMMDDhhmm	5	年月日时分, 密文＋MAC
										case 0x0A:	//两套阶梯时区切换时间	YYMMDDhhmm 5
											if( !BDMODE ) break;
										case 0x06:	//两套时区表切换时间            YYMMDDhhmm	5	年月日时分, 密文＋MAC
										case 0x07:	//两套日时段切换时间            YYMMDDhhmm	5	年月日时分, 密文＋MAC
//										case 0x0B:	//两套费控模式切换时间	YYMMDDhhmm 5
											k = 6;
											Exchange( OutBuf+1, Starttime.Shiqu+5*(DI07.B08[0]-0x06), 5 );
											break;
											
										case 0x0C:	//日期、星期(其中0代表星期天)及时间	YYMMDDWWhhmmss	7 年月日星期时分秒
											k = 8;
											memcpy( OutBuf+1, Sclock.Clockmain, 7 );
											break;
											
										default:
											break;
									}										
									break;
									
								case 0x02:	  
									//年时区数（1）＋日时段表数（1）＋日时段数 （1）＋费率数（1）＋公共假日数（1）+谐波分析次数（1）+阶梯数（1）＋密钥数（1）＋crc（2）
									if( (DI07.B08[0]) && (DI07.B08[0]<=ee_feiknumber_lenth) )
									{
										if( (!BDMODE) && (DI07.B08[0] == 0x07) ) break;
											
										k = 2;
										OutBuf[1] = Feik.Num[DI07.B08[0]-1];
										if( DI07.B08[0] == 5 )	//公共假日数
										{
											OutBuf[2] = 0x00;
											k += 1;
										}
									}
									break;
									
								case 0x03:
									switch( DI07.B08[0] )
									{
										case 0x01:	//自动循环显示屏数            	NN      1   个	
										case 0x02:	//每屏显示时间					NN		1	秒	
										case 0x03:	//显示电能小数位数				NN		1	位	
										case 0x04:	//显示功率（最大需量）小数位数	NN      1   位  
										case 0x05:	//按键循环显示屏数            	NN      1   个  
										case 0x08:	//上电全显时间	NN	1	位
//											if( DI07.B08[0] == 0x08 ) Temp32.B08[2] = 0x05;
//											else Temp32.B08[2] = DI07.B08[0]-1;
											
											k = 2;
											OutBuf[1] = Hex_To_Bcd(*(&Display_Para.S_Number+DI07.B08[0]-1));
											break;
											
										case 0x06:	//电流互感器变比              	NNNNNN	3       break;
										case 0x07:	//电压互感器变比              	NNNNNN	3	    break;
											//报警金额1限值(4)＋报警金额2限值(4)+互感器变比(6)＋表号(6)+客户编号(6)+电卡类型(1)+身份认证时效(2)
											k = 4;
											Exchange( OutBuf+1, Esam_Para.Current_Ratio+3*(DI07.B08[0]-6), 3 );
											break;

										default:	
											break;
									}
									break;
									
								case 0x04:	  
									switch( DI07.B08[0] )
									{
										case 0x01:	//通信地址                     	NNNNNNNNNNNN	6
											k = 7;
											Exchange( OutBuf+1, Comm_Addr.Comm_Id, 6 );
											break;

										case 0x02:	//表号                         	NNNNNNNNNNNN	6
											k = 7;
											Exchange( OutBuf+1, Esam_Para.Meter_Id, 6 );
											break;
											
										case 0x04:	//额定电压（ASCII 码）         	XXXXXXXXXXXX	6          
											k = 7;
											Exchange( OutBuf+1, (uint08*)RatedVolt, 6 );
											break;	
											
										case 0x05:	//额定电流/基本电流（ASCII 码）XXXXXXXXXXXX	6
										case 0x06:	//最大电流（ASCII 码）         XXXXXXXXXXXX	6
										case 0x07:	//精度等级（ASCII 码）         XXXXXXXX   	4
										case 0x0B:	//电表型号（ASCII 码）         XX…XX      	10 
										case 0x0C:	//生产日期（ASCII 码）         XX…XX      	10
										case 0x0D:	//协议版本号（ASCII 码）        XX…XX      	16
										case 0x03:	//资产管理编码（ASCII 码）     NN…NN      	32	
										case 0x0F:	//电能表位置信息				11
											if(DI07.B08[0] == 0x05)
											{
												Code_Flash_Char = (uint08*)RatedCurr;
												Temp16.B08[0] = ee_ratedcurr_page;//额定电流
												Temp16.B08[1] = ee_ratedcurr_inpage;
												Temp08 = 6;//6字节
											}
											else if(DI07.B08[0] == 0x06)
											{
												Code_Flash_Char = (uint08*)MaxCurr;
												Temp16.B08[0]=ee_maxcurr_page;//最大电流
												Temp16.B08[1] = ee_maxcurr_inpage;
												Temp08 = 6;//6字节
											}
											else if(DI07.B08[0] == 0x07)
											{
												Code_Flash_Char = (uint08*)ActiveClass;
												Temp16.B08[0] = ee_activeclass_page; //有功精度等级
												Temp16.B08[1] = ee_activeclass_inpage;
												Temp08 = 4;
											}
											else if(DI07.B08[0] == 0x0B)
											{
												Code_Flash_Char = (uint08*)MaterType;
												Temp16.B08[0] = ee_matertype_page; //电表型号
												Temp16.B08[1] = ee_matertype_inpage;
												Temp08 = 10;
											}
											else if(DI07.B08[0] == 0x0c)
											{
												Code_Flash_Char = (uint08*)Facdate;
												Temp16.B08[0] = ee_meterdate_page;//生产日期
												Temp16.B08[1] = ee_meterdate_inpage;
												Temp08 = 10;
											}
											else if(DI07.B08[0] == 0x0D)
											{
												Code_Flash_Char = (uint08*)ProtocolVer;
												Temp16.B08[0] = ee_protocolver_page;//协议版本号
												Temp16.B08[1] = ee_protocolver_inpage;
												Temp08 = 16;
											}
											else if(DI07.B08[0] == 0x03) 
											{
												Temp16.B08[0] = ee_zichannum_page;//资产管理编号
												Temp16.B08[1] = ee_zichannum_inpage;
												Temp08 = 32;
											}
											else
											{
												Temp16.B08[0] = ee_meterpos_page;//电能表位置信息
												Temp16.B08[1] = ee_meterpos_inpage;
												Temp08 = 11;
											}
											result = I2C_Read_Eeprom( Temp16.B08[0], Temp16.B08[1], OutBuf+1, Temp08 );
																						
											for( i=0; i<Temp08; i++ )	//如果全部为0xff则采用默认的值
											{
												if( OutBuf[1+i] != 0xff ) break;
											}
											
											if( (result || (i==Temp08)) && (DI07.B08[0] != 0x03) && (DI07.B08[0] != 0x0f) )
											{
												Exchange( OutBuf+1, Code_Flash_Char, Temp08 );  
											}
											k = Temp08+1;
											break; 
										
//										case 0x08:	//无功准确度等级（ASCII 码）    XXXXXXXX    	4	            
//											break;    											          	

										case 0x09:	//电表有功常数                  XXXXXX      	3	imp/kWh
											k = 4;
											memcpy( OutBuf+1, Meter.Const.B08, 3 );//转换成低字节在前
											break;
											
//										case 0x0A:	//电表无功常数                  XXXXXX      	3   imp/kvarh   
//											k = 4;
//											OutBuf[1] = 0x00;
//											OutBuf[2] = 0x32;
//											OutBuf[3] = 0x00;
//											break;
											
										case 0x0E:	//客户编号						NNNNNNNNNN		6
											k = 7;
											Exchange( OutBuf+1, Esam_Para.User_Id, 6 );
											break;

										default:	break;                                      		
									}
									break;
									
								case 0x05:	  
									switch( DI07.B08[0] )
									{
										case 0xFF:	//电表运行状态字块	XXXX	14	
										case 0x01:	//电表运行状态字1   XXXX	2
											//[0].B16	电表运行状态字1					
											//bit15:	时钟故障
											//bit14:	透支状态
											//bit13:	存储器故障
											//bit12:	内部程序错误（预留）
											//bit09:	ESAM错
											//bit08:	控制回路错误
											//bit07:	reserved.
											//bit06:	费控模式状态  (0本地，1远程)
											//bit05:	无功功率方向	(0 正向、1 反向)
											//bit04:	有功功率方向	(0 正向、1 反向)
											//bit03:	停电抄表电池	(0 正常，1 欠压)
											//bit02:	时钟电池		(0 正常，1 欠压)    
											//bit01:	需量积算方式	(0 滑差，1 区间)
											//bit00:	reserved.
											Temp16.B16 = 0x00;
											
											if( clock_error )	//bit15:	时钟故障
											{
												Temp16.B08[0] |= B1000_0000;
											}
											
											if( remainder_tick_flag )	//bit14:	透支状态
											{
												Temp16.B08[0] |= B0100_0000;
											}

											if( eeprom_error_flag )	//bit13:	存储器故障
											{
												Temp16.B08[0] |= B0010_0000;
											}
											
											if( esam_error )	//bit09:	ESAM 状态		(0 正常，1 故障)
											{
												Temp16.B08[0] |= B0000_0010;
											}
											
											if( relay_error )	//bit08:	控制回路错误
											{
												Temp16.B08[0] |= B0000_0001;
											}
											
											if( !BDMODE )	//bit06:	费控模式状态
											{
												Temp16.B08[1] |= B0100_0000;
											}
																						
											if( dl_back_flag )			//有功功率方向(0 正向、1 反向)
											{
												Temp16.B08[1] |= B0001_0000;
											}

											if( low_voltage_flag || Bat_Delay_Timer )		//停电抄表电池(0 正常，1 欠压)+时钟电池(0 正常，1 欠压)
											{
												Temp16.B08[1] |= B0000_1100;
											}

											k = 0x00;
											OutBuf[ k++ ] = 0x00;
											OutBuf[ k++ ] = Temp16.B08[1];
											OutBuf[ k++ ] = Temp16.B08[0];

											if( DI07.B08[0] == 0x01 )
											{
												break;
											}
											
										case 0x02:	//电表运行状态字2	XXXX	2	
											Temp16.B16 = 0x00;
											if( dl_back_flag )	//bit0：A 相有功功率方向( 0 正向，1 反向 )
											{
												Temp16.B08[1] |= B0000_0001;
											}
											
											if( DI07.B08[0] == 0xFF )
											{
												OutBuf[ k++ ] = Temp16.B08[1];
												OutBuf[ k++ ] = Temp16.B08[0];
											}
											else
											{
												k = 3;
												OutBuf[1] = Temp16.B08[1];
												OutBuf[2] = Temp16.B08[0];
												break;
											}
											
										case 0x03:	//电表运行状态字3	XXXX	2	
											//[2].B16	电表运行状态字3(操作类)
											//bit15:	远程开户(0开户,1未开户)
											//bit14:	本地开户(0开户,1未开户)
											//bit13:	身份认证状态(0失效，1有效)
											//bit12:	保电状态	(0非保电,1保电)
											//bit11:	当前阶梯	(0当前套阶梯，1备用套阶梯)
											//bit10:	当前阶梯表(0第1张阶梯表，1第2张阶梯表)
											//bit9~8:	电能表类型, 		(00 非预付费表,01 电量型预付费表,10 电费型预付费表)	
											//bit7:		预跳闸报警状态		(0 无，1 有)
											//bit6:		继电器命令状态		(0 通，1 断)
											//bit5:		当前运行时区		(0 第一套，1 第二套)
											//bit4:		继电器状态			(0 通，1 断)
											//bit3:		编程允许			(0 禁止，1 许可)
											//bit2~1:	供电方式			(00 主电源，01 辅助电源，10 电池供电)
											//bit0:		当前运行时段		(0 第一套，1 第二套)
											Temp16.B16 = 0x00;
											
											Get_Usercard_Serial();//读卡序列号,同时判断是否开户
											if( comm_open_flag == 0 )	//bit15: 远程开户(0开户,1未开户)
											{
												Temp16.B08[0] |= B1000_0000;
											}

											if( card_open_flag == 0 )	//bit14: 本地开户(0开户,1未开户)
											{
												Temp16.B08[0] |= B0100_0000;
											}
											
											if( Comm_Auth_Judge() )	//bit13:	身份认证状态
											{
												Temp16.B08[0] |= B0010_0000;
											}
											
											if( relay_commu_close_flag || self_close_flag )	//bit12:	保电状态
											{
												Temp16.B08[0] |= B0001_0000;
											}
											
											if( BDMODE )
											{
												if( Tariff.CurLad_N )	
												{
													Temp16.B08[0] |= B0000_0100;	//bit10: 当前阶梯表 (0第1张阶梯表,1第2张阶梯表)
												}
												
												Temp16.B08[0] |= B0000_0010;	//bit9~8：电费型预付费表
											}

											if( remainder_alarm1_flag||remainder_alarm2_flag||remainder_zero_flag||relay_commu_alarm_flag)	//bit7：预跳闸报警状态
											{
												Temp16.B08[1] |= B1000_0000;
											}

											if( relay_commu_open_flag )		//bit6：继电器命令状态
											{
												Temp16.B08[1] |= B0100_0000;
											}

											if( Tariff.Status&B0000_1000 )		//bit5：当前运行时区
											{
												Temp16.B08[1] |= B0010_0000;
											}

											if( comm_relaystatus_flag )		//bit4：继电器状态
											{
												Temp16.B08[1] |= B0001_0000;
											}

											if( IR_EnTimer )			//bit3：红外认证/编程允许
											{
												Temp16.B08[1] |= B0000_1000;
											}

											if( Tariff.Status&B0000_0100 )		//bit0：当前运行时段
											{
												Temp16.B08[1] |= B0000_0001;
											}

											if( DI07.B08[0] == 0xFF )
											{
												OutBuf[ k++ ] = Temp16.B08[1];
												OutBuf[ k++ ] = Temp16.B08[0];
											}
											else
											{
												k = 3;
												OutBuf[1] = Temp16.B08[1];
												OutBuf[2] = Temp16.B08[0];
												break;
											}

										case 0x04:	//电表运行状态字4   XXXX	2	
											Temp16.B16 = 0x00;
											if( load_over_flag )	//bit4：A 相过流
											{
												Temp16.B08[1] |= B0001_0000;
											}
											
											if( DI07.B08[0] == 0xFF )
											{
												OutBuf[ k++ ] = Temp16.B08[1];
												OutBuf[ k++ ] = Temp16.B08[0];
											}
											else
											{
												k = 3;
												OutBuf[1] = Temp16.B08[1];
												OutBuf[2] = Temp16.B08[0];
												break;
											}			
											
										case 0x05:	//电表运行状态字5   XXXX	2	
										case 0x06:	//电表运行状态字6   XXXX	2	
											Temp16.B16 = 0x00;
											if( DI07.B08[0] == 0xFF )
											{
												for( i=0; i<2; i++ )
												{
													OutBuf[ k++ ] = 0x00;
													OutBuf[ k++ ] = 0x00;
												}
											}
											else
											{
												k = 3;
												OutBuf[1] = Temp16.B08[1];
												OutBuf[2] = Temp16.B08[0];
												break;
											}
											
										case 0x07:	//电表运行状态字7   XXXX	2	（合相故障状态）
											//bit10:	开表盖
											Temp16.B16 = 0x00;
											if( Cover.Status )	//bit9:	开表盖
											{
												Temp16.B08[0] |= B0000_0010;
											}
											if( below_60u_flag )	//bit5:	掉电
											{
												Temp16.B08[1] |= B0010_0000;
											}
											
											if( DI07.B08[0] == 0xFF )
											{
												OutBuf[ k++ ] = Temp16.B08[1];
												OutBuf[ k++ ] = Temp16.B08[0];
											}
											else
											{
												k = 3;
												OutBuf[1] = Temp16.B08[1];
												OutBuf[2] = Temp16.B08[0];
												break;
											}
											
										case 0x08:	//密钥状态字	XXXXXXX		4
											if( DI07.B08[0] == 0xFF )
											{
												GetKey( OutBuf+k );
												k += 4;
											}
											else
											{
												GetKey( OutBuf+1 );
												k = 5;
											}
											break;
											
										default:	
											break;
									}
									break;
									
								case 0x06:	  
									switch( DI07.B08[0] )
									{
										case 0x01:  //有功组合方式特征字  	NN	1	
											//波特率特征字（5）＋有功组合方式特征字1(1)＋冻结数据模式字(6)＋电表运行特征字1(1) +上报模式字(6)+crc（2）
											k = 2;
											OutBuf[1] = Mode.Comp_B;
											break;
											
//										case 0x02:  //无功组合方式1 特征字  NN	1	
//											break;
//											
//										case 0x03:	//无功组合方式2 特征字  NN	1	
//											break;
//											
										default:	
											break;
									}
									break;
									
								case 0x07:	  
									switch( DI07.B08[0] )
									{
										case 0x01:	//调制型红外光口波特率特征字	NN  1
//										case 0x02:	//接触式红外光口波特率特征字    NN  1											
										case 0x03:	//通信口1 波特率特征字          NN  1
//										case 0x04:	//通信口2 波特率特征字          NN  1
										case 0x05:	//通信口3 波特率特征字          NN  1
											//波特率特征字（5）＋有功组合方式特征字1(1)＋冻结数据模式字(6)＋电表运行特征字1(1) +上报模式字(6)+crc（2）
											k = 2;
											if( DI07.B08[0] == 3 ) OutBuf[1] = Mode.Bps[0];
											else if( DI07.B08[0] == 5 ) OutBuf[1] = Mode.Zbps[0];
											else OutBuf[1] = DI07.B08[0]+0x03;
											break;

										default:
											break;
									}
									break;
									
								case 0x08:	
									switch( DI07.B08[0] )
									{
										case 0x01:	//周休日特征字          	NN	1
											k = 2;
											OutBuf[1] = Week_Holiday.Status;
											break;
											
										case 0x02:	//周休日釆用的日时段表号	NN	1
											k = 2;
											OutBuf[1] = Week_Holiday.Shiduan_N;
											break;

										default:
											break;
									}
									break;

								case 0x0A:
									switch( DI07.B08[0] )
									{
										case 0x01:	//负荷记录起始时间		MMDDhhmm	4
											k = 5;
											Exchange( OutBuf+1, Week_Holiday.Lstarttimer+1, 4 );
											break;
												
										case 0x02:	//第1 类负荷记录间隔时间    NNNN	2
										case 0x03:	//第2 类负荷记录间隔时间    NNNN	2
										case 0x04:	//第3 类负荷记录间隔时间    NNNN	2
										case 0x05:	//第4 类负荷记录间隔时间    NNNN	2
											k = 3;
											Temp16.B16 = Week_Holiday.Lspacetimer[DI07.B08[0]-2];
											OutBuf[1] = Temp16.B08[1];
											OutBuf[2] = Temp16.B08[0];
											break;

										default:
											break;
									}
									break;
								case 0x09:	
									//波特率特征字（5）＋有功组合方式特征字1(1)＋冻结数据模式字(6)＋电表运行特征字1(1) +上报模式字(6)+crc（2）
									switch( DI07.B08[0] )
									{
										case 0x01:	//负荷记录模式字		NN	1								
										case 0x02:	//冻结数据模式字		NN	1	定时冻结数据模式字
										case 0x03:	//瞬时冻结数据模式字	NN	1
										case 0x04:	//约定冻结数据模式字    NN	1
										case 0x05:	//整点冻结数据模式字    NN	1
										case 0x06:	//日冻结数据模式字      NN	1
											k = 2;
											OutBuf[1] = Mode.Freeze[DI07.B08[0]-1];
											break;
											
										default:
											break;
									}
									break;
									
								case 0x0B:	
									switch( DI07.B08[0] )
									{
										case 0x01:	//每月第1 结算日	DDhh	2	日时
										case 0x02:	//每月第2 结算日    DDhh	2	日时
										case 0x03:	//每月第3 结算日    DDhh	2	日时
											ReadE2WithBackup( ee_histroyparameter_page, ee_histroyparameter_inpage, Buf, ee_histroyparameter_lenth );

											k = 3;
											OutBuf[1] = Buf[2*(DI07.B08[0]-1)+0];
											OutBuf[2] = Buf[2*(DI07.B08[0]-1)+1];
											break;

										default:
											break;
									}
									break;
								
								case 0x0C:	//0级密钥~9级密钥：02级密码，04级密码
//									if( DI07.B08[0] && (DI07.B08[0]<=0x0A) )
//									{
										k = 2;
										OutBuf[0] = 0xFF; //结果
										OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
//									}
									break;

								case 0x10:
									if(!BDMODE) break;
										
									switch( DI07.B08[0] )
									{
										case 0x01:	//报警金额1 限值	XXXXXX.XX	4	元
										case 0x02:  //报警金额2 限值	XXXXXX.XX	4	元
											k = 5;
											if( DI07.B08[0] == 0x01 )
											{
												Temp08 = 0;
											}
											else
											{
												Temp08 = 4;
											}

											Exchange( OutBuf+1, Esam_Para.Alarm_Limit1.B08+Temp08, 4 );
											break;
										
										case 0x03:	//透支金额限值    	NNNNNN.NN	4	元
										case 0x04:	//囤积金额限值    	NNNNNN.NN	4	元
										case 0x05:	//合闸允许金额限值  NNNNNN.NN   4	元
											//透支金额限值(4)＋囤积金额限值(4)＋合闸允许金额限值(4)＋Crc(2)
											k = 5;
											Exchange( OutBuf+1, Money_Para.Tick.B08+4*(DI07.B08[0]-0x03), 4 );
											break;	
											
										default:
											break;
									}
									break;
								
								case 0x11:
									switch( DI07.B08[0] )
									{
										case 0x01:	//电表运行特征字1	NN	1
											//波特率特征字（5）＋有功组合方式特征字1(1)＋冻结数据模式字(6)＋电表运行特征字1(1) +上报模式字(6)+crc（2）
											k = 2;
											OutBuf[1] = Mode.Feature1;
											break;
											
										case 0x04:	//主动上报模式字	NNNNNNNNNNNNNNNN	8
											//波特率特征字（5）＋有功组合方式特征字1(1)＋冻结数据模式字(5)＋电表运行特征字1(1)＋上报模式字(6) + crc（2）
											k = 9;
											memcpy( OutBuf+1, Mode.Report, 8 );
											break;
											
										default:
											break;
									}
									break;
								
								case 0x12:
									switch( DI07.B08[0] )
									{
										case 0x01:	//整点冻结起始时间	YYMMDDhhmm	5	年月日时分
											Temp16.B08[0] = 5;	//长度
											Temp16.B08[1] = 0;	//偏移量
											break;
											
										case 0x02:  //整点冻结时间间隔	NN          1	分钟      
											Temp16.B08[0] = 1;	//长度
											Temp16.B08[1] = 5;	//偏移量
											break;
											
										case 0x03:  //日冻结时间      	hhmm        2	时分      
											Temp16.B08[0] = 2;	//长度
											Temp16.B08[1] = 6;	//偏移量
											break;
											
										case 0x04:	//定时冻结时间	4	MMDDhhmm
											Temp16.B08[0] = 4;	//长度
											Temp16.B08[1] = 8;	//偏移量
											break;
																						
										default:
											Temp16.B08[0] = 0;	//长度
											break;
									}
									
									if( Temp16.B08[0] )
									{
										I2C_Read_Eeprom44( ee_freezeparameter_page, Buf, ee_freezeparameter_lenth );
										
										k = Temp16.B08[0]+1;
										Exchange( OutBuf+1, Buf+Temp16.B08[1], Temp16.B08[0] );
									}
									break;

//								case 0x13:	
//									switch( DI07.B08[0] )
//									{
//										case 0x01:	//无线通信在线及信号强弱指示	NN	1	
//											break;
//											
//										default:
//											break;
//									}
//									break;
									
								case 0x14:
									switch( DI07.B08[0] )
									{
										case 0x01:	//跳闸延时时间（NNNN 为跳闸前告警时间）	NNNN	2
										case 0x02:	//继电器拉闸控制电流门限值	XXX.XXX	3
										case 0x03:	//继电器延时拉闸时间	XXXX	2
										case 0x04:	//红外认证时效	XXXX	1
										case 0x05:	//上报复位时间	XXXX	1
											if( DI07.B08[0] == 0x02 )
											{
												Temp16.B08[0] = 0;	//偏移地址0
												Temp16.B08[1] = 3;	//字节数
											}
											else if( DI07.B08[0] >= 0x04 )
											{
												Temp16.B08[0] = DI07.B08[0]+0x03;//偏移地址7,8
												Temp16.B08[1] = 1;
											}
											else
											{
												Temp16.B08[0] = DI07.B08[0]+2;//偏移地址3,5	
												Temp16.B08[1] = 2;	
											}

											k = Temp16.B08[1]+1;
											Exchange( OutBuf+1, Mode.Limit_I+Temp16.B08[0], Temp16.B08[1] );	
											break;	
											
										case 0xee:	//继电器合闸允许方式	XX	1
											k = 2;
											OutBuf[1] = Mode.Wz_Clocerelay;
											break;
										
										case 0xf0:	//PLCEVTOUT脚状态  1	BCD	
											k = 2;
											OutBuf[1] = F_Plcevtout_Sta;
											break;		
											
										case 0xF1:	//停电事件电压触发定值	1	BCD
										case 0xF2:	//停电事件电流触发定值	1	BCD
										case 0xF3:	//停电事件判定延时时间定值	1	BCD
										case 0xF4:	//开表盖事件判定延时时间定值	1	BCD
										case 0xF5:	//时钟电池欠压事件触发下限定值	1	BCD
										case 0xF6:	//时钟电池欠压事件判定延时时间定值	1	BCD	
											k = 2;
											OutBuf[1] = Threshold[DI07.B08[0]-0xF1];
											break;	
											
										default:
											break;
									}
									break;
									
								case 0x15:
									switch( DI07.B08[0] )
									{
										case 0x01:	 //主动上报状态字	NNNNNNNNNNNNNNNNNNNNNNNN	12+N
											k = Ready_Reportdata( OutBuf );
											break;
											
										case 0x02:	 //插卡状态字	NNNN	2	
											k = 3;
											OutBuf[1] = InsertCardSta;
											OutBuf[2] = 0x00;
											InsertCardSta = 0;	//清插卡状态字为未知
											break;
											
										default:
											break;
									}
									break;

								default:
									break;
							}
							break;

						case 0x01:	//第一套时区表和日时段表数据
						case 0x02:	//第二套时区表和日时段表数据
							//年时区数（1）＋日时段表数（1）＋日时段数 （1）＋费率数（1）＋公共假日数（1）+谐波分析次数（1）+阶梯数（1）＋密钥数（1）＋crc（2）
							if( DI07.B08[2] == 0x01 )
							{
								Temp16.B08[0] = ee_shiqu1_page;
								Temp16.B08[1] = ee_shiduantable1_1_page;
							}
							else
							{
								Temp16.B08[0] = ee_shiqu2_page;
								Temp16.B08[1] = ee_shiduantable2_1_page;
							}

							switch( DI07.B08[1] )
							{
								case 0x00:
									switch( DI07.B08[0] )
									{
										case 0x00:	//第一套时区表数据, 第1~14时区起始日期及日时段表号(14*3), MMDDNN
											Temp32.B08[0] = Bcd_To_Hex(Feik.Num[0]);	//设定的"年时区数"
											if( Temp32.B08[0] > MAXREGION )
											{
												Temp32.B08[0] = MAXREGION;
											}

											I2C_Read_Eeprom( Temp16.B08[0], 0x00, Buf, ee_shiqu_lenth );
											
											k = Temp32.B08[0]*3+1;
											memcpy( OutBuf+1, Buf, k-1 );
											break;
											
										default:
//											Temp32.B08[0] = Bcd_To_Hex(Feik.Num[1]);	//设定的"日时段表数"
//											if( Temp32.B08[0] > MAXSIDUANTB )
//											{
//												Temp32.B08[0] = MAXSIDUANTB;
//											}
				
											Temp32.B08[1] = Bcd_To_Hex(Feik.Num[2]);	//设定的"日时段数"
											if( Temp32.B08[1] > MAXSIDUAN )
											{
												Temp32.B08[1] = MAXSIDUAN;
											}
				
											if( DI07.B08[0] <= MAXSIDUANTB )	//读取的日时段表号不能超过设定的"日时段表数"
											{
												I2C_Read_Eeprom( Temp16.B08[1]+(DI07.B08[0]-1), 0x00, Buf, ee_shiduantable_lenth );
	
												k = Temp32.B08[1]*3+1;
												memcpy( OutBuf+1, Buf, k-1 );
											}
											break;
									}
									break;
									
								default:
									break;
							}
							break;

						case 0x07:	//当前套阶梯时区表，备用套阶梯时区表
							if( !BDMODE ) break; //远程模式
								
							if( (DI07.B08[1]!=0x00) || (DI07.B08[0]>0x01) ) break; //阶梯时区表不能超过2个

							I2C_Read_Eeprom( ee_jtshiqu1_page+DI07.B08[0], ee_jtshiqu_inpage, OutBuf+1, ee_jtshiqu_len );
							
							k = 12+1;
							break;
							
						case 0x03:	//第n 公共假日日期及日时段表号
//							ReadE2WithBackup( ee_feik_number_page, ee_feiknumber_inpage, Buf, ee_feiknumber_lenth );

							switch( DI07.B08[1] ) 
							{
								case 0x00:
//									Temp32.B08[0] = Bcd_To_Hex(Buf[4]);	//设定的"日时段数"
//									if( Temp32.B08[0] > 14 )	//公共假日：1~14, YYMMDDNN, 4
//									{
//										Temp32.B08[0] = 14;
//									}
//		
//									if( (DI07.B08[0]) && (DI07.B08[0] <= Temp32.B08[0]) )	//读取的公共假日不能超过设定的"公共假日数"
									if( (DI07.B08[0]) && (DI07.B08[0] <= MAXHOLIDAY) )		//读取的公共假日不能超过设定的"公共假日数"
									{
										Temp16.B08[0] = (DI07.B08[0]-1)/15;
										Temp16.B08[1] = ((DI07.B08[0]-1)%15)*4;
										
										result = I2C_Read_Eeprom( ee_holiday_page+Temp16.B08[0], Temp16.B08[1], OutBuf+1, 4 );
										if( result != 0x00 ) break;
											
										k = 5;
									}
									break;
									
								default:
									break;
							}
							break;							
							
						case 0x04:
							switch( DI07.B08[1] )
							{
								case 0x01:	//自动循显
								case 0x02:	//按键循显
									//循显屏数(1)＋循显时间(1)＋电能小数位数(1)＋功率位数(1)＋键显屏数(1)＋波特率(1)+ 跳闸延时时间(2)＋crc(2)
									if( (DI07.B08[0]) && (DI07.B08[0]<=SCREENNUM) )
									{
										n = 2*(DI07.B08[1]-1)+((DI07.B08[0]-1)/62);
										Temp08 = ((DI07.B08[0]-1)%62);
										
										result = Item[n].Display[Temp08];
										
										//------ 组合 正向 反向 (当前，上1~上12结算日),序号从1开始 ------
										if( result && (result <= 195) )
										{
											k = 6;
											OutBuf[3] = (result-1)/65;	//组合,正向,反向
											OutBuf[1] = (result-1-65*OutBuf[3])/5;	//0:当前; 1~12结算
											OutBuf[2] = (result-1)%5;
											OutBuf[4] = 0x00;
											OutBuf[5] = 0x00;
											
										}
										//------ 其他显示项目，序号从200开始--------
										else if( result >= 200 )
										{
											result -= 200;
											
											Temp08 = disp_num;	//disp_table表中总计显示项目数

											if( result < Temp08 )
											{
												Temp32.B32 = disp_table[result];
												for( n=0; n<=result; n++ )
												{
													if( Temp32.B32 != disp_table[result-n] ) break;
												}
												
												if( result == 0 )
													result = 0xff;		//全屏显示
												else
													result = n-1;
												Inverse( Temp32.B08, 4 );
											}
//											else if( result < Temp08+14 )		//时段
//											{
//												Temp32.B32 = disp_table[Temp08-1];
//												result -= (Temp08-1);
//											}
											else
											{
												Temp32.B32 = 0;
												result = 0;
											}

											k = 6;
											memcpy( OutBuf+1, Temp32.B08, 4 );
											OutBuf[5] = result;
										}
									}
									break;
								
								default:
									break;
							}
							break;

						case 0x05:	//费率电价
						case 0x06:	//当前套第1张阶梯表~备用套第1张阶梯表
							if( !BDMODE ) break; //远程模式
								
							if( DI07.B08[2] == 0x05 )
							{
								if( (DI07.B08[1]!=0x01) && (DI07.B08[1]!=0x02) ) break;

								if( DI07.B08[0] && (DI07.B08[0]<=ee_tariffprice_lenth/4) )
								{
									Temp32.B08[1] = 1;
									Temp32.B08[2] = DI07.B08[0]-1;
								}
								else if( DI07.B08[0] == 0xFF )
								{
									Temp32.B08[1] = ee_tariffprice_lenth/4;
									Temp32.B08[2] = 0;
								}
								else break;

								I2C_Read_Eeprom( ee_tariffprice1_page+DI07.B08[1]-1, 0, Buf, ee_tariffprice_lenth );
	
								k = 1;
								for( i=0; i<Temp32.B08[1]; i++ )
								{
									Exchange( OutBuf+k, Buf+4*(Temp32.B08[2]+i), 4 );
									k += 4;
								}
							}
							else //第一套第1~n 阶梯值		NNNNNN.NN	4	kWh
							{
								if( ((DI07.B08[1] != 0x06)&&(DI07.B08[1] != 0x07)&&(DI07.B08[1] != 0x0a)&&(DI07.B08[1] != 0x0b)) || (DI07.B08[0]!=0xFF) ) break;

								k = 71;
								ReadE2WithJieti( DI07.B08[1]-0x06, OutBuf+1 );//读当前或者备用套阶梯电量/电价/年结算时间
								for( i=0; i<13; i++ ) Inverse( OutBuf+1+4*i, 4 );
//								for( i=0; i< 6; i++ ) Inverse( OutBuf+1+52+3*i, 3 );
								break;
							}
							break;

						case 0x09:
							switch( DI07.B08[1] )
							{
								case 0x08:
									switch( DI07.B08[0] )
									{
										case 0x01:	//过流事件电流触发下限	NNN.N	2	A
											k = 3;
											OutBuf[1] = Week_Holiday.Max_I.B08[1];
											OutBuf[2] = Week_Holiday.Max_I.B08[0];						
											break;
											
										case 0x02:	//过流事件判定延时时间	NN	1	S
											k = 2;
											OutBuf[1] = Week_Holiday.Max_I_Period;
											break;
											
										default:
											break;
									}
									break;
									
								default:
									break;
							}
							break;		

						case 0x80:
							switch( DI07.B08[1] )
							{
								case 0x00:
									switch( DI07.B08[0] )
									{
										case 0x01:	//厂家软件版本号(ASCII 码)	NN…NN	32
										case 0x02:	//厂家硬件版本号(ASCII 码)	NN…NN	32
										case 0x03:	//厂家编号(ASCII 码)		NN…NN	32
										case 0x04:	//软件备案号			NN…NN	8	
											Temp08 = 32;//32字节
											if(DI07.B08[0] == 0x01)
											{
												Temp16.B08[0] = ee_softver_page;	//软件备案号
												Temp16.B08[1] = ee_softver_inpage;
											}
											else if(DI07.B08[0] == 0x02)
											{
												Temp16.B08[0] = ee_hardver_page;	//硬件备案号
												Temp16.B08[1] = ee_hardver_inpage;
											}
											else if(DI07.B08[0] == 0x03)
											{
												Temp16.B08[0] = ee_factorynum_page;	//厂家编号
												Temp16.B08[1] = ee_factorynum_inpage;
											}
											else
											{
												Temp16.B08[0] = ee_softbk_page; 	//软件备案号
												Temp16.B08[1] = ee_softbk_inpage;
												Temp08 = 8;//8字节
											}
											result = I2C_Read_Eeprom( Temp16.B08[0], Temp16.B08[1], OutBuf+1, Temp08 );
																						
											for( i=0; i<Temp08; i++ )	//如果全部为0xff则采用默认的值
											{
												if( OutBuf[1+i] != 0xff ) break;
											}
											
											if( result || (i==Temp08) )
											{
												if( DI07.B08[0] == 0x04 ) Exchange( OutBuf+1, (uint08*)Softbk, 8 );	
												else  memset( OutBuf+1, 0x00, Temp08 );	//西力默认空
											}		  
											k = Temp08+1;
											break; 
												
										default:
											break;
									}
									break;
								
								default:
									break;
							}
							break;
						
						default: break;
					}
					break;
				
				case 0x05:	//冻结数据
//					if( DI07.B08[2] > 7 ) break;
						
					ReadFreezedRec( DI07.B08[2], DI07.B08[1], DI07.B08[0], OutBuf );
					if( OutBuf[0] == 0x00 ) break;

					k = OutBuf[0]+1;
					OutBuf[0] = 0x00;	//结果
					break;
			
				default: break;
			}

			if( k == 0x00 )
			{
				
				OutBuf[ k++ ] = 0xFF;
				OutBuf[ k++ ] = 0x02;	//错误信息字ERR：无请求数据
			}
			break;
				
		case 0x12:	//读取后续帧
			if( Uart[Ch].Flag&B0000_0001 ) break; //通讯地址全99   不支持
			if( Uart[Ch].Buf[9] != 0x05 ) break;	//Len error
			
			switch( DI07.B08[3] )
			{
				case 0x03:
					switch( DI07.B08[2] )
					{
						case 0x30:
							switch( DI07.B08[1] )
							{
								case 0x02:
								case 0x05:	//日时段表 编程记录									
//								case 0x08:	//节假日编程, 公共假日编程
								case 0x10:	//阶梯表编程记录
									if( (DI07.B08[0]) && (DI07.B08[0] <= 0x0A) && Uart[Ch].Buf[14] )
									{
										RdHisEvent( DI07.B08, Uart[Ch].Buf[14], OutBuf );
										if( OutBuf[0] == 0x00 ) break;

										if( (DI07.B08[1] == 0x05) && (Uart[Ch].Buf[14] != 3) )	Remain = 1;
//										if( (DI07.B08[1] == 0x08) && (Uart[Ch].Buf[14] != 5) )	Remain = 1;
										if( (DI07.B08[1] == 0x10) && (Uart[Ch].Buf[14] != 2) )	Remain = 1;

										k = OutBuf[0]+1;
										OutBuf[0] = 0x00;
									}
									break;
									
								default:
									break;
							}
							break;
							
						default:
							break;
					}
					break;
					
				case 0x06:	//负荷记录
					if( (DI07.B08[2] == 0x10) && (DI07.B08[1]) && (DI07.B08[1] <= 0x06) )
					{
						k = ReadLoadRec( Ch, Uart[Ch].Buf+8, OutBuf );
						if( k == 0 ) RdLoadPara[Ch][4] = 0;
						else if(RdLoadPara[Ch][4]) Remain = 1;
					}
					break;
					
				default:
					break;
			}
			
			if( (k ==0) && (Mode.Feature1&0x04) && ChkBufZero( Report_Data, 5 ) )
			{
				DI07.B32 = 0x01150004;
				
				//主动上报状态字	NNNNNNNNNNNNNNNNNNNNNNNN	12+N
				k = Ready_Reportdata( OutBuf );
				OutBuf[k++] = Uart[Ch].Buf[14];//返回帧序号
				memcpy( Uart[Ch].Buf+10, DI07.B08, 4 );//返回新的标识
			}
			
			if( k == 0x00 )
			{
				OutBuf[ k++ ] = 0xFF;
				OutBuf[ k++ ] = 0x02;	//错误信息字ERR：无请求数据
			}
			break;

		case 0x13:	//读通信地址, 地址域必须是全AAH
			//……不做编程记录……
			memset( Buf, 0xaa, 6 );
			if( (Cmp_Data( Buf, Uart[Ch].Buf+1, 6 ) !=3)	//非全aa	
				|| (Uart[Ch].Buf[9] != 0x00) )
			{
				Uart[Ch].Overtime_Timer = 2;
				return;
			}

			k = 7;
			Exchange( OutBuf+1, Comm_Addr.Comm_Id, 6 );
			break;

		case 0x15:	//写通信地址, 地址域必须是全AAH
			//……不做编程记录……
			if( ((Uart[Ch].Flag&B0000_0010) != B0000_0010)	//地址域必须是全AAH
				|| (!progenable) 							//编程按键无效,24hour编程禁止有效
				|| (Uart[Ch].Buf[9] != 0x06) )				//数据长度不合法
			{
				Uart[Ch].Overtime_Timer = 2;
				return;
			}
			
			Exchange( Buf, Uart[Ch].Buf+10, 6 );

			result = WriteE2WithBackup( ee_commaddr_page, ee_commaddr_inpage, Buf, ee_commaddr_lenth );	//写入E2PROM
			if( result )
			{
				Uart[Ch].Overtime_Timer = 2;
				return;
			}

			memcpy( Comm_Addr.Comm_Id, Buf, 8 );

			k = 1;
			break;

		case 0x08:	//广播校时
			//……不做编程记录……
			if( ((Uart[Ch].Flag&B0000_0010) == B0000_0010) || (Uart[Ch].Buf[9] != 0x06) ) //不支持索位
			{
				Uart[Ch].Overtime_Timer = 2;
				return;
			}

			result = JudgeClockNoWeek( 0, &Uart[Ch].Buf[10], 6 );
			if( result != 0 ) //
			{
				break; //错误
			}
			
			if( already_setclock_flag == 1 )
			{
				k = 2; 
				OutBuf[0] = 0xFF;
				OutBuf[1] = 0x04;	//Error: bit2密码错误/未授权
				break; 			//今日已校时, 不允许再校时
			}

			if( !clock_error )	//系统时钟正确
			{
				//----- 不允许在00:00:00的±5min内广播校时------
				Buf[0] = 0x00;
				Buf[1] = 0x05;
				Buf[2] = 0x00;
				Temp08 = Cmp_Bcd_Inv( Buf, Sclock.Clockbackup, 3 );
				if( Temp08 == 1 )
				{
					k = 2; 
					OutBuf[0] = 0xFF;
					OutBuf[1] = 0x04;	//Error: bit2密码错误/未授权
					break;	//当前时间小于00:05:00
				}
	
				Buf[0] = 0x00;
				Buf[1] = 0x55;
				Buf[2] = 0x23;
				Temp08 = Cmp_Bcd_Inv( Buf, Sclock.Clockbackup, 3 );
				if( Temp08 == 2 )
				{
					k = 2; 
					OutBuf[0] = 0xFF;
					OutBuf[1] = 0x04;	//Error: bit2密码错误/未授权
					break;	//当前时间大于23:55:00
				}
				
				//----- 不允许广播校时超±5min分钟------
				result = Judge_Min_Space( Uart[Ch].Buf+10 );
				if( result  == 1 )
				{
					k = 2; 
					OutBuf[0] = 0xFF;
					OutBuf[1] = 0x04;	//Error: bit2密码错误/未授权
					break;	//不允许广播校时超±5min
				}
				
				//------ 不允许在结算日的±5min内广播校时 -------
				ReadE2WithBackup( ee_histroyparameter_page, ee_histroyparameter_inpage, Buf, ee_histroyparameter_lenth );	//结算日时间: 日时
				for( i=0; i<9; i++ )
				{
					Buf[6] = 0x00; //结算时间
					Buf[7] = 0x00;
					if( i < 3 )
					{
						memcpy( Buf+8, Buf+2*i, 2 ); //Hour+Day
						Buf[10] = Sclock.Clockbackup[4];
					}
					else
					{
						if( !BDMODE ) continue; //远程模式
						memcpy( Buf+8, Price.Year_Ladder+3*(i-3), 3 );//Hour+Day+Month
					}
					Buf[11] = Sclock.Clockbackup[5];
					
					result = Judge_Min_Space( Buf+6 );
					if( result == 0 ) break;	//不允许在结算日的±5min内广播校时
				}

				if( result == 0 )
				{
					k = 2; 
					OutBuf[0] = 0xFF;
					OutBuf[1] = 0x04;	//Error: bit2密码错误/未授权
					break;
				}
			}

			memcpy( Buf+0, Uart[Ch].Buf+10, 3 );

			if( clock_error ) { Buf[3] = 0x00; }
			else { Buf[3] = Sclock.Clockbackup[6]; }

			memcpy( Buf+4, Uart[Ch].Buf+13, 3 );

			result = Set_Rtc( 0, Buf, 7 );
			if( result != 0x00 ) break; //设置时钟失败

			already_setclock_flag = 1; //

			CommSetRtc_Adj();	//设置时钟调整相关数据
			
			I2C_Read_Eeprom44( ee_wdt_page, Eeprom.Buffer,ee_wdt_lenth );//广播校时次数加1
			Eeprom.Buffer[1]++;
			if( Eeprom.Buffer[1] == 0 ) Eeprom.Buffer[0]++;
			I2C_Write_Eeprom44( ee_wdt_page, Eeprom.Buffer, ee_wdt_lenth );
			
			Init_Demand();	//当前需量清零
		
			k = 1;
			break;

		case 0x16:	//冻结命令
			//68+表号+68+16+04+mmhhDDMM+CS+16
			if( Uart[Ch].Buf[9] != 0x04 )	//数据域长度错误
			{
				Uart[Ch].Overtime_Timer = 2;
				return;
			}

			Temp08 = 0x00; //冻结时间中99H的个数
			for( n=0; n<4; n++ )
			{
				if( Uart[Ch].Buf[10+n] == 0x99 )
				{
					Temp08++;
				}
				else
				{
					if( Temp08 || JudgeClockNoWeek(1+n, &Uart[Ch].Buf[10+n], 1) ) //前一字节为99H或本字节有误, 则冻结时间错误
					{
						break;
					}
				}
			}
			
			if( n < 4 )
			{
				break;
			}

			switch( Temp08 )
			{
				case 0x00:	//mmhhDDMM, 普通冻结
				case 0x01:	//99DDhhmm表示以月为周期定时冻结
					if( Uart[Ch].Buf[12] > 0x28 )  break;		//只能是1~28日冻结
				case 0x02:	//9999hhmm表示以日为周期定时冻结
				case 0x03:	//999999mm表示以小时为周期定时冻结
					//整点冻结起始时间(5)＋整点冻结时间间隔(1)＋日冻结时间(2)+ 定时冻结时间(4)+ crc（2）
					I2C_Read_Eeprom44( ee_freezeparameter_page, Buf, ee_freezeparameter_lenth );

					Exchange( Buf+8, Uart[Ch].Buf+10, 4 );

					result = I2C_Write_Eeprom44( ee_freezeparameter_page, Buf, ee_freezeparameter_lenth );
					if( result != 0x00 ) break;

					k = 1;
					break;
					
				case 0x04:	//99999999为瞬时冻结
					//瞬时冻结时间(5)+瞬时冻结正向有功电能数据(5*4)+瞬时冻结反向有功电能数据(5*4)+瞬时冻结变量数据(3)
					Freeze_Dl( ee_realtimefreeze1_page, ee_immedf_inpage );

					k = 1;
					break;
					
				default:
					break;
			}
			break;
			
		case 0x17:	//更改通信速率, 波特率
			//……不做编程记录……

			//Bit7 Bit6     Bit5    Bit4    Bit3    Bit2    Bit1   Bit0
			//保留 19200bps 9600bps 4800bps 2400bps 1200bps 600bps 保留
			
			if( Uart[Ch].Flag&B0000_0011 )
			{
				k = 2; 
				OutBuf[0] = 0xFF;
				OutBuf[1] = 0x04;	//Error: bit2密码错误/未授权
				break;
			}
			
			if( Uart[Ch].Buf[9] != 0x01 ) //数据域长度不为1, 错误
			{
				Uart[Ch].Overtime_Timer = 2;
				return;
			}
			
			//0x01:调制型红外光口波特率特征字//0x03:通信口1 波特率特征字//0x05:通信口3 波特率特征字			
			Temp08 = Set_Comm_Rate( Ch*2+1, Uart[Ch].Buf[10] );
			if( Temp08 )
			{
				k = 2;
				OutBuf[0] = 0xFF;
				OutBuf[1] = 0x08;	//错误信息字ERR：通信速率不能更改(Bit3)
			}
			else
			{
				k = 2;
				OutBuf[1] = Uart[Ch].Buf[10];	//更改后波特率
			}
			break;

			
		case 0x19:	//最大需量清零
			//本命令必须与编程键配合使用
			if( Uart[Ch].Buf[9] != 0x08 ) //数据域长度错误
			{
				Uart[Ch].Overtime_Timer = 2;
				return;
			}

			if( ((progenable)&&(Uart[Ch].Buf[10] != 0x03))	//不是04级权限
			|| ((!progenable)&&(Veri_Psw( Uart[Ch].Buf+10 ) != 0x00) ))	//密码错误
			{
				k = 2;
				OutBuf[0] = 0xFF; //结果
				OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
				break;
			}

			//1. 需量清零 记录
			//清需总次数(3)+发生时刻(6)+操作者代码(4)+清需时正向有功总最大需量和发生时间(8)+清需时反向有功总最大需量和发生时间(8)
//			Temp08 = Read_Event_Point( ee_cleardemand_inpage );
//			I2C_Read_Eeprom( ee_clear_demand_page+Temp08, ee_clear_demand_inpage, Buf, ee_clear_demand_lenth );

			//存储各项数据时倒序，低字节在前，高字节在后
			Number_Add1( ee_cleardemand_inpage);		//总次数+1(3)
			memcpy( Buf, Sclock.Clockbackup, 6 );	//发生时刻(6)
			memcpy( Buf+6, Uart[Ch].Buf+14, 4 );	//操作者代码(4)
			I2C_Read_Eeprom( ee_p_demand_page, ee_p_demand_inpage, Buf+10, ee_pdemand_lenth );	//当前正向有功总最大需量和发生时间(8)
			I2C_Read_Eeprom( ee_n_demand_page, ee_n_demand_inpage, Buf+18, ee_ndemand_lenth );	//当前反向有功总最大需量和发生时间(8)
			
			Temp08 = Write_Event_Point( ee_cleardemand_inpage );
			I2C_Write_Eeprom( ee_clear_demand_page+Temp08, ee_clear_demand_inpage, Buf, ee_clear_demand_lenth );

			//2. 当前最大需量及发生时间数据清零
			Init_Demand();
			memset( Buf, 0x00, 64 );
			I2C_Write_Eeprom( ee_p_demand_page, ee_p_demand_inpage, Buf, ee_pdemand_lenth );
			I2C_Write_Eeprom( ee_n_demand_page, ee_n_demand_inpage, Buf, ee_ndemand_lenth );
			
			k = 1;
			OutBuf[0] = 0x00;
			break;
		
			
		case 0x1A:	//电表清零, 清空电能表内电能量、冻结量、事件记录、等数据, 电表清零必须记录清零时刻和清零前的电能量
//			if( BDMODE ) //本地模式, 测试模式
//			{
//				k = 2;
//				OutBuf[0] = 0xFF;	//Operation result.
//				OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
//				break;
//			}
		case 0x1B:	//事件清零, 清空电能表内存储的全部事件记录数据
			//本命令必须与编程键配合使用。命令执行时电能表应保证电表清零事件记录不被清除，并自动保存该事件的相应数据
			//……不做编程记录……
			if( !(Uart[Ch].Flag & B0000_0011) && progenable && (Uart[Ch].Buf[10] == 0x03) )//厂内模式允许直接清零
			{
				Ic_Comm_RxBuf[0] = Uart[Ch].Buf[8];
				Exchange( Ic_Comm_RxBuf+8, Uart[Ch].Buf+18, 4 );	//事件清零标识
			}
			else
			{
				if( (Uart[Ch].Flag & B0000_0011) || (Uart[Ch].Buf[10] != KEYTYPE2) || (identity_prog_flag == 0) || hangup_comm_flag )	//安全认证编程标志 失效
				{
					k = 2;
					OutBuf[0] = 0xFF; //结果
					OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
					break;	//国网电能表只支持98级远程控制
				}
				
				if( Uart[Ch].Buf[8] == 0x1A ) //电表清零
					result = Decrypt( Ch, 0x93, 0x08 );
				else
					result = Decrypt( Ch, 0x93, 0x0c );
				if( result )
				{
					k = 0x00; 
					OutBuf[ k++ ] = 0xFF;
					OutBuf[ k++ ] = 0x04;
					break;
				}
				
				if( Uart[Ch].Buf[8] != Ic_Comm_RxBuf[0] ) break;
			}

			k = 2;
			OutBuf[0] = 0xFF; //结果
			OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
			
			//验证通过后按照明文定义的指令执行相应操作
			if( Ic_Comm_RxBuf[0] == 0x1A )
			{
				MeterClear = 0x55aa;	//电表清零
			}
			if( Ic_Comm_RxBuf[0] == 0x1B )
			{
				Inverse( Ic_Comm_RxBuf+8, 4 );	//事件清零标识调序
				memcpy( DI07.B08, Ic_Comm_RxBuf+8, 4 );	//事件清零标识

				if( DI07.B08[0] != 0xFF ) break;
				
				if( DI07.B32 == 0xFFFFFFFF ) ClrRecTimes( 0, 0x55 );//清冻结和事件有效指针(除电量相关)
				else if( DI07.B32 == 0xFF143003 )//清非法插卡次数页
				{
					Clr_Otherdata(0);//清非法插卡次数
				}
				else
				{
					if( (DI07.B08[3] == 0x19) ||(DI07.B08[3] == 0x1D)||(DI07.B08[3] == 0x1E)||(DI07.B08[3] == 0x22)||
					   ((DI07.B08[3] == 0x03)&&((DI07.B08[2] == 0x11)||(DI07.B08[2] == 0x33))))	//拉合闸特殊处理
					{
						if( DI07.B08[3] == 0x22 )
						{
							if( DI07.B08[1] != 0x00 ) break;
						}
						else
						{
							if( DI07.B08[1] != 0xFF ) break;
						}
						
						i = Find_EventTable(DI07.B08+2);		//判断是否找到合法数据标识
					}
					else if( DI07.B08[3] == 0x03 )
					{
						i = Find_EventTable(DI07.B08+1);		//判断是否找到合法数据标识
					}
					else break;

					if( i != 24 )
					{				
						i += ee_clearn_inpage;
						if( i < ee_powerdownn_inpage ) break; 
						
						ClrRecTimes( i, 0x01 );
					}
					else
					{
						break;
					}
				}
					
				//事件清零记录
				//存储各项数据时倒序，低字节在前，高字节在后
				memcpy( Eeprom.Buffer+6, Uart[Ch].Buf+14, 4 );	//操作者代码(4)
				memcpy( Eeprom.Buffer+10, Ic_Comm_RxBuf+8, 4 ); 	//事件清零数据标识码(4)
				Event_Record( ee_cleareventn_inpage );
			}
			k = 1;
			OutBuf[0] = 0x00;
			break;

		case 0x1C:	//跳合闸、报警、保电	
			if( Uart[Ch].Buf[10] != KEYTYPE2 )	//判断是否为96级密码
			{
				if( (Uart[Ch].Flag & B0000_0011) || (Uart[Ch].Buf[9] != 16) || (Uart[Ch].Buf[10] != 0x03) ||
				    (!progenable && ( Veri_Psw( Uart[Ch].Buf+10 ))) )//厂内模式允许直接拉合闸或者明文合闸允许/直接合闸
				{
					k = 2;
					OutBuf[0] = 0xFF; //结果
					OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
					break;
				}
				else
				{
					memcpy( Ic_Comm_RxBuf,  Uart[Ch].Buf+18, 8 );//复制2字节数据和6字节有效截止时间
					result = JudgeEndClock();
					if(result)
					{
						k = 2;
						OutBuf[0] = 0xFF; //结果
						OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
						break;
					}
					Temp08 = Ch;
				}
			}
			else
			{
				if( (Uart[Ch].Flag & B0000_0011) || ((Ch == 0x01) && (progenable==0)&&(IR_EnTimer == 0)) || (identity_prog_flag == 0) || hangup_comm_flag )	//安全认证编程标志 失效
				{
					k = 2;
					OutBuf[0] = 0xFF; //结果
					OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
					break;	//国网电能表只支持98级远程控制
				}
                        	
				//P0～P2无效, 无效验证
				//将N1~Nm写入ESAM的控制命令文件进行解密, 解密成功，以解密后对应的明文所定义的命令执行相应的控制操作。
				//远程控制命令：80fb060010+密文；
				//获取远程控制命令的明文：00C00000＋LC（要获取的明文数据长度，为1 字节的十六进制数）；
				//68+表号+68+1C+1C+98000000+00000000+密文+cs+16
				//将密文写入ESAM 08文件解密: 04D6880014+密文
				//N1为控制命令类型，1AH代表跳闸，1BH代表合闸允许，2AH代表报警，2BH代表报警解除，3AH代表保电，3BH代表保电解除。
				//N2保留。
				//N3～N8代表命令有效截止时间，数据格式为ssmmhhDDMMYY。当电表当前时间已经超过这个时间时，该命令将被认为无效，不被执行
				
				Temp08 = Ch+0x20;
			}
			
			//验证通过后按照明文定义的指令执行相应操作
			result = Relay_Ctrl(Temp08);
			if(result)
			{
				k = 2;
				OutBuf[0] = 0xFF; //结果
				OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
			}
			else k = 1;
			
			break;
			
		case 0x1D:	//多功能端子输出控制命令, 无需编程键配合和密码验证, 电能表在使用过程中，发生掉电再上电情况时，不管端子当前输出何种信号均恢复至秒信号输出。
			if( Uart[Ch].Buf[9] != 0x01 )	//数据域长度错误
			{
				Uart[Ch].Overtime_Timer = 2;
				return;
			}
			
			switch( Uart[Ch].Buf[10] )
			{
				case 0x00:	//时钟秒脉冲
				case 0x01:	//需量周期
				case 0x02:	//时段投切
					Outputtype = Uart[Ch].Buf[10];
					reg_veri_flag = 1;
					k = 2;
					OutBuf[1] = Uart[Ch].Buf[10];
					break;
					
				default:
					break;
			}
			break;
			
		case 0x09:	//寻卡命令
			if( (identity_prog_flag == 0) || (Keyswitch_Valid == 0xaa) )	//安全认证编程标志 失效
			{
				k = 2;
				OutBuf[0] = 0xFF; //结果
				OutBuf[1] = 0x04; //Error: bit2密码错误/未授权
				break;	//国网电能表只支持98级远程控制
			}
			
			if( Ram_Ptr[ 9 ] != 12 ) break;

			result = JudgeClockNoWeek( 0, Uart[Ch].Buf+16, 6 );
			if( result != 0x00 ) break; //错误
	
			result = Cmp_Bcd_Inv( Uart[Ch].Buf+16, Sclock.Clockbackup, 6 );
			if( result == 0x02 ) break; //错误
			
			if(Uart[Ch].Buf[ 14 ] == 0x1a)
			{
				Find_Card = 0x55;	//启动寻卡模式
//				rf_card_flag = 1;
				CardCheck1 = 0;	//立即寻卡一次
			}
			else if(Uart[Ch].Buf[ 14 ] == 0x1b)
			{
				Find_Card = 0xaa;	//结束寻卡模式
//				rf_card_flag = 0;
			}
			else break;
			
			k = 1;
			break;
			
		//--------------------- 扩展命令区 -------------------------------
		case 0x1E://扩展命令			
			if( (!BDMODE) && (SELFCLOSE == 0x55) )	//自保电功能		
			{
				if((Uart[Ch].Buf[ 9 ] == 1) && ((Uart[Ch].Flag & B0000_0011) != 0))	//心跳帧,广播
				{
					Uart[Ch].Flag |= B0000_0001; 	//广播命令不返回数据
					
					Self_Timer3 = 0;			//心跳帧之间延时清零
					if(Self_Cnt < 3) Self_Cnt++;
					
					if(self_close_flag && (Self_Timer24 == 0) && (Self_Cnt >= 3))//自保延时到，且连续收到3条心跳帧
					{
						ClrSelfClose();
					}
					
					Temp08 = Uart[Ch].Buf[10];
					if((Temp08 < 0x06) || (Temp08 > 0x90) || IsBCD(Temp08)) break;	//心跳时间间隔6~90分钟
					
					ReadE2WithBackup( ee_hearttime_page, ee_hearttime_inpage, Buf, ee_hearttime_lenth );
					
					Temp08 = Bcd_To_Hex( Temp08 );
					if(Temp08 != Buf[0])
					{
						WriteE2WithBackup( ee_hearttime_page, ee_hearttime_inpage, &Temp08, ee_hearttime_lenth );
					}
					break;
				}
			}
			
			if( Uart[Ch].Buf[ 10 ] == 5 )//扩展读长数据ee
			{
				Temp16.B08[1] = Uart[Ch].Buf[ 11 ];		//Di1
				Temp16.B08[0] = Uart[Ch].Buf[ 12 ];		//Di2		

				Temp08 = 68;
				if( (Temp16.B16>=0x2000) || (Uart[Ch].Buf[ 13 ] == 0x81) )
				{					
					if(  Temp16.B16==0xeeee )
					{
						memset(Uart[Ch].Buf+14, 0x00, 14);
						if( BDMODE && (Keyswitch_Valid!=0xAA) )  memcpy( Uart[Ch].Buf+14, "BD-CPU", 6 );
						else if( !BDMODE && (Keyswitch_Valid!=0xAA) ) memcpy( Uart[Ch].Buf+14, "YC-CPU", 6 );
						else if( BDMODE && (Keyswitch_Valid==0xAA) )  memcpy( Uart[Ch].Buf+14, "BDNCPU", 6 );
						else if( !BDMODE && (Keyswitch_Valid==0xAA) ) memcpy( Uart[Ch].Buf+14, "YCNCPU", 6 );					
						if( Relay_Mode != 0x55 ) memcpy( Uart[Ch].Buf+20, "-NZ-", 4);	
						else memcpy( Uart[Ch].Buf+20, "-WZ-", 4);		
						memcpy( Uart[Ch].Buf+14+14, Verdate, 18 );//返回版本信息
						Uart[Ch].Buf[14+32] = 0xaa;
						Uart[Ch].Buf[14+33] = clkmode;		//1分频为8M主时钟,2分频为4M主时钟
						Uart[Ch].Buf[14+34] = sleeptmr0ie;	//0x20打开tmr0停电唤醒,0x00关闭tmr0停电唤醒仅支持1h唤醒
						Uart[Ch].Buf[14+35] = Correction_Mode;	//0x55-开启电压补偿功能,其他-关闭(目前仅rn8209c支持)
						Uart[Ch].Buf[14+36] = SELFCLOSE;	//自保电功能：00关闭；55开启
						Uart[Ch].Buf[14+37] = 0xaa;
					}
					else
					{
						Temp32.B08[3] = Uart[Ch].Buf[ 11 ];		//Di1
						Temp32.B08[2] = Uart[Ch].Buf[ 12 ];		//Di2
						Temp32.B08[1] = Uart[Ch].Buf[ 13 ];
						Temp32.B08[0] = 0;
	
						if( Uart[Ch].Buf[ 13 ] == 0x01 )//表示扩展读寄存器(0x012000~0x0123ff)
						{
							memcpy( Uart[Ch].Buf+14, (unsigned char *)Temp32.B32, 64 );
						}
						else if( Uart[Ch].Buf[ 13 ] == 0x81 )//表示扩展读flash
						{
							memcpy( Uart[Ch].Buf+14, (unsigned char ecode *)Temp32.B32, 64 );
						}
						else	//表示扩展读ram(0x000000~0x000fff)
						{
							memcpy( Uart[Ch].Buf+14, (unsigned char near *)(Temp16.B16-0x2000), 64 );
						}
					}
				}
				else
				{
					I2C_Read_Eeprom( Temp16.B16, 0, Uart[Ch].Buf+14, Uart[Ch].Buf[13]);
//					Inverse(Uart[Ch].Buf+14, Uart[Ch].Buf[13]);
					if( Temp16.B16 >= 0x300 ) Temp08 = 76;
				}
				
				Uart[Ch].Buf[ 9 ] = Temp08;
				Uart[Ch].Buf[ Temp08+11 ] = 0x16;
							
				Uart[Ch].Flag |= B0000_1000;//规约处理结束准备发送数据标志
				return;
			}
			break;
			
		case 0x1F://扩展命令	
//			if((Uart[Ch].Buf[ 10 ]!=0xAF) ) break;	//厂内命令不支持载波通道
				
			switch( Uart[Ch].Buf[ 10 ] )
			{
				case 0x13:	//初始 ((Cover.Status == 0)&&((Fwdl.Fwdl_T.B32>=0x10000)||(Bkdl.Bkdl_T.B32>=0x10000)))噶?
					if(  (!progenable)  )
					{
						k = 2;
						OutBuf[0] = 0xFF; //结果
						OutBuf[1] = 0x04; //Error: bit4密码错误/未授权
						break;
					}
					
					if( (Uart[Ch].Buf[11] == 0x14)&&(Uart[Ch].Buf[12] == 0x54)&&(Uart[Ch].Buf[13] == 0x80) )
					{
						MeterClear = 0x88BB;
						
//						Clear_Meter( Ch, Uart[Ch].Buf+10 );
//						Factory_Init();
						
						k = 1;
					}
					break;
					
				case 0x0f:	//软件编程开关控制指令
					if( Uart[Ch].Buf[9] != 0x03 ) break;//数据长度错误
					
					if( Uart[Ch].Buf[11] == 0xaa )
					{
						Buf[0] = 0x00;	//软件编程禁止
						if( I2C_Write_Eeprom44( ee_factorymode_page, Buf, ee_factorymode_lenth ) ) break;
						progenable = 0;	//编程禁止
					}
					else if(  Uart[Ch].Buf[11] == 0x55 )
					{
					//	if( (Key_Status==1) && (esam_error==0) ) break;	//在有esam的情况下私钥不允许设置
					//	else
						{
							Buf[0] = 0x55;	//软件编程使能
							if( I2C_Write_Eeprom44( ee_factorymode_page, Buf, ee_factorymode_lenth ) ) break;
							progenable = 1;	//编程使能
						}
					}
					else
					{
						break;		//非法数据
					}
					
					k = 1;
					break;	
					
				case 0x03:
					//设置温度曲线的参数说明：
					//0 高温温度斜率微调 //1 低温温度斜率微调
					//2 高温晶体曲线微调（高温修调二次函数系数）x*0.0001
					//3 低温晶体曲线微调（低温修调二次函数系数）x*0.0001
					//4 常温顶点调校值
					if( Ch == 0 ) break;//禁止红外操作
					if( Uart[Ch].Buf[9] != 0x0a ) break;//数据长度错误
					if( Wr_Ext_Number == 50 ) break;
					else if ( Wr_Ext_Number > 50 ) Wr_Ext_Number = 25;
					else Wr_Ext_Number++;
					
					Temperpara_Verify();	//温度系数校验
					memcpy( Buf, Tpara.Offset, ee_temperature_lenth );//复制温度系数
					
					if( Uart[Ch].Buf[19] == 0x00 )//全部更新
					{
						memcpy( Buf+2, Uart[Ch].Buf+16, 3 );
					}
					else if( Uart[Ch].Buf[19] == 0x55 )//更新高温温度系数
					{
						Buf[2]=Uart[Ch].Buf[16];
					}
					else if( Uart[Ch].Buf[19] == 0x5a )//更新低温温度系数
					{
						Buf[3]=Uart[Ch].Buf[17];
					}
					else if( Uart[Ch].Buf[19] == 0xaa )//更新调校值
					{
						Buf[4]=Uart[Ch].Buf[18];
					}
					else	break;
					if( (Buf[4]&0x7f) > 3 ) break;	//调校值修正范围为0~3
					result = WriteE2WithBackup( ee_temperature_page, ee_temperature_inpage, Buf, ee_temperature_lenth );
					if( result ) break;
					memcpy( Tpara.Offset, Buf, ee_temperature_lenth );//更新ram温度系数
					Tpara.Crc16.B16 = Crc_16Bits(Tpara.Offset, ee_temperature_lenth ); //CRC计算
					
					Adcctrl.Timer=8;//立刻启动温度adc
					k = 1;
					break;
					
				case 0x04://读晶体负载电容等数据
					k = 7;
					OutBuf[1] = const_tx_flag;	//0xc3表示温度调校过
					OutBuf[2] = Tpara.RtcTMode;	//读RTC补偿工作模式，0x55:测试模式，0xAA常温查表插值模式，其他：普通模式
//					OutBuf[3] = XLCAPCTRLL;		//读晶体负载电容
//					OutBuf[4] = XLCAPCTRLH;
					OutBuf[5] = Temperature_Adc.B08[1];	//读温度ADC数据
					OutBuf[6] = Temperature_Adc.B08[0];
					break;

				case 0x05://写晶体负载电容	NNNN	2
				case 0x07://设置RTC补偿工作模式，0x55:测试模式，0xAA常温查表插值模式，其他：普通模式
				case 0x0a://写常温顶点调校值（通信调时钟误差）
				case 0xa9://关闭温度补偿（通信调时钟误差，准备测量时钟误差）
				case 0xaa://写常温顶点调校值（通信调时钟误差）(下发2字节日计时误差,-15秒~15秒,补码)
				case 0x0b://常温温度ADC微调
					//5 常温顶点调校值（通信调时钟误差）
					//6 常温温度ADC微调
					//7 预留
					//8 RTC补偿工作模式，0x55:测试模式，0xAA常温查表插值模式，其他：普通模式
					//9~10	晶体匹配电容(普通模式有效)
					if( (Ch == 0)||(!progenable)  ) break;//禁止红外操作,编程禁止

					Temperpara_Verify();	//温度系数校验
					memcpy( Buf, Tpara.Offset, ee_temperature_lenth );//复制温度系数
					if( Uart[Ch].Buf[ 10 ] == 0x05 )
					{
						if( (Uart[Ch].Buf[9] != 0x03) || (Tpara.RtcTMode == 0x55) || (Tpara.RtcTMode == 0xaa) ) break;	
						Buf[9] = Uart[Ch].Buf[12];
						Buf[10] = Uart[Ch].Buf[11];
					}
					else if( Uart[Ch].Buf[ 10 ] == 0x07 )
					{
						if( (Uart[Ch].Buf[9] != 0x02)) break;//数据长度错误,编程禁止
						Buf[8] = Uart[Ch].Buf[11];
					}
					else if( Uart[Ch].Buf[ 10 ] == 0x0a )
					{
						if( (Uart[Ch].Buf[9] != 0x02)||(const_tx_flag == 0xc3) ) break;	//刚编程调校过
						Buf[5] = Uart[Ch].Buf[11];	
					}
					else if( Uart[Ch].Buf[ 10 ] == 0xa9 )//关闭温度补偿（通信调时钟误差，准备测量时钟误差）
					{
						if( (Uart[Ch].Buf[9] != 0x01)||(const_tx_flag == 0xc3) ) break;	//刚编程调校过
						RtcStopProcFlag = 0x55;//关闭RTC温度补偿功能
						Adcctrl.Timer=8;//立刻启动温度adc
						k = 1;
						break;
					}
					else if( Uart[Ch].Buf[ 10 ] == 0xaa )//(下发2字节日计时误差,-15秒~15秒,补码)
					{
						if( (Uart[Ch].Buf[9] != 0x03)||(const_tx_flag == 0xc3)||(RtcStopProcFlag!=0x55)) break;	//刚编程调校过
						
						Temp16.B08[0] = Uart[Ch].Buf[ 12 ];
						Temp16.B08[1] = Uart[Ch].Buf[ 11 ];
						if( ((short)Temp16.B16 > 15000) || ((short)Temp16.B16 < -15000) ) break;	//结果非法	
						Buf[5] = Get_RTCTop_Proc( Temp16.B16 );	//折算顶点调校值
						RtcStopProcFlag = 0x00;//打开RTC温度补偿功能
					}
					else
					{
						if( (Uart[Ch].Buf[9] != 0x02)||((Uart[Ch].Buf[11]&0x7F)>50) ) break;//超出允许调整范围
						Buf[6] = Uart[Ch].Buf[11];	
					}
					result = WriteE2WithBackup( ee_temperature_page, ee_temperature_inpage, Buf, ee_temperature_lenth );
					if( result ) break;						
					
					if( (Uart[Ch].Buf[ 10 ]&0x0f) == 0x0a )//写常温顶点调校值时时间记录
					{
						I2C_Read_Eeprom( ee_rtcadj_time_page, ee_rtcadjtime_inpage, Buf, ee_rtcadjtime_lenth );
						Buf[0]++;//写常温顶点调校值次数加1
						memcpy( Buf+1, Sclock.Clockbackup, 7 );//写常温顶点调校值时时间
						memcpy( Buf+8, Fun_Para.Temper_M.B08, 2 );//写常温顶点调校值时温度
						Buf[7] = Uart[Ch].Buf[ 10 ];//通讯标识
						I2C_Write_Eeprom( ee_rtcadj_time_page, ee_rtcadjtime_inpage, Buf, ee_rtcadjtime_lenth );		
					}
					
					Tpara.Crc16.B16 += 1;
					temperpara_veri_flag = 1;
					Adcctrl.Timer=8;//立刻启动温度adc
					k = 1;
					break;
					
				case 0x06://修改负载电容（19.968的偏移量，±4.096pf）	NNNN	2	
					if( Ch == 0 ) break;//禁止红外操作
 					if( (Uart[Ch].Buf[9] != 0x03)||(!progenable) ) break;//数据长度错误,编程禁止
 					if( Tpara.RtcTMode!=0x55 ) break;//仅在测试模式下有效
					
					Temp16.B08[0] = Uart[Ch].Buf[12];
					Temp16.B08[1] = Uart[Ch].Buf[11];
					if( (((int16)Temp16.B16)<-1024)||(((int16)Temp16.B16))>1024 ) break;//（19.968的偏移量，±4.096pf=1024*0.004）
					if( const_tx_flag != 0xc3 )//通信校准时钟误差
					{
						TempCap16.B08[0] = constF_XcapH;
						TempCap16.B08[1] = constF_XcapL;
					}
					else
					{
						TempCap16.B08[0] = const_XcapH;
						TempCap16.B08[1] = const_XcapL;
					}

					TempCap16.B16 = (int16)TempCap16.B16 +	(int16)Temp16.B16;
//					XLCAPCTRLH  = TempCap16.B08[0];
//					XLCAPCTRLL  = TempCap16.B08[1];
//					CAPREFRESH  = 0x55; //更新电容值		
					Adcctrl.Timer=8;//立刻启动温度adc
					k = 1;
					break;
					
				case 0x08://设置查表插值数据
					if( Ch == 0 ) break;//禁止红外操作
 					if( (Uart[Ch].Buf[9] != 85)||(!progenable) ) break;//数据长度错误,编程禁止
					result = I2C_Write_Eeprom( ee_ptatvalue_page, ee_ptatvalue_inpage, Uart[Ch].Buf+11, ee_ptatvalue_lenth );
					result |= I2C_Write_Eeprom( ee_capvalue_page, ee_capvalue_inpage, Uart[Ch].Buf+11+42, ee_capvalue_lenth );
					if( result != 0x00 ) break;
					I2C_Read_Eeprom( ee_ptatvalue_page, ee_ptatvalue_inpage, RtcTadcBuf.PTAT_Value[0].B08, ee_ptatvalue_lenth );
					I2C_Read_Eeprom( ee_capvalue_page, ee_capvalue_inpage, RtcTcapBuf.CAP_Value[0].B08, ee_capvalue_lenth );
					k = 1;
					break;
					
				case 0x09://读常温顶点调校值（通信调时钟误差）
					k = 4;
					OutBuf[1] = const_tx_flag;	//0xc3表示温度调校过
					OutBuf[2] = Tpara.Offset[5];	//常温顶点调校值
					OutBuf[3] = Tpara.Offset[6];	//温度微调值
					break;

				case 0x15://程序累加和	NNNNNNNN	4
					k = 5;
					Calculate_Program_Cs( OutBuf+1 );
					Inverse( OutBuf+1, 4 );
					break;	
					
				case 0x16://载波复位模式字(扩展)	NN	1
					k = 2;
					OutBuf[1] = Mode.Plc;
					break;
				
				case 0x17://载波复位模式字(扩展)	NN	1
					if( Uart[Ch].Buf[9] != 0x02 ) break;//数据长度错误
					if( Wr_Ext_Number == 50 ) break;
					else if ( Wr_Ext_Number > 50 ) Wr_Ext_Number = 25;
					else Wr_Ext_Number++;
					
					if( Uart[Ch].Buf[11] != Mode.Plc )//发生改变才写数据ee
					{
						result = UpdateSysPara( 0x80+27, Uart[Ch].Buf+11, 1 );
						if( result != 0x00 ) break;
					}

					k = 1;
					break;	
					
				case 0xDF:	//继电器状态抄收命令
					if( (Uart[Ch].Buf[9] != 0x01) || (!progenable) ) break;//数据长度错误,编程禁止

					//检测继电器检测电路的实际通断情况
					for( Temp16.B16=0; Temp16.B16<3600/clkmode; Temp16.B16++ )	//30ms去抖判断
					{
						if( RELAYTEST_N ) break;
					}
					
					if( Temp16.B16 == 3600/clkmode ) OutBuf[1] = 0xaa; //断
					else OutBuf[1] = 0x55;	//通
					
					k = 2;
					break;			
				
				case 0xAF:	//东软载波：申请地址及报警事件信息
					if( (Uart[Ch].Flag&B0000_0001) && (Uart[Ch].Buf[9] == 0x02) && (Uart[Ch].Buf[11] == 0x02) )	//广播, Len=2
					{
						Uart[Ch].Flag &= B1111_1110;
						
						k = 9;
						OutBuf[1] = 0xAF;
						OutBuf[2] = 0x02;
						Exchange( OutBuf+3, Comm_Addr.Comm_Id, 6 );
					}
					break;
				
				default:
					break;
			}
			break;			
		
		case 0x60:	//读计量芯片寄存器	
			if(Uart[Ch].Buf[10] < 0xf0)//读取计量芯片寄存器值
			{
				memset(&Uart[Ch].Buf[11], 0x00, 4);
				result = Comm_Read_Reg(&Uart[Ch].Buf[10]);
				if(result) break;
			}
			else if(Uart[Ch].Buf[10] < 0xf5)//读取U、I1、I2、P、F系数
			{
				result = Uart[Ch].Buf[10] - 0xf0;
				if( result == 2 ) result = 3;				//为了兼容之前的校表软件
				else if(result == 3 ) result = 2;
				Exchange( Uart[Ch].Buf+11, Emu_Para1.rate[result].B08, 4 );
			}
			else break;	//非法请求
			
			k = 6;
			memcpy(&OutBuf[1], &Uart[Ch].Buf[10], 5 );
			break;
			
		case 0x61:	//写计量芯片寄存器			
			if(!progenable)
			{
				k = 2;
				OutBuf[0] = 0xFF; //结果
				OutBuf[1] = 0x04; //Error: bit4密码错误/未授权
				break;
			}
			
			if(Uart[Ch].Buf[10] < 0xf0)//写寄存器
			{
				result = Comm_Write_Reg(&Uart[Ch].Buf[10]);
				if(result) break;
			}
			else if(Uart[Ch].Buf[10] < 0xf5)//写U、I1、I2、P、F系数
			{
				result = Uart[Ch].Buf[10] - 0xf0;
				if( result == 2 ) result = 3;
				else if(result == 3 ) result = 2;
//				memcpy( Emu_Para.rate[result].B08, Uart[Ch].Buf+11, 4 );
//				WriteE2WithBackup( ee_emupara_page, ee_emupara_inpage, Emu_Para.rate[0].B08, ee_emupara_lenth );
				memset( Emu_Para1.rate[result].B08, 0, 4 );
				
				if(Uart[Ch].Buf[11] != 0 )
				  Exchange( Emu_Para1.rate[result].B08, Uart[Ch].Buf+11, 4 );
				else if (Uart[Ch].Buf[12] != 0  )
				  Exchange( Emu_Para1.rate[result].B08, Uart[Ch].Buf+12, 3 );
				else if (Uart[Ch].Buf[13] != 0  )
				  Exchange( Emu_Para1.rate[result].B08, Uart[Ch].Buf+13, 2 );
				else
					Exchange( Emu_Para1.rate[result].B08, Uart[Ch].Buf+14, 1 );
				
				WriteE2WithBackup( ee_7051_para_page, ee_7051_para_inpage, Emu_Para1.rate[0].B08, ee_7051_para_lenth );
			       
				Emu_Para1.Crc16.B16+=1;
			}
			else if(Uart[Ch].Buf[10] == 0xff)//校表初始化命令
			{
				for(i=0; i<4; i++)
				{
					if(Uart[Ch].Buf[11+i] != 0xff) break;
				}
				if(i != 4) break;
				
				Comm_Write_Reg(&Uart[Ch].Buf[10]);
			}
			else break;	//非法请求 
			
			check_7051_flag1=1;
			k = 1;
			break;

		default:
			k = 2;
			OutBuf[0] = 0xFF;	//Operation result.
			if( Uart[Ch].Buf[8]&0x40 )	//红外通信口
				OutBuf[1] = 0x04; 	//Error: bit4密码错误/未授权
			else
				OutBuf[1] = 0x02;	//数据标识错, 无请求数据
			break;
	}

	if( Uart[Ch].Flag&B0000_0001 )	//地址为广播地址, 不应答
	{
		Uart[Ch].Overtime_Timer = 2;
		return;
	}
	
	if( (DI07.B08[3]==0x07) && (DI07.B08[2]==0xA0) )//交互终端命令
	{
//		if( DIVSEL != rundivsel )	DIVSEL	= rundivsel;		//系统频率恢复到4M
	}

	Temp08 = 0;
	if( k == 0x00 ) //不发送应答帧
	{
		k=2;
		OutBuf[1]=1;	   //错误信息字ERR：其他错误(Bit0)
		Uart[Ch].Buf[8]|=0x40;		//否认帧
	}
	else
	{
		if(OutBuf[0] == 0x00)	 //确认帧
		{ 	   
		   if((Uart[Ch].Buf[8]==0x03)||(Uart[Ch].Buf[8]==0x11)||(Uart[Ch].Buf[8]==0x12))
		   	Temp08 += 4;
		   	   
		   if((Remain)||
		      (((Uart[Ch].Buf[8] == 0x11) || (Uart[Ch].Buf[8] == 0x12)) && (DI07.B32 != 0x01150004) && (Mode.Feature1&0x04)
  			   && ChkBufZero( Report_Data, 5 ))
		     )
		   	Uart[Ch].Buf[8]|=0x20;		 //后续帧
        	
		}
		else
		{
//		  	if( Uart[Ch].Buf[8] == 0x03 )	//安全认证
//		  	{
//				if(hangup_comm_flag)	OutBuf[2] |= 0x01;	//远程安全认证锁住状态位
//		  	}
		  	Uart[Ch].Buf[8]|=0x40;		//否认帧
		}
	}
	
	Uart[Ch].Buf[8]|=0x80;		   //应答帧
	Uart[Ch].Buf[9]=(k-1+Temp08); //len
	memcpy( Uart[Ch].Buf+10+Temp08, OutBuf+1, k );	//搬移数据

//	Uart[Ch].Buf[7] = 0x68;
//	Uart[Ch].Buf[0] = 0x68;
	Exchange( Uart[Ch].Buf+1, Comm_Addr.Comm_Id, 6 );
	for( i=0; i<Uart[Ch].Buf[9]; i++ )
	{
		Uart[Ch].Buf[10+i] += 0x33;
	}	
	Uart[Ch].Buf[i+10]=Cal_ChkSum(Uart[Ch].Buf,Uart[Ch].Buf[9]+10);
	Uart[Ch].Buf[i+11]=0x16;

	Uart[Ch].Flag |= B0000_1000;//规约处理结束准备发送数据标志
	
	if( MeterClear == 0x55aa )	//清零
	{
		Clear_Meter( Ch, OptID.B08 );	//0x1a清零命令或者钱包初始化命令
	}
//	else if( MeterClear == 0xaaaa )	//清零
//	{
//		Clear_Meter( Ch, OptID.B08 );	//0x1a清零命令或者钱包初始化命令
//	}
	else if( MeterClear == 0x88bb )	//电表初始化
	{
	  Clear_Meter( Ch, OptID.B08 );
		Factory_Init();
	}
	else if( MeterClear == 0xaa55 )	//密钥更新
	{
		UpdateEsamKey();
	}
	else if( MeterClear == 0x1122 )	//模式切换
	{
		Freeze_Dl(ee_modeswith_note_page, ee_modeswith_inpage);//约定冻结：模式切换记录
		
		UpdataRunFile();	//更新ESAM运行信息文件
		
		ProgramRecord( DI07.B08, OptID.B08 );
	}
}
