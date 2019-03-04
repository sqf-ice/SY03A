//RTC function
#include "ex_var.h"
#include "ex_func.h"
#include <string.h>
//#include <intrins.h>

/*============================================================================*
	函数：unsigned char Add_Bcd( unsigned char  *sr, unsigned char  *de, unsigned char len )
	功能：	BCD码加法。
	
*============================================================================*/	

unsigned char Add_Bcd( unsigned char  *sr, unsigned char  *de, unsigned char len )
{
  unsigned char  i = 0;
	unsigned int sum = 0;
	unsigned char temp  = 0;
	unsigned char data1 = 0;
	unsigned char data2 = 0;
	
	while(len--)
	{
		data1 = sr[i];
		data2 = de[i];
		sum += (unsigned int)data1 + (unsigned int) data2;
		
		temp = (data1&0xF0) + (data2&0xF0);
		if((temp != (sum&0xF0)) || ((sum&0x0F) > 0x09))
		{
			sum += 0x06;	
		}
		if(sum >= 0xa0)
		{
			sum += 0x60;	
		}
		sr[i] = (unsigned char)sum;
		sum >>= 8;
		i++;
	}
  return(sum);	
}


unsigned char Lib_BcdArray_Inc(unsigned char *sr,unsigned char len)
{
	unsigned char byte = 1;
	while(len--)
	{
		if(byte == 1)
		{
			if(*sr == 0x99)
			{
				*sr = 0x00;
			}
			else
		{
				byte = 0;
				if((*sr&0x0f) == 0x09)
				{
					*sr = *sr + 0x10;
					*sr&=0xF0;
				}
				else
				{
					*sr = *sr + 0x01;	
				}
			}
		}
		sr++;
	}
	return(byte);	
}
/*============================================================================*
	函数：unsigned char Add_Bcd( unsigned char  *sr, unsigned char  *de, unsigned char len )
	功能：	BCD码减法。
	
*============================================================================*/		
void Sub_Bcd(unsigned char  *sr,unsigned char  *de,unsigned char len)
{       
  unsigned char i = 0;	
	unsigned char data[10];

	memmove(data,de,len);
	for(i=0;i<len;i++)
	{
		data[i] = 0x99 - data[i];	
	}
	Lib_BcdArray_Inc(data,len);
	
	Add_Bcd(sr,data,len);

}
/*============================================================================*
	函数：	void Hex2Bcd( unsigned char *Hexop, unsigned char *BCDopt, unsigned char Len );
	功能：	多字节HEX转换BCD
	参数：	*Hexop  HEX 初始值为待转化的高字节地址； 
			*BCDopt  BCD 初始值为待转化的高字节地址； 
			Len  长度，最大4个字节
  返回值:	无
*============================================================================*/	
	
