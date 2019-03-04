#include "ex_func.h"
#include "ex_var.h"
//#include <intrins.h>
#include <string.h>
#include <absacc.h>

void Init_Pad_Io(void)
{	
	GPIOA->DAT = 0x0000FFFF;
	GPIOB->DAT = 0x0000FFFB;
	GPIOC->DAT = 0x0000FFFF;
	GPIOD->DAT = 0x0000FFFF;
	GPIOE->DAT = 0x0000FFFF;
	GPIOF->DAT = 0x00000000;
}

void Init_Pad_Mdoe(void)
{
  GPIO_InitType GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUTPUT_CMOS;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIOBToF_Init(GPIOB, &GPIO_InitStruct);
}

void Init_Int(void)	//保证INT0和INT3为高电平
{
}

void INIT_F251WDT(void)
{	
    WDT_Enable();//使能WDT
}

void Clear_Wdt(void)
{
    WDT_Clear();
}

void Clear_SYS_Wdt(void)
{
    WDT_Clear();
}

void Delay_Initial(unsigned char Num) //延时Num*5ms(最长延时最好小于800ms)
{
	unsigned char i;
	
	Clear_Wdt();
	for( i=0; i<Num; i++ )
	{
		I2c_Wait_5ms();
		if( FLAG_EA == 0 ) break;
	}
	Clear_Wdt();
}


void Clear_Nv_Ram(void)//清startup里未清除的部分ram
{
//	memset( (uint08 near*)(0xF50), 0x00, 0x56 );
}



void Nvram_Judge(void)	//掉电不清除ram合法性判断
{
	if( (Password_Err[0]>Defpassworderrcnt) || (Password_Err[1]>Defpassworderrcnt) || (Mac_Err_Times > 200) ) Clear_Nv_Ram();//清startup里未清除的部分ram
	
}


