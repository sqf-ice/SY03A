#include "ex_func.h"
#include "ex_var.h"
//#include <intrins.h>
#include <string.h>
#include <absacc.h>

void LcdIO(unsigned char RegData)//ÏÔÊ¾IO¿ÚÅäÖÃ
{
//	if( XBYTE[RegData+0x2300] != B1000_0000 ) XBYTE[RegData+0x2300] = B1000_0000;
}
void InputIO(unsigned char RegData)//ÊäÈëIO¿ÚÅäÖÃ
{
//	if( XBYTE[RegData+0x2300] != B0010_0000 ) XBYTE[RegData+0x2300] = B0010_0000;
}
void OutputIO(unsigned char RegData)//Êä³öIO¿ÚÅäÖÃ
{
//	if( XBYTE[RegData+0x2300] != B0100_0000 ) XBYTE[RegData+0x2300] = B0100_0000;
}
void AnInIO(unsigned char RegData)//ÊäÈëIO¿ÚÅäÖÃ(Êý×ÖÌØÊâ¹¦ÄÜ¿Ú)
{
//	if( XBYTE[RegData+0x2300] != B0010_0010 ) XBYTE[RegData+0x2300] = B0010_0010;
}
void AnOutIO(unsigned char RegData)//Êä³öIO¿ÚÅäÖÃ(Êý×ÖÌØÊâ¹¦ÄÜ¿Ú)
{
//	if( XBYTE[RegData+0x2300] != B0100_0010 ) XBYTE[RegData+0x2300] = B0100_0010;
}
void OtherIO(unsigned char RegData, unsigned char ConstData)//ÆäËüIO¿ÚÅäÖÃ
{
//	if( XBYTE[RegData+0x2300] != ConstData ) XBYTE[RegData+0x2300] = ConstData;
}//Ð£Ñé¼Ä´æÆ÷


unsigned char CheckSysReg(  __IO uint32_t *RegAddr, uint32 Value )
{
	if( *RegAddr != Value ) 
	{
		*RegAddr = Value;
		return 1;
	}
	else
	{
		return 0;
	}
}


void Veri_Pad( void )
{
	CheckSysReg( &GPIOA->OEN	, 0x0000FFF4 );
	CheckSysReg( &GPIOA->IE	  , 0x00007CC0 );
	CheckSysReg( &GPIOA->ATT	, 0x00000000 );
	CheckSysReg( &GPIOA->WKUEN, 0x00500000 );//0x01400000 );//PE2Ç¿Çý¶¯Ê¹ÄÜ
	CheckSysReg( &GPIOA->SEL	, 0x00000080);
	CheckSysReg( &GPIOA->IOANODEG	, 0x00000000 );	//Ä£Äâ¿ÚÒº¾§¡¢AD¹¦ÄÜÑ¡Ôñ
	if(	*PMU_CONTROL != 0x00000000 )*PMU_CONTROL =0x00000000;
	NVIC_SetPriority(PMU_IRQn, 0);   //Âö³åÖÐ¶ÏÓÅÏÈ¼¶×î¸ß£¬0¼¶
 	NVIC_EnableIRQ(PMU_IRQn );
//	NVIC_DisableIRQ(PMU_IRQn );
	
	CheckSysReg( &GPIOB->OEN	, 0x0000D003 );
	CheckSysReg( &GPIOB->IE	  , 0x00005003 );
	CheckSysReg( &GPIOB->ATT	, 0x00000000 );
	
	CheckSysReg( &GPIOC->OEN	, 0x0000FFF4 );
	CheckSysReg( &GPIOC->IE	  , 0x00000004 );
	CheckSysReg( &GPIOC->ATT	, 0x00000000);
	
	CheckSysReg( &GPIOD->OEN	, 0x0000FFFF );
	CheckSysReg( &GPIOD->IE 	, 0x00000000 );
	CheckSysReg( &GPIOD->ATT	, 0x00000000 );
	
	CheckSysReg( &GPIOE->OEN	, 0x00009DFF );
	CheckSysReg( &GPIOE->IE	  , 0x00008028 );
	CheckSysReg( &GPIOE->ATT	, 0x00000000 );
	
	CheckSysReg( &GPIOF->OEN	, 0x00000000 );
	CheckSysReg( &GPIOF->IE	  , 0x00000000 );
	CheckSysReg( &GPIOF->ATT	, 0x00000000 );
}

