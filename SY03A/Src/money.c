#include "ex_func.h"
#include "ex_var.h"
//#include <intrins.h>
#include <string.h>
#include <absacc.h>


//Style=0x55通讯退费,否则正常钱包扣减
void Remainmoney_Process(unsigned char *BcdA, unsigned char Style )	//金额扣减处理
{		
	unsigned char Buf[24];
	
	Remainmoney_Verify();	//剩余金额参数校验
	Decimal.Status |= B0000_0010;//金额变化标志
	memcpy( Buf, Money.Remain, 12 );//复制剩余金额
	
	memset( Buf+12, 0x00, 6 );
	if( Style == 0x55 )	//通讯退费
	{
		memcpy( Buf+12, BcdA, 4 );//复制退费金额
		Decimal.Status |= B0000_0001;//剩余金额写数据ee
		UpdateMoneytimer = 0;	//钱包扣减处理
	}
	else
	{
		memcpy( Buf+12, BcdA, 4 );//复制单价
	}
	
	if ( Cmp_Bcd_Inv( Buf, Buf+12, 6) !=2 )
	{
		if ( Cmp_Bcd_Inv( Buf, Buf+12, 6) ==3 ) Decimal.Status |= B0000_0001;//防止剩余金额为零出现非法校验
		Sub_Bcd( Buf, Buf+12, 6);//剩余电量减去单位电价
	}
	else
	{
		Sub_Bcd( Buf+12, Buf, 6);//单位电价减去剩余电量
		memset( Buf, 0x00, 6 );//剩余电量清零
		Add_Bcd( Buf+6, Buf+12, 6);//得到负的剩余电量
	}
	memcpy( Money.Remain, Buf, 12 );//复制剩余金额
	Money.Crc16.B16 = Crc_16Bits( Money.Remain, ee_remainmoney_lenth );	 //CRC计算
	
	if(Decimal.Status & B0000_0001)
	{
		WriteE2WithBackup( ee_remainmoney_page, ee_remainmoney_inpage, Money.Remain, ee_remainmoney_lenth );//剩余金额写数据ee	

		if( UpdateMoneytimer == 0 )
		{
			Moneybag_Process1();//钱包同步更新处理
			UpdateMoneytimer = default_updatemoneytimer;	//重新启动更新ESAM钱包定时器	
		}
	
		Decimal.Status &= (~B0000_0001);//清电量整度标志
	}
	
	run_alarm_flag = 1;	//置电量状态判断标志
}


void MonthDaymoney_Process(unsigned char *BcdA )	//金额扣减处
{
  unsigned char Buf[6];
	
	memset( Buf, 0x00, 6 );
	memcpy( Buf, BcdA, 4 );
	Add_Bcd( Money_UserINFO.Day_Moneyused, Buf, 6);
	Money_UserINFO.DayCrc16.B16=Crc_16Bits( Money_UserINFO.Day_Moneyused,6 );

	Add_Bcd( Money_UserINFO.Month_Moneyused, Buf, 6);  
	Money_UserINFO.MonthCrc16.B16=Crc_16Bits( Money_UserINFO.Month_Moneyused,6 );
	
}

void Calc_Pwr_Proc(void)	//正反向电量累计处理和金额处理
{
	unsigned char Buf[24];
	union B16_B08 Temp16;
	union B32_B08 TempB;
	uint16 Page;	
	unsigned char Inpage;
	unsigned char *Dl_Buf;
	unsigned char n;
	
	if( FLAG_EA == 0 ) return;//掉电不处理,程序直接返回

	if ( Decimal.Unit_Pluse > 0x00 )
	{
//  	NVIC_DisableIRQ(PMU_IRQn );
		n = Decimal.Unit_Pluse;
		Decimal.Unit_Pluse = 0;
//    NVIC_EnableIRQ(PMU_IRQn );
		if( Fwdl_Verify() ) return;				//正向电量校验
		Dl_Buf = Fwdl.Fwdl_T.B08;
		Page = ee_fwdl_page;
		Inpage = ee_fwdl_inpage;
		Decimal.Status |= B0100_0000;		//置正向电量改变标志
	}
	else if ( Decimal.Unit_Bkpluse> 0x00 )
	{
//    NVIC_DisableIRQ(PMU_IRQn );
		n = Decimal.Unit_Bkpluse;
		Decimal.Unit_Bkpluse = 0;
//    NVIC_EnableIRQ(PMU_IRQn );
		if( Bkdl_Verify() ) return;				//反向电量校验
		Dl_Buf = Bkdl.Bkdl_T.B08;
		Page = ee_bkdl_page;
		Inpage = ee_bkdl_inpage;
		Decimal.Status |= B1000_0000;		//置反向电量改变标志
	}
	else
	{
		return;//异常
	}
	
	if( n > maxtimes ) return; //电量增量异常
	
	for( ; n>0; n-- ) 
	{
		TempB.B32 = 1;	//电量增量
		memcpy( Buf, Dl_Buf, ee_fwdl_lenth );
		Add_Bcd( Buf, TempB.B08, 4 );//电量增加
		Add_Bcd( Buf+((Tariff.Current&3)+1)*4, TempB.B08, 4 );//费率电量增加
		Add_Bcd( Buf+20, TempB.B08, 4 );//本月用电量增加
		memcpy( Dl_Buf, Buf, ee_fwdl_lenth );
		Temp16.B16 = Crc_16Bits( Dl_Buf, ee_fwdl_lenth ); //CRC计算
		Dl_Buf[ee_fwdl_lenth] = Temp16.B08[0];
		Dl_Buf[ee_fwdl_lenth+1] = Temp16.B08[1];
		
		if( Dl_Buf[3] == 0x00 )//判断总电量是否到整度
		{
			WriteE2WithBackup( Page, Inpage, Dl_Buf, ee_fwdl_lenth );
			Decimal.Status |= B0000_0001;//置电量整度标志
			if( Inpage == ee_fwdl_inpage ) Decimal.Status &= ~(B0100_0000);//清正向电量改变标志
			else Decimal.Status &= ~(B1000_0000);//清反向电量改变标志
		}
		
		if( BDMODE ) 	//金额处理
		{
			Price_Judge();	//费率阶梯电价判断
			Remainmoney_Process(Price.Current_X.B08, 0);  //当前电价
			run_price_flag = 1;	//置费率阶梯判断标志
			MonthDaymoney_Process(Price.Current_X.B08);  //当前电价
		}
	}
}