void Hex2Bcd( unsigned char  *sr, unsigned char  *de, unsigned char len )
{
        unsigned char i = 0;
	unsigned char num = 0;
	unsigned char cbit = 0;
	unsigned char flag = 0;
	unsigned short  temp = 0;
	unsigned char hex[8] = {0,};
	unsigned char bcd[8] = {0,};
	
	memmove(hex,sr,len);
	for(num=0;num<len*8;num++)
	{
		cbit = 0;
		for(i=0;i<len;i++)
		{
			flag = 0;
			if((hex[i]&0x80) == 0x80)
			{
				flag = 1;
			}
			hex[i] = hex[i] << 1;
			if(cbit == 1)
			{
				hex[i] |= 0x01;
			}
			cbit = flag;
		}
		for(i=0;i<8;i++)
		{
			flag = 0;
			if((bcd[i]&0x80) == 0x80)
			{
				flag = 1;	
			}
			bcd[i] = bcd[i] << 1;
			if(cbit == 1)
			{
				bcd[i] |= 0x01;
			}
			cbit = flag;
		}
		if(num != (len*8-1))
		{
			for(i=0;i<8;i++)
			{			
				temp = (bcd[i]&0x0F) + 0x03;
				if(temp>0x07)
				{
					bcd[i] += 0x03;
				}
				temp = (bcd[i]&0xF0) + 0x30;
				if(temp>0x70)
				{
					bcd[i]+=0x30;
				}		
			}
		}
	}
	memmove(de,bcd,len);
}
/*==========================================================================*/
/*============================================================================*
	函数：	void Bcd2Hex( uint08 *BCDopt, uint08 *HEXopt, uint08 Len );
	功能：	多字节BCD转换HEX
	参数：	*BCDopt  BCD 初始值为待转化的高字节地址； 
			*HEXopt  HEX 初始值为待转化的高字节地址；
			Len  长度，最大4个字节
  返回值:	无
*============================================================================*/	
void Bcd2Hex( unsigned char  *sr, unsigned char  *dr, unsigned char len )
{       
	unsigned char i = 0;
	unsigned char num = 0;
	unsigned char cbit = 0;
	unsigned char flag = 0;
	unsigned short temp = 0;
	unsigned char hex[8] = {0,};
	unsigned char bcd[8] = {0,};
	
	memmove(bcd,sr,len);
	for(num=0;num<8*8;num++)
	{
		cbit = 0;
		for(i=len;i>0;i--)
		{
			flag = 0;
			if((bcd[i-1]&0x01) == 0x01)
			{
				flag = 1;
			}
			bcd[i-1] = bcd[i-1] >> 1;
			if(cbit == 1)
			{
				bcd[i-1] |= 0x80;
			}
			cbit = flag;
			}
		for(i=8;i>0;i--)
		{
			flag = 0;
			if((hex[i-1]&0x01) == 0x01)
			{
				flag = 1;
			}
			hex[i-1] = hex[i-1] >> 1;
			if(cbit == 1)
			{
				hex[i-1] |= 0x80;
			}
			cbit = flag;
		}
		if(num != (len*8-1))
		{
			for(i=0;i<8;i++)
			{			
				temp = bcd[i]&0x0F;
				if(temp>0x07)	
				{
					bcd[i] -= 0x03;
				}
				temp = bcd[i]&0xF0;
				if(temp>0x70)
				{
					bcd[i] -= 0x30;
				}
			}		
		}
	}
	memmove(dr,hex,len);
}
unsigned char Hex_To_Bcd( unsigned char value )
{
	return ( ((value/10)<<4)+(value%10) );
}

unsigned char Bcd_To_Hex( unsigned char value )
{
	return ( ((value&0xF0)>>4)*10+(value&0x0F) );
}

//============================================================================
unsigned char ChkBufZero( unsigned char *Buf,unsigned char Len )
{
	unsigned char i;
	
	for( i=0; i<Len; i++ )
	{
		if( Buf[i] )	return(1);	//非0
	}
	
	return(0);	//全0
}


//高字节在前的数据比较
unsigned char Cmp_Data( unsigned char *BufA, unsigned char *BufB, unsigned char Len )
{
	unsigned char i;

	for( i=0; i<Len; i++ )
	{
		if( BufA[i] > BufB[i] )
		{
			return 1; //A>B
		}
		else
		{
			if( BufA[i] < BufB[i] )
			{
				return 2; //A<B
			}
		}
	}

	return 3; //A=B
}
//??????????????
unsigned char Cmp_DataLH( unsigned char *BufA, unsigned char *BufB, unsigned char Len )
{
	unsigned char i;

	for( i=0; i<Len; i++ )
	{
		if( BufA[Len-1-i] > BufB[i] )
		{
			return 1; //A>B
		}
		else
		{
			if( BufA[Len-1-i] < BufB[i] )
			{
				return 2; //A<B
			}
		}
	}

	return 3; //A=B
}

//低字节在前的数据比较
unsigned char Cmp_Bcd_Inv( uint08 *BcdA, uint08 *BcdB, uint08 Len )
//return: 0-A=B; 1:A>B; 2:A<B
{
	unsigned char i;

	for( i=0; i<Len; i++ )
	{
		if( BcdA[Len-1-i] > BcdB[Len-1-i] )
		{
			return 1; //A > B
		}
		else
		{
			if( BcdA[Len-1-i] < BcdB[Len-1-i] )
			{
				return 2; //A < B
			}
		}
	}

	return 0; //A==B
}

