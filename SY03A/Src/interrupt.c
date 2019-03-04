#include "ex_var.h"
#include "ex_func.h"
//#include <intrins.h>
#include <string.h>
#include <stdio.h>

//未使用中断预留入口防止程序飞转(具体见技术手册88页)
//void int0_int( void ) interrupt 0 using 1
//{
//	IE0_= 0;
//	EX0 = 0;
//}
//void int1_int( void ) interrupt 2 using 1
//{
//	IE1_= 0;
//	EX1 = 0;
//}
//void timer1_int( void ) interrupt 3 using 1
//{
//	TF1 = 0;
//	ET1 = 0;
//}
//void timer2_int( void ) interrupt 5 using 1
//{
//	{TF2 = 0;EXF2 = 0;}
//	ET2 = 0;
//}
//void pca_int( void ) interrupt 6 using 1
//{
//	{CF = 0;CCF0 = 0;CCF1 = 0;CCF2 = 0;CCF3 = 0;CCF4 = 0;}
//	EC = 0;
//}
//void Eint3_int (void) interrupt 11 using 1
//{
//	AIF3 = 0;
//	AIE3 = 0;
//}
//void Eint4_int (void) interrupt 12 using 1
//{
//	AIF4 = 0;
//	AIE4 = 0;
//}
//void Eint5_int (void) interrupt 13 using 1
//{
//	AIF5 = 0;
//	AIE5 = 0;
//}
//void Eint6_int (void) interrupt 14 using 1
//{
//	AIF6 = 0;
//	AIE6 = 0;
//}

//void Trap_int (void) interrupt 15 using 2
//{
//}

////time16bit定时,定时时间4096us,
//void timer0_int( void ) interrupt 1 using 1
//{
//	unsigned char Ch;
//	uint16 i;

//	TF0 = 0;			//硬件可以自动清零
//	TR0 = 0;
//	TL0 = init_tl0;
//	TH0 = init_th0;
//	TR0 = 1;
//	if( VectorTimer == 0 ) VectorTimer  = ms_timer_delay;
//	if( VectorTimer == 1 ) run_readrtc_flag = 1;	//允许读时钟
//	if( (VectorTimer == 50) || (((VectorTimer == 20)||(VectorTimer == 80))&&display_rtc_flag) ) run_display_flag = 1;	//允许刷新显示
//	if( VectorTimer == 100 )
//	{
//		run_relaychk_flag = 1;	//允许继电器检测
//		RELAYCTRLON;		//打开控制电平
//		Clear_SYS_Wdt();	//清系统看门狗
//	}
//	VectorTimer--;
//	
//	DisKeySamp<<=1;
//	if(DISKEY_N)  DisKeySamp++;	//显示按键去抖判断

//	CardKeySamp<<=1;
//	if(KEY_N)  CardKeySamp++;	//插卡去抖判断
//	
//	//ADC处理
//	if((Adcctrl.Status & B0000_0011)&&(ADCDATAH & B1000_0000))
//	{
//		ADCResultArr.Buffer[ADCResultArr.ReadCount&0x0f] = ((ADCDATAH&B0000_0111)*0x100+ADCDATAL);
//		ADCResultArr.ReadCount++;

//		//转换32次取后面16次以保证转换数据准确性
//		if(ADCResultArr.ReadCount >= 32)
//		{
//			if ( Adcctrl.Status == B0000_0001 ) 
//			{
//				Adcctrl.Status = B0001_0000;//电池adc结束
//			}
//			else
//			{
//				Adcctrl.Status = B0010_0000;//温度adc结束
//			}
//		}
//	}
//	
//	if ( Shiduanctrl.Timer != 0 ) //时段投切输出脉冲
//	{
//		Shiduanctrl.Timer--;
//	}
//	if( Shiduanctrl.Timer == 0 ) SHIDUANCTRL_OFF;
//		
//	if( Relay_Mode != 0x55 )//内置继电器
//	{
//		if ( Relayctrl.Timer != 0 ) 
//		{
//			Relayctrl.Timer--;
//		}
//		if( Relayctrl.Timer == 0 ) {RELAYOFF1; _nop_(); RELAYOFF2;}//磁保持继电器无效
//	}
//	else
//	{
//		if(Mode.Feature1&0x01)//脉冲方式
//		{
//			if ( Relayctrl.Timer != 0 ) 
//			{
//				Relayctrl.Timer--;
//			}
//			if( relay_status_flag && (Relayctrl.Timer == 0) ) {RELAYOFF3;}//
//		}
//	}
//	
//	if ( Resetworking_Timer != 0 ) 
//	{
//		Resetworking_Timer--;
//	}
//	if( Resetworking_Timer == 0 )
//	{
//		PLC_RST1;
//	}
//	
//	if ( Zb_Relayctrl_Timer != 0 ) 
//	{
//		Zb_Relayctrl_Timer--;
//	}	
//	
//	Beep_Ctrl();//蜂鸣器控制
//		
//	for ( Ch=0; Ch<3; Ch++ )
//	{
//		if(Uart[Ch].Overtime_Timer != 0)
//		{
//			Uart[Ch].Overtime_Timer--;
//			if( Uart[Ch].Overtime_Timer == 0 ) 
//			{
//					if( Ch==0 ) 
//					{	
//						UARTIE &= B1111_1110;	//关闭TXIE0
//						UARTIE |= B0000_0010;	//打开RXIE0
////						RXSTA0 &= B1111_0001;
//						{RXSTA0 = 0;RXSTA0 = B0101_0000;}//复位接收FIFO
//					}
//					if( Ch==1 ) 
//					{	
//						UARTIE &= B1111_1011;	//关闭TXIE1
//						UARTIE |= B0000_1000;	//打开RXIE1
////						RXSTA1 &= B1111_0001;
//						{RXSTA1 = 0;RXSTA1 = B0101_0000;}//复位接收FIFO
//						R485CTRLR;		//允许接收						
//					}
//					if( Ch==2 ) 
//					{	
//						UARTIE &= B1110_1111;	//关闭TXIE2
//						UARTIE |= B0010_0000;	//打开RXIE2
////						RXSTA2 &= B1111_0001;
//						{RXSTA2 = 0;RXSTA2 = B0101_0000;}//复位接收FIFO
//					}
//					Uart[Ch].Step = 0x00;
//					Uart[Ch].Flag = 0x00;
//					Uart[Ch].Response_Timer = 0x00;
//			}
//		}
//				
//	}
//	for ( Ch=0; Ch<3; Ch++ )
//	{
//		if(Uart[Ch].Response_Timer != 0)
//		{
//			Uart[Ch].Response_Timer--;
//			if( Uart[Ch].Response_Timer == 0 ) 
//			{
//				if ( Uart[Ch].Flag & B0000_1000 )
//				{	
//					
//					if( Ch==0 ) 			//允许中断发送数据
//					{	
//						TXREG0  = 0x68;		//发送0x68
//						UARTIE &= B1111_1101;	//关闭RXIE0
//						UARTIE |= B0000_0001;	//打开TXIE0
//					}
//					if( Ch==1 ) 
//					{	
//						R485CTRLS;		//允许发送
//						TXREG1  = 0x68;		//发送0x68
//						UARTIE &= B1111_0111;	//关闭RXIE1
//						UARTIE |= B0000_0100;	//打开TXIE1
//					}
//					if( Ch==2 ) 
//					{	
//						TXREG2  = 0x68;		//发送0x68
//						UARTIE &= B1101_1111;	//关闭RXIE2
//						UARTIE |= B0001_0000;	//打开TXIE2
//					}
//					Uart[Ch].Len  = 5;//开始发送数据
//					Uart[Ch].Total_Len = Uart[Ch].Buf[9]+16;
//					Uart[Ch].Overtime_Timer = txtimeout_max;
//				}
//				else
//				{
//					Uart[Ch].Response_Timer++;//继续等待
//				}
//			}
//		}			
//	}
//	
//	if( Compile_Polar_485 == 0x55 )	//兼容无极性485
//	{
//		if( (Uart[1].Response_Timer == 0)&&(Uart[1].Overtime_Timer == 0)&&((UARTIF&B0000_1000)==0x00) )
//		{
//			Uart1KeySamp<<=1;
//			if(GPIO0&B0100_0000)  Uart1KeySamp++;		//保证总线空闲时判断,兼容无极性485
//			
//			if( Uart1KeySamp == 0xff ) //高电平 正极性
//			{
//				if( RTXCON1 != B0000_0000 )	RTXCON1 = B0000_0000;//正极性 接收数据不取反,发送数据不取反
//			}
//			else if( Uart1KeySamp == 0x00 )
//			{
//				if( RTXCON1 != B0000_0011 )	RTXCON1 = B0000_0011;//反极性 接收数据取反,发送数据取反		
//			}	 
//		} 
//		else Uart1KeySamp = 0x55;
//	}
//	else
//	{
//		if( RTXCON1 != B0000_0000 )	RTXCON1 = B0000_0000;//正极性 接收数据不取反,发送数据不取反
//	}
//	
//	if( (LVDSTATUS & B0000_0010) && (Relayctrl.Timer == 0) ) 
//	{	
//		for(i=0; i<0x300; i++)		//约3.75ms
//		{
//			if( (LVDSTATUS & B0000_0010) == 0 ) break;
//		}	
//		if( i==0x300 ) //掉电
//		{
//			LCDLEDOFF;//关闭背光(省电)
//			RELAYLEDOFF;//关闭继电器指示灯(省电)
//			BEEPALARMOFF;//关闭蜂鸣器(省电)
//			
//			for(i=0; i<0x300; i++)		//约3.75ms
//			{
//				if( (LVDSTATUS & B0000_0010) == 0 ) break;
//			}	
//			if( i==0x300 ) //掉电
//			{
//				EA = 0;		//关掉全局中断
//			}
//		}
//	}			
//}