//offset=0结算周期,offset=3清结算周期
//offset=1月结算,offset=4清月结算
//offset=2年结算,offset=5清年结算,offset=6年结算时间清零
void Process_Usedl( unsigned char offset )
{
	unsigned char i, len;
	unsigned char  Buf[ee_yhistroyusedl_lenth+8];
	
	memcpy( Buf, Sclock.Clockbackup+2, 4 );//复制时间
	Cal_Comb( 0, 0, 0, Buf+4 );	//计算当前组合电量	
//	Inverse( Buf+4, 4 );
	
	if( offset<3 ) 
	{				
		I2C_Read_Eeprom( ee_pre1_histroyusedl_page+offset, 0x00, Buf+8, ee_yhistroyusedl_lenth );//读上1~6总电量或者上1总电量
	}
	else if( offset<6 )
	{
		offset -= 3;
		memset( Buf, 0x00, ee_yhistroyusedl_lenth );		//清上1~7总电量或者上1~2总电量
	}
	else
	{
		offset = 2;
		memset( Buf, 0x00, 4 );			//时间清零
	
		for( i=1; i<7; i++ )
		{
			memcpy( Buf+8*i, Buf, 8 );	//上1~6电量转存
		}
	}
	
	if( offset<2 )	len= ee_histroyusedl_lenth;
	else	len= ee_yhistroyusedl_lenth;

	I2C_Write_Eeprom( ee_pre1_histroyusedl_page+offset, 0x00, Buf, len );//上1~4总电量转存	
}


//type=0年结算,年转存,清用电量
//type=1年结算,月转存,不清用电量
//type=2月结算,月转存,清用电量
void Freeze_Usedl( unsigned char type )//结算处理
{
	Clear_Wdt();
	
	if( type == 0 )
	{
		Process_Usedl( 2 );	//上1年结算日总电量转存
	}
	else
	{
		Process_Usedl( 1 );	//上1月结算日总电量转存
	}
	
	if( type == 1 )	return;//年结算,月转存,不清用电量
	
	if( Fwdl_Verify() ) return;	//正向电量校验
	memset( Fwdl.Usedl.B08, 0x00, 4 );//正向用电量清零
	Fwdl.Crc16.B16 = Crc_16Bits( Fwdl.Fwdl_T.B08, ee_fwdl_lenth ); //CRC计算
	WriteE2WithBackup( ee_fwdl_page, ee_fwdl_inpage, Fwdl.Fwdl_T.B08, ee_fwdl_lenth );
	
	if( Bkdl_Verify() ) return;	//反向电量校验
	memset( Bkdl.Usedl.B08, 0x00, 4 );//反向用电量清零
	Bkdl.Crc16.B16 = Crc_16Bits( Bkdl.Bkdl_T.B08, ee_bkdl_lenth ); //CRC计算
	WriteE2WithBackup( ee_bkdl_page, ee_bkdl_inpage, Bkdl.Bkdl_T.B08, ee_bkdl_lenth );
}


void FreezeMonth_Money(void)
{
	  I2C_Read_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, &Ic_Comm_TxBuf[6], ee_Pre12monthMon_lenth );
    
	  //更新最近1条数据
	  memcpy(Ic_Comm_TxBuf,Money_UserINFO.Month_Moneyused, 6 );
	  memset(Money_UserINFO.Month_Moneyused,0, 6 ); 
    Money_UserINFO.MonthCrc16.B16=Crc_16Bits( Money_UserINFO.Day_Moneyused, 6 );	
    I2C_Write_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, &Ic_Comm_TxBuf[0], ee_Pre12monthMon_lenth );
	
}