void Veri_Clk_Gen( void )				//Ê±ÖÓÑ¡ÔñÏà¹Ø
{
//	if( OSCEN	!= runoscen )	OSCEN 		= runoscen;		//Õñµ´Æ÷Ê¹ÄÜ¼Ä´æÆ÷,´ò¿ªrclfºÍrchf
//	if( CLKSEL	!= B0010_0000 )	CLKSEL		= B0010_0000;		//ÏµÍ³Ê±ÖÓÔ´Ñ¡Ôñ¼Ä´æÆ÷,Ñ¡ÔñrchfÎªmclk
//	if( (FDET&B0010_0001) != B0010_0000 )	FDET 	= B0010_0000;		//Í£Õñ¼ì²â¼Ä´æÆ÷,arclkÊ±ÖÓÔ´Îªxtlf,Í£ÕñÖÐ¶ÏÊ¹ÄÜ½ûÖ¹
//	if( DIVSEL	!= rundivsel )	DIVSEL		= rundivsel;		//ÏµÍ³·ÖÆµ¼Ä´æÆ÷
//	if( RCHFCFG	!= B0000_0000 )	RCHFCFG 	= B0000_0000;		//RCHFÖÐÐÄÆµÂÊ
//	if( PLLEN	!= B0000_0011 )	PLLEN 		= B0000_0011;		//PLLÊ¹ÄÜ¼Ä´æÆ÷
//	if( PLLDBL	!= B1111_0011 )	PLLDBL 		= B1111_0011;		//PLLÅäÖÃµÍÎ»
//	if( RAMBISTCLKEN!= B0000_0000 )	RAMBISTCLKEN 	= B0000_0000;
}

//Ä£ÄâµçÂ·½Ó¿Ú 
void Veri_Ana_Ctrl( void )				//µÍÑ¹¼ì²â
{  
  if ( Adcctrl.Status == B0000_0000 )//µç³Øadc´¦Àí
	{	
	  if(	ANA->ADCCTRL != 0x00000311 ) ANA->ADCCTRL = 0x00000311; //µÍÑ¹¼ì²âµçÂ·Ê¹ÄÜ	  	
  }
}


void Veri_5msClk_Gen( void )		//5ms¶¨Ê±Æ÷
{      
   uint32_t pclk;
	 pclk = CLK_GetPCLKFreq();
	 if( TMR0->RELOAD  != (5*pclk/1000) ) TMR0->RELOAD   = (5*pclk/1000) ;
	
	 if( TMR0->CTRL  != 0x00000009 ) TMR0->CTRL   = 0x00000009;
   NVIC_EnableIRQ(TMR0_IRQn); 
		 
}

void Veri_Etmr1_Pcount(unsigned char Result)
{
//	if( Result) 
//	{
//		Decimal.Fw[0] = 0;			//ÕýÏòÂö³åÎ²ÊýÇåÁã
//		Decimal.Bk[0] = 0;			//·´ÏòÂö³åÎ²ÊýÇåÁã
//		Decimal.Unit_Pluse = 0;
//		Decimal.Unit_Bkpluse = 0;
//	}
//	if( ET1LOADL 	!= 1	)	Result  = 1;
//	if( ET1LOADH 	!= 1	)	Result  = 1;
//	if( ET1PRESETL 	!= 0	)	Result  = 1;
//	if( ET1PRESETH 	!= 0	)	Result  = 1;
//	if( Result) 
//	{
//		ET1LOADL  = 1;
//		ET1LOADH  = 1;
//		ET1PRESETL  = 0;
//		ET1PRESETH  = 0;
//		ET1LOADCTRL = 0x11;//¸ßÎ»¼ÓÔØºÍµÍÎ»¼ÓÔØ
//	}
}