//============================================================================
//分钟为起始字节的时钟比较程序
unsigned char Cmp_Clock( unsigned char *Buf, unsigned char Len )
{
	unsigned char i;
	
	for( i=0; i<Len; i++ )
	{
		if( Buf[ i ] > Sclock.Clockbackup[ Len-i ] )
		{
			return 1; //大于当前时间
		}
		else
		{
			if( Buf[ i ] < Sclock.Clockbackup[ Len-i ] )
			{
				return 2; //小于当前时间
			}
		}
	}

	return 3; //相等
}


uint08 Judge_Clock( uint08 Start, uint08 *Ck, uint08 Len )//周在中间
{
	unsigned char  i,j;
	uint08 code Max[]={ 0x59,0x59,0x23,0x6,0x31,0x12,0x99 }, code Min[]={ 0x00,0x00,0x00,0x00,0x01,0x01,0x00 };

	for( i=0; i<(Len&0x07f); i++ )
	{
		j=i+Start;
		if((j>=3)&&(Len&0x80))	j++;
		
		if( (Ck[i] > Max[j]) || (Ck[i] < Min[j]) ) return 1;//合法性检查错误
		if( IsBCD(Ck[i]) ) return 1;	//BCD码检查错误		   
	}
	return 0;//正确
}


unsigned char JudgeClockNoWeek( uint08 Start, uint08 *Ck, uint08 Len )
{
	return(Judge_Clock( Start, Ck, Len+0x80 ));
}


unsigned char Read_Rtc( unsigned char *RTC_Buf )
{
	unsigned char i, n, Result;
	unsigned char BufA[7], BufB[7];

	for( n=0; n<3; n++ )
	{
		Result  = RTC->LOAD;
		RTC_WaitForSynchro();
		BufA[0] = RTC->SEC;
		BufA[1] = RTC->MIN;
		BufA[2] = RTC->HOUR;
		BufA[3] = RTC->WEEK;
		BufA[4] = RTC->DAY;
		BufA[5] = RTC->MON;
		BufA[6] = RTC->YEAR;
    
		sub_delay(100);
		
//		Result  = RTC->LOAD;
//		RTC_WaitForSynchro();
		BufB[0] = RTC->SEC;
		BufB[1] = RTC->MIN;
		BufB[2] = RTC->HOUR;
		BufB[3] = RTC->WEEK;
		BufB[4] = RTC->DAY;
		BufB[5] = RTC->MON;
		BufB[6] = RTC->YEAR;

		for( i=0; i<7; i++ ) //比对两次时钟是否一致
		{
			if( BufA[i] != BufB[i] ) break;
		}

		if( i == 7 ) //两者一致, 表示读取成功
		{
			Result = Judge_Clock( 0, BufA, 7 ); //判断时钟是否合法
			if( Result == 0 ) //时钟合法
			{
				for( i=0; i<7; i++ )
				{
					RTC_Buf[i] = BufA[i];
				}
			
				return 0;
			}
		}
	}
	
	for( i=0; i<7; i++ )
	{
		RTC_Buf[i] = BufA[i];//即使错误也要更新时钟
	}
	return 1;
}