//需量存储
//mode =0;上电模式；mode =1;上电补存第一次最大需量模式 
void FreezeMonth_Demand(unsigned  char mode)
{
	unsigned  char Result;
	
	Result = Write_Event_Point(ee_histroyDM_inpage);
	
	memset( Ic_Comm_TxBuf, 0x00, 42 );//数据初始化0xaa
	if( mode == 0)	I2C_Read_Eeprom( ee_p_demand_page, ee_p_demand_inpage, &Ic_Comm_TxBuf[0] , ee_pdemand_lenth );
	
	I2C_Write_Eeprom( ee_histroyp_demand_page+Result, ee_histroyp_demand_inpage, &Ic_Comm_TxBuf[0], ee_pdemand_lenth );//正向需量冻结
	//反向需量冻结
	memset( Ic_Comm_TxBuf, 0x00, 42 );//数据初始化0xaa
	if( mode == 0) I2C_Read_Eeprom( ee_n_demand_page, ee_n_demand_inpage, &Ic_Comm_TxBuf[0], ee_ndemand_lenth );
	I2C_Write_Eeprom( ee_histroyp_demand_page+Result, ee_histroypn_demand_inpage,&Ic_Comm_TxBuf[0], ee_ndemand_lenth );//反向需量冻结
	
	Init_Demand();
	memset( Ic_Comm_TxBuf, 0x00, 42);
	I2C_Write_Eeprom( ee_p_demand_page, ee_p_demand_inpage, Ic_Comm_TxBuf, ee_pdemand_lenth );//当前正向最大需量清零
	I2C_Write_Eeprom( ee_n_demand_page, ee_n_demand_inpage, Ic_Comm_TxBuf, ee_ndemand_lenth );//当前反向最大需量清零
}

//日冻结金额
void FreezeDay_Money(void)
{
	  I2C_Read_Eeprom( ee_Pre7DayMon_page, ee_Pre7DayMon_inpage, &Ic_Comm_TxBuf[6], ee_Pre7DayMon_lenth );
    
	  //更新最近1条数据
	  memcpy(Ic_Comm_TxBuf,Money_UserINFO.Day_Moneyused, 6 );
	  memset(Money_UserINFO.Day_Moneyused,0, 6 ) ;
    Money_UserINFO.DayCrc16.B16=Crc_16Bits( Money_UserINFO.Day_Moneyused, 6 );	
    I2C_Write_Eeprom( ee_Pre7DayMon_page, ee_Pre7DayMon_inpage, &Ic_Comm_TxBuf[0], ee_Pre7DayMon_lenth );
	
}


//结算电量高字节在前存ee,其他冻结电量以低字节在前存数据ee
void Freeze_Dl(unsigned short page, unsigned char offset)
{
	unsigned char Result, i;

	Clear_Wdt();

	memcpy( Eeprom.Buffer, Sclock.Clockbackup+1, 5 );
		
	if( page==ee_histroy_dl_page ) 		
	{
		memcpy( Eeprom.Buffer+5,  Fwdl.Fwdl_T.B08, 20 );
		memcpy( Eeprom.Buffer+25, Bkdl.Bkdl_T.B08, 20 );
		Eeprom.Buffer[47]=Mode.Comp_B;//复制有功组合字
		Process_Usedl(0);//上1结算日总电量转存
	}
	else
	{
		for(i=0; i<5; i++)
		{
			memcpy( Eeprom.Buffer+5+4*i, Fwdl.Fwdl_T.B08+4*i, 4 );
		}
		
		for(i=0; i<5; i++)
		{
			memcpy( Eeprom.Buffer+25+4*i, Bkdl.Bkdl_T.B08+4*i, 4 );
		}
		
		memcpy( Eeprom.Buffer+45,  Emu_Data.P1.B08, 3 );
		if( dl_back_flag ) Eeprom.Buffer[47] |= 0x80;//反向功率bit7置1
	}
	
	Result = Write_Event_Point(offset);
	
	if( page==ee_dayfreezedl_page )//日冻结剩余金额和购电次数
	{ 	
		Hex2Bcd( Money.Buy_Times.B08, Eeprom.Buffer+56, 4 );
		memcpy( Eeprom.Buffer+48, Eeprom.Buffer+58, 2 );//冻结购电次数
		memcpy( Eeprom.Buffer+50, Money.Remain+2, 4 );//冻结剩余金额
		memcpy( Eeprom.Buffer+54, Money.Tick+2, 4 );//冻结透支金额
		I2C_Write_Eeprom( ee_dayfreezemoney_page+Result, ee_dayfreezemoney_inpage, Eeprom.Buffer+48, ee_dayfreezemoney_lenth );
		
		Wr_Pretime_Dayfreeze();	//写上一次日冻结时间	
	}

	I2C_Write_Eeprom( page+Result*2, 0x00, Eeprom.Buffer, ee_freezedl_lenth/2 );	//48字节分2次写
	I2C_Write_Eeprom( page+Result*2+1, 0x00, Eeprom.Buffer+ee_freezedl_lenth/2, ee_freezedl_lenth/2 );
}


void Hour_Freeze_Dl(uint16 page,unsigned char offset)//整点冻结或者半点冻结
{
	unsigned char Result,inpage;
	
	memcpy( Eeprom.Buffer, Sclock.Clockbackup+1, 5 );
	DltoEepromBuf( 5 );
	Result = Write_Event_Point(offset);
	inpage = (Result%4)*13;
	I2C_Write_Eeprom( page+(Result>>2), inpage, Eeprom.Buffer, ee_hourfreezedl_lenth );
}