//void Veri_80msClk_Gen( void )		//?????		
//{ 	    
//   uint32_t pclk;
//	 pclk = CLK_GetPCLKFreq();
//	 if( TMR1->RELOAD  != (80*pclk/1000) ) TMR0->RELOAD   = (80*pclk/1000) ;
//	
//	 if( TMR1->CTRL  != 0x00000009 ) TMR0->CTRL   = 0x00000009;
//   NVIC_EnableIRQ(TMR1_IRQn); 
//		 
//}
//	
	void Veri_Uart_All( void )
{
	 unsigned char Result;
	 uint32_t pclk;
   
	 if ( ((Uart[0].Flag == 0)&&(Uart[1].Flag == 0)&&(Uart[2].Flag == 0))  )
   {	
		  
		 pclk = CLK_GetPCLKFreq();
   	 //	ÔØ²¨ 
		 if( UART2->CTRL2  != 0x00000002 ) UART2->CTRL2 = 0x00000002;
	   if( UART2->CTRL  != 0x0000000a ) UART2->CTRL   = 0x0000000a;	  
	   if( UART2->BAUDDIV  != (pclk/2400) ) UART2->BAUDDIV   = (pclk/2400);
	   UART2->INTSTS = 0x000000ff;
     UART2->STATE  = 0x000000ff;
		 NVIC_SetPriority(UART2_IRQn, 3);
		 NVIC_EnableIRQ(UART2_IRQn); 
		 //	RS485
		 if( UART5->CTRL2 != 0x00000002 ) UART5->CTRL2  = 0x00000002;
	   if( UART5->CTRL  != 0x0000000a ) UART5->CTRL   = 0x0000000a;	  
		 switch( Mode.Bps[0]&B0111_1110 )
	   {
		     case B0000_0100: //1200bps
			     Result = 0;
			   break;
				 
		     case B0000_1000: //2400bps
			     Result = 1;
			   break;
        	
		     case B0001_0000: //4800bps
			     Result = 2;
			   break;
        	
		     case B0010_0000: //9600bps
			     Result = 3;
			   break;
        	
		     default: 
			     Result = 1;
			   break;
		 }
	   if( UART5->BAUDDIV  != (pclk/BPS_SPBRGL1[ Result ]) ) UART5->BAUDDIV   = (pclk/BPS_SPBRGL1[ Result ]);
	   UART5->INTSTS = 0x000000ff;
     UART5->STATE  = 0x000000ff;
		 NVIC_SetPriority(UART5_IRQn, 3);
	   NVIC_EnableIRQ(UART5_IRQn); 
		 //	ºìÍâ
	   if( UART4->CTRL2  != 0x00000002 ) UART4->CTRL2   = 0x00000002;
	   if( UART4->CTRL  != 0x0000000a ) UART4->CTRL   = 0x0000000a;	   
	   if( UART4->BAUDDIV  != (pclk/1200) ) UART4->BAUDDIV   = (pclk/1200);
	   UART4->INTSTS = 0x000000ff;
     UART4->STATE  = 0x000000ff;
		 if( *MISC_IREN != 0x00000010) *MISC_IREN = 0x00000010;
		 if( *MISC_DUTYL != 29 ) *MISC_DUTYL = 29 ;
		 if( *MISC_DUTYH != 29 ) *MISC_DUTYH = 29 ;
		 NVIC_SetPriority(UART4_IRQn, 3);
		 NVIC_EnableIRQ(UART4_IRQn );
		 //	¼ÆÁ¿
//		 if( UART4->CTRL2  != 0x00000002 ) UART4->CTRL2   = 0x00000002;
//	   if( UART4->CTRL  != 0x00000003 ) UART4->CTRL   = 0x00000003;	  
//	   if( UART4->BAUDDIV  != (pclk/4800) ) UART4->BAUDDIV   = (pclk/4800);
//	   UART4->INTSTS = 0x000000ff;
//     UART4->STATE  = 0x000000ff;
//		 NVIC_SetPriority(UART4_IRQn, 3);
//		 NVIC_DisableIRQ(UART4_IRQn);
	 }	 

}

void Veri_I2c( void )
{
//    if( I2C->CTRL != 0x0000000b )  I2C->CTRL = 0x0000000b;	//½ûÖ¹SPIÖÐ¶Ï
}

void Veri_Spi( void )
{
//	if( I2C->CTRL != 0x0000000b )  I2C->CTRL = 0x0000000b;	//½ûÖ¹SPIÖÐ¶Ï
}

void Veri_Rtc( void)
{
	RTC_WriteProtection(DISABLE);
	if( RTC->WKUCNT	!= 0 ) RTC->WKUCNT		= 0;	//SECIE=1
	if( RTC->PSCA	!= 0) RTC->PSCA		= 0;	//SECIE=1
	if( RTC->WKUSEC != 0 ) RTC->WKUSEC	= 0;	//SECIE=1  	
  if(((RTC->CTL )&BIT2 )==0) RTC->CTL |= BIT2;	//Ê¹ÄÜRTCÊä³ö 	
  if( RTC->DIV !=0x0010AACE ) RTC->DIV = 0x0010AACE;	//Ê¹ÄÜRTCÊä³
 // if( RTC->CAL !=0x00005558 ) RTC->CAL = 0x00005558;	//Ê¹ÄÜRTCÊä³	
	if( RTC->INTEN	!= (BIT2|BIT4|BIT5) ) RTC->INTEN = (BIT2|BIT4|BIT5);	//Í£Ö¹RTC TIMER1/2 		
	RTC_WriteProtection(ENABLE);
	NVIC_SetPriority(UART0_IRQn, 2);
	NVIC_EnableIRQ(RTC_IRQn); 
}

void Veri_U7816( void )
{
//	if( U7816CTRL0     != B1111_0000 ) U7816CTRL0    = B1111_0000;
//	if( U7816CTRL1     != B0000_0000 ) U7816CTRL1    = B0000_0000;
//	if( U7816FRMCTRL0  != B1000_0110 ) U7816FRMCTRL0 = B1000_0110;	//ÖØ·¢1´Î,Å¼Ð£Ñé,ÏÈ·¢lsb
//	if( U7816FRMCTRL1  != B0000_0100 ) U7816FRMCTRL1 = B0000_0100;	//Error signal¿í¶È1.5 etu,
//	if( U7816EGTCTRL   != B0000_0000 ) U7816EGTCTRL	 = B0000_0000;
//	if( U7816CLKDIV	   != B0000_0001 ) U7816CLKDIV	 = B0000_0001;	//mclk=8MÊ±7816=4M,mclk=4MÊ±7816=2M
//	if( U7816PREDIV	   != B0000_1011 ) U7816PREDIV	 = B0000_1011;	//1ETU=1/372
//	if( U7816INTEN	   != B0000_0000 ) U7816INTEN	 = B0000_0000;	//½ûÖ¹ÖÐ¶Ï
}

