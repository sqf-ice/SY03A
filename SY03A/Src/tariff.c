#include "ex_func.h"
#include "ex_var.h"
//#include <intrins.h>
#include <string.h>
#include <absacc.h>



void RefreshPrice( void )
{
	run_price_flag = 1;
	Price.Crc16a.B16 += 1;
}


//offset=0时区,offset=1时段,offset=2费率,offset=3阶梯,offset=4阶梯时区
void Changecontrol_Process(unsigned char offset)//翻转控制字节,清启动时间和进行约定冻结
{
	unsigned char i;
	
	Clear_Wdt();
	
	if( offset < 2 )
	{
		//翻转控制字
		I2C_Read_Eeprom44( ee_changecontrol_page, Eeprom.Buffer, ee_changecontrol_lenth);//读启动控制字节
		if (Eeprom.Buffer[offset] == 0x55)
		{
			Eeprom.Buffer[offset]=0xff;
		}
		else
		{	
			Eeprom.Buffer[offset]=0x55;
		}
		
		I2C_Write_Eeprom44( ee_changecontrol_page, Eeprom.Buffer, ee_changecontrol_lenth);
	}
	else if( offset == 2 )
	{
		I2C_Read_Eeprom ( ee_tariffprice2_page, 0, Price.Tariff[0].B08, ee_tariffprice_lenth);//读备份费率电价覆盖当前
		I2C_Write_Eeprom( ee_tariffprice1_page, 0, Price.Tariff[0].B08, ee_tariffprice_lenth);
	}
	else if( offset == 3 )
	{	
		for( i=0; i<4; i++ )
		{
			ReadE2WithJieti ( 4+i, Price.Ladder_Dl[0].B08 );//读备份套阶梯电量/电价/年结算时间覆盖当前
			WriteE2WithJieti( i, Price.Ladder_Dl[0].B08 );
		}
		RefreshPrice();
	} 
	else if( offset == 4 )
	{
		I2C_Read_Eeprom ( ee_jtshiqu2_page, ee_jtshiqu_inpage, Eeprom.Buffer, ee_jtshiqu_len ); //读备份阶梯时区表覆盖当前
		I2C_Write_Eeprom( ee_jtshiqu1_page, ee_jtshiqu_inpage, Eeprom.Buffer, ee_jtshiqu_len );
		RefreshPrice();
	}
	else return;
	
	//清启动时间
	memset( Starttime.Shiqu+offset*5, 0x00, 5 );	
	WriteE2WithBackup(ee_starttimer_page, ee_starttimer_inpage, Starttime.Shiqu, ee_starttimer_lenth);
	
	//进行约定冻结
	if( offset >= 4 )
		Freeze_Dl(ee_jtsqswith_note_page+4*(offset-4),5+offset);
	else
		Freeze_Dl(ee_appointfreeze1_page+4*offset,5+offset);
}


unsigned char FindSQ( unsigned char Num )
{
	unsigned char i, k, n;

	Sequence( Eeprom.Buffer, Num );//时段表排序
	
	for( i=0, k=0, n=0; i<Num; i++ )
	{
		if( (n != 0xff) && (Cmp_Bcd_Inv(Eeprom.Buffer+k+1,Sclock.Clockbackup+3,2) == 1) )
		{
			if( i == 0 ) n = 0xff; //小于第一时区,则为最后时区
			else break;
		}

		k += 3;
		if( ChkBufZero(Eeprom.Buffer+k+1, 2) == 0 ) break; //后面月+日为零,无效
	}
	
	return (Eeprom.Buffer[k-3]);//得到时段表号
}

void Festival_holiday_Judge(void)
{	
  unsigned char i,t,k; 
	
	k=0;Festival_holiday_flag = 0;
	
	t = Bcd_To_Hex(Feik.Num[4]);
	//法定节假日
	if( t <= max_maxholiday )//最大支持60个节假日
	{
		for( i=0; i<t; i++ )
		{
			if( (i%15) == 0 )
			{
				k = 0;
				if( I2C_Read_Eeprom( ee_holiday_page+(i/15), 0x00, Eeprom.Buffer, ee_holiday_lenth ) )
				{
					I2C_Read_Eeprom( ee_holiday_page+(i/15), 0x00, Eeprom.Buffer, ee_holiday_lenth );//再读一次
				}
			}
			
			if( Cmp_Data( Eeprom.Buffer+1+k, Sclock.Clockmain+4, 3 ) == 3 ) //比较日月年
			{
				Festival_holiday_flag = 1;
				break;
			}
			k += 4;
		}
	}
	//周修日判断
	if((Sclock.Clockmain[3]==0 )||(Sclock.Clockmain[3]==6 ))  Festival_holiday_flag = 1;

}