void Note_Reset_Number(void)	//记录上电复位和wdt复位次数,如何是上电复位清ram
{	
	unsigned char i;
	
	for( i=0; i<20; i++ ) I2c_Wait_5ms();	//延时100ms
		
	if( I2C_Read_Eeprom44( ee_meterstatus1_page, Eeprom.Buffer, ee_meterstatus1_lenth)==0 )//读电表状态字到ram成功
	{
		Meter_Status_S.B08  = Eeprom.Buffer[0]&B0000_1000;//恢复剩余金额到赊欠门限(首次)
		Meter_Status1_S.B08 = Eeprom.Buffer[1];
		Meter_Status2_S.B08 = Eeprom.Buffer[2];
	}
	
	if( I2C_Read_Eeprom44( ee_factorymode_page, Eeprom.Buffer, ee_factorymode_lenth ) == 0 )//读错误或者剩余次数非零则允许清零
	{
		if( Eeprom.Buffer[0] == 0x55 ) progenable = 1;	//编程使能
		else progenable = 0;	//编程禁止
	}
	
//	if( RSTFLAG & B0000_1000 )
//	{	
//		I2C_Read_Eeprom44( ee_wdt_page, Eeprom.Buffer,ee_wdt_lenth );//上电复位次数加1
//		Eeprom.Buffer[1]++;
//		if( Eeprom.Buffer[1] == 0 ) Eeprom.Buffer[0]++;
//		I2C_Write_Eeprom44( ee_wdt_page, Eeprom.Buffer, ee_wdt_lenth );
//		RSTFLAG=0;
//		Clear_Nv_Ram();//清startup里未清除的部分ram
//		
//		if( I2C_Read_Eeprom( ee_reportdata_page, 0, Eeprom.Buffer, ee_reportdata_lenth)==0 )//读事件上报数据
//		{
//			if( Eeprom.Buffer[0] != 0xff )//防止出现非法数据
//			{
//				System_Status = (Eeprom.Buffer[0]&0x3f);//恢复标志
//				if( Eeprom.Buffer[0]&0x80 ) Cover.Status = 1;//恢复开盖状态
//				memcpy( Report_Data, Eeprom.Buffer+1, ee_reportdata_lenth-1 );
//			}
//		}
//
//	}
	//33
//	if ( RSTFLAG & B0001_0000 )
//	{	
//		I2C_Read_Eeprom44( ee_wdt_page, Eeprom.Buffer, ee_wdt_lenth );//wdt复位次数加1
//		Eeprom.Buffer[3]++;
//		if( Eeprom.Buffer[3] == 0 ) Eeprom.Buffer[2]++;
//		I2C_Write_Eeprom44( ee_wdt_page, Eeprom.Buffer, ee_wdt_lenth );
//		RSTFLAG=0;	
//	}
//		
//	else if ( RSTFLAG & B0000_0001 )
//	{	
//		I2C_Read_Eeprom44( ee_wdt_page, Eeprom.Buffer, ee_wdt_lenth );//系统wdt复位次数加1
//		Eeprom.Buffer[5]++;
//		if( Eeprom.Buffer[5] == 0 ) Eeprom.Buffer[4]++;
//		I2C_Write_Eeprom44( ee_wdt_page, Eeprom.Buffer, ee_wdt_lenth );
//		Clear_Nv_Ram();//清startup里未清除的部分ram
//		RSTFLAG=0;	
//	}
//	
//	else if ( (LVDSTATUS & B0000_0001) || (RAMREG3 == 0xaa) )//上电唤醒或者通过缓升缓降初始化
//	{
//		Nvram_Judge();//掉电不清除ram合法性判断
//		System_Status = System_St_Bk; //恢复标志
//		LVDSTATUS = 0;
//	}
//	
//	else//异常
//	{
//		I2C_Read_Eeprom44( ee_wdt_page, Eeprom.Buffer, ee_wdt_lenth );//异常复位次数加1
//		Eeprom.Buffer[7]++;
//		if( Eeprom.Buffer[7] == 0 ) Eeprom.Buffer[6]++;
//		I2C_Write_Eeprom44( ee_wdt_page, Eeprom.Buffer, ee_wdt_lenth );
//		Clear_Nv_Ram();//清startup里未清除的部分ram
//		
//		if( I2C_Read_Eeprom( ee_reportdata_page, 0, Eeprom.Buffer, ee_reportdata_lenth)==0 )//读事件上报数据
//		{
//			if( Eeprom.Buffer[0] != 0xff )//防止出现非法数据
//			{
//				System_Status = (Eeprom.Buffer[0]&0x3f);//恢复标志
//				if( Eeprom.Buffer[0]&0x80 ) Cover.Status = 1;//恢复开盖状态
//				memcpy( Report_Data, Eeprom.Buffer+1, ee_reportdata_lenth-1 );
//			}
//		}		
//	}
	
	RAMREG3 = 0;//缓升缓降标志字节
	Nvram_Judge();//掉电不清除ram合法性判断
}