void Veri_Disp(void)
{
	if( LCD->CTRL !=(BIT4|BIT7|BIT1 )) LCD->CTRL = BIT4|BIT7|BIT1;		//4com or 8com ¨C 0x10; 6com ¨C 0x15
  if( LCD->CTRL2 !=(BIT0)) LCD->CTRL2 = BIT0;		//A²¨ÐÎ 4com = 0x00; 6com = 0x01; 8com =0x02
  
//	if( LCD->SEGCTRL0 !=0x0ff00f0f) LCD->SEGCTRL0 = 0x0ff00f0f;	
//	if( LCD->SEGCTRL1 !=0xbb8e8004) LCD->SEGCTRL1 = 0xbb8e8004;	
//	if( LCD->SEGCTRL2 !=0x00000c18) LCD->SEGCTRL2 = 0x00000c18;
		if( LCD->SEGCTRL0 !=0xfff00ffc) LCD->SEGCTRL0 = 0xfff00ffc;	//seg0~seg31
		if( LCD->SEGCTRL1 !=0xb8160407) LCD->SEGCTRL1 = 0xb8160407;	//seg32~seg63
		if( LCD->SEGCTRL2 !=0x00001c0c) LCD->SEGCTRL2 = 0x00001c0c; //seg64~seg79
	//  if( LCD->SEGCTRL0 !=0xFFFFFFFF) LCD->SEGCTRL0 = 0xFFFFFFFF;	
//	if( LCD->SEGCTRL1 !=0xFFFFFFFF) LCD->SEGCTRL1 = 0xFFFFFFFF;	
//	if( LCD->SEGCTRL2 !=0xFFFFFFFF) LCD->SEGCTRL2 = 0xFFFFFFFF;	
	if(*ANA_REG6  != B0000_0011 )	*ANA_REG6  = B0000_0011;	//½ûÖ¹²âÊÔ

}

void Veri_Sys_Reg( void )
{
	reg_veri_flag = 0;
	Veri_Pad();
	Veri_Clk_Gen();
	Veri_Ana_Ctrl();
	Veri_5msClk_Gen();
	Veri_Uart_All(); 
	Veri_Rtc();
	Veri_Disp();
	if( ANA->REGA !=(BIT1) ) ANA->REGA = (BIT1);
}


//*¶ÔÓ¦µÄÖ±½Ó½øÐÐBitÔËËãÊÇµÍÎ»ÏÈ´«µÄ·½Ê½(¼´"Éú³É¶àÏîÊ½" = 0x8408), Èô¸ßÎ»ÏÈ´«("Éú³É¶àÏîÊ½" = 0x1021)ÔòÏÂ±í²»ÊÊºÏ.
const unsigned short  crc16_ccitt_table[256] =
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


uint16 Crc_16Bits( unsigned char *buf, unsigned char len ) 
{
	unsigned char i,Temp;
	union B16_B08 CRC;
			
	CRC.B16 = 0x0000;

	for( i=0; i<len; i++ )
	{
		CRC.B16 = (CRC.B16 >> 8) ^ crc16_ccitt_table[ (CRC.B16 & 0xFF)^(uint16)(buf[i]) ]; 
	}
	
	Temp = CRC.B08[1];
	CRC.B08[1] = CRC.B08[0];
	CRC.B08[0] = Temp;//¸ß×Ö½ÚÔÚÇ°,ÐÞ¸Ä³ÉµÍ×Ö½ÚÔÚÇ°
	return CRC.B16;
}

unsigned char Crc_Check( unsigned char *buf, unsigned char len ) 
{
	if( ChkBufZero(buf,len)==0 ) return 1;	//ËùÓÐÊý¾Ý¾ùÎª0,ÈÏÎªÐ£Ñé´í

	if( Crc_16Bits( buf, len ) ) return 1;	//crcÐ£Ñé´íÎó
	
	return 0;				//Ð£ÑéÕýÈ·
} 



unsigned char Verify_para(unsigned short Page ,unsigned char Inpage, unsigned char *Buf, unsigned char Len )//Ò»ÍÐ¶þÊý¾ÝÐ£Ñéº¯Êý
{
	if( Crc_Check( Buf, Len+2 ) == 0 ) return 0;	//ram²ÎÊýcrcÕýÈ·
	
	if( I2C_Read_Eeprom( Page, Inpage, Buf, Len ) == 0 ) return 0;//ÒÑ¾­´ÓÊý¾ÝeeÖ÷»Ö¸´
	
	if( I2C_Read_Eeprom( Page+1, Inpage, Buf, Len ) != 0 ) return 1;//Êý¾ÝÈ«´í
	
	I2C_Write_Eeprom( Page, Inpage, Buf, Len );	//´ÓÊý¾Ýee±¸·Ý»Ö¸´,Í¬Ê±Ð´Ö÷
	
	return 0;
}