//status=0表示停电调用,status=1表示上电调用
//RAMREG5跨年结算次数,RAMREG6跨结算日次数,RAMREG7年结算跨第一结算日次数,RAMREG8月结算跨第一结算日次数,上电补存使用
//Ic_Comm_RxBuf作为结算时间临时数据区
void Histroy_Freeze_Dl(unsigned char status)//结算电量冻结处理
{
	unsigned char i, Result=0;
	unsigned char  Buf[10];
	
	if( BDMODE ) //本地模式
	{
		//年结算判断
		if( status ) Result = Year_Ladder_Judge(); //年阶梯启动判断和数据合法性判断

		for( i=0; i<ee_curyearjs_len; i+=3 )
		{
			if( Cmp_Data( Price.Year_Ladder+i, Sclock.Clockbackup+2, 3 ) == 0x03 ) break;
		}
		
		if( (i<ee_curyearjs_len) && (Price.Ladder_Mode == 0x55) && (Sclock.Clockbackup[3] <= 0x28) ) //时日月相等
		{
	  	if( status )
	  	{
	  		if( (Result!=0x55)&&(Result!=0x5a) ) Freeze_Usedl(0);//年结算,年用电量转存和清零
	  	}
	  	else
	  	{
	  		if( RAMREG5 < 7 ) RAMREG5++;	//支持上1~上6年用电量
	  		memcpy( Ic_Comm_RxBuf+60, Ic_Comm_RxBuf+60+4, 24 );//移位历史停电年结算时间
				memcpy( Ic_Comm_RxBuf+60+24, Sclock.Clockbackup+2, 4 );//保存当前停电年结算时间(时日月年)
	  	}
		}
	}
	else
		Price.Ladder_Mode = 0x00;	//不支持年阶梯
	
	//结算日判断
	ReadE2WithBackup( ee_histroyparameter_page, ee_histroyparameter_inpage, Buf, ee_histroyparameter_lenth );
	if( ((Buf[0]== Sclock.Clockbackup[2]) && (Buf[1]== Sclock.Clockbackup[3])) ||
	    ((Buf[2]== Sclock.Clockbackup[2]) && (Buf[3]== Sclock.Clockbackup[3])) ||
	    ((Buf[4]== Sclock.Clockbackup[2]) && (Buf[5]== Sclock.Clockbackup[3])) ) 
	{
		//结算日判断
		if( status )
		{
			if( (Result!=0x5a)&&(Result!=0xaa) )	
			{
				Freeze_Dl(ee_histroy_dl_page,ee_histroyf_inpage);
				FreezeMonth_Money();
				FreezeMonth_Demand(0);
			}
		}
		else
		{
			if( RAMREG6 < 12 ) RAMREG6++;
			memcpy( Ic_Comm_RxBuf, Ic_Comm_RxBuf+4, 44 );//移位历史停电冻结时间
			memcpy( Ic_Comm_RxBuf+44, Sclock.Clockbackup+2, 4 );//保存当前停电冻结时间(时日月年)
		}
		
		//第一结算日判断
		if((Buf[0]== Sclock.Clockbackup[2]) && (Buf[1]== Sclock.Clockbackup[3]))
		{
			//年结算跨第一结算日
			if( Price.Ladder_Mode == 0x55 )
			{
				if( status )
				{
					if( Result!=0x5a) Freeze_Usedl(1);//年结算,月转存
				}
				else
				{
					if( RAMREG7 < 2 ) RAMREG7++;
				}
			}
			else//月结算跨第一结算日
			{
				if( status )
				{
					if( Result!=0xaa) Freeze_Usedl(2);//月结算,月用电量转存和清零
				}
				else
				{
					if( RAMREG8 < 2 ) RAMREG8++;
				}
			}
		}	
	}
}


void Freeze_Judge(void)	//冻结和结算判断
{
	unsigned char i;
	unsigned char  Result,Buf[20];
	
	run_freeze_flag = 0;
	run_loadchk_flag = 1;
	
	I2C_Read_Eeprom44( ee_freezeparameter_page, Buf, ee_freezeparameter_lenth );
		
	if( FLAG_EA && ((Cmp_Clock( Buf, 5 ) != 0x01)) )//整点冻结起始时间<=当前时间
	{
		Result = Sclock.Clockbackup[1];
		if( (Result == 0) || ((Result == 0x30)&&(Buf[5] == 0x30)) )
		{
			Hour_Freeze_Dl(ee_hourfreezedl_page,ee_hourf_inpage);//整点冻结
		}
	}
	
	if( FLAG_EA && (Cmp_Clock( Buf+6, 2 ) == 0x03) ) 
	{
		Freeze_Dl(ee_dayfreezedl_page,ee_dayf_inpage); //日冻结时间=当前时间,日冻结
		FreezeDay_Money(); //日冻结金额数据
	}	
        
	for(i=0; i<3; i++)
	{
		if(Buf[8+i]== 0x99) Buf[8+i] = Sclock.Clockbackup[4-i];
	}
	if( FLAG_EA && ((Cmp_Clock( Buf+8, 4 ) == 0x03)) ) 
	{
		Freeze_Dl(ee_timingfreeze_dl_page,ee_timingf_inpage); //定时冻结
	}
		
	if( Sclock.Clockbackup[1]== 0x00 )//分钟等于0
	{
		Histroy_Freeze_Dl(1);//结算电量冻结处理
		RTC_Status &= ~RTC_INTSTS_WKUHOUR;	
//		RTCIF = 0;	//清小时变化标志
	}				
}


