#include "ex_func.h"
#include "ex_var.h"
//#include <intrins.h>
#include <string.h>
#include <absacc.h>


const struct HisEvent_STR ecode   EventTable[]=
{
	0x3001,	ee_cleardl_note_page,		  ee_cleardlnote_inpage,		  ee_cleardlnote_lenth,	    3*8+1,	//电表清零,0
	0x3003,	ee_clearevent_note_page,	ee_cleareventknote_inpage,	ee_cleareventnote_lenth,  3*8+3,	//事件清零,0
	0x0311,	ee_powerdown_note_page,		ee_powerdownnote_inpage,	  ee_powerdownnote_lenth,  	2*8+7, 	//掉电记录
	0x3004,	ee_setclock_note_page,		ee_setclocknote_inpage,		  ee_setclocknote_lenth,	  3*8+4,	//设置时钟,
	0x300d,	ee_opencover_note_page,		ee_opencovernote_inpage,	  ee_opencovernote_lenth,	  1*8+2,	//开表盖,
	0x1901, ee_maxi_note_page, 			  ee_maxinote_inpage,			    ee_maxinote_lenth,        2*8+4,	//过流
	0x1d00,	ee_openrelay_note_page,		ee_openrelaynote_inpage,	  ee_openrelaynote_lenth,	  1*8+6,	//拉闸8           
	0x1e00,	ee_closerelay_note_page,	ee_closerelaynote_inpage,  	ee_closerelaynote_lenth,  1*8+7,	//合闸8           
	0x3000,	ee_program_note_page,		  ee_programnote_inpage,		  ee_programnote_lenth,	    3*8+0,	//编程记录,
	0x3006,	ee_shiqurecord_page,		  0x00,				                ee_shiqurecord_lenth,	    3*8+6,	//时区表编程,
	0x3005,	ee_shiduanrecord_page,		0x00,				                ee_shiduanrecord_lenth,	  3*8+5,	//时段表编程,
	0x300c,	ee_jieshuanrecord_page,		ee_jieshuanrecord_inpage, 	ee_jieshuanrecord_lenth,  4*8+4,	//结算日编程,0
	0x3012, ee_keysetrecord_page, 		ee_keysetrecord_inpage, 	  ee_keysetrecord_len,	    4*8+7,	//密钥更新编程     
	0x3009,	ee_combstatusrecord_page,	ee_combstatusrecord_inpage,	ee_combstatusrecord_lenth,4*8+1,  //有功组合方式编程
	0x3600,	ee_relayerr_note_page,		ee_relayerrnote_inpage,		  ee_relayerrnote_lenth,	  0*8+0,	//负荷开关误动作记录
	0x3700,	ee_powererr_note_page,		ee_powererrnote_inpage,		  ee_powererrnote_lenth,	  1*8+5,	//电源异常记录
	0x3007, ee_week_note_page,			  ee_week_note_inpage,		    ee_week_note_len,		      3*8+7,	//周休日编程
	0x3013,	ee_errcardrecord_page,		ee_errcardrecord_inpage,	  ee_errcardrecord_len,	    0xff,	  //异常插卡
	0x2200,	ee_batterlow_note_page,		ee_batterlownote_inpage,	  ee_batterlownote_len,    	0*8+3,	//电池欠压
	0x0333,	ee_buydl_note_page,			  ee_buydlnote_inpage,		    ee_buydlnote_lenth,		    0xff,	  //购电8
	0x3400,	ee_dmoney_note_page,		  ee_dmoneynote_inpage,		    ee_dmoneynote_lenth,	    0xff,	  //退费记录
	0x300f,	ee_flpricerecord_page,		ee_flpricerecord_inpage,	  ee_flpricerecord_len,	    4*8+5,	//费率表编程
	0x3010,	ee_jtpricerecord_page,		ee_jtpricerecord_inpage,	  ee_jtpricerecord_len,	    4*8+6,	//阶梯表编程
	0x3002,	ee_clear_demand_page,		  ee_clear_demand_inpage,	    ee_clear_demand_lenth,	  3*8+2,	//需量清零,0
};


void Exchange(unsigned char *BufA, unsigned char *BufB, unsigned char Len )
{
	unsigned char n;
	for( n=0; n<Len; n++ )
	{
		BufA[n] = BufB[Len-1-n];//交换数据
	}
}

void Inverse( unsigned char *Buf, unsigned char Len )
{
	unsigned char i, Temp08;
	
	for( i=0; i<Len/2; i++ )
	{
		Temp08 = Buf[i];
		Buf[i] = Buf[Len-1-i];
		Buf[Len-1-i] = Temp08;//交换数据
	}
}


void Number_Add1_Direct(unsigned char *BufA) //其他加1
{	
	unsigned char  BufB[4];
	union B32_B08 Add;
	
	if ( ( BufA[0]==0xff) || (BufA[1]==0xff) || (BufA[2]==0xff) )
	{
		BufA[0] = 0;//非法数据清零
		BufA[1] = 0;
		BufA[2] = 0;
	}
	BufB[0] = BufA[0];
	BufB[1] = BufA[1];
	BufB[2] = BufA[2];
	BufB[3] = 0;	//低字节在前变成高字节在前
	Add.B32 = 1;
	Add_Bcd( BufB, Add.B08, 4 );
	BufA[0] = BufB[0];	//高字节在前变成低字节在前
	BufA[1] = BufB[1];
	BufA[2] = BufB[2];
}


