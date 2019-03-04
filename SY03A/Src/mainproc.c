#include "main.h"
#include "variable4.h"
#include "ex_func.h"
#include "ex_var.h"
#include <string.h>
#include <stdio.h>

#ifndef  ASSERT_NDEBUG
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_errhandler error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_errhandler error line source number
  * @retval None
  */
void assert_errhandler(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


//;================================================================
//;主程
//;================================================================
void Comm_Event_Proc( void )
{
	unsigned char i;
	
	PowerMeasure();		//正反向电量累计处理和金额处理
	
	for( i=0; i<3; i++ )
	{
		if(Uart[i].Flag&B0000_0100)
		{
			Uart_Proc(i);	//通讯处理
			Clear_Wdt();
			break;
		}
	}
		
	if( FLAG_EA == 0 )			//发生欠压(起去抖作用)
	{	
		Sleep();		//进入sleep
	}
}


void Vector_Event_Proc( void )
{
	if( run_alarm_flag && (Zb_Relayctrl_Timer==0) )
	{
		Mt_Alarmstatus_Judge();	//电表报警状态判断
	}	
	
	if( run_price_flag && (run_freeze_flag==0) )
  {
		Year_Ladder_Judge();	//年阶梯启动判断和数据合法性判断
		Price_Judge();	//费率阶梯电价判断
	}	
	
	if( run_timezone_flag )
	{
		Timezone_Judge();	//时区时段费率判断
	}
	
	if( run_loadchk_flag )
	{
		Load_Judge();		//负荷记录判断
	}
	
	if( run_freeze_flag )
	{
		Freeze_Judge();		//冻结和结算判断
	}		
	
	if( Dm_process_flag )	
	{
	  Demand_Process();	//需量处理
	}
	
	if( run_readrtc_flag )
	{
		Read_Sysclock();	//读时钟
	}
	
	if( run_display_flag )
	{			
		Lcd_Display();		//刷新显示
	}
	
	if( run_relaychk_flag )
	{
		Relaystatus_Check();	//继电器检测
	}	
	
	Cover_Check();		//开盖检测
	Displaykey_Check();	//显示按键检测		
}

void Sclock_Event_Proc( void )
{

	if( fwdl_veri_flag )
	{
		Fwdl_Verify();		//正向电量校验
	}	
	
	if( bkdl_veri_flag )
	{
		Bkdl_Verify();		//反向电量校验
	}
	
	if( mode_veri_flag )
	{
		Mode_Verify();		//特征模式字校验
	}
				
	if( reg_veri_flag )
	{
		Veri_Sys_Reg();		//寄存器校验
	}
	
	if( addr_veri_flag )
	{
		Commaddr_Verify();	//通讯地址校验	
	}
	
	if( Dm_veri_flag )
	{
		Demand_Para_Verify();		//需量参数校验
	}
	
	if( disppm_veri_flag )
	{
		Display_Para_Verify();	//显示参数校验
	}
	
	if( dispitem_veri_flag )
	{
		Displayitem_Verify();	//显示项目校验
	}
	if( temperpara_veri_flag )
	{
		Temperpara_Verify();	//温度系数校验	
	}
}


void Energy_Event_Proc( void )
{
	if( check_7051_flag1 )
	{
//		ATT7051_Check();	//校验一次7051调教寄存器
	}
	if( read_back_flag )
	{
		
//		Emu_Data.U.B32 = 0x2200;
    ParaMeasure();	//读瞬时UIP值
//		Judge_Uip_Data();	//UIP数据判断
	}
}
	
int main( void )
{
	Init_System();				    //系统初始化
	
	for( ; ; )
	{
		Clear_Wdt();
		Init_Dw80251();
//		FLAG_EA = 1;		
		PowerLow = 0;
		
		Init_Int();			        //保证INT为高电平(预留)
	
		Adc_Proc();				        //电池和温度adc处理
		
		Vector_Event_Proc();		//time0时标处理
		Comm_Event_Proc();		  //通讯电量和电源处理
		
		Sclock_Event_Proc();		//rtc时标处理
		Comm_Event_Proc();		  //通讯电量和电源处理
		
		Energy_Event_Proc();		//计量处理
		Comm_Event_Proc();		  //通讯电量和电源处理
		
		Card_Proc();			      //IC卡处理
		Comm_Event_Proc();		  //通讯电量和电源处理
	}
}