unsigned char Dl_Check( unsigned char *buf ) 
{
	unsigned char i, BufT[8];
	
	if( IsMultiBcd(buf, 24) ) return 1;	//·ÇBcdÂë, ´íÎó
		
	if( Def_Dl_Difference < 0x03 ) return 0; //½ö½øÐÐBcdÂëÈÝ´í
	
	memset( BufT, 0x00, 4 );
	for( i=1; i<5; i++ ) Add_Bcd( BufT, buf+i*4, 4 );
	if( Cmp_Data( buf, BufT, 4) == 2 ) return 1; //×ÜµçÁ¿ < (4·ÑÂÊºÍ), ´íÎó

	memset( BufT+4, 0x00, 3 );
	BufT[7] = Def_Dl_Difference;
	Add_Bcd( BufT, BufT+4, 4 );
	if( Cmp_Data( buf, BufT, 4) == 1 ) return 1; //×ÜµçÁ¿ > (4·ÑÂÊºÍ+0.xx), ´íÎó
	
	return 0;				//Ð£ÑéÕýÈ·
}

unsigned char Verify_Dl(unsigned char Page ,unsigned char Inpage, unsigned char *Buf, unsigned char Len )//Ò»ÍÐ¶þÊý¾ÝÐ£Ñéº¯Êý
{
	if( (Crc_Check( Buf, Len+2 ) == 0) && (Dl_Check( Buf ) == 0) ) return 0;	//ram²ÎÊýcrcÕýÈ·
	
	if( (I2C_Read_Eeprom( Page, Inpage, Buf, Len ) == 0) && (Dl_Check( Buf ) == 0) ) return 0;//ÒÑ¾­´ÓÊý¾ÝeeÖ÷»Ö¸´
	
	if( (I2C_Read_Eeprom( Page+1, Inpage, Buf, Len ) == 0) && (Dl_Check( Buf ) == 0) )
	{
		I2C_Write_Eeprom( Page, Inpage, Buf, Len );	//´ÓÊý¾Ýee±¸·Ý»Ö¸´,Í¬Ê±Ð´Ö÷
		return 0;
	}
	else  return 1;//Êý¾ÝÈ«´í
}


void Starttimer_Verify(void)	//Æô¶¯Ê±¼äÐ£Ñé
{
	Verify_para( ee_starttimer_page, ee_starttimer_inpage, Starttime.Shiqu, ee_starttimer_lenth );
}

void Esam_Para_Verify()	//±¨¾¯½ð¶î1,2µÈ²ÎÊý(ESAMÒ»Àà²¿·Ö²ÎÊý)Ð£Ñé
{
	
	if(Verify_para( ee_esampara_page, ee_esampara_inpage, Esam_Para.Alarm_Limit1.B08, ee_esampara_lenth ))
	{
		memset( Esam_Para.Alarm_Limit1.B08, 0x00, ee_esampara_lenth );//·Ç·¨Ä¬ÈÏ
		Esam_Para.Current_Ratio[2] = 1;Esam_Para.Voltage_Ratio[2] = 1;//±ä±È1
		Esam_Para.Meter_Id[5] = 0xff;//±íºÅ0x0000000000ff
		Esam_Para.Identity_Delay.B16 = Defaultauthtimer;//Éí·ÝÈÏÖ¤ÓÐÐ§Ê±³¤
	}	
	esam_biaohao_flag = 1;//ÖÃÒÑ¾­²ÉÓÃesam±íºÅ±êÖ¾
}


void Remainmoney_Verify(void)	//Ê£Óà½ð¶î²ÎÊýÐ£Ñé
{
	Verify_para( ee_remainmoney_page, ee_remainmoney_inpage, Money.Remain, ee_remainmoney_lenth );
  
	if( Money_UserINFO.DayCrc16.B16 != Crc_16Bits( Money_UserINFO.Day_Moneyused, 6 ) ) 
	{	
		memset(Money_UserINFO.Day_Moneyused,0, 6 );
		Money_UserINFO.DayCrc16.B16=Crc_16Bits( Money_UserINFO.Day_Moneyused, 6 );	
	}
	if( Money_UserINFO.MonthCrc16.B16 != Crc_16Bits( Money_UserINFO.Month_Moneyused, 6 )  )
	{	
		memset(Money_UserINFO.Month_Moneyused,0, 6 );
		Money_UserINFO.MonthCrc16.B16=Crc_16Bits( Money_UserINFO.Month_Moneyused, 6 );	
  }
	//	
//	if(I2C_Read_Eeprom( ee_DayUserINFO_page, ee_UserINFO_inpage, Money_UserINFO.Day_Moneyused, 16 ))
//	
//	memset(Money_UserINFO.Day_Moneyused,0, 16 );

}

void Money_Para_Verify()	//ÉÞÇ·½ð¶î,¶Ú»ý½ð¶îµÈ²ÎÊýÐ£Ñé
{
	if(Verify_para( ee_limitmenory_page, ee_limitmenory_inpage, Money_Para.Tick.B08, ee_limitmenory_lenth ))
	{
		memcpy( Money_Para.Tick.B08, Def_Limitmenory_table, ee_limitmenory_lenth );//ÉÞÇ·½ð¶îÏÞÖµ/¶Ú»ý½ð¶îÏÞÖµ/ºÏÕ¢ÔÊÐí½ð¶î»Ö¸´Ä¬ÈÏ
	}	
}