////lvd,xstp_int
//void nmi_int (void) interrupt 7 using 2
//{
//	if( (FDET&B0000_0001) ) //低频晶振停振中断
//	{
//		FDETFLAG = 0;		//清XTLF停振标志
//	}

////	if(  )	//低压检测中断
////	{
////	}
//	
//	if( (LVDSTATUS&B0000_0001) && (LVDEFCFG!=Sleeplvdefcfg) ) //LVIF=1, 发生欠压报警中断
//	{
//		LVDSTATUS = 0;	//软件清除 "电压变化中断标志位"
//	}
//}  

//TI RI未用.uart1,etmr1,etmr2
//void serial_int (void) interrupt 4 using 1
//{
//	unsigned char Temp_IF, Temp;
//		
//	//uart1
//	{
//		Temp_IF = UARTIE&UARTIF; //任何对寄存器的操作都会导致其清零, 必须先缓存

//		if( (Temp_IF&B0000_1000) ) //Receive
//		{
//			Temp = RXREG1; //从UART2数据寄存器中取接收数据		
//	
//			Judge_645_Frame( 1, Temp );			
//			if ( Uart[1].Step == 0x08 )
//			{
//				UARTIE &= B0011_0011;	//关闭RXIE1
//				UARTIF &= B1111_0111;	//clr UART1.rxif
//				display_zaibo_flag = 0;
//			}			
//		}

//		if( (Temp_IF&B0000_0100) ) //Transmit
//		{
//			Tx_645_Frame( 1 );
//			UARTIF &= B1111_1011;	//clr UART1.txif
//		}
//	}
//	
//	//etmr1
//	{	
//		Temp_IF = ( ET1IE & ET1IF );
//		if( Temp_IF&B0000_1000 ) //比较匹配中断
////		if( Temp_IF )
//		{
////			if((Emu_Data.I1.B32) && (energy_rst_flag == 0))
//			{
//				if( dl_back_flag == 0 )
//				{
//					Decimal.Fw[0]++;		//正向脉冲尾数加1					
//					if ( Decimal.Fw[0] >= Meter.Const_H )
//					{
//						Decimal.Fw[0] = 0;
//						Decimal.Unit_Pluse++;
//					}
//				}
//				else
//				{
//					Decimal.Bk[0]++;		//反向脉冲尾数加1
//					if ( Decimal.Bk[0] >= Meter.Const_H )
//					{
//						Decimal.Bk[0] = 0;
//						Decimal.Unit_Bkpluse++;
//					}
//				}
//			}
//			
//			ET1IF = 0x00;
//		}
//	}
//	
////	if(  ) //etmr2
////	{
////		Temp_IF = ( ET2IE & ET2IF );
////		if( Temp_IF )
////		{	
////			ET2IF = 0x00;
////		}
////	}
//}

////AIF0,rstflag,disp,uart0,tmpov,i2c
//void Eint0_int (void) interrupt 8 using 1
//{
//	unsigned char  Temp_IF, Temp;
//	AIF0 = 0;
//	
//	//uart0
//	{
//		Temp_IF = UARTIE&UARTIF; //任何对寄存器的操作都会导致其清零, 必须先缓存
//		if( (Temp_IF&B0000_0010)  ) //Receive
//		{
//			Temp = RXREG0; //从UART数据寄存器中取接收数据
//			
//			Judge_645_Frame( 0, Temp );
//			if ( Uart[0].Step == 0x08 )
//			{
//				UARTIE &= B0011_1100;	//关闭RXIE0
//				UARTIF &= B1111_1101;	//clr UART0.rxif
//				display_zaibo_flag = 0;
//			}
//		}
//		
//		if( (Temp_IF&B0000_0001) ) //Transmit
//		{
//			Tx_645_Frame( 0 );
//			UARTIF &= B1111_1110;	//clr UART0.txif
//		}
//	}
//	
//	if( RSTFLAG & B0000_0100 )	//编程使能中断
//	{
//		RSTFLAG &= B1111_1011;	//清编程使能中断标志
//	}
//	
////	if(   ) //disp
////	{
////	}
////	if(   ) //i2c
////	{
////	}
////	if(   ) //TMP_OV
////	{
////	}
//}

