/**
* @file    v85xx_isr.c
* @author  weaktea
* @version V1.0
* @date    2016/07/05
* @brief   Main Interrupt Service Routines.
******************************************************************************/
#include "ex_var.h"
#include "v85xx_isr.h"
#include "main.h"
#include "ex_func.h"
//#include <intrins.h>
#include <string.h>
#include <stdio.h>

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
  while(1)
  {}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  while(1)
  {}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}

/**
  * @brief  This function handles PMU interrupt request.
  * @param  None
  * @retval None
  */
void PMU_IRQHandler(void)
{
	
	if((((*PMU_IOAINT)&BIT10 )||((*PMU_IOAINT)&BIT11 ) )&&(FLAG_EA==0x5555AAAA))  
	{		
//		PLUSELED_ON;		
//		TMR1->RELOAD   = 0x0002AAAA ;	
//    TMR1->CTRL   = 0x00000009;
//		NVIC_EnableIRQ(TMR1_IRQn);
//    NVIC_SetPriority(TMR1_IRQn, 1);
//		*PMU_IOAINT =PMU_IOAINT_IOAINT;
//	  
//		if( dl_back_flag == 0 )
//		{
//					Decimal.Fw[0]++;		//正向脉冲尾数加1					
//					if ( Decimal.Fw[0] >= Meter.Const_H )
//					{
//						Decimal.Fw[0] = 0;
//						Decimal.Unit_Pluse++;
//					}
//		}
//		else
//		{
//					Decimal.Bk[0]++;		//反向脉冲尾数加1
//					if ( Decimal.Bk[0] >= Meter.Const_H )
//					{
//						Decimal.Bk[0] = 0;
//						Decimal.Unit_Bkpluse++;
//					}
//		}
//		
//		
//		Demand[0]++;//总需量脉冲数脉冲数(滑差时间内)加1
//		Tar_Demand[0]++;//费率需量脉冲数脉冲数(滑差时间内)加1
//      PowerRead_Flag = 0x5A;
		
	}
	else if ( FLAG_EA==0 )
	{
		 if ((*PMU_IOAINT)&BIT12 ) 
		 {
			 DisKeySamp1 = 0xff;
			 KEY_Status = 1; 
		 }
		 
	   if ((*PMU_IOAINT)&BIT13 )
		 {
			 DisKeySamp = 0xff;
			 KEY_Status = 1; 
		 }
//		 if ((*PMU_IOAINT)&BIT15 ) KEY_Status |= B0010_0000; 
//		Displaykey_Process();//显示按键处理
//		if( Stop_Disptimer==0 ) 
//		{
//		  Display.Timer = Display_Para.Loop_Time;//第1次循环显示,第2次按键停显30s
//			Display.Status &= B1111_1110;//清按键显示标志
//			Stop_Disptimer = Get_Valid_Num(0)*Display.Timer;//循环显示一周
//						
//			if( Stop_Disptimer==0 ) Stop_Disptimer = Display.Timer;//防止循环显示项目数为0
//		}
//		else
//		{
//			Stop_Disptimer = default_keydisplaytimer/2;//30s
//		}
//		Lcd_Display();			//刷新显示
//    Lcd_Disp_On();
	
	}
		
	
  PMU->IOAINT = 0x0000FFFF;
}

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{	
  
	if((RTC->INTSTS&RTC_INTSTS_WKUSEC )&&(PowerLow==0))  //rtc
	{		
		RTC->INTSTS = RTC_INTSTS_WKUSEC;
		
		read_back_flag = 1;//每秒读一次7051标志
		
		if( Display.Timer != 0 ) Display.Timer--;
		
		if( Successled.Timer != 0 ) Successled.Timer--;//通讯符号延时定时器减1
		
		if( Keyclose_timer!= 0 ) Keyclose_timer--;//按键闭合继电器定时器减1
		
		Adcctrl.Timer++;
		if( Adcctrl.Timer == 5 )//启动电池adc 
  	{
      ANA->REG1	= 0x10 ;
			ANA->INTSTS= 0x00000003;
			ANA->ADCCTRL = 0x00000002;
			
		  ANA->REG3 |= BIT0;
			ANA->ADCCTRL |=  BIT31;
			Adcctrl.Status= B0000_0001;
			
		}
		if( Adcctrl.Timer == 10 )//启动温度adc 
		{
			Adcctrl.Timer = 0;
			ANA->REG1	= 0x00 ;
			ANA->INTSTS= 0x00000003;
			ANA->ADCCTRL = 0x0000000a;
			ANA->ADCCTRL |= BIT31;
		  ANA->REG3 |= BIT0;			
			Adcctrl.Status= B0000_0010;
		}	
		
//		if( Display_Space_Timer != 0 ) //红外遥控唤醒延时
//		{
//			Display_Space_Timer--;	
//			if( Display_Space_Timer==0 ) Lcdled.B16 = Hw_Lcdled_Timer;//启动背光定时器
//		}
		
		if( Relay_Space_Timer != 0 ) //上电延时动作继电器
		{
			Relay_Space_Timer--;
			if( Relay_Space_Timer==0 ){ relay_direct_flag = 1; run_alarm_flag = 1; }
		}
		if( PLC_RST1_Timer != 0 )
		{			
			PLC_RST1_Timer--;			
		}
		
//		
//		PowerValue.ActPowerP += 1;
		
		PowerRead_Flag = 0x5A;
		
		
	}
	if ( PowerLow == 1 )
	{
		if (RTC->INTSTS&RTC_INTSTS_WKUSEC )    RTC_Status |= RTC_INTSTS_WKUSEC;
		if (RTC->INTSTS&RTC_INTSTS_WKUHOUR )   RTC_Status |= RTC_INTSTS_WKUHOUR;
		if (RTC->INTSTS&RTC_INTSTS_MIDNIGHT )  RTC_Status |= RTC_INTSTS_MIDNIGHT;
	}	
	RTC->INTSTS = 0x0000ffff;
}