void Number_Add1(unsigned char Offset)	//事件记录次数加1
{
	unsigned char  Inpage, Buf[3];
	
	Inpage = Read_Event_Number( Buf, Offset );
	
	Number_Add1_Direct( Buf );
	
	I2C_Write_Eeprom( ee_recordnumber_page, Inpage, Buf, 3 );
	
	if( (Offset!=ee_openrelayn_inpage)&&(Offset!=ee_closerelayn_inpage)&&(Offset!=ee_powerdownn_inpage) )	//拉合闸在成功以后上报,掉电在结束后上报
	{
		if( (Offset!=ee_clearn_inpage) || (Key_Status==1) )	//私钥下允许清零上报
		{
			Set_Reportdata(EventTable[Offset-ee_clearn_inpage].offset2);	//进行事件上报
		}
	}	
}


unsigned char Read_Event_Number( unsigned char *BufA, unsigned char Offset )
{
	unsigned char  Inpage;
	
	Inpage = (Offset-ee_clearn_inpage)*3;//找到页内地址
	if( Offset > ee_errcardn_inpage ) Inpage -= 9;	//跳过无总次数的事件: 电池欠压，购电记录，退费记录
	
	if( RdRecTimes( Offset ) == 0 ) //判断有效指针,如果为0从头开始计算次数
	{
		BufA[0] = 0;//清零当前次数
		BufA[1] = 0;
		BufA[2] = 0;
	}
	else
	{
		I2C_Read_Eeprom( ee_recordnumber_page, Inpage, BufA, 3 );
	}
	
	return Inpage;
}


const uint08 code Max[] =
	{ 	max_timingfreeze_p,max_dayfreeze_p,   max_histroy_p,     max_hourfreeze_p,
		3,2,2,2,2,2,2,
		max_eventrecord_p, max_eventrecord_p ,max_eventrecord_p, max_eventrecord_p,
		max_eventrecord_p, max_eventrecord_p ,max_eventrecord_p, max_eventrecord_p,
		max_eventrecord_p, max_eventrecord_p ,max_eventrecord_p, max_eventrecord_p,2,
		max_eventrecord_p, max_eventrecord_p ,max_eventrecord_p, max_eventrecord_p, max_eventrecord_p,1,
		max_eventrecord_p, max_eventrecord_p, max_eventrecord_p, max_eventrecord_p,max_histroydm_p,max_cleardemand_p
	};
	
unsigned char Read_Event_Point( unsigned char Inpage )
{
	unsigned char Result;
	unsigned char Buf[ee_eventpoint_lenth+2];
	
	ReadE2WithBackup( ee_event_point_page, ee_eventpoint_inpage, Buf, ee_eventpoint_lenth );
	Result = Buf[Inpage];
	if( Result >= Max[Inpage] ) Result = 0;	//数据非法
	return ( Result );
}


unsigned char Write_Event_Point( unsigned char Inpage )	
{
	unsigned char Buf[ee_eventpoint_lenth+2];
	
	//冻结和事件有效指针加1处理
	ReadE2WithBackup( ee_validrec_page, 0x00, Buf, ee_eventpoint_lenth );
	if( Buf[Inpage] < Max[Inpage] ) 
	{
		Buf[Inpage]++;//有效指针加1
		WriteE2WithBackup ( ee_validrec_page, 0x00, Buf, ee_eventpoint_lenth );
	}
	
	ReadE2WithBackup( ee_event_point_page, ee_eventpoint_inpage, Buf, ee_eventpoint_lenth );
	if( Buf[Inpage] >= Max[Inpage] ) Buf[Inpage] = 0;	//数据非法
	Buf[Inpage]++;
	if( Buf[Inpage] >= Max[Inpage] ) Buf[Inpage] = 0;//循环回0
	WriteE2WithBackup ( ee_event_point_page, ee_eventpoint_inpage, Buf, ee_eventpoint_lenth );
	return ( Buf[Inpage] );
}

//unsigned char Write_DmEvent_Point( void )	
//{
//	unsigned char Buf[4];
//	
//	//冻结和事件有效指针加1处理
//	ReadE2WithBackup( ee_validrecdemand_page, ee_validrecdemand_inpage, Buf, ee_validrecdemand_lenth );
//	if( Buf[0] < 12 ) 
//	{
//		Buf[0]++;//有效指针加1
//		WriteE2WithBackup ( ee_validrecdemand_page, ee_validrecdemand_inpage, Buf, ee_validrecdemand_lenth );
//	}
//	
//	ReadE2WithBackup( ee_event_demand_page, ee_event_demand_inpage, Buf, ee_event_demand_lenth );
//	if( Buf[0] >= 12 ) Buf[0] = 0;	//数据非法
//	Buf[0]++;
//	if( Buf[0] >= 12 ) Buf[0] = 0;//循环回0
//	WriteE2WithBackup ( ee_event_demand_page, ee_event_demand_inpage, Buf, ee_event_demand_lenth);
//	return ( Buf[0] );
//}

unsigned char RdRecTimes( unsigned char Inpage )//返回有效指针
{
	unsigned char Buf[ee_eventpoint_lenth+2];
	
	ReadE2WithBackup( ee_validrec_page, 0x00, Buf, ee_eventpoint_lenth );
	return ( Buf[Inpage] );
}