////AIF1.ioc,7816,uart2,etmr3
//void Eint1_int (void) interrupt 9 using 1
//{
//	unsigned char Temp_IF, Temp;
//	AIF1 = 0;

////	if( (BkpIF&B0000_0010) ) //etmr3
////	{
////	}

////	if( (BkpIF&B0000_1000) ) //u7816
////	{
////	}

//	if( (GPIO0IE&GPIO0IF)&B0100_0000 ) //ioc
//	{
//		if( (PCON&B0000_1100)==0 ) EA = 0;	//关掉全局中断(sleep唤醒)
//		else GPIO0IF = 0;
//	}
//	
//	if( (GPIO4IE&GPIO4IF)&B0110_0000 ) //ioc
//	{
//		if( (PCON&B0000_1100)==0 ) EA = 0;	//关掉全局中断(sleep唤醒)
//		else GPIO4IF=0;
//	}
//	else	//uart2
//	{
//		Temp_IF = UARTIE&UARTIF; //任何对寄存器的操作都会导致其清零, 必须先缓存
//	
//		if( (Temp_IF&B0010_0000) ) //Receive
//		{
//			Temp = RXREG2; //从UART2数据寄存器中取接收数据

//			Judge_645_Frame( 2, Temp );
//			if ( Uart[2].Step == 0x08 )
//			{
//				UARTIE &= B0000_1111;	//关闭RXIE2
//				UARTIF &= B1101_1111;	//clr UART2.rxif
//				display_zaibo_flag = 1;
//			}	
//		}
//	
//		if( (Temp_IF&B0001_0000) ) //Transmit
//		{
//			Tx_645_Frame( 2 );
//			UARTIF &= B1110_1111;	//clr UART2.txif
//		}
//	}
//}
//	
////AIF2.rtc,spi,etmr4
//void Eint2_int (void) interrupt 10 using 1
//{
//	AIF2 = 0;

//	if( RTCIE&B0000_0100 ) //rtc
//	{
//		if( (PCON&B0000_1100)==0 ) EA = 0;	//关掉全局中断(sleep唤醒)
//		else {RTCIF &= B1111_1110; RTCTMRIF = 0;}	//仅清除秒中断标志
//	}
//	else if( RTCIF&RTCIE ) //rtc
//	{		
//		if( Display.Timer != 0 ) Display.Timer--;
//		
//		if( Successled.Timer != 0 ) Successled.Timer--;//通讯符号延时定时器减1
//		
//		if( Keyclose_timer!= 0 ) Keyclose_timer--;//按键闭合继电器定时器减1
//		
//		Adcctrl.Timer++;
//		if( Adcctrl.Timer == 5 )//启动电池adc 
//		{
//			if( ADCTRIMH  != const_adc_TrimBH ) ADCTRIMH = const_adc_TrimBH;//LSB=2.34mV
//			if( ADCTRIML  != const_adc_TrimBL ) ADCTRIML = const_adc_TrimBL;//校准基准源	
//			ADCCTRL = B0001_1110;
//			Adcctrl.Status= B0000_0001;
//		}

//		if( Adcctrl.Timer == 10 )//启动温度adc 
//		{
//			Adcctrl.Timer = 0;
//			if( Lasttesting_Valid != 0x55 )//非成测数据
//			{
//				if( ADCTRIMH  != 0x06 ) ADCTRIMH = 0x06;//LSB=1.5mV
//				if( ADCTRIML  != 0x40 ) ADCTRIML = 0x40;//校准基准源
//			}
//			else
//			{
//				if( ADCTRIMH  != const_adc_TrimH ) ADCTRIMH = const_adc_TrimH;//LSB=1.5mV
//				if( ADCTRIML  != const_adc_TrimL ) ADCTRIML = const_adc_TrimL;//校准基准源
//			}			
//			ADCCTRL = B0001_1111;
//			Adcctrl.Status= B0000_0010;
//		}	
//		
////		if( Display_Space_Timer != 0 ) //红外遥控唤醒延时
////		{
////			Display_Space_Timer--;	
////			if( Display_Space_Timer==0 ) Lcdled.B16 = Hw_Lcdled_Timer;//启动背光定时器
////		}
//		
//		if( Relay_Space_Timer != 0 ) //上电延时动作继电器
//		{
//			Relay_Space_Timer--;
//			if( Relay_Space_Timer==0 ){ relay_direct_flag = 1; run_alarm_flag = 1; }
//		}
//				
//		Lcdled_Control();	//背光控制
//			
//		RTCIF &= B0000_0100;	//清除小时变化以外的标志
//	}

////	if(  ) //spi
////	{
////	}

////	if(  ) //ETMR4
////	{
////	}
//}