void Pd_Histroyfreeze_Judge(void)	//上电结算日补存判断处理
{
	unsigned char  i;
	unsigned char  Buf[10];
	
	//停电补冻结处理
	if( ((RAMREG5 > 0)&&(RAMREG5 < 8)) || ((RAMREG7 > 0)&&(RAMREG7 < 3)) )
	{
		memcpy( Buf, Sclock.Clockbackup+2, 4 );//保存当前时间(时日月年)
		for( i=0; i<RAMREG5; i++ )
		{	
			memcpy( Sclock.Clockbackup+2, Ic_Comm_RxBuf+60+28-(RAMREG5-i)*4, 4 );//冻结时间
			Freeze_Usedl(0);//年结算,年用电量转存和清零
		}
		memcpy( Sclock.Clockbackup+2, Buf, 4 );//恢复当前时间(时日月年)		
	
		for( i=0; i<RAMREG7; i++ )
		{	
			Freeze_Usedl(1);//年结算,月转存
		}
	}
	else if( (RAMREG8 > 0) && (RAMREG8 < 3) )
	{
		for( i=0; i<RAMREG8; i++ )
		{	
			Freeze_Usedl(2);//月结算,月用电量转存和清零
		}
	}
	
	if( (RAMREG6 > 0) && (RAMREG6 < 13) )
	{
		memcpy( Buf, Sclock.Clockbackup+2, 4 );//保存当前时间(时日月年)
		for( i=0; i<RAMREG6; i++ )
		{	
			memcpy( Sclock.Clockbackup+2, Ic_Comm_RxBuf+48-(RAMREG6-i)*4, 4 );//冻结时间			
			Freeze_Dl(ee_histroy_dl_page,ee_histroyf_inpage); //结算冻结
		}
		memcpy( Sclock.Clockbackup+2, Buf, 4 );//恢复当前时间(时日月年)
	}
	
	if( (RAMREG6 > 0) && (RAMREG6 < 13) )
	{			
		for( i=0; i<RAMREG6; i++ )
		{	
			if( i== 0) FreezeMonth_Demand(0);
			else  FreezeMonth_Demand(1);
		}		
	}
	
	
	RAMREG5 = 0;//跨年结算次数
	RAMREG6 = 0;//跨结算日次数
	RAMREG7 = 0;//年结算跨第一结算日次数
	RAMREG8 = 0;//月结算跨第一结算日次数
}