//void ClrRecTimes( unsigned char Inpage, unsigned char mode )//清有效指针
//{
//	unsigned char Temp1;
//	unsigned char Buf[ee_eventpoint_lenth+2];
//	union B16_B08 Temp16;
//	
//	Temp16.B16 = 0;
////	Temp16.B08[0] = 0;
////	Temp16.B08[1] = 0;
//	
//	if( mode == 1 )//清零(单个事件清零)
//	{
//		Temp16.B08[0] = Inpage;
//		Temp16.B08[1] = Inpage+1;
//		Clr_Reportdata(Inpage);	//清单项上报状态字和新增次数(有事件记录类)
//	}
//	else if(mode == 0x55)//事件全清
//	{
//		{Report_Data[0] = 0;Clr_Reportnum(0);}	//清上报状态字和新增次数
//		{Report_Data[1] = 0;Clr_Reportnum(1);}	//清上报状态字和新增次数
//		for( Temp1=ee_clearn_inpage+2; Temp1<ee_eventpoint_lenth; Temp1++ )
//		{
//			Clr_Reportdata(Temp1);	//清上报状态字和新增次数(有事件记录类)
//		}
//		
//		Reset_Rep_Timer = 0;

//		Temp16.B08[0] = ee_clearn_inpage+2;
//		Temp16.B08[1] = ee_eventpoint_lenth;
//	}
//	else if(mode == 0xaa)//电表清零
//	{
//		Temp16.B08[0] = 0;
//		Temp16.B08[1] = ee_eventpoint_lenth;
//	}
//	else if(mode == 0xff)//出厂全清
//	{
//		Temp16.B08[0] = 0;
//		Temp16.B08[1] = ee_eventpoint_lenth;
//	}
//	else
//	{
//		return;
//	}
//	
//	if( mode != 1 )
//	{		
//		System_Status &= (~0x3e);//清已经上报标志,清零后重新上报
//		
//		Clr_Otherdata(0);	//清非法插卡次数
//	}
//	
//	ReadE2WithBackup( ee_validrec_page, 0x00, Buf, ee_eventpoint_lenth );
//	
//	if(mode == 0xaa)//电量全清
//	{
//		Temp1 = Buf[ee_clearn_inpage];//仅仅保留清零指针
//		memset( Buf, 0x00, ee_eventpoint_lenth );
//		Buf[ee_clearn_inpage] = Temp1;
//	}
//	else if(mode == 0xff)//出厂全清
//	{
//		memset( Buf, 0x00, ee_eventpoint_lenth );
//	}
//	
//	WriteE2WithBackup( ee_validrec_page, 0x00, Buf, ee_eventpoint_lenth );
//	ReadE2WithBackup( ee_validrec_page, 0x00, Buf, ee_eventpoint_lenth );
//}

void ClrRecTimes( unsigned char Inpage, unsigned char mode )//清有效指针
{
	unsigned char Temp1;
	unsigned char Buf[ee_eventpoint_lenth+2];
	union B16_B08 Temp16;
	
	if( mode == 1 )//清零(单个事件清零)
	{
		Temp16.B08[0] = Inpage;
		Temp16.B08[1] = Inpage+1;
		Clr_Reportdata(Inpage);	//清单项上报状态字和新增次数(有事件记录类)
	}
	else if(mode == 0x55)//事件全清
	{
		{Report_Data[0] = 0;Clr_Reportnum(0);}	//清上报状态字和新增次数
		{Report_Data[1] = 0;Clr_Reportnum(1);}	//清上报状态字和新增次数
		for( Temp1=ee_clearn_inpage+2; Temp1<ee_eventpoint_lenth; Temp1++ )
		{
			Clr_Reportdata(Temp1);	//清上报状态字和新增次数(有事件记录类)
		}
		
		Reset_Rep_Timer = 0;

		Temp16.B08[0] = ee_clearn_inpage+2;
		Temp16.B08[1] = ee_eventpoint_lenth;
	}
	else if(mode == 0xaa)//电表清零
	{
		Temp16.B08[0] = 0;
		Temp16.B08[1] = ee_eventpoint_lenth;
	}
	else if(mode == 0xff)//出厂全清
	{
		Temp16.B08[0] = 0;
		Temp16.B08[1] = ee_eventpoint_lenth;
	}
	else
	{
		return;
	}
	
	if( mode != 1 )
	{		
		System_Status &= (~0x3e);//清已经上报标志,清零后重新上报
		
		Clr_Otherdata(0);	//清非法插卡次数
	}
	
	ReadE2WithBackup( ee_validrec_page, 0x00, Buf, ee_eventpoint_lenth );
	for( Temp1=Temp16.B08[0]; Temp1<Temp16.B08[1]; Temp1++ )
	{
		switch( Temp1 )
		{
			case ee_clearn_inpage: 
				if( mode == 0xaa ) continue; //电表清零命令时，保留电表清零记录
				break;
				
			case ee_covern_inpage: 
				Cover.Status = 0;  //开盖则重新产生开盖记录
				break;
				
			case ee_maxi_inpage:  
				load_over_flag = 0;	
				Current_Over_Count2 = 0;
				Clr_Otherdata(1); //过流持续时间(4)
				break;
				
			case ee_batterlow_inpage: 
				low_voltage_flag = 0; 
				Bat_Delay_Timer = 0;
				Clr_Otherdata(2); //电池欠压总累计时间(4)
				break;
				
			case ee_powerdownn_inpage:  //清零掉电记录
				below_60u_flag = 0;	
				Voltage_Low_Count = 0;
				Clr_Otherdata(3); //掉电总累计时间(4)
				break;
				
			case ee_programn_inpage:  //编程记录
				first_prog_flag = 0; 
				break;

//			case ee_relayerrn_inpage: 
//				relay_error = 0;  //负荷开关误动作会重新产生记录
//				break;

			case ee_relayerrn_inpage: 
				Init_Metererrstatus();  //清零继电器故障状态
				break;
				
			default: break;
		}

		Buf[Temp1] = 0x00; //清除有效次数
	}
	
	WriteE2WithBackup( ee_validrec_page, 0x00, Buf, ee_eventpoint_lenth );
}