unsigned char Clk_Adj_Scan(unsigned char trimA, unsigned char trimB)
{
//	unsigned char temp08A,temp08B,GoodResult,i,n;
//	union B16_B08 Temp16;
//	uint16 DiffValue,TempDiffValue;
//	
//	DiffValue = 0xffff;
//	GoodResult = (trimA+trimB)/2.0+0.5;
//	
////	ET2CTRL1 = B0010_0100;	//ET2~~ , 捕捉模式,周期捕捉,上沿清零，连续捕捉
////	ET2CTRL2 = B1100_0000;	//计数源Fcpu、捕捉源32768、门控
////	ET2CFG1  = B0000_0001;	//时钟1=32768, Group2信号=fcpu, Group1信号=32768
////	
////	ET2PRESCALE = 127;//128分频

////	ET2IE	 = 0;		//禁止ET2中断
////	ET2IF	 = 0;		//bit0：捕捉中断；bit1：溢出中断
////	
//	if(trimA<trimB) 	//从小到大
//	{
//		temp08A = trimA;
//		temp08B = trimB;
//	}
//	else
//	{
//		temp08A = trimB;
//		temp08B = trimA;	
//	}
//		
////	for( i=temp08A; i<=temp08B; i++ )//扫描trim值
////	{
////		RCTRIM = i;
////		Clear_Wdt();
////		
////		ET2CTRL1 |= B1000_0000;	//启动ET2
////		for( n=0; n<6; n++ ) //理论上判断需要3.906ms = 128/32768 s
////		{					  
////			for( Temp16.B16=0x00; Temp16.B16<1500; Temp16.B16++ )	//7.5ms@8M	  15ms@4M	
////			{
////				if( ET2IF & B0000_0001 ) break;
////			}		
////			ET2IF = 0x00;
////		}	
////		ET2CTRL1 &= B0111_1111; 
////		if( Temp16.B16 >= 1500 ) continue;
////        	
////		Temp16.B08[0] = ET2PRESETH;
////		Temp16.B08[1] = ET2PRESETL;
////		
////		if(Temp16.B16>(31250/clkmode))
////		{
////			TempDiffValue = Temp16.B16-(31250/clkmode);
////		}
////		else
////		{
////			TempDiffValue = (31250/clkmode)-Temp16.B16;
////		}
////		
////		if( TempDiffValue<DiffValue )//判断结果是否变得更好了
////		{
////			DiffValue = TempDiffValue;
////			GoodResult = RCTRIM;
////		}
//	}
//	return GoodResult;
    return 0;
}

void Clk_Adj_Proc( void )
{
//	union B16_B08 Temp16;
//	unsigned char n,m,tempbyte,TrimBuf;	
//		
//	Adj_Clk_Timer_S = Adjclk_timer_delay_s;	//重新启动定时8M时钟调校功能(计量uart专用)
//	Adj_Clk_Timer = Adjclk_timer_delay;	//重新启动定时8M时钟调校功能
	
//	ET2CTRL1 = B0010_0100;	//ET2~~ , 捕捉模式,周期捕捉,上沿清零，连续捕捉
//	ET2CTRL2 = B1100_0000;	//计数源Fcpu、捕捉源32768、门控
//	ET2CFG1  = B0000_0001;	//时钟1=32768, Group2信号=fcpu, Group1信号=32768
//	
//	ET2PRESCALE = 127;//128分频

//	ET2IE	 = 0;		//禁止ET2中断
//	ET2IF	 = 0;		//bit0：捕捉中断；bit1：溢出中断
//	
//	for( m=0; m<6; m++ )
//	{
//		Clear_Wdt();
//		
//		ET2CTRL1 |= B1000_0000;	//启动ET2
//		for( n=0; n<6; n++ ) //理论上判断需要3.906ms = 128/32768 s
//		{					  
//			for( Temp16.B16=0x00; Temp16.B16<1500; Temp16.B16++ )	//7.5ms@8M	  15ms@4M	
//			{
//				if( ET2IF & B0000_0001 ) break;
//			}		
//			ET2IF = 0x00;
//		}	
//		ET2CTRL1 &= B0111_1111; 
//		if( Temp16.B16 >= 1500 ) continue;
//        	
//		Temp16.B08[0] = ET2PRESETH;
//		Temp16.B08[1] = ET2PRESETL;
//		
//		if( (Temp16.B16>(31250-120)/clkmode)&&(Temp16.B16<(31250+120)/clkmode) ) return; //修正到理想值直接返回
//        	
//		TrimBuf = RCTRIM;
//		if( Temp16.B16 > 31250/clkmode )	//31250 = 8M*128/32768; 156.25 = (15625*2/4*2M)*40k
//		{
//			Temp16.B16 -= 31250/clkmode;
//			tempbyte = Temp16.B16/(156.25/clkmode);
//        	
//			if( tempbyte > 0x20 ) tempbyte = 0x20;
//			
//			if( RCTRIM >= tempbyte )
//			{
//				RCTRIM -= tempbyte;
//			}
//			else
//			{
//				RCTRIM = 0x00;
//			}
//			
//		}
//		else
//		{
//			Temp16.B16 = 31250/clkmode-Temp16.B16;
//			tempbyte = Temp16.B16/(156.25/clkmode);
//        	
//			if( tempbyte > 0x20 ) tempbyte = 0x20;
//        	
//			if( (RCTRIM+tempbyte)<=0x7F)
//			{
//				RCTRIM += tempbyte;
//			}
//			else
//			{
//				RCTRIM = 0x7F;
//			}			
//		}
//			
//		if( m == 5 )				//未找到理想值,采用最后2次的值取平均
//		{
////			TrimBuf += RCTRIM;
////			RCTRIM = TrimBuf/2.0+0.5;				
//			RCTRIM = Clk_Adj_Scan(TrimBuf,RCTRIM);		
//		}
//	}	
}