unsigned char Set_Rtc( unsigned char Start, unsigned char *Ck, unsigned char Len )
{
	unsigned char i, n, Result;
	unsigned char BufA[7],BufB[7];
	
	if( Start || (Len!=7) ) return 1; //入口参数非法

	Result = Judge_Clock( Start, Ck, Len );
	if( Result == 1 )
	{
		return 1;
	}

	if( Len < 7 )
	{
		Result = Read_Rtc( BufA );
		if( Result == 1 )
		{
			return 1;
		}
	}

	for( i=0; i<Len; i++ )
	{
		BufA[ Start+i ] = Ck[i];
	}
  
	//RTC_WriteProtection(DISABLE);
	  
	for( n=0; n<3; n++ )
	{
		RTC->PWD = RTCPWD_KEY;
    RTC->CE = RTCCE_SETKEY; 
		
		RTC->SEC 	= Ck[0];
		RTC->MIN	= Ck[1];
		RTC->HOUR	= Ck[2];
		RTC->WEEK	= Ck[3];
		RTC->DAY	= Ck[4];
		RTC->MON	= Ck[5];
		RTC->YEAR	= Ck[6];
	  
		RTC->PWD = RTCPWD_KEY;
    RTC->CE = RTCCE_CLRKEY; 
//		RTCLOAD = 0x55;
    RTC_WaitForSynchro();
//		I2c_Wait_5ms();//RTC有一个5拍的滤波，在此之前读时钟会取消置数
////		if( (RTCSTAT&1) == 1 )  //加载成功
		{
			Result = Read_Rtc( BufB );
			if( Result == 0 ) //读取成功
			{
				for( i=0; i<7; i++ )
				{
					if( Ck[i] != BufB[i] ) break;
				}

				if( i == 7 ) //当前值和设置值相等, 设置成功
				{
//					RTC_WriteProtection(ENABLE);
					return 0; //设置成功
				}
			}
		}
	}
//  RTC_WriteProtection(ENABLE);
	return 1; //设置失败
}


void Syn_Clock( void )	//同步主和备份时钟
{
	Sclock.Clockbackup[0] = Sclock.Clockmain[0];//周由中间存放到最后
	Sclock.Clockbackup[1] = Sclock.Clockmain[1];
	Sclock.Clockbackup[2] = Sclock.Clockmain[2];
	Sclock.Clockbackup[3] = Sclock.Clockmain[4];
	Sclock.Clockbackup[4] = Sclock.Clockmain[5];
	Sclock.Clockbackup[5] = Sclock.Clockmain[6];
	Sclock.Clockbackup[6] = Sclock.Clockmain[3];
}