unsigned char Event_Record( unsigned char Inpage )
{
	unsigned char i, result, ptr, offset, len;
	uint16  page;
	
	ptr = (Inpage&0x7F);
	if( ptr < ee_clearn_inpage ) return 0x00;
	
	page   = EventTable[ptr-ee_clearn_inpage].page;
	offset = EventTable[ptr-ee_clearn_inpage].offset;
	len    = EventTable[ptr-ee_clearn_inpage].len;
	
	if( (Inpage&0x80) == 0x00 ) //事件记录为单次记录
	{
		if( (ptr == ee_buymoneyn_inpage) || (ptr == ee_dmoneyn_inpage) ) //购电记录和退费记录从分钟开始，其余从秒开始
		{
			memcpy( Eeprom.Buffer, Sclock.Clockbackup+1, 5 );
		}
		else
		{
			if( ptr != ee_setclockn_inpage ) memcpy( Eeprom.Buffer, Sclock.Clockbackup, 6 ); //设置时钟不在此函数设置发生时刻
			if( ptr != ee_batterlow_inpage ) Number_Add1(ptr);	//欠压记录没有总次数
		}

		result = Write_Event_Point(ptr);
	}
	else	//事件记录分成开始和结束两次
	{
		result = Read_Event_Point(ptr);
		I2C_Read_Eeprom( page+result, offset, Eeprom.Buffer, len );	//时间

		if( RdRecTimes( ptr ) ) //次数为0不允许记录
		{
			if (ptr == ee_programn_inpage)
			{
				for( i=0; i<4*9; i++ )
				{
					Eeprom.Buffer[49-i] = Eeprom.Buffer[45-i];
				}
				memcpy( Eeprom.Buffer+10, Eeprom.Buffer+50, 4 );	//数据标识(4)
			}
			else
			{
				memcpy( Eeprom.Buffer+6, Sclock.Clockbackup, 6 );
			
				if( (ptr != ee_powererrn_inpage) )	//除电源异常以外,其他需要记录结束时刻电量
				{
					DltoEepromBuf( len-8 );
				}
			}
		}
	}
	
	I2C_Write_Eeprom( page+result, offset, Eeprom.Buffer, len );
	
	return (result);
}


void Read_Uip_Data(void)	//读电压电流功率功率因数(用于记录)
{
	memcpy( Eeprom.Buffer+20,  Emu_Data.U.B08, 2 );	//电压xxx.x
	memcpy( Eeprom.Buffer+22,  Emu_Data.I1.B08, 3 );	//电流xxx.xxx
	if( dl_back_flag ) Eeprom.Buffer[24] |= 0x80;
	memcpy( Eeprom.Buffer+25,  Emu_Data.P1.B08, 3 );	//有功功率xx.xxxx
	if( dl_back_flag ) Eeprom.Buffer[27] |= 0x80;
	memcpy( Eeprom.Buffer+28,  Emu_Data.Cosa.B08, 2 );	//功率因数x.xxx
	if( dl_back_flag ) Eeprom.Buffer[29] |= 0x80;
}


void DltoEepromBuf( unsigned char st )
{
	memcpy( Eeprom.Buffer+st+0,  Fwdl.Fwdl_T.B08, 4 );	//正向总电量
	memcpy( Eeprom.Buffer+st+4,  Bkdl.Bkdl_T.B08, 4 );	//反向总电量
}


void Note_Relay_Event(void)//记录断开闭合继电器事件
{
	I2C_Read_Eeprom( ee_operatorcode_page, ee_operatorcode_inpage, Eeprom.Buffer+6, 4 );//读4字节操作者代码

	DltoEepromBuf( 10 );
	
	if( relay_status_flag == 0 ) //合闸记录
	{
		if( self_close_flag )
		{
			memset(Eeprom.Buffer+6, 0xff, 4);
			I2C_Write_Eeprom( ee_operatorcode_page, ee_operatorcode_inpage, Eeprom.Buffer+6, 4 );//写4字节操作者代码
		}
		Event_Record(ee_closerelayn_inpage);
	}
	else
	{
		Event_Record(ee_openrelayn_inpage);
	}
	
//	if(PowerDetCnt0 <= 2)//如果电压低于0.8Un继电器发生动作,此时因无法进入继电器检测,则继电器实际状态默认为命令状态
//	{
//		real_relaystatus_flag = relay_status_flag;
//		comm_relaystatus_flag = real_relaystatus_flag;//刷新继电器实际状态(通讯bit4专用)
//	}
}