void Init_Dw80251(void)
{
//	IE   = B0001_0010;	//使能TIME0中断,使能串口(被外设占用)中断
//	IPH  = B0000_0000;	//无优先级
//	IPL  = B0000_0000;	//无优先级
//	
//	AIE  = B0000_0111;	//使能附加中断0/1/2（被芯片各种外设占用）
//	AIPH = B0000_0000;	//无优先级
//	AIPL = B0000_0000;	//无优先级
//	
//	TMOD = B0000_0001;	//16bit定时,定时时间2731*1.5us=4096us
////	TL0  = 0x55;		//65536-2731=0XF555,@8M
////	TH0  = 0xF5;
//	TR0  = 1;		//启动time0
}

void Clock_Init(void)
{
  CLK_InitTypeDef CLK_Struct;

  CLK_Struct.ClockType = CLK_TYPE_AHBSRC \
                        |CLK_TYPE_PLLL   \
                        |CLK_TYPE_HCLK   \
                        |CLK_TYPE_PCLK;
  CLK_Struct.AHBSource      = CLK_AHBSEL_LSPLL;
  CLK_Struct.PLLL.Frequency = CLK_PLLL_13_1072MHz;
  CLK_Struct.PLLL.Source    = CLK_PLLLSRC_XTALL;
  CLK_Struct.PLLL.State     = CLK_PLLL_ON;
  CLK_Struct.HCLK.Divider   = 1;
  CLK_Struct.PCLK.Divider   = 6;
  CLK_ClockConfig(&CLK_Struct);
}