void Read_Sysclock( void )
{
	unsigned char Result,Buf[10];

	run_readrtc_flag = 0;	//清刷新时钟标志 
	
	Result = Read_Rtc( Sclock.Clockmain );//读时钟
	if( Result == 1 )
	{
		I2c_Wait_5ms(); 	//延时
		Result = Read_Rtc( Sclock.Clockmain );//再读时钟
		if( Result == 1 )
		{
			if( clock_error == 0 ) 
			{
				Set_Reportdata(0*8+7);//置时钟错误上报标志
				clock_error = 1;//置时钟错误标志
				memcpy( Buf, Sclock.Clockbackup, 3 );
				Buf[3] = Sclock.Clockbackup[6];
				memcpy( Buf+4, Sclock.Clockbackup+3, 3 );
				Set_Rtc( 0, Buf, 7 );//把ram里正确的时钟写回rtc
				return;
			}
		}
	}
	if( Result == 0 ) clock_error = 0;//清时钟错误标志
		
	if(FLAG_EA)
	{	
		if ( Sclock.Clockbackup[0] != Sclock.Clockmain[0] )//秒改变
		{
						
			if( Keyswitch_Valid != 0x55 )	//卡槽无触点，1s寻卡一次
			{
				if( Find_Card != 0xaa ) rf_card_flag = 1;	//每秒寻卡一次
			}
			else
			{
				if( Find_Card == 0x55 ) rf_card_flag = 1;	//每秒寻卡一次
			}
			
			if( Sclock.Clockmain[0]&B0000_0001 ) read_uip_flag1 =1;		//每秒交替读取I1,I2,U (80ms)
			if( Sclock.Clockmain[0] == 0x10 ) fwdl_veri_flag = 1;		//正向电量校验
			if( Sclock.Clockmain[0] == 0x15 ) bkdl_veri_flag = 1;		//反向电量校验
			if( Sclock.Clockmain[0] == 0x20 ) mode_veri_flag = 1;		//特征模式字校验
			if( Sclock.Clockmain[0] == 0x25 ) reg_veri_flag = 1;		//寄存器校验
			if( Sclock.Clockmain[0] == 0x30 ) addr_veri_flag = 1;		//通讯地址校验
			if( Sclock.Clockmain[0] == 0x35 ) disppm_veri_flag = 1;		//显示参数校验
			if( Sclock.Clockmain[0] == 0x40 ) dispitem_veri_flag = 1;	//显示项目校验
			
//			if( Sclock.Clockmain[0] == 0x42) init_7051_flag = 1;		//初始化寄存器
			if( Sclock.Clockmain[0] == 0x46) check_7051_flag1 = 1;		//校验校表寄存器
			if( Sclock.Clockmain[0] == 0x55) temperpara_veri_flag = 1;	//温度系数校验
			if( Sclock.Clockmain[0] == 0x02 ) Dm_process_flag= 1;//每分钟的第2秒计算需量
			
			
			//关闭载波复位和事件上报功能
			//if( progenable && (F_Plcevtout_Sta == 0x55) ) EVENOUTON;//事件上报
			//else if( progenable && (F_Plcevtout_Sta == 0xaa) ) EVENOUTOFF;//无事件上报输出低
			//else
			//{
//				if( ChkBufZero( Report_Data, 5 ) )
//				{
//					EVENOUTON;		//事件上报
//				}
//				else
//				{
//					EVENOUTOFF;		//无事件上报输出低
//				}
//			}
		if( PLC_RST1_Timer == 0 )
		  PLC_RST1;	
			//身份认证时效
			if( Identitytimer.B32 != 0 )
			{
				Identitytimer.B32--;
	
				if( Identitytimer.B32 == 0 )
				{
					Clear_Rand_Gene();
					FailKey();	//更新密钥失败处理
				}	
			}
			
			//跳闸延时时间
			if( Opendelaytimer.B32 != 0 ) //继电器跳闸延时
			{
				Opendelaytimer.B32--;
				if( Opendelaytimer.B32 == 0 ) run_alarm_flag = 1;	//延时到置标志
			}
						
			Judge_Relay_Delay();	//大电流延时跳闸判断
		 	if( relay_delay_flag )
		 	{
		 		CurrentTimer.B32--; //大电流延时拉闸定时器减1
		 		if( CurrentTimer.B32 == 0 ) run_alarm_flag = 1;	//延时到置标志
		 	}
			
			//红外认证时效
			if( IR_EnTimer )
			{
				IR_EnTimer--;
				if( IR_EnTimer == 0 ) identity_ir_flag = 0;	//清红外请求标志
			}
			
			//上报自动复位时间
			if( Reset_Rep_Timer )
			{
				Reset_Rep_Timer--;	//上报复位定时器减1
				
				if( Reset_Rep_Timer == 0 )
				{
					Xor_Data( Report_Data, Report_Data2, 5 );//保留30min内新的上报
					for( Result=0; Result<5; Result++ ) Clr_Reportnum(Result);//清零新增上报次数
				}
			}
			//定时8M时钟调校功能(计量uart专用)
			if( Adj_Clk_Timer_S )
			{
				Adj_Clk_Timer_S--;	//定时8M时钟调校定时器减1s
				if( Adj_Clk_Timer_S == 0 )  Clk_Adj_Proc();//根据外部的32K晶体来调整内部8M的偏差
			}
			
//			//扩展功能
//			if( (Key_Status==1) && (esam_error==0) )
//			{
//				progenable = 0;	//编程禁止	
//			}
			Lcdled_Control();	//背光控制	
//			if( FDETFLAG )	//xtlf发生停振
			{
//				FDETFLAG = 0;
				Result = I2C_Read_Eeprom( ee_xtlfstop_time_page, ee_xtlfstoptime_inpage, Buf, ee_xtlfstoptime_lenth );
				if( (Result || (ChkBufZero(Buf,ee_xtlfstoptime_lenth)==0)) && (clock_error==0) ) //判断是否发生记录
				{
					//记录晶振停振的发生时刻
					I2C_Write_Eeprom( ee_xtlfstop_time_page, ee_xtlfstoptime_inpage, Sclock.Clockbackup, ee_xtlfstoptime_lenth );
				}
			}
			
			//开盖延时定时器减1
			if( Cover_Space_Timer != 0 ) Cover_Space_Timer--;		 
		}
		if ( Sclock.Clockbackup[0] > Sclock.Clockmain[0] )//分钟改变
		{
			if( UpdateMoneytimer != 0 ) 	//更新ESAM钱包定时器减1
			{
				UpdateMoneytimer--;
				
				if( UpdateMoneytimer == default_updatemoneytimer )
				{
					UpdateMoneytimer = 0;
				}
			}
		 	
		 	if( Sclock.Clockmain[1]&B0000_0001 )
			{
				GetKeyStatus();//读当前密钥状态(2min一次)
			}
		 	
		 	if( relay_error || (Sclock.Clockmain[1]==0x30) ) relay_direct_flag = 1;
		 	
 		 	if( load_over_flag ) 
 		 	{
 		 		Keeptime.Times[1].B32++; 	//电流过载总持续时间加1分钟
 		 	}
 		 	if( low_voltage_flag )  
 		 	{
 		 		Keeptime.Times[2].B32++;	//电池欠压持续时间加1分钟
 		 	}
 		 	if( below_60u_flag )  
 		 	{
 		 		Keeptime.Times[3].B32++;	//掉电总持续时间加1分钟
 		 	}
			
			//定时8M时钟调校功能
			if( Sclock.Clockmain[1]==0x00 )
			{
				if( Adj_Clk_Timer )
				{
					Adj_Clk_Timer--;	//定时8M时钟调校定时器减1h
					if( Adj_Clk_Timer == 0 )  Clk_Adj_Proc();//根据外部的32K晶体来调整内部8M的偏差
				}
			}
		 	
		 	//扩展功能
		 	if(( Sclock.Clockmain[1]== 0x00)&&( Sclock.Clockmain[2]== 0x20))	//每天20:00一次
			{	
				if((Mode.Plc==0x0a)||(Mode.Plc==0xaa))
				{
					Resetworking_Timer = resetworking_timer_delay;//启动复位脉冲
					PLC_RST0;		//复位载波(输出低)
				}
			}

			if( (!BDMODE) && (SELFCLOSE == 0x55) )	//自保电功能
			{
				if(relay_commu_open_flag && (Opendelaytimer.B32 == 0x00))//收到拉闸命令并且已经拉闸
				{
					Result = ReadE2WithBackup( ee_hearttime_page, ee_hearttime_inpage, Buf, ee_hearttime_lenth );
					if( Result ) Buf[0] = default_hearttime;
						
					Self_Timer3++;
					if(Self_Timer3 >= (Buf[0] + 5)) Self_Cnt = 0;
					
					if( Self_Timer24 ) 
					{
						Self_Timer24--;
						if( Self_Timer24 == 0x00 ) Self_Cnt = 0;
					}
	
					if((self_close_flag == 0) && (Self_Timer3 >= (3*Buf[0] + 5))) //连续3次没有收到自检帧
					{
						self_close_flag = 1;
						Self_Timer24 = 24*60;		//启动自保电1天延时
						Store_Meterstatus();
					}
				}
			}
			else self_close_flag = 0;
		}
		if ( Sclock.Clockbackup[2] > Sclock.Clockmain[2] )//小时改变
		{		
		  Festival_holiday_Judge(); //启动节假日判断		
	  }
	}
	
	if ( Sclock.Clockbackup[0] > Sclock.Clockmain[0] )//分钟改变(防止在零点附近出现缓升缓降)
	{
		run_freeze_flag = 1;	//冻结和结算判断
		
		if(( Sclock.Clockmain[1]== 0x00)&&( Sclock.Clockmain[2]== 0x00))	//每天00:00一次
		{
			Clr_Zero_Data();	//零点清零相关数据
		//	Find_Card = 0x00;	//结束寻卡模式
		}	
	}	
		
	Syn_Clock();//同步主和备份时钟	 
}