void Meter_Const_Verify(void)	//µç±í³£ÊýÐ£Ñé
{
	Meter.Const.B32	 = Def_Meterconst_Bcd;
	Meter.Const_H = Def_Meterconst_Hex;
}


void Meter_RN7302Para_Verify(void)	//µç±í³£ÊýÐ£Ñé
{
		//	////201800000175
	if(Verify_para(ee_para1_RN8302_page ,ee_para1_RN8302_inpage, Adj_Block[0].AdjBuff, ee_para1_RN8302_lenth ))
	{
		
		memset( Adj_Block[1].AdjBuff,0, 32 );
		Adj_Block[0].Adj.Igain =63965 ;
	  Adj_Block[0].Adj.Ugain =2131 ;
		Adj_Block[0].Adj.Pgain =0;
		Adj_Block[0].Adj.UTofst =0x80 ;
		Adj_Block[0].Adj.ITofst[0] =0x86;
		Adj_Block[0].Adj.ITofst[1] =0x86 ;
		Adj_Block[0].Adj.ITofst[2] =0x86;
	
	}
	if(Verify_para(ee_para2_RN8302_page ,ee_para2_RN8302_inpage, Adj_Block[1].AdjBuff, ee_para2_RN8302_lenth ))
	{
		memset( Adj_Block[1].AdjBuff,0, 32 );
		Adj_Block[1].Adj.Igain =63916 ;
	  Adj_Block[1].Adj.Ugain =2255 ;
		Adj_Block[1].Adj.Pgain =48 ;
		Adj_Block[1].Adj.UTofst =0x80 ;
		Adj_Block[1].Adj.ITofst[0] =0x84;
		Adj_Block[1].Adj.ITofst[1] =0x84 ;
		Adj_Block[1].Adj.ITofst[2] =0x84 ;
	}
	
	if(Verify_para(ee_para3_RN8302_page ,ee_para3_RN8302_inpage, Adj_Block[2].AdjBuff, ee_para3_RN8302_lenth ))
	{
		memset( Adj_Block[2].AdjBuff,0, 32 );
		Adj_Block[2].Adj.Igain =63928 ;
	  Adj_Block[2].Adj.Ugain =2222 ;
		Adj_Block[2].Adj.Pgain =103 ;
		Adj_Block[2].Adj.UTofst =0x80 ;
		Adj_Block[2].Adj.ITofst[0] =0x86;
		Adj_Block[2].Adj.ITofst[1] =0x86 ;
		Adj_Block[2].Adj.ITofst[2] =0x86 ;
	}
		//	////201800000174
//	if(Verify_para(ee_para1_RN8302_page ,ee_para1_RN8302_inpage, Adj_Block[0].AdjBuff, ee_para1_RN8302_lenth ))
//	{
//		
//		memset( Adj_Block[1].AdjBuff,0, 32 );
//		Adj_Block[0].Adj.Igain =64004 ;
//	  Adj_Block[0].Adj.Ugain =2305;
//		Adj_Block[0].Adj.Pgain =17;
//		Adj_Block[0].Adj.UTofst =0x80 ;
//		Adj_Block[0].Adj.ITofst[0] =0xA8;
//		Adj_Block[0].Adj.ITofst[1] =0xA8 ;
//		Adj_Block[0].Adj.ITofst[2] =0xA8;
//	
//	}
//	if(Verify_para(ee_para2_RN8302_page ,ee_para2_RN8302_inpage, Adj_Block[1].AdjBuff, ee_para2_RN8302_lenth ))
//	{
//		memset( Adj_Block[1].AdjBuff,0, 32 );
//		Adj_Block[1].Adj.Igain =63993 ;
//	  Adj_Block[1].Adj.Ugain =1817 ;
//		Adj_Block[1].Adj.Pgain =0 ;
//		Adj_Block[1].Adj.UTofst =0x80 ;
//		Adj_Block[1].Adj.ITofst[0] =0x80;
//		Adj_Block[1].Adj.ITofst[1] =0x80 ;
//		Adj_Block[1].Adj.ITofst[2] =0x80 ;
//	}
//	
//	if(Verify_para(ee_para3_RN8302_page ,ee_para3_RN8302_inpage, Adj_Block[2].AdjBuff, ee_para3_RN8302_lenth ))
//	{
//		memset( Adj_Block[2].AdjBuff,0, 32 );
//		Adj_Block[2].Adj.Igain =64017 ;
//	  Adj_Block[2].Adj.Ugain =2450 ;
//		Adj_Block[2].Adj.Pgain =0 ;
//		Adj_Block[2].Adj.UTofst =0x80 ;
//		Adj_Block[2].Adj.ITofst[0] =0x80;
//		Adj_Block[2].Adj.ITofst[1] =0x80 ;
//		Adj_Block[2].Adj.ITofst[2] =0x80 ;
//	}
	////201800000173
//	if(Verify_para(ee_para1_RN8302_page ,ee_para1_RN8302_inpage, Adj_Block[0].AdjBuff, ee_para1_RN8302_lenth ))
//	{
//		
//		memset( Adj_Block[1].AdjBuff,0, 32 );
//		Adj_Block[0].Adj.Igain =63980 ;
//	  Adj_Block[0].Adj.Ugain =2205 ;
//		Adj_Block[0].Adj.Pgain =65504;
//		Adj_Block[0].Adj.UTofst =0x80 ;
//		Adj_Block[0].Adj.ITofst[0] =0x84;
//		Adj_Block[0].Adj.ITofst[1] =0x84 ;
//		Adj_Block[0].Adj.ITofst[2] =0x84 ;
//	
//	}
//	if(Verify_para(ee_para2_RN8302_page ,ee_para2_RN8302_inpage, Adj_Block[1].AdjBuff, ee_para2_RN8302_lenth ))
//	{
//		memset( Adj_Block[1].AdjBuff,0, 32 );
//		Adj_Block[1].Adj.Igain =63860 ;
//	  Adj_Block[1].Adj.Ugain =2206 ;
//		Adj_Block[1].Adj.Pgain =24 ;
//		Adj_Block[1].Adj.UTofst =0x80 ;
//		Adj_Block[1].Adj.ITofst[0] =0x85;
//		Adj_Block[1].Adj.ITofst[1] =0x85 ;
//		Adj_Block[1].Adj.ITofst[2] =0x85 ;
//	}
//	
//	if(Verify_para(ee_para3_RN8302_page ,ee_para3_RN8302_inpage, Adj_Block[2].AdjBuff, ee_para3_RN8302_lenth ))
//	{
//		memset( Adj_Block[2].AdjBuff,0, 32 );
//		Adj_Block[2].Adj.Igain =63848 ;
//	  Adj_Block[2].Adj.Ugain =2158 ;
//		Adj_Block[2].Adj.Pgain =0 ;
//		Adj_Block[2].Adj.UTofst =0x80 ;
//		Adj_Block[2].Adj.ITofst[0] =0x85;
//		Adj_Block[2].Adj.ITofst[1] =0x85;
//		Adj_Block[2].Adj.ITofst[2] =0x85;
//	}
	

}