void Powerd_Data_Process(void)	//时钟错误用掉电时间恢复和尾数恢复
{
	unsigned char  Buf[ee_powerdown_lenth+2];
	
	if( I2C_Read_Eeprom( ee_power_down_page, 0x00, Buf, ee_powerdown_lenth ) == 0 )
	{
		memcpy( Decimal.Fw, Buf+7, 4 );//恢复正向/反向脉冲尾数和脉冲增量
		
		if( clock_error == 0 )
		{
			if( Cmp_Bcd_Inv(Sclock.Clockbackup, Buf, 6)==2 ) clock_error = 1;//上电时间小于掉电时间
			else
			{
				Buf[7] = Buf[5];//保存掉电年
				Buf[5] = Hex_To_Bcd(Bcd_To_Hex(Buf[5])+10);	//掉电时间加n年(n=10)
				if( Cmp_Bcd_Inv(Sclock.Clockbackup, Buf, 6)==1 ) clock_error = 1;//上电时间大于掉电时间加n年	
				Buf[5] = Buf[7];//掉电年恢复
			}
			
			if( clock_error ) Set_Reportdata(0*8+7);//置时钟错误上报标志
		}
		
		if( clock_error )
		{
			memcpy( Buf+7, Buf, 3 );//秒分时
			memcpy( Buf+11, Buf+3, 3 );//日月年
			Buf[10] = Buf[6];//周
			Set_Rtc( 0, Buf+7, 7 );//周在中间
			Read_Sysclock();	//读时钟
			
			//保存故障点时钟即新时钟起始时间
			I2C_Write_Eeprom( ee_newclock_page, ee_newclock_inpage, Sclock.Clockbackup, ee_newclock_lenth );
		}
		
		if( Buf[18] != 0xff )//防止出现非法数据
		{
			memcpy( Sclock.Clockbackup, Buf, 7 );//使用掉电时间完成事件记录
			if( Buf[18]&0x80 ) Note_Run_Event( ee_powerdownn_inpage ); //掉电事件起始记录
			if( Buf[18]&0x40 ) relay_error = 1;	//负荷开关异常结束记录
			if( Buf[18]&0x20 )
			{ 
				Note_Run_Event(ee_maxi_inpage|0x80);	//记录电流过载事件(结束)
			}
			Syn_Clock();	//同步时钟
		}
		
		Wr_Powerdown_Data(0);//写掉电相关数据到数据ee(主要目的清脉冲尾数和相关标志)
	}
		
	run_loadchk_flag = 0;	//防止上电误触发负荷记录
	if( Crc_Check( Load.Validpoint[0].B08, ee_loadpoint_lenth+2 ) )	//ram负荷记录指针错误
	{
		memcpy( Load.Validpoint[0].B08, Buf+23, ee_loadpoint_lenth+2 );//读负荷记录指针到ram
	}
}
void MeterParaRead( void )
{
    unsigned long Tmp1;

    Tmp1 = (unsigned short)(1.5*MeterParameter.MeterUn);
    WORD_BCD2( (unsigned char*)&MeasPara.Un150, Tmp1 );
    
    Tmp1 = (unsigned short)(0.9*MeterParameter.MeterUn);
    WORD_BCD2( (unsigned char*)&MeasPara.Un90, Tmp1 );

    Tmp1 = (unsigned short)(0.75*MeterParameter.MeterUn);
    WORD_BCD2( (unsigned char*)&MeasPara.Un75, Tmp1 );

    Tmp1 = (unsigned short)(0.68*MeterParameter.MeterUn);
    WORD_BCD2( (unsigned char*)&MeasPara.Un68, Tmp1 );

    Tmp1 = (unsigned short)(0.60*MeterParameter.MeterUn);
    WORD_BCD2( (unsigned char*)&MeasPara.Un60, Tmp1 );

    Tmp1 = (unsigned short)(0.40*MeterParameter.MeterUn);
    WORD_BCD2( (unsigned char*)&MeasPara.Un40, Tmp1 );

    Tmp1 = (unsigned short)(MeterParameter.MeterIb*MeterParameter.CT/10);
    HEX4_BCD4( (unsigned char*)&MeasPara.StartI, Tmp1 );//??????BCD XXX.XXX
    Tmp1 = (unsigned short)(MeterParameter.MeterIb*MeterParameter.CT);
    HEX4_BCD4( (unsigned char*)&MeasPara.StartI2, Tmp1 );//??????BCD XX.XXXX
    
    Tmp1 = (MeterParameter.MeterUn*MeterParameter.MeterIb*MeterParameter.CT)/10000;//??????
    HEX4_BCD4( (unsigned char*)&MeasPara.StartP, Tmp1 );//BCD XX.XXXX
    
    Tmp1 = (MeterParameter.MeterUn*MeterParameter.MeterIb*MeterParameter.CT)/1000;//??????
    HEX4_BCD4( (unsigned char*)&MeasPara.StartP2, Tmp1 );//BCD XX.XXXXX

    Tmp1 = MeterParameter.MeterIb*5;
    HEX4_BCD4( (unsigned char*)&MeasPara.Ib05, Tmp1 );//?????????0.05Ib
    
    Tmp1 = MeterParameter.MeterIb*30;
    HEX4_BCD4( (unsigned char*)&MeasPara.Ib30, Tmp1 );//?????????0.3Ib
    
    Tmp1 = MeterParameter.MeterIb*35*10;
    HEX4_BCD4( (unsigned char*)&MeasPara.Ib350, Tmp1 );//?????????3.5Ib
    
    Tmp1 = MeterParameter.MeterImax*12*10;
    HEX4_BCD4( (unsigned char*)&MeasPara.Imax120, Tmp1 );//?????1.2Imax
    
    MeasPara.DmdCp = MeterParameter.MeterCP;//????????
    
//    //Ir_Power_Cnt =0;//????????
//    Ir_Power_Time = 0;//?????
//    memcpy((unsigned char*)&Timer.KeyAble_Tmr,(unsigned char *)&SystermInitTab,3);//????????
}