//开盖合盖记录/电源异常开始和结束记录/负荷开关误动作开始和结束记录/过流记录/掉电记录
void Note_Run_Event( unsigned char Type )	//记录运行事件记录
{
	unsigned char Buf[6], Temp08;
	
	if( (Type&0x80) ==  0x00 )	//开始记录
	{
		memset( Eeprom.Buffer, 0, 42 );

		if( Type == ee_relayerrn_inpage )	//负荷开关异常
		{
			if( (Relay_Mode == 0x55) && (relay_status_flag == 0) && (relay_success_flag==0) ) return;	//外置继电器表合闸成功才做负荷开关误动作记录和上报
			
			if( relay_failed_flag ) return;		//故障期间只做一次记录
			else
			{
				relay_failed_flag = 1;		//置继电器错误标志
				Store_Meterstatus();
			}
			
			Eeprom.Buffer[12] = real_relaystatus_flag;	//继电器状态
			DltoEepromBuf( 13 );
		}
		else
		{
			if( Type == ee_maxi_inpage )	//过流
			{
				Read_Uip_Data();	//读电压电流功率功率因数(用于记录)
			}
			
			DltoEepromBuf( 12 );
		}
	}
	else
	{
		if((Type&0x7F) == ee_relayerrn_inpage )	//负荷开关异常
		{
			if( relay_failed_flag == 0 ) return;		//故障期间只做一次记录
			
			Temp08 = Read_Event_Point( ee_relayerrn_inpage );
			I2C_Read_Eeprom( ee_relayerr_note_page+Temp08, ee_relayerrnote_inpage+6, Buf, 6 );
			if( ChkBufZero(Buf,6) ) return;			//结束时间非0表示已经记录过
		}
	}
	
	Event_Record( Type );
}


unsigned int Read_Load_Point( unsigned char Inpage )
{
	if( Load.Point[Inpage].B16 >= max_ploadrec_p ) return 0;	//数据非法
	return ( Load.Point[Inpage].B16 );
}


unsigned int Write_Load_Point( unsigned char Inpage )	
{
	unsigned char n;
	union B16_B08 Addr;
	
	//负荷有效指针加1处理
	if( Load.Validpoint[Inpage].B16 < max_ploadrec_p ) 
	{
		Load.Validpoint[Inpage].B16++;//有效指针加1
	}
	
	if( Load.Point[Inpage].B16 >= max_ploadrec_p ) Load.Point[Inpage].B16 = 0;	//数据非法
	Load.Point[Inpage].B16++;
	if( Load.Point[Inpage].B16 >= max_ploadrec_p ) Load.Point[Inpage].B16 = 0;//循环回0
	
	Load.Crc16.B16 = Crc_16Bits( Load.Validpoint[0].B08, ee_loadpoint_lenth ); //CRC计算
	
	for( n=0; n<3; n++ ) //3通道
	{
		if( RdLoadPara[n][4] )
		{
			memcpy( Addr.B08, RdLoadPara[n]+6, 2 );
			if( (Load.Validpoint[Inpage].B16 == max_ploadrec_p) //有效次数达到最大值
			 && (Addr.B16 == Load.Point[Inpage].B16) ) //当下次读取的地址被覆盖时，指向下一个地址。
			{
				if( ++Addr.B16 >= max_ploadrec_p ) Addr.B16 = 0x00;
				memcpy( RdLoadPara[n]+6, Addr.B08, 2 );
			}
		}
	}
	
	return ( Load.Point[Inpage].B16 );
}


//该函数输出当前时间针对2000年1月1日0点0分的分钟数差值
//每月其始的相对天数
const uint16 code gui_MonthBeginDay[12] = {0, 31, 59, 90,120, 151, 181, 212, 243, 273, 304, 334};
uint16 CalcAbsDay(unsigned char * psTm)
{//以2000年1月1日为基准到现在的所有天数,因此时间不能在2000年之前(其实2000年用00表示,百年以上都略去)
	uint08 ucYear = Bcd_To_Hex(psTm[4]);        //年转换成Hex
	uint08 ucMonth = Bcd_To_Hex(psTm[3])-1;      //月转换成Hex
	uint08 ucDay = Bcd_To_Hex(psTm[2]) - 1;      //日转换成Hex
	if((ucYear & 0x03) == 0)                            //判断是否过了闰年的2月
	{
	    if(ucMonth >= 2)
	        ucDay++;                                    //再加一天
	}
	else
	{
	    ucDay++;                                        //非闰年加上00年闰月多出来的一天
	}
	//year >> 2,是加上过去的整4年的闰年的一天
	return ( (uint16)ucYear*365 + gui_MonthBeginDay[ucMonth] + (ucDay + (ucYear>>2)) );
}
uint16 CalcAbsMin(unsigned char* psTm)
{
	//以当日00时00分00秒为基准秒
	uint08 ucMin = Bcd_To_Hex(psTm[0]);       //分转换成Hex
	uint08 ucHour = Bcd_To_Hex(psTm[1]);        //时转换成Hex
	return ( (uint16)ucHour*60 + ucMin );
}
//[0]分[1]时[2]日[3]月[4]年
unsigned long int CalcTime(unsigned char* psTm)
{
    	uint16 Temp16a,Temp16b;
    	Temp16a = CalcAbsDay(psTm);
    	Temp16b = CalcAbsMin(psTm);
   	return( (unsigned long int)Temp16a*1440 + Temp16b );
}