/**
  * @brief  This function handles U32K0 interrupt request.
  * @param  None
  * @retval None
  */
void U32K0_IRQHandler(void)
{
}

/**
  * @brief  This function handles U32K1 interrupt request.
  * @param  None
  * @retval None
  */
void U32K1_IRQHandler(void)
{
}

/**
  * @brief  This function handles I2C interrupt request.
  * @param  None
  * @retval None
  */
void I2C_IRQHandler(void)
{
}

/**
  * @brief  This function handles SPI1 interrupt request.
  * @param  None
  * @retval None
  */
void SPI1_IRQHandler(void)
{
}

/**
  * @brief  This function handles UART0 interrupt request.
  * @param  None
  * @retval None
  */
void UART0_IRQHandler(void)
{
     
}

/**
  * @brief  This function handles UART1 interrupt request.
  * @param  None
  * @retval None
  */
void UART1_IRQHandler(void)
{
}

/**
  * @brief  This function handles UART2 interrupt request.
  * @param  None
  * @retval None
  */
void UART2_IRQHandler(void)
{
	   unsigned char  Temp;
	
		 if((UART2->INTSTS)&UART_INTSTS_RXIF) 
	   {
			   UART2->INTSTS = UART_INTSTS_RXIF; //任何对寄存器的操作都会导致其清零, 必须先缓存
	
		
			   Temp = UART2->DATA; //从UART2数据寄存器中取接收数据

			   Judge_645_Frame( 0, Temp );
			   if ( Uart[0].Step == 0x08 )
		   	{
				 //UARTIE &= B0000_1111;	//关闭RXIE2
				  //UART2->CTRL &= ~(BIT3|BIT1 );	//clr UART2.rxif
				 display_zaibo_flag = 1;
			  }
		 }	
		if( (UART2->INTSTS)&UART_INTSTS_TXDONE)
	  {
		    UART2->INTSTS = UART_INTSTS_TXDONE;
			  Tx_645_Frame( 0 );
//			UARTIF &= B1110_1111;	//clr UART2.txif
		}
		UART2->INTSTS = 0xffffffff;
}

/**
  * @brief  This function handles UART3 interrupt request.
  * @param  None
  * @retval None
  */
void UART3_IRQHandler(void)
{
}

/**
  * @brief  This function handles UART4 interrupt request.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
	unsigned char  Temp;
	
		 if((UART4->INTSTS)&UART_INTSTS_RXIF) 
	   {
			   UART4->INTSTS = UART_INTSTS_RXIF; //任何对寄存器的操作都会导致其清零, 必须先缓存
	
		
			   Temp = UART4->DATA; //从UART2数据寄存器中取接收数据

			   Judge_645_Frame( 1, Temp );
			   if ( Uart[1].Step == 0x08 )
		   	{
				 display_zaibo_flag = 0;
					
			  }
		 }	
		if( (UART4->INTSTS)&UART_INTSTS_TXDONE)
	  {
		    UART4->INTSTS = UART_INTSTS_TXDONE;
			  Tx_645_Frame( 1 );

		}	
		UART4->INTSTS = 0xffffffff;
}

/**
  * @brief  This function handles UART5 interrupt request.
  * @param  None
  * @retval None
  */