void Init_Relay( void )
{	
	if( Relay_Mode != 0x55 )//内置继电器
	{
		{RELAYOFF1; __NOP(); RELAYOFF2;}//磁保持继电器无效
	}
	else
	{
		if(Mode.Feature1&0x01)//脉冲方式
		{
			RELAYOFF3;
		}
		else
		{
			if(relay_status_flag==0)RELAYOFF3;//继电器闭合
			else RELAYON3;//继电器断开
		}
	}
}

void IntRTC( void )
{
	NVR_RTCINFO RTCData;
  RTC_AutCalType RTC_AutoCalInitStruct;
  uint32_t retval;
	
	retval = NVR_GetInfo_LoadRTCData(&RTCData);
  if (retval)
  {
    return;
  }
  
	RTC_AutoCalInitStruct.ADCSource = RTC_ADCS_DATA;
  RTC_AutoCalInitStruct.ATClockSource = RTC_ATCS_SEC;
  RTC_AutoCalInitStruct.ATDelay = RTC_ATDELAY_15MS;
  RTC_AutoCalInitStruct.Period = 30 - 1;
  RTC_AutoCalInit(&RTC_AutoCalInitStruct);
	
//	RTC_AutoCalCmd(ENABLE);
  
  while (RTC_GetINTStatus(RTC_INTSTS_ACDONE) == 0);
  RTC_ClearINTStatus(RTC_INTSTS_ACDONE);
	
}