//Type=0表示进入sleep前调用,1表示sleep中调用,2表示sleep结束调用
/*RAMREG9标志说明：
bit0：补存结算日冻结标志
bit1：补存整点冻结标志
bit2：补存日冻结标志
bit3：补存定时冻结标志

bit5：掉电时，已跨整点冻结起始时间
bit6：日冻结临时标志
bit7：定时冻结临时标志*/
void Pd_Freeze_Judge(unsigned char CH)
{
	unsigned char  i;
	unsigned char Buf[ee_freezeparameter_lenth];
	
	I2C_Read_Eeprom44( ee_freezeparameter_page, Eeprom.Buffer, ee_freezeparameter_lenth );//读其他冻结时间
	
	memcpy(Buf, Eeprom.Buffer, ee_freezeparameter_lenth);
	
	//整点冻结判断
	if( (Cmp_Clock( Eeprom.Buffer, 5 ) != 0x01) )//整点冻结起始时间<=当前时间
	{
		if(CH == 0)
		{
			RAMREG9 |= B0010_0000;	//掉电已过整点冻结起始时间标志
		}
		
		if(CH == 1)
		{
			RAMREG9 |= B0000_0010;	//上电补存整点冻结标志
			RAMREG9 &= B1101_1111;	//清临时标志
		}
	}
	
	if(CH < 2)
	{
		//日冻结判断
		if((RAMREG9 & B0100_0000) != 0)
		{
			RAMREG9 |= B0000_0100;	//上电补存日冻结电量
			RAMREG9 &= B1011_1111;	//清临时标志
		}
		if((RAMREG9 & B0000_0100) == 0)
		{
			if(Eeprom.Buffer[6] == Sclock.Clockbackup[2]) //时
			{
				if(CH == 1)
				{
					if(Eeprom.Buffer[7] == 0)	RAMREG9 |= B0000_0100;	//分, 日冻结标志
					else	RAMREG9 |= B0100_0000;	//日冻结临时标志
				}
				else
				{
					if(Sclock.Clockbackup[1] < Eeprom.Buffer[7]) RAMREG9 |= B0100_0000;	//日冻结临时标志
				}
			}
		}
		
		//定时冻结判断
		if((RAMREG9 & B1000_0000) != 0)
		{
			RAMREG9 |= B0000_1000;	//上电补存定时冻结电量
			RAMREG9 &= B0111_1111;	//清临时标志
		}
		if((RAMREG9 & B0000_1000) == 0)
		{
			for(i=0; i<3; i++)
			{
				if(Eeprom.Buffer[8+i]== 0x99) Eeprom.Buffer[8+i] = Sclock.Clockbackup[4-i];
			}
			if( (Eeprom.Buffer[ 8] == Sclock.Clockbackup[4]) &&
			    (Eeprom.Buffer[ 9] == Sclock.Clockbackup[3]) &&
				(Eeprom.Buffer[10] == Sclock.Clockbackup[2]) )
			{
				if(CH == 1)
				{
					if(Eeprom.Buffer[11] == 0)	
					{
						RAMREG9 |= B0000_1000;	//上电补存定时冻结电量
						RAMREG9 &= B0111_1111;	//清临时标志
					}
					else	RAMREG9 |= B1000_0000;	//定时冻结临时标志
				}
				else
				{
					if(Sclock.Clockbackup[1] < Eeprom.Buffer[11]) RAMREG9 |= B1000_0000;	//定时冻结临时标志
				}				
			}
		}
	}
	else//上电补存判断
	{
		if( Eeprom.Buffer[5] == 0x30 )	//整点冻结间隔时间是30min
		{    			
			if( (RAMREG9 & B0010_0000) != 0 )//掉电时已过启动时间
			{
				if( (RAMREG1 < 30) && (Sclock.Clockbackup[1] >= 0x30) )	//同时掉电分钟小于30min,上电分钟大于30min
				{
					RAMREG9 |= B0000_0010;//整点冻结
				}
			}
			else if( (Cmp_Clock( Eeprom.Buffer, 5 ) != 0x01) ) //上电后过启动时间
			{
				if( (Eeprom.Buffer[4] <= 0x30) && (Sclock.Clockbackup[1] >= 0x30) )	//启动时间分钟小于30min,上电分钟大于30min
				{
					RAMREG9 |= B0000_0010;//整点冻结
				}
			}
		}
		
		if((RAMREG9 & B0000_0010) != 0)
		{
			Hour_Freeze_Dl(ee_hourfreezedl_page,ee_hourf_inpage);//整点冻结
		}
			
		if(((((RAMREG9 & B0100_0000) != 0) && (Sclock.Clockbackup[1] >= Buf[7])))	|| ((RAMREG9 & B0000_0100) != 0))
		{
			Freeze_Dl(ee_dayfreezedl_page,ee_dayf_inpage); //日冻结
		}
		
		if(((((RAMREG9 & B1000_0000) != 0) && (Sclock.Clockbackup[1] >= Buf[11]))) || ((RAMREG9 & B0000_1000) != 0))
		{
			Freeze_Dl(ee_timingfreeze_dl_page,ee_timingf_inpage);//定时冻结
		}
		
		RAMREG9 = 0;
	}
}


void Init_Dayfreeze_Judge(void)	//日冻结补存判断
{
	unsigned char i, n, k,result,result1,result2;
	unsigned char Buf[50],BufA[6],BufB[6],BufC[6];
	const unsigned char code days[] = { 0, 0x31, 0x28, 0x31, 0x30, 0x31, 0x30, 0x31, 0x31, 0x30, 0x31, 0x30, 0x31 };
	
	if(clock_error) return;	//当前时间非法
	
	I2C_Read_Eeprom44( ee_pre_dayfreeze_page, BufB, ee_predayfreeze_lenth );//读上一次日冻结时间(分时日月年)
	if( JudgeClockNoWeek( 1, BufB, 5 ) ) return;	//时间非法
											
	memcpy( BufA, Sclock.Clockbackup+1, 5 );//当前时间(分时日月年)
	
	result = Cmp_Bcd_Inv( BufA, BufB, 5 );//比较分时日月年5字节
	if( result != 1 ) return;//A=B或者A<B,返回
	
	I2C_Read_Eeprom44( ee_freezeparameter_page, Buf, ee_freezeparameter_lenth );//读日冻结时间

	//组成每次日冻结时间
	memcpy( BufC, BufA, 5 );//当前时间
	BufC[0] = Buf[7];//日冻结分
	BufC[1] = Buf[6];//日冻结时
	
	n = 0;
	for( i=0; i<8; i++ )
	{
		result1 = Cmp_Bcd_Inv( BufB, BufC, 5 );
		result2 = Cmp_Bcd_Inv( BufA, BufC, 5 );
		
		//BufB<BufC<=BufA补
		if((result1==2)&&(result2!=2))
		{
			n++;//跨日冻结次数加1
			memcpy( Buf+n*5, BufC, 5 );//冻结时间
		}
		
		//冻结时间加1日
		if( BufC[2] == 0x01 )//比较当前日是否为1
		{
			if( BufC[3] == 0x01 )
			{
				BufC[4] = Hex_To_Bcd(Bcd_To_Hex(BufC[4])-1);//年减1
				BufC[3] = 0x12;//12月
				BufC[2] = 0x31;//31日
			}
			else
			{
				BufC[3] = Hex_To_Bcd(Bcd_To_Hex(BufC[3])-1);//月减1
				//闰年判断
				k = days[Bcd_To_Hex(BufC[3])];
				if( (BufC[3] == 0x02) && ((Bcd_To_Hex(BufC[4])%4)==0 ) ) k++;	//闰年
				BufC[2] = k;//最大日
			}	
		}
		else
		{
			BufC[2] = Hex_To_Bcd(Bcd_To_Hex(BufC[2])-1);//日减1
		}
	}
	
//	if( n > 7 ) n = 7;
//	
//	if( n )
//	{
//		memcpy( Buf, Sclock.Clockbackup+1, 5 );//保存当前时间(分时日月年)
//		
//		for(i=0; i<n; i++)
//		{
//			memcpy( Sclock.Clockbackup+1, Buf+(n-i)*5, 5 );//冻结时间
//			Freeze_Dl(ee_dayfreezedl_page,ee_dayf_inpage); //日冻结补存
//		}
//	
//		memcpy( Sclock.Clockbackup+1, Buf, 5 );//恢复当前时间(分时日月年)
//	}
	
	if( n ) Freeze_Dl(ee_dayfreezedl_page,ee_dayf_inpage); //日冻结补存(南网要求上电补一次)
		
}

