#include "ex_func.h"
#include "ex_var.h"
//#include <intrins.h>
#include <string.h>
#include <absacc.h>


void Enable_Closerelay(void)//按键闭合继电器或者插卡闭合继电器
{	
	if( relay_commu_disable_close_flag )
	{
		relay_commu_disable_close_flag = 0;
		Store_Meterstatus();
	}
	
	if( !BDMODE ) return; //远程模式

	if((!card_closerelay_flag) && (first_alaram2_flag))
	{
		first_alaram2_flag = 0;
		card_closerelay_flag = 1;
		Store_Meterstatus();
	}
	if((!card_closerelay_flag1) && (first_eqzero_flag))
	{
		first_eqzero_flag = 0;
		card_closerelay_flag1 = 1;
		Store_Meterstatus();
	}
}


void Store_Meterstatus(void)//保存电表状态字节
{
	run_alarm_flag = 1;
	I2C_Write_Eeprom44( ee_meterstatus1_page, &Meter_Status_S.B08, ee_meterstatus1_lenth);
}


void Displaykey_Process(void)
{
	Lcdled.B16 = lcdled_timer_delay;//启动背光定时器
	LCDLEDON;
	Init_Led_Timer = 0;
	Display.Timer = default_keydisplaytimer;
	if (Display.Status & B0000_0001 ) 
	{
		if (DisKeySamp1 == 0xff )
		{ 	
			
			Display.Number--;
			if (Display.Number < 1 )
				Display.Number = Display_Para.K_Number;
			
		}
	 if (DisKeySamp == 0xff )
		{  
			Display.Number++;
		  if (Display.Number > Display_Para.K_Number) Display.Number = 1;
		}
		
	}
	else
	{
		Display.Number  = 1;
		Display.Status |= B0000_0001;//置按键显示标志
		Ic_Type_Reg = 0;//清插卡显示相关标志
	}		
	Display.Status |= B1000_0000;
	run_display_flag = 1;	//置刷新显示标志
}


void Displaykey_Check(void)
{

	if( (!(Display.Status & B1000_0000)) && ((DisKeySamp==0xff)||(DisKeySamp1==0xff))) 
	{
		Displaykey_Process();
		Keyclose_timer = 3;//启动3s按键闭合继电器定时器
		DisKeySamp=0x0f;
	}
	else if( (Display.Status & B1000_0000 ) && ((DisKeySamp==0)&&(DisKeySamp1==0))) 
	{
		Display.Status &= B0111_1111;
		Keyclose_timer = 0;
		DisKeySamp1=0;
	}
	else if( (Display.Status & B1000_0000 ) && (DisKeySamp==0xff) )
	{
		if( Keyclose_timer == 0 )
		{
			Enable_Closerelay();	//按键闭合继电器
		}
	}
}


void Cover_Check( void )
{
	uint16 i;

	if( (Cover.Status == 0) && (COVKEY_P) ) //
	{
		for( i=0; i<650/clkmode; i++ )			//5ms延时判断
		{
			if( COVKEY_N ) break;
		}
		
		if( i == 650/clkmode ) //开盖
		{
			if( Cover_Space_Timer )	return;		//开盖延时未到
			
			Cover.Status = 1;
			Note_Run_Event(ee_covern_inpage);	//记录开盖事件记录
		}
	}
	else if( (Cover.Status) && (COVKEY_N) )
	{
		for( i=0; i<650/clkmode; i++ )
		{
			if( COVKEY_P ) break;
		}

		if( i == 650/clkmode ) //合盖
		{
			if( Cover_Space_Timer )	return;		//开盖延时未到
			
			Cover.Status = 0;
			Note_Run_Event(ee_covern_inpage|0x80);	//记录合盖事件记录
			if( RAMREG2 ) RAMREG2--;	//停电开盖次数减1
		}
	}
	
	Cover_Space_Timer = Bcd_To_Hex(Threshold[3]); //取开表盖事件判定延时时间定值
}


void Relaystatus_Check(void) //继电器状态检测(脉冲方式,断开高电平)
{
	uint16 i;

	run_relaychk_flag = 0;	//清继电器状态检测标志
	
	if( Relay_Space_Timer || (PowerDetCnt0 < 4) || ( FLAG_EA == 0 ) )
	{
		if( relay_error ) Relayerror_Count = Defrelayerrorcount_r;
		else Relayerror_Count = 0;
		return; 
	}
	
	if( Relay_Mode != 0x55 )//内置继电器
	{
		if ( Relayctrl.Timer ) return; 
	}
	else
	{
		if((Mode.Feature1&0x01)&&Relayctrl.Timer ) return; 
	}
	
	for( i=0; i<3600/clkmode; i++ )				//25ms去抖判断
	{
		if( RELAYTEST_P ) break;
	}
	
	if( ((relay_status_flag==1) && ((power_zero_flag==0) || (i != 3600/clkmode))) //命令为断开,但是有功率或者反馈通
	    ||  ( (relay_status_flag==0) && (power_zero_flag==1) && (i == 3600/clkmode) //继电器命令为闭合
		  && ( (Relay_Mode != 0x55) || ((Relay_Mode == 0x55)&&(relay_cleardl_flag==0)) ) ) )
	{		
		if( relay_status_flag ) comm_relaystatus_flag = 0;//刷新继电器实际状态(通讯bit4专用)
		else comm_relaystatus_flag = 1;
		if( Relayerror_Count1 < Defrelayerrorcount ) Relayerror_Count1++;
		else
		{
			if( relay_status_flag ) real_relaystatus_flag = 0;//刷新继电器实际状态
			else real_relaystatus_flag = 1;
		}
		Relayerror_Count2 = 0;
		
		if( relay_error ) return;
		
		if( Relayerror_Count < Defrelayerrorcount_r )
		{
			Relayerror_Count++;	//继电器错误错误计数器加1
		}
		else
		{
			Note_Run_Event(ee_relayerrn_inpage);	//负荷开关异常开始记录
			relay_error = 1;//置继电器错误
		}
	}
	else
	{
		comm_relaystatus_flag = relay_status_flag;	//刷新继电器实际状态(通讯bit4专用)
		if( Relayerror_Count2 < Defrelayerrorcount ) Relayerror_Count2++;
		else
		{
			real_relaystatus_flag = relay_status_flag;	//刷新继电器实际状态
		
			if( relay_success_flag == 0 )
			{
				relay_success_flag = 1;				//置继电器操作成功标志
				Store_Meterstatus();
				
				if( relay_status_flag )
				{
					if( RdRecTimes( ee_openrelayn_inpage ) && (Relay_modechange_flag==0) )  Set_Reportdata(1*8+6);//进行拉闸成功上报
				}
				else
				{
					if( RdRecTimes( ee_closerelayn_inpage ) && (Relay_modechange_flag==0) )  Set_Reportdata(1*8+7);//进行合闸成功上报
				}
			}
		}
		Relayerror_Count1 = 0;
		
		if( Relayerror_Count )	//继电器错误计数器
		{
			Relayerror_Count--;
		}
		else
		{		
			if( relay_error ) Note_Run_Event(ee_relayerrn_inpage|0x80);	//负荷开关异常结束记录
			relay_error = 0;
		}
	}	
}