void Init_System( void )
{	
	__disable_irq();
	FLAG_EA = 0;                         //关闭全局终中断	
	BDMODE = 1;                     //本地表计
	IICPowerON = 0; 
	INIT_F251WDT();
	Clear_Wdt();
	
	Clock_Init();

	Init_Pad_Io();	
	Veri_Pad();
	Release_Rf();
	
	Veri_Sys_Reg();
	
//************计量复位处理**************//
	UART4->CTRL &= ~(BIT0|BIT1 );  
	GPIOB->OEN &= ~BIT0; GPIOB->IE &= ~BIT0;//配置端口为输出模式
	METER_RST0;                    //计量芯片复位；
	NVIC_DisableIRQ(PMU_IRQn );
//************PLC复位处理******************//
	PLC_RST0;		                    //复位载波(输出低)
//************上电LCD显示处理**************//
	
	
	Lcd_Disp_Off();
	Display_Para_Verify();	        //显示数据校验
	Displayitem_Verify();	          //显示项目校验
	Display.Number=0xff;
	Display.Timer = Display_Para.A_Time;//上电全显
	Init_Led_Timer = Display_Para.A_Time;//点亮背光	
	Lcdled.B16 = Display_Para.A_Time;//点亮背光
	Lcd_Display();		              //上电全显
	Lcd_Disp_On();	                //打开lcd正常显示
	
	//	if( !(LVDSTATUS & B0000_0010) )//未发生欠压
//	{
		RELAYLEDON;		//跳闸灯和报警灯亮一下
		LCDLEDON;
//	}


  Note_Reset_Number();	          //记录上电复位和wdt复位次数
	Resetworking_Timer = resetworking_timer_delay;//启动复位脉冲	
	Mode_Verify();		              //特征模式字校验
	Init_Relay();		                //上电继电器处理	
	Veri_Uart_All();	              //通讯波特率校验
	Commaddr_Verify();	            //通讯地址校验
	Demand_Para_Verify();		        //需量参数校验
	
	Read_Sysclock();	              //读时钟
	Fwdl_Verify();		              //正向电量校验
	Bkdl_Verify();		              //反向电量校验
	Powerd_Data_Process();	        //掉电数据处理
	Mt_Alarmstatus_Judge();	        //电表报警状态判断
	Powerd_Hourwakeup(2);         	//小时唤醒事件判断
	RTCIF = 0;
	Temperpara_Verify();          	//温度系数校验	
	
	Clear_Wdt();
	Timezone_Judge();	              //时区时段费率判断
	
	GetKeyStatus();		              //读当前密钥状态
	
	if( BDMODE )                    //本地模式
	{
		Year_Ladder_Judge();	        //年阶梯启动判断和数据合法性判断
		Price_Judge();		            //费率阶梯电价判断
	}
	
	Festival_holiday_Judge();                 //上电立即启动节假日判断
	 MeterParaRead(  );
////	if( !(LVDSTATUS & B0000_0010) )//未发生欠压
////	{
//		RELAYLEDON;		//跳闸灯和报警灯亮一下
//		LCDLEDON;
////	}
	Adcctrl.Timer=8;	              //保证很快启动电池电压和温度adc
	below_60u_flag = 1;
	Voltage_Low_Count = Deflowvoltagecount;
	
	*PMU_CONTROL =0x00000000;       //计量芯片初始化前关闭端口中断
	METER_RST1;		                  //计量芯片使能(保证足够延时后才处理计量芯片)	  
	GPIOB->OEN |= BIT0; GPIOB->IE |= BIT0;//恢复端口为输入模式
	
	PowerDetCnt2 = Def_Powerdetcnt2;//上电3s内掉电，不做电源异常检测
	
	//IntRTC();
	
  FLAG_EA = 0x5555AAAA;	
	__enable_irq(); 
	
	PowerLow = 0;                   //上电RTC处理标志
	
	PLC_RST1_Timer=2;               //载波复位延时2s
	
	Delay_Initial(150);	            //延时750ms
	if( FLAG_EA == 0 ) Sleep();         	//进入sleep

//	Clk_Adj_Proc();		//根据外部的32K晶体来调整内部8M的偏差
//	Clk_Adj_Proc();		//根据外部的32K晶体来调整内部8M的偏差
//	Clk_Adj_Proc();		//根据外部的32K晶体来调整内部8M的偏差
	
	Init_Dayfreeze_Judge();	        //上电日冻结补存
	Calc_Pwr_Proc();              	//正反向电量累计处理和金额处理
	
	
	Delay_Initial(20); 
	
	Meter_RN7302Para_Verify();
  RN8302Init();
	
	
	
	
	IICPowerON = 1;                 //上下电EEPROM处理差异标志。
//	*PMU_CONTROL =0x00000001; 	    //打开计量
//	NVIC_EnableIRQ(PMU_IRQn );
	
	Pav_Temp = 0;	
	Pav_Cnt= 0;
	Init_Demand();		//需量计算初始化
	
	Esam_Timer = 4;		              //上电4秒后复位ESAM(等待电源更加稳定)
	Relay_Space_Timer = 2;	        //启动2秒上电延时控制继电器定时器
	PowerDetCnt0 = Def_PowerDetCnt0;//上电继电器检测电源去抖次数
	Read_Sysclock();	              //再次读时钟
	comm_relaystatus_flag = real_relaystatus_flag;//刷新继电器实际状态(通讯bit4专用)
	Adcctrl.Timer = 3;              //保证立即启动ADC电池电压检测	 
  RFTESTCNT=0;
	KeySTS_OK_flag = 0; //密钥更新标志清零
	KEY_Status = 0;
	RTC_Status = 0;
}


float GetADCResult(void) 
{
	uint16 i,j,temp;
	for(i=0;i<15;i++) //BubbleADC
	{
		for(j=i+1;j<16;j++) 
		if(ADCResultArr.Buffer[i]>ADCResultArr.Buffer[j]) 
		{
			temp=ADCResultArr.Buffer[i];
			ADCResultArr.Buffer[i]=ADCResultArr.Buffer[j];
			ADCResultArr.Buffer[j]=temp;
		} 
	}

	temp = 0;
	for(i=1;i<15;i++)
	{
		temp +=	ADCResultArr.Buffer[i];
	}

	return (temp/14.0);
}