unsigned char Fwdl_Verify(void)		//ÕýÏòµçÁ¿Ð£Ñé
{
	fwdl_veri_flag = 0;
	Meter_Const_Verify();
	return (Verify_Dl( ee_fwdl_page, ee_fwdl_inpage, Fwdl.Fwdl_T.B08, ee_fwdl_lenth ));
}

unsigned char Bkdl_Verify(void)		//·´ÏòµçÁ¿Ð£Ñé
{
	bkdl_veri_flag = 0;
	Meter_Const_Verify();
	return (Verify_Dl( ee_bkdl_page, ee_bkdl_inpage, Bkdl.Bkdl_T.B08, ee_bkdl_lenth ));
}

void Demand_Para_Verify(void)		//·´ÏòµçÁ¿Ð£Ñé
{
	
	Dm_veri_flag = 0;

	if(Verify_para( ee_Demand_Period_page, ee_Demand_Period_inpage,&Demand_Para.Max_Demand_Period, ee_Demand_Period_len ))
	{	
		memcpy( &Demand_Para.Max_Demand_Period,(uint08*)Def_Demand_table, ee_Demand_Period_len );
	}
}

void Mode_Verify(void)			//ÌØÕ÷Ä£Ê½×ÖÐ£Ñé
{
	unsigned char Buf[ee_featureword_lenth+2];
	
	mode_veri_flag = 0;
//	WriteE2WithBackup( ee_featureword_page, ee_featureword_inpage, (uint08*)Def_featureword_table, ee_featureword_lenth );
	if( Verify_para( ee_featureword_page, ee_featureword_inpage, Mode.Bps, ee_featureword_lenth ) )
	{
		memcpy( Mode.Bps, Def_featureword_table, ee_featureword_lenth );
	}
	
	if( (Memoryerr_Ctrl == 0x55) &&	//´æ´¢Æ÷¹ÊÕÏÉÏ±¨±àÒëÊ¹ÄÜ
	    (I2C_Read_Eeprom(ee_featureword_page, ee_featureword_inpage, Buf, ee_featureword_lenth) 
		&& I2C_Read_Eeprom(ee_featureword_page+1, ee_featureword_inpage, Buf, ee_featureword_lenth)) )
	{
		if( eeprom_error_flag == 0 )
		{
			Set_Reportdata(0*8+5);//ÖÃ´æ´¢Æ÷¹ÊÕÏÉÏ±¨±êÖ¾
		}
		eeprom_error_flag = 1; //ÖÃ´æ´¢Æ÷¹ÊÕÏ±êÖ¾
	}
	else	eeprom_error_flag = 0; //Çå´æ´¢Æ÷¹ÊÕÏ±êÖ¾
	
	//ÊÂ¼þÅÐ¶ÏÉè¶¨·¶Î§Ð£Ñé(Î÷Á¦ÐÂÔö)
	if( Verify_para( ee_eventthreshold_page, ee_eventthreshold_inpage, Threshold, ee_eventthreshold_lenth ) )
	{
		memcpy( Threshold, Def_eventthreshold_table, ee_eventthreshold_lenth );
	}
}