void Judge_645_Frame( uint08 CH, uint08 RcvData ) //using 1 //接收处理
{
	unsigned char i, n, m, status=0;
	
	Uart[CH].Overtime_Timer = txtimeout_max;//接收超时
			
	switch( Uart[CH].Step )
	{
		case 0x00: //等待接收0x68
				if( RcvData == 0x68 )
				{
					Uart[CH].Step = 0x01; //等待接收id.
					Uart[CH].Len = 0;
				}
				else
				{
					status = 0xaa;	//表示接收错误		
				}
				break;
			
		case 0x01: //等待接收id.
				if ( (Uart[CH].Len) >= 6 )
				{
					Uart[CH].Step = 0x02; //等待接收第二个0x68
				}
				break;
	
		case 0x02: //等待接收第二个0x68
				if( RcvData == 0x68 )
				{
					if( CH == 0 )
					{
						Lcdled.B16 = Hw_Lcdled_Timer;//启动背光定时器
						LCDLEDON;
					}
					
					Uart[CH].Flag = 0x00;//通讯标志清零
					
					for( i=0; i<6; i++ )
					{
						if( Uart[CH].Buf[ 1+i ] != 0x99 )  break;//表示接收部分错误
					}
					if( i == 6 ) 
					{
						Uart[CH].Flag = B0000_0001;//广播表号标志
						Uart[CH].Step = 0x03; //等待接收CMD
						break;
					}
					
					for( i=0; i<6; i++ )
					{
						if( Uart[CH].Buf[ 1+i ] != 0xaa )  break;//表示接收部分错误
					}	
					if( i == 6 ) 
					{
						Uart[CH].Flag = B0000_0010;//全aa表号标志
						Uart[CH].Step = 0x03; //等待接收CMD
						break;
					}
					
					for( i=0; i<6; i++ )
					{	
						if( (Uart[CH].Buf[ 1+i ] != Comm_Addr.Comm_Id[ 5-i ]) )
						{
							for( ; i<6; i++ )
							{
								if( Uart[CH].Buf[ 1+i ] != 0xaa ) break;//表示接收部分错误
							}
							if( i == 6 ) Uart[CH].Flag = B0000_0010;//置索位地址标志
							break;
						}
					}
					if( i == 6 )  
					{
						Uart[CH].Step = 0x03; //等待接收CMD
						break;	
					}	
				}
				status = 0x55;
				break;
	
		case 0x03: //等待接收cmd.
				Uart[CH].Step = 0x04; //等待接收LEN
				break;
	
		case 0x04: //等待接收len
				if( RcvData == 0x00 )
				{
					Uart[CH].Total_Len = RcvData;
					Uart[CH].Step = 0x06; //接收cs
				}
				else 
				{
					if( RcvData <= (urx_buf_size-12) )
					{
						Uart[CH].Total_Len = RcvData;
						Uart[CH].Step = 0x05; //等待接收DATA
					}
					else
					{
						status = 0xaa;	//表示接收错误
					}
				}
				break;
	
		case 0x05: //等待接收DATA
				if( Uart[CH].Len == ( Uart[CH].Total_Len + 0x09) ) //帧数据域溢出
				{
					Uart[CH].Step = 0x06; //等待接收cs.								
				}
				
				if( Uart[CH].Len > ( Uart[CH].Total_Len + 0x09) ) //帧数据域溢出
				{
					status = 0xaa; //表示接收错误									
				}
				
				break;
	
		case 0x06: //等待接收cs.
				Uart[CH].Step = 0x07; //等待接收0x16
				break;
	
		case 0x07: //等待接收0x16
				if( RcvData == 0x16 )
				{
					Uart[CH].Step = 0x08;//字节接收正确
					Uart[CH].Overtime_Timer = 0x00;//清接收超时
					Uart[CH].Response_Timer = framedelay_mx;//置响应超时
					Uart[CH].Flag |= B0000_0100;//置整帧数据接收完成标志
				}
				else
				{
					status = 0xaa;	//表示接收错误
				}
				break;
	
		default: 
				status = 0xaa;	//表示接收错误
				break;
		
	}

	if( status == 0x00 )	//字节接收正确
	{
		Uart[CH].Buf[Uart[CH].Len] = RcvData;
		Uart[CH].Len++;
	}
	
	if( status == 0x55 )	//表号错误或者第二个68h错误,需要重新找68h
	{
		status = 0xaa;//接收错误
		
//		if(CH)
		{
			Uart[CH].Buf[Uart[CH].Len] = RcvData;
			Uart[CH].Len++;
			for( n=1; n<Uart[CH].Len; n++ )
			{
				if( Uart[CH].Buf[n] == 0x68 )
				{
					
					i = n;
					for( m=0; m<(Uart[CH].Len-n); m++ )
					{
						Uart[CH].Buf[m] = Uart[CH].Buf[i];
						i++;
					}
					Uart[CH].Len -= n;
					Uart[CH].Step = 0x01;
					if ( Uart[CH].Len ==0x07 ) Uart[CH].Step = 0x02;
					status = 0x00;//接收纠错成功
					break;
					       
				}					
			}
		}
	}	
//	
//	if((*(&RXSTA0+CH))&B0000_1110)
//	{ 	   
//	   (*(&RXSTA0+CH))&=B1111_0001;
//	   status = 0xaa;
//	   if(CH==0)
//	   {
////		if ( Display_Space_Timer==0 ) 
////		{
////			for( i=0; i<20; i++ )//去抖150us
////			{
////				if( RXHW_P ) break;
////			}
////			if( i==20 ) Display_Space_Timer = 2;//启动2s去抖时间	
////		}
//	   }
//	}
//		
	if( status == 0xaa )	//接收错误
	{
		Uart[CH].Step = 0x00;
		Uart[CH].Overtime_Timer = txtimeout_max/5;
	}
}



void Tx_645_Frame( uint08 CH )	//using 1	//数据发送
{
	if( Uart[CH].Len >= Uart[CH].Total_Len )
	{
		if( (CH>0) && (Uart[CH].Flag&B0001_0000) ) reg_veri_flag = 1; //通过寄存器校验完成bps修改
		Uart[CH].Overtime_Timer = 2;//通过超时控制复位通讯
	}
	else
	{
//		if( Uart[CH].Len < 4 ) *(&TXREG0+CH) = 0xfe;
//		else *(&TXREG0+CH) = Uart[CH].Buf[Uart[CH].Len-4];
		Uart[CH].Len++;
		Uart[CH].Overtime_Timer = txtimeout_max;//发送超时
	}
}


void Beep_Ctrl()	//using 1	//蜂鸣器控制
{
	if(!Beep_Timer) 
	{
		BEEPALARMOFF;
		return;
	}
	
	Beep_Timer--;
		
	if(!Ic_Err_Reg)
	{
		if( Beep_Timer == 155 )	
		{
			BEEPALARMON;		//卡片成功,只鸣叫一声
		}
		
		if(!Beep_Timer)
		{
			BEEPALARMOFF;
		}
	}
	else
	{
		if( (Beep_Timer%31) ==0 ) 
		{
			if( (Beep_Timer/31)&0x01 )
			{	
				BEEPALARMON;		//5,3,1响
			}
			else
			{
				BEEPALARMOFF;		//4,2,0灭
			}
		}
	}
}


void Lcdled_Control()	//using 1	//背光控制
{
	if( Lcdled.B16 != 0 ) Lcdled.B16--;
	
	if( Lcdled.B16 || Opendelaytimer.B32 || relay_delay_flag || relay_commu_alarm_flag || Bat_Delay_Timer || low_voltage_flag ||
		(dl_back_flag && ((Mode.Plc==0xa0)||(Mode.Plc==0xaa))) )
	{
		LCDLEDON;
	}
	else if( (BDMODE) && (remainder_alarm1_flag || remainder_alarm2_flag || remainder_zero_flag || charge_relay_open_flag) ) 
	{
		LCDLEDON;
	}
	else if( relay_status_flag && (relay_commu_disable_close_flag ==0) && relay_commu_open_flag )	//新增通讯拉闸点亮背光
	{
		LCDLEDON;
	}
	else if( (Relay_Mode!=0x55) && (relay_status_flag==0) && relay_error && (relay_commu_close2_flag==0) ) //新增合闸失败点亮背光(内置继电器)(除保电解除)
	{
		LCDLEDON;
	}
	else if( (Relay_Mode==0x55) && (relay_status_flag==0) && (real_relaystatus_flag == 1) && (relay_success_flag==0) )//新增合闸延时过程点亮背光(外置继电器)
	{
		LCDLEDON;
	}
	else
	{
		LCDLEDOFF;
	}
	
	
	if( Init_Led_Timer != 0 ) Init_Led_Timer--;	//led指示灯定时器减1
	
	if( Init_Led_Timer )
	{
		RELAYLEDON;
	}
	else if( relay_commu_disable_close_flag || (BDMODE&&(relay_commu_open_flag==0)&&(relay_status_flag==1)&&(first_eqzero_flag || first_alaram2_flag)) )      //1s闪烁跳闸指示灯
	{
		if( Sclock.Clockbackup[0] & B0000_0001 ) RELAYLEDON;
		else RELAYLEDOFF;	
	}
	else if( (Relay_Mode==0x55) && (relay_status_flag==0) && (real_relaystatus_flag == 1) && (relay_success_flag==0) )
	{
		if( Sclock.Clockbackup[0] & B0000_0001 ) RELAYLEDON;
		else RELAYLEDOFF;	
	}
	else if( real_relaystatus_flag )
	{
		RELAYLEDON;
	}
	else
	{
		RELAYLEDOFF;
	}		
}