const uint08 code LoadOffset[4] = { ee_ploadrec1_inpage, ee_ploadrec2_inpage, ee_ploadrec3_inpage, ee_ploadrec4_inpage };	//4类数据页类偏移量
const uint08 code LoadLen[4] = { ee_ploadrec1_len, ee_ploadrec2_len, ee_ploadrec3_len, ee_ploadrec4_len };	//4类数据页数据长度
const uint08 code LoadOffset2[4] = { 0, 5, 8, 10 };
//const uint08 code LoadLen2[4] = { 5, 3, 2, 8 };
void Load_Judge(void)		//负荷记录判断
{
	unsigned char i;
	uint16 Result;
	unsigned long int Temp32;
	unsigned char TimeRcd;
	
	if( (FLAG_EA == 0) || clock_error ) return;//掉电或者时钟错误不处理

	run_loadchk_flag = 0;
	run_timezone_flag = 1;

//	if( (Cmp_Clock( Week_Holiday.Lstarttimer, 5 ) != 0x01) )//负荷记录起始时间<=当前时间
	{
		Temp32 = CalcTime( Sclock.Clockbackup+1 );
//		Exchange( Eeprom.Buffer, Week_Holiday.Lstarttimer, 5 );//变成低字节在前
//		Temp32 -= CalcTime( Eeprom.Buffer );//计算当前时间和启动时间的分钟差

		for(i=0; i<4; i++)
		{
			TimeRcd = ((Week_Holiday.Lspacetimer[i])>>8)&0x00ff;
			if( Temp32%Bcd_To_Hex(TimeRcd) == 0 )
			{
				if( (Mode.Freeze[0]>>i)&B0000_0001 )
				{
					Clear_Wdt();
					Read_Uip_Data();	//读电压电流功率功率因数(用于记录),但是不支持频率
					DltoEepromBuf( 30 );	//读正向电量+反向电量
					memcpy( Eeprom.Buffer+15+LoadOffset2[i], Sclock.Clockbackup+1, 5 );	//复制时间
					        				
					Result = Write_Load_Point(i);
					I2C_Write_Eeprom( ee_ploadrec_page+Result, LoadOffset[i], Eeprom.Buffer+15+LoadOffset2[i], LoadLen[i] );
				}
			}
		}
		
		if( Sclock.Clockbackup[1]==0 )
		{
			Wr_Powerdown_Data(0);//写掉电相关数据到数据ee(每小时写一次)
			Save_Keep_Timer(1);//保存相关累计时间
		}
	}
}

uint16 CalStAddr( uint16 RecTimes, uint16 Addr )
{
	if( Addr < (RecTimes-1) )	Addr += max_ploadrec_p;
	Addr -= (RecTimes-1);	//最早记录块地址
	return Addr;
}


uint16 CalRecTimes( uint16 StAddr, uint16 EndAddr )
{
	uint16 Temp16;
	
	if( StAddr <= EndAddr ) Temp16 = EndAddr-StAddr+1;
	else Temp16 = max_ploadrec_p+EndAddr-StAddr+1;
	
	return Temp16;
}

//------------------------- 负荷记录 抄读和清除 ----------------------------
uint16 FindSpecPos( unsigned char Type, unsigned char *Clock, uint16 RecTimes, uint16 Addr )
{
	unsigned char i, Result, Temp08;
	unsigned char RecTimer[5];
	uint16 StAddr, EndAddr;
	uint16 QStAddr, QEndAddr;

	StAddr = CalStAddr( RecTimes, Addr );
	EndAddr = Addr;	//最近记录块地址

	QStAddr = StAddr;
	QEndAddr = EndAddr;
	
	//首先根据有效记录数算出二分后的地址
	Temp08 = 1;
	for( i=0; i<11; i++ )	//max_ploadrec_p条最多分成11次
	{
		//  计算二分之一处地址
		if( QStAddr <= QEndAddr )//  起始地址小于等于结束地址，没环绕的情况
			Addr = (QStAddr+QEndAddr)/2;
		else//  起始地址大于结束地址，环绕的情况
			Addr = ((QStAddr+QEndAddr+max_ploadrec_p)/2)%max_ploadrec_p; //起始地址加上起始到结束的一半条数
		
		I2C_Read_Eeprom( ee_ploadrec_page+Addr, LoadOffset[Type], RecTimer, 5 );
		Result = Cmp_Bcd_Inv( Clock, RecTimer, 5 );
		if( Result == 0 ) break; //给定时间 == 当前时间
		
		if( (Temp08 == 0) || (RecTimes == 1) )
		{
			if( Result == 1 ) //给定时间 > 当前时间
			{
				if( Addr == EndAddr )
				{
					Addr = 0x0FFF;
					break;
				}

				Addr += 1;
				if( Addr >= max_ploadrec_p ) Addr -= max_ploadrec_p;
			}
			
			break;
		}
		
		if( RecTimes == 2 )
		{
			Temp08--;	//还需再判断一次
			if( Result == 1 ) //给定时间 > 当前时间
			{
				Addr += 1;
				if( Addr >= max_ploadrec_p ) Addr -= max_ploadrec_p;
			}
			else if( Result == 2 ) //给定时间 < 当前时间
			{
				if( Addr == StAddr ) break;
				
				if( Addr == 0 ) Addr = max_ploadrec_p;
				Addr -= 1;
			}
		}

		if( Result == 2 ) //给定时间 < 当前时间
		{
			QEndAddr = Addr;
		}
		else //给定时间 > 当前时间
		{
			QStAddr = Addr;
		}

		RecTimes = CalRecTimes( QStAddr, QEndAddr );
	}

	return (Addr|(Result<<14));
}


void ClearPloadRec( unsigned char *Clock )
{
	unsigned char i, Result;
	uint16 StAddr, EndAddr, RecTimes;
	
	memset( RdLoadPara, 0x00, sizeof(RdLoadPara) );	//只要删除负荷记录就取消负荷记录的后续帧读取
	
	for( i=0; i<4; i++ ) //找到各类的最早记录起始地址
	{
		RecTimes = Load.Validpoint[i].B16; //读取每个类的有效记录数
		if( RecTimes )
		{
			EndAddr = Read_Load_Point(i); //最近记录块
			StAddr = CalStAddr( RecTimes, EndAddr ); //最早记录块

			EndAddr = FindSpecPos( i, Clock, RecTimes, EndAddr );

			Result = (EndAddr>>14);  //最高2Bit为比较结果.
			EndAddr &= 0x0FFF;  //去掉最高2Bit的比较结果.
			if( EndAddr != 0x0FFF )
			{
				RecTimes = CalRecTimes( StAddr, EndAddr );

				if( Result == 2 )
				{
					RecTimes--;
					if( EndAddr == 0 ) EndAddr = max_ploadrec_p;
					EndAddr--;
				}
				
				Load.Validpoint[i].B16 = RecTimes;
				Load.Point[i].B16 = EndAddr;
			}
		}
	}
	
	Load.Crc16.B16 = Crc_16Bits( Load.Validpoint[0].B08, ee_loadpoint_lenth ); //CRC计算
}