void Adc_Proc(void)		//电池和温度adc处理
{
	unsigned short i;
//	signed char j; short Temp16;
// 	float T25ADC,AverADCf;
// 	float Fa,T,T25,Rtcadj,TopF_Rtc_Adj;
	float Fa,T;
 	union B16_B08 Temp;
// 	unsigned char Result;
//	signed char BufC[ee_temperature_lenth+2];
//	unsigned char tempCapH,tempCapL;
//  ADCInitType ADC_InitStruct;
  NVR_RTCINFO NVR_PxStruct;
	int32_t P2;
  int16_t P1, P0;
  int16_t adc_data;
  uint32_t retval;
	
	if ( Adcctrl.Status == B0000_0010 )//温度adc数据处理,用于温度补偿
	{		
		Adcctrl.Status = 0;
	
		for(i=0; i<200; i++)//
		{
			if(ANA->INTSTS & BIT0)
			{
				break;
			}
		}
		if ( i == 200 ) return ;
		
		adc_data = ANA->ADCDATAA ;
		ANA->INTSTS = BIT0;		
		
	  retval = NVR_GetInfo_LoadRTCData(&NVR_PxStruct);
  
	  if (retval & 0x1U)
    {
      return ;
    }
    else
    {
      P0 = NVR_PxStruct.RTCTempP0;
      P1 = NVR_PxStruct.RTCTempP1;
      P2 = NVR_PxStruct.RTCTempP2;
    }	
		
	  T = (float)((((P0 * ((adc_data*adc_data)>>16)) + P1*adc_data + P2) >> 8) );
		Temp.B16 = T;		
		if( Temp.B16& 0x8000 )//计算当前温度
		{
			Temp.B16 = ~Temp.B16+1;
			Temp.B16 = Temp.B16*10/256;
			Hex2Bcd( Temp.B08, Fun_Para.Temper_M.B08, 2 );//转换成2字节bcd,现在无需idata数据
 			Fun_Para.Temper_M.B08[1] |= B1000_0000;//最高位置1,表示负的温度			
		}
		else 
		{			
			Temp.B16 = Temp.B16*10/256;
			Hex2Bcd( Temp.B08, Fun_Para.Temper_M.B08, 2 );//转换成2字节bcd,现在无需idata数据
		}
    ANA->ADCCTRL &= ~BIT31;
	  ANA->REG3 &=  ~BIT0;	
	}

	if ( Adcctrl.Status == B0000_0001 )//电池adc处理
	{	
		Adcctrl.Status = 0;
	
		for(i=0; i<1000; i++)//
		{
			if(ANA->INTSTS & BIT0)
			{
				break;
			}
		}
		if ( i == 200 ) return ;
		
		Fa = ANA->ADCDATA2 ;
		if( Fa >60000 ) Fa = 0 ;
		ANA->INTSTS = BIT0;
		Fun_Para.Voltage_B.B16 = Fa*0.015392+ 6.667986;

		Hex2Bcd( Fun_Para.Voltage_B.B08, Fun_Para.Voltage_B.B08, 2 );
		
		Temp.B16 = Threshold[4]<<4;	//取电池欠压事件触发下限定值
		
		if( Fun_Para.Voltage_B.B16 < Temp.B16 )	//电池低于阀值，且延时60s后做电池欠压记录
		{
			if( low_voltage_flag == 0 )
			{
				if( Bat_Delay_Timer < Bcd_To_Hex(Threshold[5]) ) //取电池欠压事件判定延时时间定值
					Bat_Delay_Timer += 10;
				else
				{
					low_voltage_flag = 1; //电池低压标志
					Set_Reportdata(0*8+3);//置时钟电池欠压上报标志
					DltoEepromBuf( 6 );
					Event_Record( ee_batterlow_inpage );
				}
			}
		}
		else 
		{
			low_voltage_flag = 0;
			Bat_Delay_Timer = 0x00;
		}
		
		if( Fun_Para.Voltage_B.B16 < 0x80 ) Fun_Para.Voltage_B.B16 = 0; //电压低于0.8V强制归零
	 
		ANA->ADCCTRL &= ~BIT31;
	  ANA->REG3 &=  ~BIT0;	
	}
}