unsigned char Judge_Io_Status()	//检测电源异常io口状态
{	
	uint16 i;
	
	for( i=0; i<260/clkmode; i++ )	//2ms延时判断
	{
		if( RELAYTEST_P ) break;
	}
	
//	if( LVDSTATUS & B0000_0010 )
	{	
		if( i == 260/clkmode )	return 0x55;	//低电平持续2ms则认为电源异常(继续欠压)
	}
	
	return 0;
}


void PowerError_Check()		//电源异常检测
{
	uint16 i;
	unsigned char j;
	
	for(j=0;j<10;j++)
	{
		Clear_Wdt();
		
		RAMREG11 = 0x00;
		
		if( RELAYTEST_N )	
		{
			RAMREG11 = Judge_Io_Status();	
		}
			
		if( (RELAYTEST_P) && (RAMREG11 == 0x00) )
		{
			for( i=0; i<3600/clkmode; i++ )	//25ms延时判断
			{
				if( RELAYTEST_N ) break;
			}
				
			if( i != 3600/clkmode )
			{					
				RAMREG11 = Judge_Io_Status();
			}
		}
		
		if( RAMREG11 == 0 ) break;
	}
}


//mode=1写脉冲尾数和事件标志,mode=0脉冲尾数和事件标志清零
//每小时/设置时间/清零/掉电时写数据ee
void Wr_Powerdown_Data( unsigned char mode )	//写掉电相关数据到数据ee
{
	unsigned char  Buf[ee_powerdown_lenth+2];
	
	memset( Buf, 0x00, ee_powerdown_lenth );
	
	memcpy( Buf, Sclock.Clockbackup, 7 );//时钟

	if( mode )
	{
		memcpy( Buf+7, Decimal.Fw, 4 );//保存正向/反向脉冲尾数和脉冲增量
		
		if( below_60u_flag==0 ) Buf[18] |= 0x80;//掉电事件起始记录
		if(relay_error ) Buf[18] |= 0x40;//负荷开关异常结束记录
		if( load_over_flag )
		{
			Buf[18] |= 0x20;//记录电流过载事件(结束)
		}
	}
	
	memcpy( Buf+23, Load.Validpoint[0].B08, ee_loadpoint_lenth+2 );//保存负荷记录指针(带crc)
	
	I2C_Write_Eeprom( ee_power_down_page, 0x00, Buf, ee_powerdown_lenth);//写掉电数据到数据ee
}

//总累计时间处理
//掉电及唤醒时，计算总累计时间并更新到EE，计算后，RAM数据清零，重新累计
//Store=0：只读；Store=1：存储
void Save_Keep_Timer( unsigned char Store )
{
	unsigned char i;
	
	if( Store && (Crc_Check( Keeptime.Times[0].B08, ee_illegalcardnum_len )==0) ) return;
	
	I2C_Read_Eeprom( ee_illegalcardnum_page, ee_illegalcardnum_inpage, Eeprom.Buffer, ee_illegalcardnum_len );
	
	
	for( i=0; i<ee_illegalcardnum_len/4; i++ ) 
//		(unsigned long)(*(Eeprom.Buffer+i*4)) += Keeptime.Times[i].B32;
	*(unsigned long *)(Eeprom.Buffer+i*4) += Keeptime.Times[i].B32;
	
	if( Store)
	{
		I2C_Write_Eeprom( ee_illegalcardnum_page, ee_illegalcardnum_inpage, Eeprom.Buffer, ee_illegalcardnum_len );
		memset( Keeptime.Times[0].B08, 0x00, ee_illegalcardnum_len );
	}
}

//通讯抄读相关累计时间
void Get_Keep_Timer(unsigned char Ch, unsigned char *AckBuf)
{
	Save_Keep_Timer(0);
	
	Hex2Bcd( Eeprom.Buffer+Ch*4, AckBuf, 4 );
	Inverse(AckBuf+1, 3);
	AckBuf[0] = 3;
}

void Store_Data()
{
//	GPIO27CFG = 0;//关闭脉冲(省电)
	LCDLEDOFF;//关闭背光(省电)
	RELAYLEDOFF;//关闭继电器指示灯(省电)
	BEEPALARMOFF;//关闭蜂鸣器(省电)
	if( BDMODE ) //本地模式
	{
		if( Decimal.Status&B1100_0000 ) I2C_Write_Eeprom( ee_remainmoney_page, ee_remainmoney_inpage, Money.Remain, ee_remainmoney_lenth+2 );
	}

	if( Decimal.Status&B0100_0000 ) I2C_Write_Eeprom( ee_fwdl_page, ee_fwdl_inpage, Fwdl.Fwdl_T.B08, ee_fwdl_lenth+2 );
	if( Decimal.Status&B1000_0000 ) I2C_Write_Eeprom( ee_bkdl_page, ee_bkdl_inpage, Bkdl.Bkdl_T.B08, ee_bkdl_lenth+2 );
	
	Read_Sysclock();//读时钟
	Wr_Powerdown_Data(1);//写掉电相关数据到数据ee
	
	real_relaystatus_flag = (relay_status_flag^relay_error);
	System_St_Bk = (System_Status&0x3f);//保存状态字
	if( Cover.Status ) System_St_Bk |= 0x80;//保存开盖状态
	I2C_Write_Eeprom( ee_reportdata_page, 0, &System_St_Bk, ee_reportdata_lenth);//保存事件上报数据防止掉电
	
	Save_Keep_Timer(1);//保存相关累计时间
	
	RAMREG0 = 0x00;//表示未小时唤醒
	RAMREG1 = Bcd_To_Hex(Sclock.Clockbackup[1]);//停电时的分钟数
	RAMREG2 = 1;//开盖1次限制
	RAMREG3 = 0;//清缓升缓降标志字节
	RAMREG4 = 0;//跨过整点标志
	
	RAMREG5 = 0;//跨年结算次数
	RAMREG6 = 0;//跨结算日次数
	RAMREG7 = 0;//年结算跨第一结算日次数
	RAMREG8 = 0;//月结算跨第一结算日次数
	
	RAMREG9 = 0;//冻结状态字
	Pd_Freeze_Judge(0);//停电冻结处理
	
	RAMREG11 = 0;//电源异常状态
	RAMREG12 = Sleepdispdays;//停显7天有功组合总电量
	
	memset( Emu_Data.U.B08, 0x00, 28 );//电压电流功率功率因数清零
	Emu_Data.Cosa.B32 = 0x1000;
	System_Status1 = 0;//清电表状态字1
	
	Cal_Comb( 0x00, 0xFF, 0x01, Ic_Comm_TxBuf );//计算上1结算日组合电量(杭州西力特殊要求)
	Cal_Comb( 0x00, 0xFF, 0x02, Ic_Comm_TxBuf+20 );//计算上2结算日组合电量(杭州西力特殊要求)
}