extern unsigned char Add_Min_Clock( unsigned char *Clock, unsigned char Min );


unsigned char ReadLoadRec( unsigned char Ch, unsigned char *InBuf, unsigned char *OutBuf )
//InBuf[0]: 0x11读数据; 0x12:读后续帧 
//05给定数据标示; 0A给定时间
//InBuf[2~5]:DI0~DI3
//InBuf[6]: NN或SN
//InBuf[7~10]:mmhhDDMMYY, 给定时间时有效 
{
	unsigned char i, t, Result, Spec; //00:未从E2读取; FF:从E2中读取
	unsigned char RdBuf[16];
	struct
	{
		union B16_B08  Addr[2];	//[0~3]最早记录块; [4~7]最近记录块
		union B16_B08  RecTimes;
	} RdStr;
	unsigned char offset, len;
	union B16_B08 Temp16;
	unsigned char BkClock[6], FirstVal[5]; //期望时间
	unsigned long TmpA;

	Temp16.B08[1] = InBuf[3];
	Temp16.B08[0] = InBuf[2];

	switch( Temp16.B16 )
	{
		case 0x0101:	//A相电压
			i = 0; //起始类
			offset = 5;
			len = 2;
			break;

		case 0x0201:	//A相电流
			i = 0; //起始类
			offset = 7;
			len = 3;
			break;
		
		case 0x0300:	//总有功功率
			i = 1; //起始类
			offset = 5;
			len = 3;
			break;

		case 0x0500:	//总功率因数
			i = 2; //起始类
			offset = 5;
			len = 2;
			break;

		case 0x0601:	//正向有功电量
			i = 3; //起始类
			offset = 5;
			len = 4;
			break;
			
		case 0x0602:	//反向有功电量
			i = 3; //起始类
			offset = 9;
			len = 4;
			break;

		default:
			OutBuf[0] = 0xFF;
			OutBuf[1] = 0x02;	//数据标识错, 无请求数据
			return 2;
	}

	Spec = Bcd_To_Hex((Week_Holiday.Lspacetimer[i]>>8)&0x00ff); //负荷记录间隔

	RdStr.RecTimes.B16 = Load.Validpoint[i].B16; //读取每个类的有效记录数
	if( RdStr.RecTimes.B16 == 0x00 ) 	return 0x00;
	
	//Addr: 对应类的块地址 [0~3]: 最早记录; [4~7]: 最近记录
	//RecTimes: 对应类的剩余有效记录数
	//RdLoadPara: 数据标识(4)+剩余记录块数(1)+后续帧序号(1)+起始地址(4)+剩余有效记录数(2)+给定时间(5)+上一次电量(4)
	if( InBuf[0] == 0x12 ) //读取后续帧
	{
		if( RdLoadPara[Ch][4] == 0 ) return 0x00; //无后续帧

		Result = Cmp_Data( RdLoadPara[Ch], InBuf+2, 4 );
		if( Result != 3 ) return 0x00; //数据标识错

		if( InBuf[6] != RdLoadPara[Ch][5] ) return 0x00; //帧序号错误

		memcpy( RdStr.Addr[0].B08, RdLoadPara[Ch]+6, 6 );
		memcpy( BkClock+1, RdLoadPara[Ch]+12, 5 );

		t = 0x00;
	}
	else if( InBuf[0] == 0x11 ) //读负荷记录
	{
		memset( RdLoadPara[Ch], 0x00, sizeof(RdLoadPara[Ch]) ); //删除历史抄读记录, 重新记录

		RdLoadPara[Ch][4] = Bcd_To_Hex(InBuf[6]);	//抄读的记录块数
		if( RdLoadPara[Ch][4] == 0x00 ) return 0x00; //抄读次数大于零

		RdStr.Addr[1].B16 = Read_Load_Point(i); //最近记录块
		Temp16.B16 = CalStAddr( RdStr.RecTimes.B16, RdStr.Addr[1].B16 );
		I2C_Read_Eeprom( ee_ploadrec_page+Temp16.B16, LoadOffset[i], FirstVal, 5 );

		//从给定时间开始，第一个负荷记录时间
		memcpy( BkClock+1, InBuf+7, 5 );
		Result = CalcTime(BkClock+1)%Spec;
		if( Result )
		{
			Result = Spec-Result;
			Add_Min_Clock( BkClock, Result );
		}

		if( Cmp_Bcd_Inv( FirstVal, BkClock+1, 5 ) == 1 ) //最早记录时间大于给定时间(0-A=B; 1:A>B; 2:A<B)
		{
			TmpA = CalcTime(FirstVal)/Spec-CalcTime(BkClock+1)/Spec;
			
			if( TmpA >= RdLoadPara[Ch][4] ) return 0x00;
			if( (TmpA==0) && (RdLoadPara[Ch][4]==1) ) return 0x00;
			
			RdLoadPara[Ch][4] -= TmpA;	//扣除跨过的块数
			
			//从最早记录开始，第一个负荷记录时间
			memcpy( BkClock+1, FirstVal, 5 );
			Result = CalcTime(BkClock+1)%Spec;
			if( Result )
			{
				Result = Spec-Result;
				Add_Min_Clock( BkClock, Result );
			}
		}
		
		if( Cmp_Bcd_Inv(BkClock+1, Sclock.Clockbackup+1, 5) == 1 ) return 0x00; //给定时间大于当前时间，无记录
		if( FLAG_EA == 0 ) return 0x00;	//掉电不处理
		
		RdStr.Addr[0].B16 = FindSpecPos( i, BkClock+1, RdStr.RecTimes.B16, RdStr.Addr[1].B16 );
		Result = (RdStr.Addr[0].B16>>14);
		RdStr.Addr[0].B16 &= 0x0FFF; //去掉最高2Bit的比较结果.
		if( RdStr.Addr[0].B16 == 0x0FFF )
		{
			RdStr.RecTimes.B16 = 0x00; //抄读时间大于所有记录时间，记录有效次数清零
		}
		else
		{
			RdStr.RecTimes.B16 = CalRecTimes( RdStr.Addr[0].B16, RdStr.Addr[1].B16 );
		}
		
		if( i == 3 ) //抄读电量时，需要确定上一次的电量用于填充冻结遗漏点
		{
			if( Result == 1 ) //抄读时间大于所有记录时间
				Temp16.B16 = RdStr.Addr[1].B16;
			else //抄读时间小于返回的地址所指向的记录时间
			{
				Temp16.B16 = RdStr.Addr[0].B16;
				if( Temp16.B16 == 0 ) Temp16.B16 = max_ploadrec_p;
				Temp16.B16--;
			}
				
			I2C_Read_Eeprom( ee_ploadrec_page+Temp16.B16, LoadOffset[3]+offset, RdLoadPara[Ch]+17, 4 ); //补上一次电量					
		}
		
		t = 5;
		memcpy( OutBuf+1, BkClock+1, 5 );
	}
	else return 0x00;

	if( RdStr.RecTimes.B16 ) 	//存在记录时，确保从E2中读取
		memset( RdBuf, 0x00, 5 );
	else //没有记录时, 确保不读取E2数据
		memset( RdBuf, 0x99, 5 );
	
	OutBuf[0] = 0x00;
	while( RdLoadPara[Ch][4] )
	{
		Clear_Wdt();
		
		if( FLAG_EA == 0 ) return 0x00;	//掉电不处理
		
		if( Cmp_Bcd_Inv(BkClock+1, Sclock.Clockbackup+1, 5) == 1 )
		{
			RdLoadPara[Ch][4] = 0;
			break;
		}

		//----- 预估此时间点的所有记录能否放在此帧中------
		if( InBuf[0] == 0x11 ) Result = 4;	//数据标识
		else Result = 5; //数据标识+序号

		if( t+len > 200-Result )
		{
			//保留当前最小时间和抄读到第几个记录, 读取后续帧要使用, 达到最大帧长度, 结束查询.
			//RdLoadPara: 数据标识(4)+剩余记录块数(1)+后续帧序号(1)+起始地址(4)+剩余有效记录数(2)+给定时间(5)+上一次电量(4)
			memcpy( RdLoadPara[Ch]+0, InBuf+2, 4 );
			RdLoadPara[Ch][5]++;	//后续帧序号
			memcpy( RdLoadPara[Ch]+ 6, RdStr.Addr[0].B08, 6 ); //地址(4)+剩余记录数(2)
			memcpy( RdLoadPara[Ch]+12, BkClock+1, 5 ); //给定时间
			break;
		}

		Result = Cmp_Bcd_Inv( RdBuf, BkClock+1, 5 ); //0-A=B; 1:A>B; 2:A<B
		if( Result == 2 ) //E2中有数据且需要填充
		{
			while( RdStr.RecTimes.B16 )
			{
				Clear_Wdt();
				if( FLAG_EA == 0 ) return 0x00;	//掉电不处理
				I2C_Read_Eeprom( ee_ploadrec_page+RdStr.Addr[0].B16, LoadOffset[i], RdBuf, LoadLen[i] );

				Result = Cmp_Bcd_Inv( RdBuf, BkClock+1, 5 ); //0-A=B; 1:A>B; 2:A<B
				if( Result != 1 ) //记录时间小于等于期待时间
				{
					RdStr.RecTimes.B16--;
					if( ++RdStr.Addr[0].B16 >= max_ploadrec_p ) RdStr.Addr[0].B16 = 0x00;
				}
				
				if( Result != 2 ) break; //记录时间大于等于期待时间，跳出查询
				if( i == 3 ) memcpy( RdLoadPara[Ch]+17, RdBuf+offset, 4 ); //补上一次电量
			}
		}
		
		if( Result == 0 )
		{
			memcpy( OutBuf+1+t, RdBuf+offset, len );
			if( i == 3 ) memcpy( RdLoadPara[Ch]+17, RdBuf+offset, 4 ); //补上一次电量
		}
		else //补数据
		{
			if( i != 3 ) memset( OutBuf+1+t, 0xFF, len );
			else memcpy( OutBuf+1+t, RdLoadPara[Ch]+17, 4 ); //补上一次电量
		}
		
		t += len;
		RdLoadPara[Ch][4]--;
		Add_Min_Clock( BkClock, Spec );
	}

	if( InBuf[0] == 0x12 ) OutBuf[++t] = InBuf[6];
	
	return (++t);
}