void Wr_Pretime_Dayfreeze(void)//日冻结写/清零写/校时写/设置日冻结时间写
{
	I2C_Write_Eeprom44( ee_pre_dayfreeze_page, Sclock.Clockbackup+1, ee_predayfreeze_lenth );//写上一次日冻结时间
}


void Judge_Relay_Delay(void)	//大电流延时跳闸判断
{
//	if( Relay_Mode != 0x55 )//内置继电器
	{
		if( over_xxx_flag && CurrentTimer.B32 )//拉闸时电流大于限值延时跳闸
		{
			if( relay_commu_open_flag && (Opendelaytimer.B32 == 0x00) ) relay_delay_flag = 1;//置大电流延时跳闸标志
			else if( BDMODE && (!relay_commu_close_flag) && (charge_relay_open_flag || relay_dl_status_flag) ) relay_delay_flag = 1;	//置大电流延时跳闸标志
			else
			{
				CurrentTimer.B32 = Calculate_Timer(1);
				relay_delay_flag = 0;	//清大电流延时跳闸标志
			}
		}
		else relay_delay_flag = 0;	//清大电流延时跳闸标志
	}
//	else	relay_delay_flag = 0;	//清大电流延时跳闸标志
}				


void Closerelay1(void)               //闭合继电器
{
	if( Relay_Mode != 0x55 )//内置继电器
	{
		Relayctrl.Timer = Relaytimer_Delay_N;
		Powertimer.Timer = Powertimer_Delay_N;
		RELAYOFF1;
		__NOP();
		__NOP();
		RELAYON2;
	}
	else
	{
		if(Mode.Feature1&0x01)//脉冲方式
		{
			Relayctrl.Timer = 0;
			RELAYOFF3;
		}
		else
		{
			RELAYOFF3;
		}
	}
}

void Openrelay1(void)
{
	if( Relay_Mode != 0x55 )//内置继电器
	{
		Relayctrl.Timer = Relaytimer_Delay_N;
		RELAYOFF2;
		__NOP();
		__NOP();
		RELAYON1;
	}
	else
	{
		if(Mode.Feature1&0x01)//脉冲方式
		{
			Relayctrl.Timer = Relaytimer_Delay_W;
			RELAYON3;	
		}
		else
		{
			RELAYON3;
		}
	}
}

void Init_Metererrstatus(void)	//清零继电器故障状态
{
	relay_error = 0;
	Relayerror_Count  = 0;
	Relayerror_Count1 = Defrelayerrorcnt_s;//赋值起始次数
	Relayerror_Count2 = Defrelayerrorcnt_s;//赋值起始次数
	relay_failed_flag = 0;	//清继电器错误标志(上报)
	Store_Meterstatus();
}


void Closerelay(void)               //闭合继电器(带写状态字)
{
	if( (relay_status_flag == 0) && (relay_direct_flag == 0) ) return;
	
	Closerelay1();
	if( relay_status_flag == 1 )
	{
		if( relay_error ) Note_Run_Event(ee_relayerrn_inpage|0x80);	//负荷开关异常结束记录
		relay_status_flag = 0;
		relay_success_flag = 0;	//清继电器成功标志
		Relay_modechange_flag = 0;
		Init_Metererrstatus();	//清零继电器故障状态
		Note_Relay_Event();//做合闸记录
	}
	Relay_Space_Timer = 0;	
}



void Openrelay(void)                //断开继电器(带写状态字)	
{
	if( relay_status_flag && (relay_direct_flag ==0 ) )  return;
	
	Judge_Relay_Delay();	//大电流延时跳闸判断
	if( relay_delay_flag ) return;	//大电流延时拉闸
	
	Openrelay1();
	if( relay_status_flag == 0 ) 
	{
		if( relay_commu_open_flag )
		{
			Opendelaytimer.B32 = 0x00;	//已经拉闸则清拉闸延时
		}
		else if( (BDMODE) && (charge_relay_open_flag || relay_dl_status_flag) )
		{
			memset( Eeprom.Buffer, 0xff, 4 );	//设置操作者代码为0xff
			I2C_Write_Eeprom( ee_operatorcode_page, ee_operatorcode_inpage, Eeprom.Buffer, 4 );
		}
		
		if( relay_error ) Note_Run_Event(ee_relayerrn_inpage|0x80);	//负荷开关异常结束记录
		relay_status_flag = 1;
		relay_success_flag = 0;	//清继电器成功标志
		Relay_modechange_flag = 0;
		if( Relay_Mode == 0x55 ) relay_cleardl_flag = 0; //清外置表刚清零标志
		Init_Metererrstatus();	//清零继电器故障状态
		Note_Relay_Event();//做拉闸记录
	}
	Relay_Space_Timer = 0;
}