void Config_Shiduan_Process(void)//找到当前时区得到当前时段表
{
	unsigned char t,n,page;
	unsigned char Buf[16];
	
	I2C_Read_Eeprom44( ee_changecontrol_page, Buf+7, ee_changecontrol_lenth);//读启动控制字节
	if( Buf[7] != 0x55 ) page = ee_shiqu1_page;//第一套为当前时区
	else page = ee_shiqu2_page;//第二套为当前时区
	if ( I2C_Read_Eeprom( page, 0, Eeprom.Buffer, ee_shiqu_lenth )!= 0x00)
	{
		I2C_Read_Eeprom( page, 0, Eeprom.Buffer, ee_shiqu_lenth);
	}
	//时区判断(时段表号+日+月)
	t = Bcd_To_Hex(Feik.Num[0]);
	if( (t == 0) || (t > 14) ) t = 1;
	n = FindSQ(t);	//得到时段表号
	
	if( Buf[8] != 0x55 ) page = ee_shiduantable1_1_page;//第一套为当前时段表
	else page = ee_shiduantable2_1_page;//第二套为当前时段表

	//周休日判断
	if( Sclock.Clockmain[3] <= 6 )//如果是非法的周就不判断周休日
	{
		if( ((Week_Holiday.Status>>Sclock.Clockmain[3]) & B0000_0001)==0 )//0代表休息,1代表工作
		{
			n = Week_Holiday.Shiduan_N;
		}
	}
	//节假日判断
//	t = Bcd_To_Hex(Feik.Num[4]);
//	if( t <= max_maxholiday )//最大支持60个节假日
//	{
//		for( i=0; i<t; i++ )
//		{
//			if( (i%15) == 0 )
//			{
//				k = 0;
//				if( I2C_Read_Eeprom( ee_holiday_page+(i/15), 0x00, Eeprom.Buffer, ee_holiday_lenth ) )
//				{
//					I2C_Read_Eeprom( ee_holiday_page+(i/15), 0x00, Eeprom.Buffer, ee_holiday_lenth );//再读一次
//				}
//			}
//			
//			if( Cmp_Data( Eeprom.Buffer+1+k, Sclock.Clockbackup+3, 3 ) == 3 ) //比较日月年
//			{
//				n = Eeprom.Buffer[k];
//				break;
//			}

//			k += 4;
//		}
//	}
	if( (n == 0)||(n > 8) ) n=1; 
	n--;
	Tariff.Current_N = n;
	I2C_Read_Eeprom( page+n, 0x00, Tariff.Table, ee_shiduantable_lenth);	
}


//根据阶梯时区表确定阶梯表
void Config_ShiQuLad_Proc( void )
{
	unsigned char n;

	I2C_Read_Eeprom ( ee_jtshiqu1_page, ee_jtshiqu_inpage, Eeprom.Buffer, ee_jtshiqu_len ); //读阶梯时区表
	
	//时区判断(阶梯表号+日+月)
	n = FindSQ(4);	//得到阶梯表号
	
	//读当前阶梯表
	if( (n == 0) || (n > 2) ) n = 1;
	n--;
	Tariff.CurLad_N = n;
	ReadE2WithJieti( Tariff.CurLad_N, Price.Ladder_Dl[0].B08 ); //读当前套阶梯电量/电价/年结算日
}


void Sequence( unsigned char *Buf, unsigned char Num )//费率+分钟+小时
{
	unsigned char i, n;
	unsigned char BufB[3];

	if( Num < 2 ) return;
	
	for( n=0; n<Num-1; n++ )
	{
		for (i=0; i<Num-1-n; i++)
		{
			if ( Cmp_Bcd_Inv( Buf+i*3+1, Buf+(i+1)*3+1, 2 ) == 0x01 ) //前数据 > 后数据
			{
				memcpy( BufB, Buf+i*3, 3 );//复制数据
				memcpy( Buf+i*3, Buf+(i+1)*3, 3 );//复制数据
				memcpy( Buf+(i+1)*3, BufB, 3 );//复制数据
			}
		}
	}
}
	