void Lcd_Disp_Off()//配置lcd黑屏显示
{			 	
//	//未选择LCD模式时，LCD电路关断不彻底，有额外2.4uA功耗
//	//不选中或关断LCD时，将除MD设置为00外，ENMODE也需要设置为00
//	LCDSET 	 = Sleeplcdset;//显示方式
//	LCDTEST   = B0000_0000;
//	DISPFREQ  = 0x3F;//最低刷新频率
//	LCDBIAS  = 0x00;//最新电压偏置
//	ENMODE 	  = 0x0F;//最低显示模式
////	DISPCTRL = B0000_0000;	//关闭lcd显示
// 	memset( (unsigned char xdata *)(0x2093), 0x00, 42 );	//刷新显示
// 	LCDTranslate64();
 }

void Lcd_Disp_On()//配置lcd停电显示
{
//	LCDSET 	 = Sleeplcdset;//显示方式
//	DISPFREQ  = Sleeplcdfreq;//刷新频率
//	LCDBIAS  = Sleeplcdbias;
//	ENMODE 	  = Sleepenmode;
//	DISPCTRL = B0100_0000;		//打开lcd显示
}


void Configure_Register_Sleep()//sleep前配置寄存器
{
//	if(sleeptmr0ie == 0x20)//0x20打开tmr0停电唤醒,0x00关闭tmr0停电唤醒
//	GPIO06CFG = B0100_0000;//RX485, RXD1
//	else
//	GPIO06CFG = B0010_0000;//RX485, RXD1
//	GPIO07CFG = 0;//TX485, TXD1
//	GPIO12CFG = 0;//METERIC_RX RXD3
//	GPIO13CFG = 0;//METERIC_TX TXD3
//	GPIO14CFG = B0100_0000;//RXHW, RXD0
//	GPIO15CFG = 0;//TXHW, TXD0
//	GPIO16CFG = B0100_0010;//SCL
//	GPIO17CFG = B0110_0010;//SDA
////	GPIO17CFG = B0000_0010;//SDA
//	GPIO26CFG = 0;//ACTIVEP
//	GPIO27CFG = 0;//脉冲输出EXOUT1  有不希望出现的脉冲
//	GPIO32CFG = B0100_0000;//PLCA
//	GPIO33CFG = B0100_0000;//PLCRST	
//	GPIO34CFG = B0100_0000;//MADE_CHK	
//	GPIO35CFG = B0100_0000;//PLCEVT
//	GPIO36CFG = B0100_0000;//RX3,	RXD2
//	GPIO37CFG = B0100_0000;//TX3,	TXD2
//	GPIO40CFG = 0;//MCUCLK,U7816的时钟输出
//	GPIO41CFG = B0100_0000;//SAMIO,	ESAM的IO口
//	GPIO42CFG = 0;//BEEP(保护区定义)
//	GPIO45CFG = B0010_0000;//DISPKEY(Cmp1)
//	GPIO46CFG = B0010_0000;//TAIL
//	GPIO55CFG = 0;//MRST
//	GPIO57CFG = 0;//BEEP
//	GPIO60CFG = 0;//RelayCtrl
//	GPIO61CFG = 0;//RelayTest
//	GPIO62CFG = 0;//485Ctrl
//	GPIO63CFG = 0;//LEDRelay	
//	GPIO65CFG = 0;//TM输出
//	GPIO70CFG = 0;//EsamVcc
//	GPIO74CFG = 0;//RFMO
//	GPIO75CFG = 0;//RFMI
//	GPIO76CFG = 0;//RFCLK
//	GPIO77CFG = 0;//RFCS
//	GPIO80CFG = 0;//RelayOff,继电器控制脚 
//	GPIO81CFG = 0;//RelayOn, 继电器控制脚
//	GPIO82CFG = 0;//LCDLED 
//	GPIO83CFG = 0;//SAMRST
//	GPIO84CFG = 0;//RFKEY,带上拉
//	GPIO85CFG = 0;//RFRST  

//	EXIN1DFCTL = 0;
//	EXIN2DFCTL = 0;
//	
//	GPIO0 &= B1011_1111;
//	GPIO1 &= B1110_1111;
//	GPIO2 &= B1111_1111;
//	GPIO3 &= B0000_0011;
//	GPIO4 &= B1111_1001;
//	GPIO8 &= B1111_1111;
	
//	ADCCTRL   = 0; //不使能ADC
//	PLLEN	  = 0; //关闭锁相环

//	RXSTA0 = 0;
//	RXSTA1 = 0;
//	RXSTA2 = 0;
//	TXSTA0 = 0;
//	TXSTA1 = 0;
//	TXSTA2 = 0;
//	TMLSREG = 0;
//	U7816CTRL0 = 0;
//	U7816CTRL1 = 0;	

	Lcd_Disp_On();//配置lcd停电显示
	Stop_Disptimer = 0;//停电显示时间清零
	Display.Number = 0;
	Display.Status = 0;//清按键显示相关标志
	Ic_Type_Reg = 0;//清插卡显示相关标志  
	
	Lcd_Display();

//	if( (PowerDetCnt1 < 4)&&(PowerDetCnt2 < 8) )//允许进行电源异常检测
//	{
//		PowerDetCnt0 = 3;
//		RTCIE = B0000_0101+sleeptmr0ie;	//小时唤醒和秒唤醒,tmr0唤醒(可配)
//	}
//	else
//	{
//		PowerDetCnt0 = 0;
//		RTCIE = B0000_0100+sleeptmr0ie;	//小时唤醒,tmr0唤醒(可配)
//	}

//	GPIO4IE = B0110_0000;		//gpio4.5,4.6中断使能	按键唤醒唤醒
//	GPIO4IF = 0;			//清标志位
//	GPIO0IE = B0100_0000;		//gpio06 = rx485
//	GPIO0IF = 0;
//	
//	UARTIE 	= B0000_0000;		//关闭uart中断
//	IE  	= B0000_0000;
//	AIE  	= B0000_0110;		//使能1/2（被芯片各种外设占用）
}