void UART5_IRQHandler(void)
{
     unsigned char  Temp;
	
		 if((UART5->INTSTS)&UART_INTSTS_RXIF) 
	   {
			   UART5->INTSTS = UART_INTSTS_RXIF; //任何对寄存器的操作都会导致其清零, 必须先缓存
	
		
			   Temp = UART5->DATA; //从UART2数据寄存器中取接收数据

			   Judge_645_Frame( 2, Temp );
			   if ( Uart[2].Step == 0x08 )
		   	{
				 display_zaibo_flag = 0;//UARTIE &= B0000_1111;	//关闭RXIE2
				  //UART2->CTRL &= ~(BIT3|BIT1 );	//clr UART2.rxif
				 
			  }
		 }	
		if( (UART5->INTSTS)&UART_INTSTS_TXDONE)
	  {
		    UART5->INTSTS = UART_INTSTS_TXDONE;
			  Tx_645_Frame( 2 );
//			UARTIF &= B1110_1111;	//clr UART2.txif
		}
		UART5->INTSTS = 0xffffffff;
}

/**
  * @brief  This function handles ISO78160 interrupt request.
  * @param  None
  * @retval None
  */
void ISO78160_IRQHandler(void)
{
}

/**
  * @brief  This function handles ISO78161 interrupt request.
  * @param  None
  * @retval None
  */
void ISO78161_IRQHandler(void)
{
}

/**
  * @brief  This function handles TMR0 interrupt request.
  * @param  None
  * @retval None
  */