void Week_Holiday_Verify(void)	//ÖÜÐÝÈÕºÍ¸ººÉ²ÎÊýÐ£Ñé
{
	if(Verify_para( ee_weekholiday_page, ee_weekholiday_inpage, &Week_Holiday.Status, ee_weekholiday_lenth ))
	{
		memcpy( &Week_Holiday.Status, Def_weekloadpara_table, ee_weekholiday_lenth );
	}
}

void feik_Num_Verify( void ) //Ê±ÇøÊýµÈ²ÎÊýÐ£Ñé
{
	if(Verify_para( ee_feik_number_page, ee_feiknumber_inpage, Feik.Num, ee_feiknumber_lenth )) //¶ÁÊ±ÇøÊý
	{
		memcpy( Feik.Num, Def_feik_table, ee_feiknumber_lenth );
	}
}


void Temperpara_Verify(void)	//ÎÂ¶ÈÏµÊýÐ£Ñé
{
	temperpara_veri_flag = 0;
	if(Verify_para( ee_temperature_page, ee_temperature_inpage, Tpara.Offset, ee_temperature_lenth ))
	{
		memset( Tpara.Offset, 0x00, ee_temperature_lenth );//Ä¬ÈÏÎª0 
		Tpara.Offset[0]	= 0x80;
	}
	if( Tpara.RtcTMode == 0xAA )//0xAA³£ÎÂ²é±í²åÖµÄ£Ê½
	{
		if( Crc_Check( RtcTadcBuf.PTAT_Value[0].B08, ee_ptatvalue_lenth+2 ) != 0 ) 
		{
			I2C_Read_Eeprom( ee_ptatvalue_page, ee_ptatvalue_inpage, RtcTadcBuf.PTAT_Value[0].B08, ee_ptatvalue_lenth );
		}
		if( Crc_Check( RtcTcapBuf.CAP_Value[0].B08, ee_capvalue_lenth+2 ) != 0 )
		{
			I2C_Read_Eeprom( ee_capvalue_page, ee_capvalue_inpage, RtcTcapBuf.CAP_Value[0].B08, ee_capvalue_lenth );
		} 		
	}
}


void Commaddr_Verify(void)		//Í¨Ñ¶µØÖ·Ð£Ñé
{
	addr_veri_flag = 0;
	if( Verify_para( ee_commaddr_page, ee_commaddr_inpage, Comm_Addr.Comm_Id, ee_commaddr_lenth ) )
	{
		memset( Comm_Addr.Comm_Id, 0x00, 6 );//Í¨Ñ¶µØÖ·Ä¬ÈÏÎª01
		Comm_Addr.Comm_Id[5] = 0x01;
	}
}

void RdMeterMode( void )	//¸üÐÂ·Ñ¿ØÄ£Ê½Bit¶¨Òå£ºBDMODE
{
	if( Display_Para.MeterCtrl == 1 ) BDMODE = 0; //Ô¶³ÌÄ£Ê½
	else BDMODE = 1; //±¾µØÄ£Ê½
}

void Display_Para_Verify(void)	//ÏÔÊ¾²ÎÊýÐ£Ñé
{
	disppm_veri_flag = 0;
	if( Verify_para( ee_displaypara_page, ee_displaypara_inpage, &Display_Para.S_Number, ee_displaypara_lenth ) )
	{
		memcpy( &Display_Para.S_Number, Def_DisplayPara_table+8*METERTYPE, ee_displaypara_lenth );
	}
	Hw_Lcdled_Timer = Display_Para.Loop_Time*Display_Para.S_Number*2;
	
	RdMeterMode(); //¸üÐÂ·Ñ¿ØÄ£Ê½Bit¶¨Òå£ºBDMODE
}

void Displayitem_Verify(void)	//ÏÔÊ¾ÏîÄ¿Ð£Ñé(Ò»ÍÐÒ»)
{
	unsigned char i,j;
	
	dispitem_veri_flag = 0;
	for( i=0; i<4; i++ )
	{
		if( Crc_Check( Item[i].Display, ee_displayitem_lenth+2 ) == 0 ) continue;//ram²ÎÊýcrcÕýÈ·
		if( I2C_Read_Eeprom( ee_loop_displayitem_page+i, 0x00, Item[i].Display, ee_displayitem_lenth) )//ÒÑ¾­´ÓÊý¾Ýee»Ö¸´
		{
			if( (i==0) || (i==2) )//Ð£Ñé´íÎó²ÉÓÃÄ¬ÈÏÖµ(30Ïî)
			{
				for( j=0; j<30; j++ )
				{
					Item[i].Display[j] = j+1;
				}
			}
		}		
	}
}