void Calculate_Min(unsigned char Result)//停电分钟计算处理
{
	unsigned char i;
	if( Result == 0 ) return;//非法返回
	if( Result > 60 ) return;//非法返回
	i = Result;//i范围0~60min
//	if( Progkey.Timer >= i )//编程延时减60
//	{
//		Progkey.Timer -= i;
//	}
//	else	
//	{
//		Progkey.Timer = 0;
//	}

	if(low_voltage_flag) 
	Keeptime.Times[2].B32 += i;//累计电池欠压时间
	Keeptime.Times[3].B32 += i;//累计掉电累计时间
	Keeptime.Times[4].B32 += i;	//累计电池工作时间	

	if( (!BDMODE) && (SELFCLOSE == 0x55) )
	{
		if( Self_Timer24 >= i )//自保电延时
		{
			Self_Timer24 -= i;
		}
		else	
		{
			Self_Timer24 = 0;
		}
	}
}


//Status==0停电调用;Status==1唤醒调用,Status==2初始化调用
void Powerd_Hourwakeup( unsigned char Status )	//小时唤醒事件判断
{
//	unsigned char i;
//	 
//	if( Status < 2 )
//	{
//		if ( RTCIF & B0000_0100 )//1小时唤醒	
//		{			
//			RTCIF &= B1111_1011;	//清小时变化标志
//			
//			if( Status == 1 )
//			{
//				if( RAMREG0 == 0x55 )
//				{
//					Calculate_Min(60);//已小时唤醒
//				}
//				else
//				{
//					Calculate_Min((60-RAMREG1));//第一次小时唤醒
//					RAMREG0 = 0x55;
//				}
//				
//				//停电温度补偿
//				ADCResultArr.ReadCount = 0;
//				
//				if( Lasttesting_Valid != 0x55 )//非成测数据
//				{
//					if( ADCTRIMH  != 0x06 ) ADCTRIMH = 0x06;//LSB=1.5mV
//					if( ADCTRIML  != 0x40 ) ADCTRIML = 0x40;//校准基准源	
//				}
//				else
//				{
//					if( ADCTRIMH  != const_adc_TrimH ) ADCTRIMH = const_adc_TrimH;//LSB=1.5mV
//					if( ADCTRIML  != const_adc_TrimL ) ADCTRIML = const_adc_TrimL;//校准基准源		
//				}
//				ADCCTRL = B0001_1111;//打开温度adc
//				I2c_Wait_5ms();
//				
//				//转换18次取后面16次以保证转换数据准确性
//				for( i=0; i<18; i++ )
//				{
//					I2c_Wait_5ms();
//					ADCResultArr.Buffer[ADCResultArr.ReadCount&0x0f] = ((ADCDATAH&B0000_0111)*0x100+ADCDATAL);
//					ADCResultArr.ReadCount++;
//				}
//				Adcctrl.Status = B0010_0000;//温度adc结束
//				Adc_Proc();
//				ADCCTRL   = 0; //不使能ADC
//			}					
//			
//			Read_Sysclock();//读时钟
//			RAMREG4 = 0x55;	//置跨过整点标志	
//			Histroy_Freeze_Dl(0);//结算电量冻结处理
//			Pd_Freeze_Judge(1);//停电冻结处理
//			
//			if(RAMREG12) RAMREG12--;//停电显示时间减1
//			if(RAMREG12 == 0)
//			{
//				if(Stop_Disptimer == 0) 
//				Lcd_Disp_Off();	//关闭lcd显示
//			}
//		}
//		
//		if ( RTCIF & B0000_1000 )//1天唤醒
//		{
//			Clr_Zero_Data();//清零点清零数据
//		}
//		
//		RTCIF &= B0000_0100;	//清除小时变化以外的标志
//	}
//	else
//	{
//		if ( RTCIF & B0000_0100 )//1小时唤醒	
//		{
//			Freeze_Judge();		//冻结和结算判断
//		}
//	}
}

	
void Reset()	//唤醒复位
{	
	Read_Sysclock();//读时钟
	
	if( RAMREG0 == 0x55 )
	{
		Calculate_Min(Bcd_To_Hex(Sclock.Clockbackup[1]));//已小时唤醒
	}
	else
	{
		Calculate_Min((Bcd_To_Hex(Sclock.Clockbackup[1])-RAMREG1));//未小时唤醒
	}
	
	if( RAMREG4 == 0x55 ) Pd_Histroyfreeze_Judge();	//停电结算日补存判断

	Save_Keep_Timer(1);
	
	Pd_Freeze_Judge(2);//停电日冻结处理
	
	if( RAMREG11 == 0xaa )//补记电源异常记录
	{
		memcpy( Ic_Comm_RxBuf+106, Sclock.Clockbackup, 6 );//保存当前时间
		memcpy( Sclock.Clockbackup, Ic_Comm_RxBuf+100, 6 );//电源异常发生时间			
		Note_Run_Event(ee_powererrn_inpage);		//电源异常开始记录
		memcpy( Sclock.Clockbackup, Ic_Comm_RxBuf+106, 6 );//恢复当前时间
		Note_Run_Event(ee_powererrn_inpage|0x80);	//电源异常结束记录
	}
	
	System_St_Bk = System_Status; //备份系统状态字	
	RAMREG3 = 0xaa;

	(*( void (*)( ) )0xFF0000) ();	//软复位,从头开始运行
}