void Timezone_Judge(void)	//时区时段费率判断
{
	unsigned char i,t,Result,Status=0;
	unsigned char BufA[42];

	feik_Num_Verify();
	if( run_timezone_flag && ( FLAG_EA == 0 ) ) return;
	Starttimer_Verify();//启动时间校验
	if( run_timezone_flag && ( FLAG_EA == 0 ) ) return;
	Week_Holiday_Verify();//周休日参数校验
	
	run_timezone_flag = 0;
	if( BDMODE ) //本地模式
		run_price_flag = 1;
	else
		run_alarm_flag = 1;

	//启动时间判断
	if( !clock_error ) 
	{
		if((Cmp_Clock( Starttime.Shiqu, 5 ) != 0x01) && ChkBufZero( Starttime.Shiqu, 5 ))
		{
			Changecontrol_Process(0);//翻转控制字节,清启动时间和进行约定冻结
			Status = 0x01;//重新判断时段表
		}
		if((Cmp_Clock( Starttime.Shiduan, 5 ) != 0x01) && ChkBufZero( Starttime.Shiduan, 5 ))
		{
			Changecontrol_Process(1);//翻转控制字节,清启动时间和进行约定冻结
			Status = 0x01;//重新判断时段表
		}
	}
	//时段校验
	if( Status == 0x01 || Sclock.Clockbackup[1]==0 )//启动时间到或者每小时
	{	
		Config_Shiduan_Process();//找到当前时区得到当前时段表
	}
	else if( Crc_Check( Tariff.Table, ee_shiduantable_lenth+2 ) ) 
	{
		Config_Shiduan_Process();//找到当前时区得到当前时段表
	}
	//费率判断(费率+分钟+小时)
	t = Bcd_To_Hex(Feik.Num[2]);
	if ( (t == 0) || (t > max_shiduancount) ) t=1;
	memcpy( BufA, Tariff.Table, 42 );//保存时段表
	Sequence( BufA, t );//时段表排序
	t = t*3;//时段数*3
	for (i=0; i<t; i++)
	{
		if ( ((BufA[i+2] > Sclock.Clockbackup[2])) 
			|| ((BufA[i+2] == Sclock.Clockbackup[2]) 
			&& (BufA[i+1] > Sclock.Clockbackup[1])) ) 
		{
			if( i == 0 ) i=t;//小于第一时段,则为最后时段
			i = i - 3;
			break;
		}
//		if ( (i >=3) && (BufA[i]==0) && (BufA[i+1]==0) )
//		{
//			i = i - 1;//后面时段分钟和小时均为0,无效
//			break;
//		}
		i++;
		i++;	
	}
	if( i == t ) i = i - 3; //最后一个时段(需注意最后一个时段数据为0)
	if( (BufA[i]==0) || (BufA[i]>4) ) Tariff.Current = 0;	//非法默认为尖费率
	else Tariff.Current = (BufA[i]-1);
	Result = i/3;//得到当前时段号
	if(Result != Tariff.Period_N) 
	{
		Init_Demand1();
		
		if( Outputtype==0x02 )
		{
			Shiduanctrl.Timer = shiduanctrl_timer_delay;
			SHIDUANCTRL_ON;//
		}
	}
	Tariff.Period_N = Result;//更新当前时段号	
	
	//更新状态字
	Tariff.Status &= B0000_0011;
	I2C_Read_Eeprom44( ee_changecontrol_page, Eeprom.Buffer, ee_changecontrol_lenth);//读启动控制字节
	if ( Eeprom.Buffer[0] == 0x55 ) Tariff.Status |= B0000_1000;//当前时区
	if ( Eeprom.Buffer[1] == 0x55 ) Tariff.Status |= B0000_0100;//当前时段表
}


unsigned char JudgeYearClock( unsigned char *YearLadder )//判断年结算日合法性(年结算日只能是1至12月中某月的1号至28号内的整点时刻)
{
	unsigned char i;
	
	for (i=0; i<6; i++)
	{
		if( JudgeClockNoWeek( 2, YearLadder+3*i, 3 ) == 0 )
		{
			if( YearLadder[1+3*i] <= 0x28 )  return 0;//正确
		}
	}
	
	return 1;//错误
}