void Alarmstatus_Judge(void)	//电表报警状态判断
{
	unsigned char Result1=0;
	
	Remainmoney_Verify();	//剩余金额参数校验
	Money_Para_Verify();	//赊欠金额,囤积金额等参数校验

//	RTCIE = 0;//防止液晶会闪烁一下
	Meter_Status  &=   B1000_1000;
	if( BDMODE ) //本地模式
	{
		if( ChkBufZero( &Money.Remain[2], 4 ) )//剩余金额大于0
		{
			remainder_tick_flag = 0;//清赊欠标志
			
			Result1 = Cmp_DataLH( &Money.Remain[2],  Esam_Para.Alarm_Limit1.B08,4 );
			if( Result1 == 0x01 ) 
			{
				relay_dl_status_flag = 0;//闭合继电器
			}
			else
			{	
				Result1 = Cmp_DataLH( &Money.Remain[2], Esam_Para.Alarm_Limit2.B08, 4 );
				if( Result1 == 0x01 )
				{
					remainder_alarm1_flag = 1;
					relay_dl_status_flag = 0;//闭合继电器
				}
				else
				{
					remainder_alarm2_flag = 1;
					if ( card_closerelay_flag ) //外部置该标志,需要保存到数据ee
					{
						first_alaram2_flag =0;
						relay_dl_status_flag = 0;//闭合继电器
					}
					else
					{
						first_alaram2_flag =1;
						relay_dl_status_flag = 1;//断开继电器
					}
				}
			}
				
		}
		else //剩余金额小于等于0
		{
			remainder_zero_flag =1;
			if( ChkBufZero( Money.Tick, 4 ) )
			{
				if( remainder_tick_flag == 0 )
				{
					Set_Reportdata(1*8+1);//置透支状态上报标志
				}
				remainder_tick_flag = 1;//置赊欠标志
			}
			else remainder_tick_flag = 0;//清赊欠标志
			
			Result1 = Cmp_DataLH( &Money.Tick[2], Money_Para.Tick.B08, 4 );
			
			if( Result1 != 0x02 )
			{
				if(remainder_maxtick_flag == 0)
				{
					remainder_maxtick_flag = 1;//剩余金额到赊欠门限(首次)
					Store_Meterstatus();
				}				
				relay_dl_status_flag = 1;//断开继电器,到达赊欠门限或者赊欠门限等于0
			}
			else
			{			
				if ( card_closerelay_flag1 ) //外部置该标志,需要保存到数据ee
				{
					first_eqzero_flag =0;
					relay_dl_status_flag = 0;//闭合继电器
				}
				else
				{
					first_eqzero_flag =1;
					relay_dl_status_flag = 1;//断开继电器
				}
				
//				if( remainder_maxtick_flag )//发生过剩余金额到赊欠门限，则必须充值到合闸允许金额才能合闸
//				{
//					relay_dl_status_flag = 1;//断开继电器,到达赊欠门限或者赊欠门限等于0
//				}		
			}
			
//		if( remainder_maxtick_flag )//发生过剩余金额到赊欠门限，则必须充值到合闸允许金额才能合闸
//		{
//			first_alaram2_flag =0;
//			first_eqzero_flag =0;
//		}		
		}
		
//		if( charge_relay_open_flag )//剩余金额小于等于合闸允许金额
//		{
//			first_alaram2_flag =0;
//			first_eqzero_flag =0;
	//	}		
	}
//	RTCIE = B0000_0001;
}


void Mt_Alarmstatus_Judge(void)	//电表报警状态判断
{
	Esam_Para_Verify();	//报警金额1,2等参数(ESAM一类部分参数)校验
	Alarmstatus_Judge();	//电表报警状态判断

	if( FLAG_EA && ( below_70u_flag == 0 ) )
	{
		//继电器控制
		if ( relay_commu_disable_close_flag )//不允许闭合继电器(只有插卡才可以清零)
		{
			Openrelay();
		}
		else if ( relay_commu_close_flag )//保电优先级最高
		{
			Closerelay();
		}
		else if( (relay_commu_open_flag && (Opendelaytimer.B32 == 0x00)) && (self_close_flag == 0) )//通讯断开继电器优先级次之
		{
			Openrelay();
		}
		else if( (BDMODE) && (charge_relay_open_flag || relay_dl_status_flag)&& (Festival_holiday_flag == 0) )//购电不闭合继电器标志, 电量决定继电器断开
		{
			Openrelay();
		}
		else//电量决定继电器闭合
		{
			Closerelay();
		}
		
		relay_direct_flag = 0;//清继电器直接动作标志
	}
	
	run_alarm_flag = 0;		
}