void Sleep( void )
{
//	unsigned char CoverDetCnt = 0;
//	uint16 i;

	//在进入sleep前再次进行去抖
//	for(i=0; i<0x300; i++)		//约3.75ms
//	{
//		if( (LVDSTATUS & B0000_0010) == 0 ) { reg_veri_flag = 1; EA = 1; return; }	//返回主程序
//	}	

	Store_Data();
	Configure_Register_Sleep();
	Powerd_Hourwakeup(0);		//小时唤醒事件判断
//	
//	for( ; ; )
//	{		
////		Clear_Wdt();
////		Clear_SYS_Wdt();
////		LVDSTATUS = 0;
////		
////		if( (LVDSTATUS & B0000_0010) == 0 )	//sleep前再次去抖
//// 		{ 			
////	 		for(i=0; i<200; i++)
////			{
////				if( (LVDSTATUS & B0000_0010) != 0 ) break;
////			}	
//			if( i==200 ) 
//			{
//				Reset();
//			}
// 		}
//		
////		PDRCFG=0x00;	//下电复位需要打开~~
////		
////		CLKSEL= B0010_0011;	//切换到arclk(v03版芯片)
////		{_nop_();_nop_();}
////		OSCEN = sleeposcen;	//关闭rchf和rclf(v03版芯片)
////		
////		Clear_Wdt();
////				
////		if( sleeptmr0ie == 0x20 )//0x20打开tmr0停电唤醒,0x00关闭tmr0停电唤醒
////		{
////			TMRCTL = B0000_0010;	//停止rtctmr0,秒同步
////			TMR0PREDIV = 0;		//rtctmr0预分频寄存器设置为0表示1分频
////			TMR0PRESET = sleepwakeuptimer;	//设置预置寄存器(减溢出),16-6,32-12,65-25
////			RTCTMR0OVC = 0;		//设置溢出值寄存器
////			RTCTMRLOAD = B0000_0001;//load rtctmr0
////			TMRCTL = B0010_0010;	//启动rtctmr0,秒同步
////			RTCTMRIF = 0;		//清rtctmr0
////		}
//		
////		PCON |= B0000_1100;	//GF0,GF1置位
////		GPIO4IF = 0;
////		GPIO0IF = 0;
////		RTCIF &= B1111_1110;	//仅清除秒中断标志

////		{LVDEFCFG = 0; LVDEFCFG = Sleeplvdefcfg;}//低压检测电路间歇使能
////		
////		if( LVDSTATUS & B0000_0010 )//发生欠压
////		{
////			EA = 1;			//打开全局中断
////			PCON = B0000_0001;	//进入sleep模式
////			EA = 0;			//关掉全局中断
////		}
////		else  PCON &= B1111_0011;	//GF0,GF1清零
////		
////		Clear_Wdt();
////		{LVDEFCFG = 0; LVDEFCFG = 0x80;}//低压检测电路使能
////		
////		OSCEN = runoscen; 		//打开rchf
////		{_nop_();_nop_();}
////		CLKSEL= B0010_0000;		//切换到rchf		
////		Clear_Wdt();
////		Clear_SYS_Wdt();
////		
////		if( (sleeptmr0ie == 0x20) && RTCTMRIF ) GPIO06CFG = B0010_0000;//RX485, RXD1(扩展电平唤醒输入)
////		if( GPIO0 & B0100_0000 ) RTCIE |= B0000_0001; 	//GPIO06高电平唤醒启动秒唤醒检测电压

//		if( GPIO4IF & B0110_0000 )//按键唤醒
//		{
//			if( GPIO4IF&B0100_0000 )//开盖
//			{
////				if( RAMREG2 )//开盖次数限制
//				{
//					CoverDetCnt = 1;
//					Cover_Space_Timer = Bcd_To_Hex(Threshold[3]); //取开表盖事件判定延时时间定值
//					RTCIE |= B0000_0001;  //打开秒中断 
//				}						
//			}
//			if( GPIO4IF&B0010_0000 )//显示按键
//			{
//				for(i=0; i<0xff; i++)
//				{
//					if( GPIO4&B0010_0000 ) break;//按键按下为低电平
//				}
//				if( i==0xff ) //按键去抖(0.3ms)
//				{
//					Displaykey_Process();//显示按键处理
//					if( Stop_Disptimer==0 ) 
//					{
//						Display.Timer = Display_Para.Loop_Time;//第1次循环显示,第2次按键停显30s
//						Display.Status &= B1111_1110;//清按键显示标志
//						Stop_Disptimer = Get_Valid_Num(0)*Display.Timer;//循环显示一周
//						
//						if( Stop_Disptimer==0 ) Stop_Disptimer = Display.Timer;//防止循环显示项目数为0
//					}
//					else
//					{
//						Stop_Disptimer = default_keydisplaytimer/2;//30s
//					}
//					Lcd_Display();			//刷新显示
//					Lcd_Disp_On();
////					RTCIE   |= B0000_0001;		//打开每秒唤醒一次
//				}
//			}
//		}
//		
////		if( RTCIF & B0000_0011 )		//定时唤醒
////		{
////			if ( (RTCIE & RTCIF) & B0000_0001 )//1s唤醒
////			{					
////				if( PowerDetCnt0 >= 3 )
////				{
////					PowerDetCnt0++;
////					if( PowerDetCnt0 >= 5 )
////					{
////						GPIO60CFG = B0100_0000;//RelayCtrl
////						GPIO61CFG = B0010_0000;//RelayTest
////						I2c_Wait_5ms();
////						PowerError_Check();//电源异常检测(合闸才判断)
////						if( RAMREG11 != 0x55 ) PowerDetCnt0 = 0;
////						else
//////						if( PowerDetCnt0 >= 8 )
////						{
////							PowerDetCnt0 = 0;
////							RAMREG11 = 0xaa;//只有记录发生时才在上电后做结束时间
////							memcpy( Ic_Comm_RxBuf+100, Sclock.Clockbackup, 6 );//保存电源异常发生时间(秒分时日月年)
////						}
////						
////						if(PowerDetCnt0 == 0)
////						{
////							GPIO60CFG = 0;
////							GPIO61CFG = 0;
////						}
////					}
////					else
////					{
////						Read_Sysclock();//读时钟
////					}
////				}
////				
//				if( CoverDetCnt )
//				{
//					if( Cover_Space_Timer )
//					{
//						Cover_Space_Timer--;
//						if( Cover_Space_Timer == 0 ) Read_Sysclock();//读时钟
//					}
//                       				 
//					Cover_Check();//开盖检测
//					if( Cover_Space_Timer == Bcd_To_Hex(Threshold[3]) )  //未检测到开盖或已做记录完毕 
//					{
//						CoverDetCnt = 0;
//					}			
////					if( RAMREG2 == 0 ) GPIO4IE &= B1011_1111;
//				}
//				
//				if( Stop_Disptimer ) 
//				{
//					Stop_Disptimer--;
//					if( Stop_Disptimer ) 
//					{
//						if( Display.Timer ) Display.Timer--;
//					
//						if( Display.Timer==0 ) display_rtc_flag = 1;
//						if( display_rtc_flag )
//						{
//							Lcd_Display();	//刷新显示
//						}
//					}	
//				}
//				
//				if( Stop_Disptimer == 0 ) 
//				{
//					Display.Number = 0 ;	//从头开始显示
//					
//					if( RAMREG12==0 ) Lcd_Disp_Off();//配置lcd黑屏显示
//					else 
//					{
//						Display.Status &= B1111_1110;//清按键显示标志
//						Lcd_Display();//停电长显7天电量
//					}
//					
////					if(( PowerDetCnt0 < 3 ) && (CoverDetCnt == 0) &&(!(GPIO0&B0100_0000))) RTCIE &= B1111_1110;//关闭每秒唤醒
//				}
//			}
//		}
//		
//		Powerd_Hourwakeup(1);	//小时唤醒事件判断	   
//	}
}