unsigned char Freeze_Change_Judge( unsigned char Change )//结算方式变换判断及处理
{
	if( JudgeYearClock( Price.Year_Ladder ) == 0 )
	{
		if( Price.Ladder_Mode == 0x00 )
		{
			//月结算到年结算
			Freeze_Usedl(2);//月结算,月用电量转存和清零
			Freeze_Dl(ee_histroy_dl_page,ee_histroyf_inpage);	
			Price.Ladder_Mode = 0x55;//年结算模式	
			Process_Usedl(6);//所有年用电量从0开始	
			return 0x5a;	//用于防止重复结算年用电量和月结算
		}
		else if( Change == 1 ) 	//年结算日改变
		{
			//年结算切换到年结算
			Freeze_Usedl(0);//年用电量转存和清零
			return 0x55;	//用于防止重复结算年用电量
		}
	}
	else
	{
		if( Price.Ladder_Mode == 0x55 )
		{
			//年结算到月结算
			Freeze_Usedl(0);//年用电量转存和清零
			Price.Ladder_Mode = 0x00;//月结算模式
			Process_Usedl(1);//月用电量从0开始
			Freeze_Dl(ee_histroy_dl_page,ee_histroyf_inpage);
			return 0xaa;	//用于防止重复结算月用电量
		}
	}
	
	return 0x00;
}

void Ladderdata_Check(void)
{
	if( Crc_Check( Price.Ladder_Dl[0].B08, ee_ladder_lenth+2 ) ) 
	{
		Config_ShiQuLad_Proc();//找到当前时区得到当前阶梯表
	}
}	


unsigned char Year_Ladder_Judge(void)//年阶梯启动判断和数据合法性判断
{
	unsigned char Buf[ee_curyearjs_len+2];
	
	ReadE2WithBackup( ee_curyearjs_page, ee_curyearjs_inpage, Buf, ee_curyearjs_len );
	
	if( JudgeYearClock( Buf ) == 0 ) Price.Ladder_Mode = 0x55;//年结算模式
	else  Price.Ladder_Mode = 0x00;//月结算模式
	
	//阶梯启动时间判断
	if( !clock_error )
	{
		//阶梯时区启动时间判断
		if((Cmp_Clock( Starttime.LadSQ, 5 ) != 0x01) && ChkBufZero( Starttime.LadSQ, 5 ))
		{
			Changecontrol_Process(4);//翻转控制字节,清启动时间和进行约定冻结
		}

		if((Cmp_Clock( Starttime.Ladder, 5 ) != 0x01) && ChkBufZero( Starttime.Ladder, 5 ))
		{
			Changecontrol_Process(3);//翻转控制字节,清启动时间和进行约定冻结			
		}
	}

	//阶梯表校验
	if( Sclock.Clockbackup[1]==0 )//启动时间到或者每小时
	{	
		Config_ShiQuLad_Proc();//找到当前时区得到当前阶梯表
	}
	else 
	{
		Ladderdata_Check();
	}
		
	//新的年结算日和当前年结算日有变化，则转存和冻结
	if( Cmp_Data(Price.Year_Ladder, Buf, ee_curyearjs_len) != 3 )  
	{
		WriteE2WithBackup( ee_curyearjs_page, ee_curyearjs_inpage, Price.Year_Ladder, ee_curyearjs_len );
		
		return Freeze_Change_Judge(1);	//年结算日改变
	}
	else return( Freeze_Change_Judge(0) );	//阶梯启动时间到,结算方式变换判断及处理,年结算日未改变
}