void TMR0_IRQHandler(void)
{
	  unsigned char Ch;
	  uint16 i;


     if(!(TMR0->INT&TMR_INT_INT_Msk)) return;
	   TMR0->INT = TMR_INT_INT_Msk; //任何对寄存器的操作都会导致其清零, 必须先缓存
		
	
	if( VectorTimer == 0 ) VectorTimer  = ms_timer_delay;
	if( VectorTimer == 1 )
		run_readrtc_flag = 1;	//?????
	if( (VectorTimer == 50) || (((VectorTimer == 20)||(VectorTimer == 70))&&display_rtc_flag) ) run_display_flag = 1;	//??????
	if( VectorTimer == 100 )
	{
		run_relaychk_flag = 1;	//???????
//		RELAYCTRLON;		//??????
//		Clear_SYS_Wdt();	//??????
	}
	VectorTimer--;
//	
	DisKeySamp<<=1;
	if(DISKEY1_N)  
	{		
		DisKeySamp++;	//????????		
	}
 
  DisKeySamp1<<=1;
	if(DISKEY_N) 
  {		 
		DisKeySamp1++;	//????????		
	}
//	CardKeySamp<<=1;
//	if(KEY_N)  CardKeySamp++;	//??????
	
	//ADC??
//	if((Adcctrl.Status & B0000_0011)&&(ADCDATAH & B1000_0000))
//	{
//		ADCResultArr.Buffer[ADCResultArr.ReadCount&0x0f] = ((ADCDATAH&B0000_0111)*0x100+ADCDATAL);
//		ADCResultArr.ReadCount++;

//		//??32????16???????????
//		if(ADCResultArr.ReadCount >= 32)
//		{
//			if ( Adcctrl.Status == B0000_0001 ) 
//			{
//				Adcctrl.Status = B0001_0000;//??adc??
//			}
//			else
//			{
//				Adcctrl.Status = B0010_0000;//??adc??
//			}
//		}
//	}
	
  if( Timer.PowerRead_Tmr < 60 )//
     Timer.PowerRead_Tmr++;
  
	if( Timer.PowerRead_Tmr >= 100 )
  {
     Timer.PowerRead_Tmr = 0;
      PowerRead_Flag = 0x5A;
  }

	if ( Shiduanctrl.Timer != 0 ) //????????
	{
		Shiduanctrl.Timer--;
	}
	if( Shiduanctrl.Timer == 0 ) SHIDUANCTRL_OFF;
		
	if( Relay_Mode != 0x55 )//?????
	{
		if ( Relayctrl.Timer != 0 ) 
		{
			Relayctrl.Timer--;
		}
		if( Relayctrl.Timer == 0 ) {RELAYOFF1; __NOP(); RELAYOFF2;}//????????
    
		if ( Powertimer.Timer != 0 ) 
		{
			Powertimer.Timer--;
		}
	}
	else
	{
		if(Mode.Feature1&0x01)//????
		{
			if ( Relayctrl.Timer != 0 ) 
			{
				Relayctrl.Timer--;
			}
			if( relay_status_flag && (Relayctrl.Timer == 0) ) {RELAYOFF3;}//
		}
	}
	
	if ( Resetworking_Timer != 0 ) 
	{
		Resetworking_Timer--;
	}
	if( Resetworking_Timer == 0 )
	{
	//	PLC_RST1;
	}
	
	if ( Zb_Relayctrl_Timer != 0 ) 
	{
		Zb_Relayctrl_Timer--;
	}	
	
	Beep_Ctrl();//?????
		
	for ( Ch=0; Ch<3; Ch++ )
	{
		if(Uart[Ch].Overtime_Timer != 0)
		{
			Uart[Ch].Overtime_Timer--;
			if( Uart[Ch].Overtime_Timer == 0 ) 
			{
					if( Ch==0 ) 
					{	
						if( UART2->CTRL  != 0x0000000a ) UART2->CTRL   = 0x0000000a;
//						UARTIE |= B0000_0010;	//??RXIE0
////						RXSTA0 &= B1111_0001;
//						{RXSTA0 = 0;RXSTA0 = B0101_0000;}//????FIFO    载波
					}
					if( Ch==1 ) 
					{	
						if( UART4->CTRL  != 0x0000000a ) UART4->CTRL   = 0x0000000a;
//						UARTIE |= B0000_1000;	//??RXIE1
////						RXSTA1 &= B1111_0001;
//						{RXSTA1 = 0;RXSTA1 = B0101_0000;}//????FIFO     红外
//						R485CTRLR;		//????						
					}
					if( Ch==2 ) 
					{	
						if( UART5->CTRL  != 0x0000000a ) UART5->CTRL   = 0x0000000a;
//						UARTIE &= B1110_1111;	//??TXIE2
//						UARTIE |= B0010_0000;	//??RXIE2
////						RXSTA2 &= B1111_0001;
//						{RXSTA2 = 0;RXSTA2 = B0101_0000;}//????FIFO  RS485
					}
					Uart[Ch].Step = 0x00;
					Uart[Ch].Flag = 0x00;
					Uart[Ch].Response_Timer = 0x00;
			}
		}
				
	}
	for ( Ch=0; Ch<3; Ch++ )
	{
		if(Uart[Ch].Response_Timer != 0)
		{
			Uart[Ch].Response_Timer--;
			if( Uart[Ch].Response_Timer == 0 ) 
			{
				if ( Uart[Ch].Flag & B0000_1000 )
				{	
					
					if( Ch==0 ) 			//????????
					{	
						UART2->CTRL &= ~(BIT3|BIT1 );	//clr UART2.rxif
				    UART2->CTRL |= (BIT0|BIT8 );	
						UART2->DATA = 0x68;	
						
					}
					if( Ch==1 ) 
					{	
//						R485CTRLS;		//????
//						TXREG1  = 0x68;		//??0x68
//						UARTIE &= B1111_0111;	//??RXIE1
//						UARTIE |= B0000_0100;	//??TXIE1
				    UART4->CTRL &= ~(BIT3|BIT1 );	//clr UART2.rxif
				    UART4->CTRL |= (BIT0|BIT8 );	
						UART4->DATA = 0x68;	
					}
					if( Ch==2 ) 
					{	
//						TXREG2  = 0x68;		//??0x68
//						UARTIE &= B1101_1111;	//??RXIE2
//						UARTIE |= B0001_0000;	//??TXIE2
						
						
						UART5->CTRL &= ~(BIT3|BIT1 );	//clr UART2.rxif
				    UART5->CTRL |= (BIT0|BIT8 );	
						UART5->DATA = 0x68;	
						
						//if( UART2->CTRL  != 0x00000100 ) UART2->CTRL   = 0x00000100;
						
					}
					Uart[Ch].Len  = 5;//??????
					Uart[Ch].Total_Len = Uart[Ch].Buf[9]+16;
					Uart[Ch].Overtime_Timer = txtimeout_max;
				}
				else
				{
					Uart[Ch].Response_Timer++;//????
				}
			}
		}			
	}
	
//	if( Compile_Polar_485 == 0x55 )	//?????485
//	{
//		if( (Uart[1].Response_Timer == 0)&&(Uart[1].Overtime_Timer == 0)&&((UARTIF&B0000_1000)==0x00) )
//		{
//			Uart1KeySamp<<=1;
//			if(GPIO0&B0100_0000)  Uart1KeySamp++;		//?????????,?????485
//			
//			if( Uart1KeySamp == 0xff ) //??? ???
//			{
//				if( RTXCON1 != B0000_0000 )	RTXCON1 = B0000_0000;//??? ???????,???????
//			}
//			else if( Uart1KeySamp == 0x00 )
//			{
//				if( RTXCON1 != B0000_0011 )	RTXCON1 = B0000_0011;//??? ??????,??????		
//			}	 
//		} 
//		else Uart1KeySamp = 0x55;
//	}
//	else
//	{
//		if( RTXCON1 != B0000_0000 )	RTXCON1 = B0000_0000;//??? ???????,???????
//	}
	
	if( (*ANA_CMPOUT & BIT8) && (Powertimer.Timer == 0) ) 
	{	
		for(i=0; i<0x300; i++)		//?3.75ms
		{
			if( (*ANA_CMPOUT & BIT8) == 0 ) break;
		}	
		if( i==0x300 ) //??
		{
			LCDLEDOFF;//
			RELAYLEDOFF;//
			BEEPALARMOFF;//
			
			for(i=0; i<0x300; i++)		//?3.75ms
			{
				if( (*ANA_CMPOUT & BIT8)== 0 ) 
				{
				    FLAG_EA = 0x5555AAAA;			
		        break;	
				}	
			}	
			if( i==0x300 ) 
			{
				FLAG_EA = 0;
        NVIC_DisableIRQ(RTC_IRQn);  		
			}
		}
	}
}

