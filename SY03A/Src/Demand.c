#include "ex_func.h"
#include "ex_var.h"
#include <string.h>
#include <absacc.h>

void Init_Demand1(void)	//复位费率需量计算数据和参数(时段转换)
{
	Tar_Slidingtimer = 1;//重新启动滑差定时器
	Tar_Demandtimer = Demand_Para.Max_Demand_Period+1;//重新启动费率需量定时器
	memset( Tar_Demand, 0x00, 122 );//数据初始化费率需量脉冲数脉冲数(滑差时间内)
}

void Calculate_Demand( unsigned short *Buf, unsigned char Type )	//最大需量计算和判断
{
	unsigned char i,Z1,Z2,Inpage,BufA[42];
	float Temp_Demand;
	union  B32_B08  Temp_Current_Demand;
	//需量计算
	Temp_Demand = 0;
	Z1 = Demand_Para.Max_Demand_Period/Demand_Para.Sliding_Time;//需量周期/滑差时间
	Z2 = 60/Demand_Para.Max_Demand_Period;//60/需量周期
	for( i=0; i<Z1; i++ )
	{
		Temp_Demand += Buf[i+1];
	}
	Temp_Demand = Temp_Demand*Z2/default_meterconst_dm;//计算当前需量浮点
	Temp_Current_Demand.B32 = (long int)(Temp_Demand*10000);//计算当前需量整型
	Hex2Bcd( Temp_Current_Demand.B08, Temp_Current_Demand.B08, 4 );//计算当前需量bcd码
		
	//最大需量判断
	if( dl_back_flag )
	{
		I2C_Read_Eeprom( ee_n_demand_page, ee_n_demand_inpage, BufA, ee_ndemand_lenth );//读当前反向最大需量数据
	}
	else	
	{
		I2C_Read_Eeprom( ee_p_demand_page, ee_p_demand_inpage, BufA, ee_pdemand_lenth );//读当前正向最大需量数据
	}
	Inpage = Type*8;
	//Inverse( BufA+Inpage, 3 );//最大需量转换成高字节在前
	
	if( Cmp_Data( Temp_Current_Demand.B08, BufA+Inpage, 3 ) == 0x01 )//当前需量>最大需量
	{
		//Inverse( Temp_Current_Demand.B08, 4 );//当前费率需量转换成低字节在前
		memcpy( BufA+Inpage, Temp_Current_Demand.B08, 3 );//当前需量更新为最大需量
		memcpy( BufA+Inpage+3, Sclock.Clockbackup+1, 5 );//最新需量发生时间
		if( dl_back_flag )
		{
			I2C_Write_Eeprom( ee_n_demand_page, ee_n_demand_inpage, BufA, ee_ndemand_lenth );//写当前反向最大需量数据
		}
		else	
		{
			I2C_Write_Eeprom( ee_p_demand_page, ee_p_demand_inpage, BufA, ee_pdemand_lenth );//写当前正向最大需量数据
		}
	}
//	else
//	{
//		Inverse( Temp_Current_Demand.B08, 4 );//当前费率需量转换成低字节在前
//	}
	
	if( Type==0 ) Current_Demand.B32 = Temp_Current_Demand.B32;//更新当前需量
}


void Init_Demand(void)	//复位需量计算数据和参数(上电,清零,时钟调整,需量周期变更,方向变化)
{
	Slidingtimer = 1;//重新启动滑差定时器
	Demandtimer = Demand_Para.Max_Demand_Period+1;//重新启动需量定时器
	memset( Demand, 0x00, 122 );//数据初始化总需量脉冲数脉冲数(滑差时间内)
	Init_Demand1();
	Current_Demand.B32 = 0;
}



void Cal_Pav(void)
{
	union B32_B08 S1_Temp;	
	
	if(Pav_Cnt == 0) return;
	
	S1_Temp.B32 = Pav_Temp/(float)Pav_Cnt;	//每2秒读一次功率值
	
	Hex2Bcd(S1_Temp.B08, Emu_Data.Pav.B08, 4);
	
	Pav_Temp = 0;
	Pav_Cnt = 0;
}


void Demand_Process()	//需量处理
{
	
	unsigned char i;
	
	Dm_process_flag = 0; //清需处理标志
	//每分钟平均功率
	Cal_Pav();
	
	
	//滑差时间判断
	Tar_Slidingtimer--;
	if( Tar_Slidingtimer==0 )
	{
		for( i=0; i<60; i++ )//费率需量滑差时间内脉冲数移位60次
		{
			Tar_Demand[60-i] = Tar_Demand[59-i];
		}
		Tar_Demand[0] = 0;
		Tar_Slidingtimer = Demand_Para.Sliding_Time;//重新启动滑差定时器	
	}
	Slidingtimer--;
	if( Slidingtimer==0 )
	{
		for( i=0; i<60; i++ )//总需量滑差时间内脉冲数移位60次
		{
			Demand[60-i] = Demand[59-i];
		}
		Demand[0] = 0;
		Slidingtimer = Demand_Para.Sliding_Time;//重新启动滑差定时器
		if( Outputtype==0x01 )//滑差时间到输出脉冲
		{
			Shiduanctrl.Timer = shiduanctrl_timer_delay;
			SHIDUANCTRL_ON;//端口复用需量输出
		}		
	}
			
	//需量周期判断
	Demandtimer--;
	if( Demandtimer==0 )//总需量滑差时间内脉冲数
	{
		Calculate_Demand( &Demand[0],0 );//总最大需量计算和判断
		Demandtimer = Demand_Para.Sliding_Time;//重新启动需量定时器(以滑差时间计算)
		if( Outputtype==0x81 )//第一次需量周期到输出脉冲
		{
			Outputtype = 0x01;
			Shiduanctrl.Timer = shiduanctrl_timer_delay;
			SHIDUANCTRL_ON;//端口复用需量输出
		}
	}
	Tar_Demandtimer--;
	if( Tar_Demandtimer==0 )//费率需量滑差时间内脉冲数
	{
		Calculate_Demand( &Tar_Demand[0],Tariff.Current+1 );//费率最大需量计算和判断	
		Tar_Demandtimer = Demand_Para.Sliding_Time;//重新启动需量定时器(以滑差时间计算)
	}

}