void Price_Judge(void)	//费率阶梯电价判断
{
	unsigned char i,t;
	union B32_B08 BufA[4];

	run_price_flag = 0;
	run_alarm_flag = 1;
	
	//阶梯数据校验
	Ladderdata_Check();
	
	//费率启动时间判断
	if( !clock_error )
	{
		if((Cmp_Clock( Starttime.Rate, 5 ) != 0x01) && ChkBufZero( Starttime.Rate, 5 ))
		{
			Changecontrol_Process(2);//翻转控制字节,清启动时间和进行约定冻结
		}
	}
	//费率电价表校验
	if( Crc_Check( Price.Tariff[0].B08, ee_tariffprice_lenth+2 ) ) 
	{
		I2C_Read_Eeprom( ee_tariffprice1_page, 0, Price.Tariff[0].B08, ee_tariffprice_lenth);//读当前费率电价
	
	 
	}
	
	//判断当前阶梯
//	t = Feik.Num[6];//阶梯数(1~6),阶梯数为0则无阶梯功能
	t = 6;//6个阶梯电量
	for (i=0; i<5; i++)
	{
		if ( Price.Ladder_Dl[5-i].B32 == Price.Ladder_Dl[4-i].B32 ) t--;//相同阶梯数减1
		else break;
	}
	
	if( (t > 0) && (Price.Ladder_Dl[t-1].B32 == 0) ) t--;//最后阶梯电量为0,则阶梯数减1
	
	if( t == 0 )	//判断阶梯电量是否为全0
	{
		Tariff.Current_Ladder = 0xff;//无阶梯
		Price.Current_L.B32 = 0;//当前阶梯电价默认为0
	}
	else
	{	
		BufA[0].B32 = Fwdl.Usedl.B32;//正向用电量
		Add_Bcd( BufA[0].B08, Bkdl.Usedl.B08, 4 );	//正向用电量+反向用电量=总用电量
		for (i=0; i<t; i++)
		{
			if ( BufA[0].B32 <= Price.Ladder_Dl[i].B32 ) 
			{
				break;//找到
			}
		}//i的范围0~6
		Tariff.Current_Ladder = i;//当前阶梯
		Price.Current_L.B32 = Price.Ladder[i].B32;//阶梯电价
	}
		
	//计算电价
	Price.Current_F.B32 = Price.Tariff[Tariff.Current&B0000_0011].B32;//费率电价		
	Exchange( BufA[1].B08, Price.Current_F.B08, 4 );
	
	Price.Current.B32 = Price.Current_L.B32;//计算当前电价=阶梯电价+费率电价
	Exchange( BufA[2].B08,Price.Current.B08, 4 );
	Add_Bcd( BufA[1].B08, BufA[2].B08, 4 );
	Price.Current.B32 = BufA[1].B32;		
	//计算变比
	BufA[1].B08[3] = 0;
	BufA[2].B08[3] = 0;
	Exchange( BufA[1].B08, Esam_Para.Current_Ratio, 3 );//电流变比
	Exchange( BufA[2].B08, Esam_Para.Voltage_Ratio, 3 );//电压变比
	Bcd2Hex( BufA[1].B08, BufA[1].B08, 4 );//电流变比
	Bcd2Hex( BufA[2].B08, BufA[2].B08, 4 );//电压变比
	
	BufA[2].B32 = BufA[1].B32*BufA[2].B32;//变比hex
	if ( (BufA[2].B32==0) || (BufA[2].B32>=10000) ) BufA[2].B32 = 1;	
		
	//计算乘以变比的阶梯电价和费率电价
	Bcd2Hex( Price.Current.B08, BufA[1].B08, 4 );//当前电价hex
	BufA[3].B32 = BufA[1].B32*BufA[2].B32;//变比*当前电价(hex)
	Hex2Bcd( BufA[3].B08, Price.Current_X.B08, 4 );//变比*当前电价(bcd)
	
	//更新状态字
	Tariff.Status &= B0000_1100;
	if( JudgeSwitch( Starttime.Rate,1) == 0 )  Tariff.Status |= B0000_0010;//启动时间合法,显示第2套费率电价
	if( JudgeSwitch( Starttime.Ladder,1) == 0 ) Tariff.Status |= B0000_0001;//启动时间合法,显示第2套阶梯电价
}


//Ch=0,1,2,3读当前套,包括计算CRC
//Ch=4,5,6,7读备用套
void ReadE2WithJieti( uint08 Ch, uint08 *Buf )	//读阶梯电量/电价/年结算日/CRC
{
	I2C_Read_Eeprom( ee_ladder1_page+Ch, 0, Buf, ee_ladder_lenth );
}
		

void WriteE2WithJieti( uint08 Ch, uint08 *Buf )	//写阶梯电量/电价/年结算日
{
	I2C_Write_Eeprom( ee_ladder1_page+Ch, 0, Buf, ee_ladder_lenth );//再写一次
}