/**
  * @brief  This function handles TMR1 interrupt request.
  * @param  None
  * @retval None
  */
void TMR1_IRQHandler(void)
{
	if(!(TMR1->INT&TMR_INT_INT_Msk)) return;
	TMR1->INT = TMR_INT_INT_Msk; //任何对寄存器的操作都会导致其清零, 必须先缓存
	PLUSELED_OFF;
	NVIC_DisableIRQ(TMR1_IRQn);
	TMR1->CTRL   = 0;
}

/**
  * @brief  This function handles TMR2 interrupt request.
  * @param  None
  * @retval None
  */
void TMR2_IRQHandler(void)
{
}

/**
  * @brief  This function handles TMR3 interrupt request.
  * @param  None
  * @retval None
  */
void TMR3_IRQHandler(void)
{
}

/**
  * @brief  This function handles PWM0 interrupt request.
  * @param  None
  * @retval None
  */
void PWM0_IRQHandler(void)
{
}

/**
  * @brief  This function handles PWM1 interrupt request.
  * @param  None
  * @retval None
  */
void PWM1_IRQHandler(void)
{
}

/**
  * @brief  This function handles PWM2 interrupt request.
  * @param  None
  * @retval None
  */
void PWM2_IRQHandler(void)
{
}

/**
  * @brief  This function handles PWM3 interrupt request.
  * @param  None
  * @retval None
  */
void PWM3_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void DMA_IRQHandler(void)
{
}

/**
  * @brief  This function handles FLASH interrupt request.
  * @param  None
  * @retval None
  */
void FLASH_IRQHandler(void)
{
}

/**
  * @brief  This function handles ANA interrupt request.
  * @param  None
  * @retval None
  */
void ANA_IRQHandler(void)
{
}

/**
  * @brief  This function handles SPI2 interrupt request.
  * @param  None
  * @retval None
  */
void SPI2_IRQHandler(void)
{
}

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/



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
					if( CH == 1 )
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
		if( Uart[CH].Len < 4 ) 
		{
			if( CH==0 ) 	UART2->DATA = 0xfe; //载波
		  else if( CH==1 ) 	UART4->DATA = 0xfe;//红外
		  else  	UART5->DATA = 0xfe;//RS485
		}
		else 
		{	
			if( CH==0 ) 	UART2->DATA = Uart[CH].Buf[Uart[CH].Len-4]; //载波
		  else if( CH==1 ) UART4->DATA = Uart[CH].Buf[Uart[CH].Len-4];//红外
		  else  	UART5->DATA = Uart[CH].Buf[Uart[CH].Len-4];//RS485
		}
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
		if( Sclock.Clockbackup[0] & B0000_0001 ) {RELAYLEDON;}
		else {RELAYLEDOFF;}	
	}
	else if( (Relay_Mode==0x55) && (relay_status_flag==0) && (real_relaystatus_flag == 1) && (relay_success_flag==0) )
	{
		if( Sclock.Clockbackup[0] & B0000_0001 ){ RELAYLEDON;}
		else {RELAYLEDOFF;}	
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
//	BEEPALARMOFF;//关闭蜂鸣器(省电)
	if( BDMODE ) //本地模式
	{
		if( Decimal.Status&B1100_0000 ) 
		{
			I2C_Write_Eeprom( ee_remainmoney_page, ee_remainmoney_inpage, Money.Remain, ee_remainmoney_lenth+2 );
			I2C_Write_Eeprom( ee_DayUserINFO_page, ee_UserINFO_inpage, Money_UserINFO.Day_Moneyused, 16 );
		}
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
	
	memset( Emu_Data.U.B08, 0x00, 32 );//电压电流功率功率因数清零
	
	
	Emu_Data.Cosa.B32 = 0x1000;
	
	
	Real.U[0] =0;Real.U[1] =0;
	Real.U[2] =0;
	Real.I[0] =0;Real.I[1] =0;
	Real.I[2] =0;
	
	memset( &Real.U[0], 0x00, 74 );
	
	Real.PF[0] =0x1000;Real.PF[1] =0x1000;Real.PF[2] =0x1000;
	Real.PF[3] =0x1000;
	System_Status1 = 0;//清电表状态字1
	
	Cal_Comb( 0x00, 0xFF, 0x01, Ic_Comm_TxBuf );//计算上1结算日组合电量(杭州西力特殊要求)
	Cal_Comb( 0x00, 0xFF, 0x02, Ic_Comm_TxBuf+20 );//计算上2结算日组合电量(杭州西力特殊要求)
}

void Lcd_Disp_Off(void)//配置lcd黑屏显示
{			 	
    LCD->CTRL = 0;
	  LCD->SEGCTRL0 = 0x00000000;	
	
	  LCD->SEGCTRL1 = 0x00000000;	
	  LCD->SEGCTRL2 = 0x00000000;	
	  
}

void Lcd_Disp_On(void)//配置lcd停电显示
{
  if( LCD->CTRL !=(BIT4|BIT7|BIT1 )) LCD->CTRL = BIT4|BIT7|BIT1;		//4com or 8com – 0x10; 6com – 0x15
  if( LCD->CTRL2 !=(BIT0)) LCD->CTRL2 = BIT0;		//A波形 4com = 0x00; 6com = 0x01; 8com =0x02
	if( LCD->SEGCTRL0 !=0xfff00ffc) LCD->SEGCTRL0 = 0xfff00ffc;	//seg0~seg31
		if( LCD->SEGCTRL1 !=0xb8160407) LCD->SEGCTRL1 = 0xb8160407;	//seg32~seg63
		if( LCD->SEGCTRL2 !=0x00001c0c) LCD->SEGCTRL2 = 0x00001c0c; //seg64~seg79
	if(*ANA_REG6  != B0000_0011 )	*ANA_REG6  = B0000_0011;	//禁止测试
  if( LCD->CTRL !=(BIT4|BIT7|BIT1 )) LCD->CTRL = BIT4|BIT7|BIT1;	
}


void Configure_Register_Sleep(void)//sleep前配置寄存器
{
  
	GPIOA->DAT = 0x000030C0;//确保按键下降沿触发唤醒
	GPIOB->DAT = 0x00006000;
	GPIOC->DAT = 0x00000000;
	GPIOD->DAT = 0x00000000;
	GPIOE->DAT = 0x00000000;
	GPIOF->DAT = 0x00000000;
	
	GPIOA->OEN = 0x0000FFFF;
	GPIOA->IE  = 0x000030C0;	
	GPIOA->WKUEN = 0x05000000 ;//PE2强驱动使能	
	GPIOA->SEL= 0x00000000;	
	GPIOA->IOANODEG	=0x00000000 ;	//模拟口液晶、AD功能选	
	*PMU_CONTROL =0x00000001;
	*PMU_IOAINT = 0x0000FFFF;
	NVIC_ClearPendingIRQ(PMU_IRQn );
 	NVIC_EnableIRQ(PMU_IRQn );
	
	GPIOB->OEN=0x00009FFF;
	GPIOB->IE= 0x00000000;
	
	GPIOC->OEN	=0x0000FFFF ;
	GPIOC->IE	= 0x00000000 ;
	
	
	GPIOD->OEN  = 0x0000FFFF;
  GPIOD->IE   = 0x00000000;
	
	GPIOE->OEN	= 0x0000FFFF;
	GPIOE->IE   = 0x00000020;
	
	GPIOF->OEN	= 0x0000FFFF;
	GPIOF->IE   = 0x00000000;
  
	TMR0->CTRL  = 0x00000000;
	NVIC_DisableIRQ(TMR0_IRQn);
	TMR1->CTRL   = 0x00000000;
	NVIC_DisableIRQ(TMR1_IRQn);
	
	ANA->ADCCTRL = 0x00000000;
	ANA->REG3 = 0x00000020;//主时钟不能关闭，BGP在休眠前关闭		
  ANA->REG1	= 0x00 ;
	
	UART0->CTRL   = 0x00000000;
  NVIC_DisableIRQ(UART0_IRQn);	
	
	UART2->CTRL   = 0x00000000;
  NVIC_DisableIRQ(UART2_IRQn);	
	
	UART0->CTRL   = 0x00000000;
  NVIC_DisableIRQ(UART4_IRQn);  
 
	UART5->CTRL   = 0x00000000;
  NVIC_DisableIRQ(UART5_IRQn);	
	*MISC_IREN = 0x00000000;
	
	

  ANA->INTEN |= BIT8|BIT11; 
  ANA->REGA  |= (BIT1|BIT3);//切换到BAT1供电、功耗降低3
  *ANA_REG9  |= BIT7;	//禁止测试
	NVIC_DisableIRQ(ANA_IRQn);
  ANA->INTSTS = 0xFFFFFFFF;
	

	NVIC_DisableIRQ(RTC_IRQn); 
	
	RTC_AutoCalCmd(DISABLE); 
  RTC_WriteProtection(DISABLE);
	if( RTC->ACCTRL	!= BIT2 ) RTC->ACCTRL 	= BIT2;	//使能RTC输出 
  RTC->CTL 	= 0;	//使能RTC输出 			
	RTC->ACCTRL = 0;
	if( RTC->INTEN	!= (BIT2|BIT4|BIT5) ) RTC->INTEN = (BIT2|BIT4|BIT5);
  RTC_WriteProtection(ENABLE);
  NVIC_DisableIRQ(RTC_IRQn); 
	
	DisKeySamp =0;
	DisKeySamp1 =0;
	Stop_Disptimer = 0;//停电显示时间清零
	Display.Number = 1;
	Display.Status = 0;//清按键显示相关标志
  memset( DispBuf, 0x00, 160 );	//清显示缓冲区
	memcpy( (unsigned char xdata *)(0x40002000), DispBuf, 160 );	
	Lcd_Disp_Off();//配置lcd停电显示
	Ic_Type_Reg = 0;//清插卡显示相关标志  
  CoverDetCnt = 0;
	
	if( (PowerDetCnt1 < 4)&&(PowerDetCnt2 < 8) )//允许进行电源异常检测
	{
		PowerDetCnt0 = 3;
	}
	else
	{
		PowerDetCnt0 = 0;
	}

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
	 
	if( Status < 2 )
	{
		if(RTC_Status&RTC_INTSTS_WKUHOUR ) //rtc
	  {		
		  RTC_Status &= ~RTC_INTSTS_WKUHOUR;
			if( Status == 1 )
			{
				if( RAMREG0 == 0x55 )
				{
					Calculate_Min(60);//已小时唤醒
				}
				else
				{
					Calculate_Min((60-RAMREG1));//第一次小时唤醒
					RAMREG0 = 0x55;
				}
				
//				//停电温度补偿
//				ADCResultArr.ReadCount = 0;
				
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
				
				//转换18次取后面16次以保证转换数据准确性
//				for( i=0; i<18; i++ )
//				{
//					I2c_Wait_5ms();
//					ADCResultArr.Buffer[ADCResultArr.ReadCount&0x0f] = ((ADCDATAH&B0000_0111)*0x100+ADCDATAL);
//					ADCResultArr.ReadCount++;
//				}
//				Adcctrl.Status = B0010_0000;//温度adc结束
//				Adc_Proc();
//				ADCCTRL   = 0; //不使能ADC
			}					
			
			Read_Sysclock();//读时钟
			RAMREG4 = 0x55;	//置跨过整点标志	
			Histroy_Freeze_Dl(0);//结算电量冻结处理
			Pd_Freeze_Judge(1);//停电冻结处理
			
			if(RAMREG12) RAMREG12--;//停电显示时间减1
			if(RAMREG12 == 0)
			{
				if(Stop_Disptimer == 0) 
				Lcd_Disp_Off();	//关闭lcd显示
			}
		}
		
		if ( RTC->INTSTS&RTC_INTSTS_MIDNIGHT )//1天唤醒
		{
			Clr_Zero_Data();//清零点清零数据
		  RTC_Status &= ~RTC_INTSTS_MIDNIGHT;
		}
//		 RTC_Status &= ~RTC_INTSTS_MIDNIGHT;
//		 (RTC->INTSTS) |= RTC_INTSTS_MIDNIGHT;	//清除小时变化以外的标志
	}
	else
	{
		if(RTC->INTSTS&RTC_INTSTS_WKUHOUR ) //rtc
	  {		
		  RTC_Status &= ~RTC_INTSTS_WKUHOUR;	
			Freeze_Judge();		//冻结和结算判断
		}
	}
}

	
void Reset()	//唤醒复位
{	
	
//	Clock_Init();
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
// 
	(*( void (*)( ) )0x00000321) ();	//软复位,从头开始运行
}


void Sleep( void )
{
	uint16 i;
//  PMU_LowPWRTypeDef LowPower_InitStruct;
	//在进入sleep前再次进行去抖
	for(i=0; i<0x300; i++)		//约3.75ms
	{
		if( (*ANA_CMPOUT & BIT8) == 0 ) { reg_veri_flag = 1; FLAG_EA = 0x5555AAAA; NVIC_EnableIRQ(RTC_IRQn); return; }	//返回主程序
	}
	IICPowerON = 0;	
	
	Store_Data();
	Configure_Register_Sleep();
	Powerd_Hourwakeup(0);		//小时唤醒事件判断 
	NVIC_EnableIRQ(RTC_IRQn); 
	for( ; ; )
	{		
		
		Clear_Wdt();	
		
		if( (*ANA_CMPOUT & BIT8)  == 0  )	//sleep前再次去抖
 		{ 			
	 		for(i=0; i<300; i++)
			{
				if( (*ANA_CMPOUT & BIT8)  != 0) break;
			}	
			if( i==300 ) 
			{				
				NVIC_DisableIRQ(RTC_IRQn); 
				Reset();
			}
 		}
		PowerLow = 1;
		Clear_Wdt(); 
		WDT_Disable();	
		while ((*PMU_IOASTS&0X00003000)!=0X00003000)
		{
		  Clear_Wdt();
		}
		ANA->REG3 |= 0x00000008;
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; 
		 __WFI();
		Clear_Wdt();
		
	  if ( KEY_Status )
	  {
		  KEY_Status = 0;
//			
		  Displaykey_Process();//显示按键处理
		  if( Stop_Disptimer==0 ) 
		  {
		    Display.Timer = Display_Para.Loop_Time;//第1次循环显示,第2次按键停显30s
			  Display.Status &= B1111_1110;//清按键显示标志
			  Stop_Disptimer = Get_Valid_Num(0)*Display.Timer;//循环显示一周
						
			  if( Stop_Disptimer==0 ) Stop_Disptimer = Display.Timer;//防止循环显示项目数为0
				
		  }
		  else
		  {
			  Stop_Disptimer = default_keydisplaytimer/2;//30s
		  }
		  Lcd_Display();			//刷新显示
      Lcd_Disp_On();
      DisKeySamp =0;
			DisKeySamp1 =0;
	  }	
		
		
		if ((RTC_Status&RTC_INTSTS_WKUSEC ))
	  {
			
			
			
			RTC_Status &= ~RTC_INTSTS_WKUSEC;
			if( PowerDetCnt0 >= 3 )
		{
			PowerDetCnt0++;
			if( PowerDetCnt0 >= 5 )
			{									
				PowerError_Check();//电源异常检测(合闸才判断)
				if( RAMREG11 != 0x55 ) PowerDetCnt0 = 0;
				else
//						if( PowerDetCnt0 >= 8 )
				{
					PowerDetCnt0 = 0;
					RAMREG11 = 0xaa;//只有记录发生时才在上电后做结束时间
					memcpy( Ic_Comm_RxBuf+100, Sclock.Clockbackup, 6 );//保存电源异常发生时间(秒分时日月年)
				}
						
			}
			else
			{
				Read_Sysclock();//读时钟
			}
		}
    if( RAMREG2 )
		{
			if( Cover_Space_Timer )
			{
				Cover_Space_Timer--;
				if( Cover_Space_Timer == 0 ) Read_Sysclock();//读时钟
			}
			Cover_Check();
		}		
//		if( CoverDetCnt )
//		{
//			
//                       				 
//			Cover_Check();//开盖检测
//			if( Cover_Space_Timer == Bcd_To_Hex(Threshold[3]) )  //未检测到开盖或已做记录完毕 
//			{
//				CoverDetCnt = 0;
//			}			
////					if( RAMREG2 == 0 ) GPIO4IE &= B1011_1111;
//		}
				
		if( Stop_Disptimer ) 
		{
			Stop_Disptimer--;
			if( Stop_Disptimer ) 
			{
				if( Display.Timer ) Display.Timer--;
					
				if( Display.Timer==0 ) display_rtc_flag = 1;
				if( display_rtc_flag )
				{
							
					Lcd_Display();	//刷新显示
        	Lcd_Disp_On();
							
				}
			}	
			if( Stop_Disptimer == 0 ) 
		  {
		       Display.Number = 1 ;	//从头开始显示
					
		      Lcd_Disp_Off();//配置lcd黑屏显示			
					
//					if(( PowerDetCnt0 < 3 ) && (CoverDetCnt == 0) &&(!(GPIO0&B0100_0000))) RTCIE &= B1111_1110;//关闭每秒唤醒
		
	  }		
		}
				
		
		Powerd_Hourwakeup(1);	//小时唤醒事件判断	   
	}	
//	KEY_Status = 0;
//	RTC_Status = 0;
//	
 
		
	}
}
	
	
	

