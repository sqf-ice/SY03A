#include "ex_func.h"
#include "ex_var.h"
#include <string.h>
#include <absacc.h>

#if(LCDTYPE == 0)
#define		disp_S1	DispBuf[0] |= 0x00020000	//???? s35 c
#define		disp_S2	DispBuf[18] |= 0x00200000	//??current s6 c
#define		disp_S3	DispBuf[16] |= 0x20000000	//pre  s7  c
#define		disp_S4	DispBuf[16] |= 0x00200000	//1  s8  c
#define		disp_S5	DispBuf[15] |= 0x01002000	//MON  s9  c
#define		disp_S6	DispBuf[15] |= 0x00000020	//DAY  s10  c
#define		disp_S7	DispBuf[14] |= 0x20000000	//TOTAL  s11  c
#define		disp_S8	DispBuf[8] |= 0x00200000	//T1   s15  c
#define		disp_S9	DispBuf[8] |= 0x00001000	//T2   s16  c
#define		disp_S10	DispBuf[8] |= 0x00002000	//T3  s17  c
#define		disp_S11	DispBuf[8] |= 0x00000020	//T4  s18  c
#define		disp_S12	DispBuf[15] |= 0x00000010	//MD  s23  c
#define		disp_S13	DispBuf[15] |= 0x00000008	//ID  s24  c
#define		disp_S14	DispBuf[14] |= 0x10000000	//CONS  s25  c 
#define		disp_S15	DispBuf[13] |= 0x00000010	//DEBT  s26  c
#define		disp_S16	DispBuf[12] |= 0x00100000	//ENERGY  s27  c
#define		disp_S17	DispBuf[12] |= 0x00001000	//PRICE  s28  c
#define		disp_S18	DispBuf[10] |= 0x00100000	//TIME  s63  c
#define		disp_S19	DispBuf[8] |= 0x00100000	//RECHARGE  s29  c
#define		disp_S20	DispBuf[8] |= 0x00000010  //REMAIN 
#define		disp_S21	DispBuf[8] |= 0x00000010	//AMOUNT   s30  c
#define		disp_S22	DispBuf[0] |= 0x00000000	//TARIFF
#define		disp_S23	DispBuf[0] |= 0x00010000	//TARIFF 1  s31 c
#define		disp_S24	DispBuf[0] |= 0x01000000	//TARIFF 2  s32 c
#define		disp_S25	DispBuf[1] |= 0x00000001	//TARIFF 3  s33 c
#define		disp_S26	DispBuf[1] |= 0x00000100	//TARIFF 4  s34 c
#define		disp_S27	DispBuf[0] |= 0x00040000	//KEY OFF  s39
#define		disp_S28	DispBuf[0] |= 0x00080000	//KEY ON  s70 
#define		disp_S29	DispBuf[0] |= 0x00100000	//KEY  s61  
#define		disp_S30	DispBuf[0] |= 0x08000000	//TEL  s40 c
#define		disp_S31	DispBuf[0] |= 0x00200000	//REVERCE  
#define		disp_S32	DispBuf[1] |= 0x00000008	//LOW BAT s41 c

//????
#define   disp_Ua   DispBuf[0] |= 0x02000000  //Ua 36 
#define   disp_Ub   DispBuf[1] |= 0x00000002  //Ub 37 
#define   disp_Uc   DispBuf[1] |= 0x00000200  //Uc 38 
#define   disp_kwarh  DispBuf[6] |= 0x00000100 //kwarh  s52
#define   disp_OD		DispBuf[6] |= 0x00010000  //OD s54 
#define   disp_forward		DispBuf[12] |= 0x00200000  //forward s12
#define   disp_P		DispBuf[12] |= 0x00002000  //P s13
#define   disp_Q		DispBuf[10] |= 0x00200000  //Q s14
#define   disp_Ia   DispBuf[0] |= 0x20000000  //Ia 45
#define   disp_Ib   DispBuf[1] |= 0x00000020  //Ib 47 
#define   disp_Ic   DispBuf[1] |= 0x00002000  //Ic 48 
#define   disp_negative_Ia   DispBuf[0] |= 0x30000000  //Ia 45  44
#define   disp_negative_Ib   DispBuf[1] |= 0x00000030  //Ib 47  46
#define   disp_negative_Ic   DispBuf[1] |= 0x00003000  //Ic 48  62
//??
#define   disp_coordinate		DispBuf[18] |= 0x10000000	//   s1
//????1234
#define		disp_I	DispBuf[18] |= 0x20000000	//   s2  
#define		disp_II	DispBuf[19] |= 0x00000020	//   s3  
#define		disp_III	DispBuf[19] |= 0x00000010	//  s4  
#define		disp_IV	DispBuf[18] |= 0x08000000	//  s5  
//????1234
#define		disp_f1	DispBuf[18] |= 0x00100000	//   s19  c
#define		disp_f2	DispBuf[16] |= 0x10000000	//   s20  c
#define		disp_f3	DispBuf[16] |= 0x08000000	//  s21  c
#define		disp_f4	DispBuf[15] |= 0x00001000	//  s22  c

#define		disp_S34	DispBuf[6] |= 0x00000002	//GHC  s49
#define		disp_S36	DispBuf[6] |= 0x00000001	//COS  s50
#define		disp_S37	DispBuf[6] |= 0x00000200	//V  s51
#define		disp_S38	DispBuf[6] |= 0x00020000	//TOKEN  s53
#define		disp_S39	DispBuf[7] |= 0x00000200	//k  s55
#define		disp_S40	DispBuf[7] |= 0x00000200	//W  s55
#define		disp_S41	DispBuf[7] |= 0x00000002	//h  s56
#define		disp_S42	DispBuf[6] |= 0x02000000	//H  s57
#define		disp_S43	DispBuf[6] |= 0x02000000	//z  s57
#define		disp_S44	DispBuf[7] |= 0x00000100	//A  s58
#define		disp_S45	DispBuf[7] |= 0x00000001	//DAY  s59
#define		disp_S46	DispBuf[6] |= 0x01000000	//OC  s60

#define		disp_dp1	DispBuf[13] |= 0x00000002	// 1
#define		disp_dp2	DispBuf[12] |= 0x00000200	// 2
#define		disp_dp3	DispBuf[8] |= 0x00020000	// 3
#define		disp_dp4	DispBuf[8] |= 0x00000002	// 4
#define		disp_dp5	DispBuf[1] |= 0x00000400	// 5
#define		disp_dp6	DispBuf[6] |= 0x00002000	// 6
#define		disp_dp7	DispBuf[5] |= 0x00002000	// 7
#define		disp_dp8	DispBuf[5] |= 0x20000000	// 8
#define		disp_dp9	DispBuf[6] |= 0x00200000	// 9
#define		disp_dp10	DispBuf[7] |= 0x00000020	// 10
#endif

void LCD_Num_Data( unsigned char Num, unsigned char show)
{
	unsigned char  dis_buff[80];	//
	unsigned char  Result,i;
//	const unsigned char code NumSegD1[16] = 
//	{0xDE,0x88,0xE6,0xEA,0xB8,0x7A,0x7E,0xC8, 0xFE,0xFA,//0~9
//	 0x24, 0x20, 0x16, 0xBC, 0x76, 0x9E};//r,-,L,H,E,U
//	const unsigned char code NumSegD2_D9[16] = 
//	{0xF5,0x60,0xB6,0xF2,0x63,0xD3,0xD7,0x70,0xF7,0xF3,//0~9
//	 0x06, 0x02, 0x85, 0x67, 0x97, 0xE5};//r,-,L,H,E,U
//	const unsigned char code NumSegD10_D13[16] = 
//	{0xDE,0x48,0x76,0x6E,0xE8,0xAE,0xBE,0x4A, 0xFE,0xEE,//0~9
//	 0x30, 0x20, 0x94, 0xF8, 0xB6, 0xDC};//r,-,L,H,E,U
//	const unsigned char code NumSegD14_D17[16] = 
//	{0xAF,0x06,0x6D,0x4F,0xC6,0xCB,0xEB,0x0E, 0xEF,0xCF,//0~9
//	 0x60, 0x40, 0xA1, 0xE6, 0xE9, 0xA7};//r,-,L,H,E,U
	
	const unsigned char code NumSegD1[16] = 
	{
		0xf5,0x60,0xd3,0xf2,0x66,0xb6,0xb7,0xe0,0xf7,0xf6,
		0x03,0x02,0x15,0x67,0x97,0x75
	};	
		const unsigned char code NumSegD2_D4[16] = 
	{
		0x6f,0x60,0x5b,0x7a,0x74,0x3e,0x3f,0x68,0x7f,0x7e,
		0x11,0x10,0x07,0x75,0x1f,0x67
	};	 
		const unsigned char code NumSegD5_D9[16] = 
	{
		0x9f,0x90,0xcb,0xda,0xd4,0x5e,0x5f,0x98,0xdf,0xde,
		0x41,0x40,0x07,0xd5,0x4f,0x97			
	};
		const unsigned char code NumSegD10_D12[16] = 
	{
		0xfa,0x0a,0xd6,0x9e,0x2e,0xbc,0xfc,0x1a,0xfe,0xbe,
		0x44,0x04,0xe0,0x6e,0xf4,0xea
	};//r,-,L,H,E,U
	const unsigned char code NumSegD13_D14[16] = 
	{
		0xf5,0x05,0xd3,0x97,0x27,0xb6,0xf6,0x15,0xf7,0xb7,
		0x42,0x02,0xe0,0x67,0xf2,0xe5
	};
	const unsigned char code NumSegD15[16] = 
	{
		0xd7,0x06,0xe3,0xa7,0x36,0xb5,0xf5,0x07,0xf7,0xb7,
		0x60,0x20,0xd0,0x76,0xf1,0xd6
	};
	const unsigned char code NumSegD16_D17[16] = 
	{
		0x5f,0x06,0x6b,0x2f,0x36,0x3d,0x7d,0x07,0x7f,0x3f,
		0x60,0x20,0x58,0x76,0x79,0x5e
	};
	 memset( dis_buff, 0x00, 80 );	//??????

	if(Num==1)
	{
		Result=NumSegD1[show&0x0f];
		DispBuf[15]|= 0x00000000+((Result&0xf0)<<22);
		DispBuf[16]|= 0x00000000+((Result&0x0f)<<18);
	}
	if(Num==2)
	{
		Result=NumSegD2_D4[show&0x0f];
		DispBuf[18]|= 0x00000000+((Result&0xf0)<<20);
		DispBuf[19]|= 0x00000000+(Result&0x0f);
	}
	if(Num==3)
	{
		Result=NumSegD2_D4[show&0x0f];
		DispBuf[16]|= 0x00000000+((Result&0xf0)<<20);
		DispBuf[18]|= 0x00000000+((Result&0x0f)<<16);
	}
	if(Num==4)
	{
		Result=NumSegD2_D4[show&0x0f];
		DispBuf[15]|= 0x00000000+((Result&0xf0)>>4);
		DispBuf[15]|= 0x00000000+((Result&0x0f)<<8);
	}
	if(Num==5)
	{
		Result=NumSegD5_D9[show&0x0f];
		DispBuf[13]|= 0x00000000+((Result&0xf0)>>4);
		DispBuf[14]|= 0x00000000+((Result&0x0f)<<24);
	}
	if(Num==6)
	{
		Result=NumSegD5_D9[show&0x0f];
		DispBuf[12]|= 0x00000000+((Result&0xf0)<<4);
		DispBuf[12]|= 0x00000000+((Result&0x0f)<<16);
	}
	if(Num==7)
	{
		Result=NumSegD5_D9[show&0x0f];
		DispBuf[8]|= 0x00000000+((Result&0xf0)<<12);
		DispBuf[10]|= 0x00000000+((Result&0x0f)<<16);
	}
	if(Num==8)
	{
		Result=NumSegD5_D9[show&0x0f];
		DispBuf[8]|= 0x00000000+((Result&0xf0)>>4);
		DispBuf[8]|= 0x00000000+((Result&0x0f)<<8);
	}
	if(Num==9)
	{
		Result=NumSegD5_D9[show&0x0f];
		DispBuf[7]|= 0x00000000+((Result&0xf0)<<12);
		DispBuf[7]|= 0x00000000+((Result&0x0f)<<24);
	}
	if(Num==10)
	{
		Result=NumSegD10_D12[show&0x0f];
		DispBuf[1]|= 0x00000000+((Result&0xf0)<<14);
		DispBuf[1]|= 0x00000000+((Result&0x0f)<<26);
	}
	if(Num==11)
	{
		Result=NumSegD10_D12[show&0x0f];
		DispBuf[2]|= 0x00000000+((Result&0xf0)>>2);
		DispBuf[2]|= 0x00000000+((Result&0x0f)<<10);
	}
	if(Num==12)
	{
		Result=NumSegD10_D12[show&0x0f];
		DispBuf[2]|= 0x00000000+((Result&0xf0)<<14);
		DispBuf[2]|= 0x00000000+((Result&0x0f)<<26);
	}
	if(Num==13)
	{
		Result=NumSegD13_D14[show&0x0f];
		DispBuf[5]|= 0x00000000+((Result&0xf0)>>2);
		DispBuf[5]|= 0x00000000+((Result&0x0f)<<10);
	}
	if(Num==14)
	{
		Result=NumSegD13_D14[show&0x0f];
		DispBuf[5]|= 0x00000000+((Result&0xf0)<<14);
		DispBuf[5]|= 0x00000000+((Result&0x0f)<<26);
	}
	if(Num==15)
	{
		Result=NumSegD15[show&0x0f];
		DispBuf[6]|= 0x00000000+((Result&0xf0)>>2);
		DispBuf[6]|= 0x00000000+((Result&0x0f)<<10);
	}
	if(Num==16)
	{
		Result=NumSegD16_D17[show&0x0f];
		DispBuf[6]|= 0x00000000+((Result&0xf0)<<14);
		DispBuf[6]|= 0x00000000+((Result&0x0f)<<26);
	}
	if(Num==17)
	{
		Result=NumSegD16_D17[show&0x0f];
		DispBuf[7]|= 0x00000000+((Result&0xf0)>>2);
		DispBuf[7]|= 0x00000000+((Result&0x0f)<<10);
	}
//	for(i=0;i<20;i++)
//	{
//		DispBuf[i]|=(((0x00000000+dis_buff[i*4+3])<<24)+((0x00000000+dis_buff[i*4+2])<<16)+((0x00000000+dis_buff[i*4+1])<<8)+(0x00000000+dis_buff[i*4]));
//	}
}

void LCDTranslate64(void)
{

}

void Lcd_Disp_Money( void )
{
	disp_S21;//disp_money;			//??			
	disp_dp3;//disp_dot2;			//2???
	disp_S34;//disp_yuan;			//?
	disp_S20;//disp_remain;			//??
}

void Lcd_Disp_Zedl(uint08 month, uint08 feilv )
{
	switch( feilv )
	{
		case 0:	disp_S7;		break;//disp_zong
		case 1:	disp_S8;		break;//disp_jian
		case 2:	disp_S9;		break;//disp_feng
		case 3:	disp_S10;		break;//disp_ping
		case 4:	disp_S11;		break;//disp_gu
		default: 			break;
	}
	
	if(month == 0)
	{
		disp_S2;//disp_dangqian;			//??
	}
	else
	{
		disp_S3;//disp_shang;			//?
		disp_S5;//disp_yue;			//?
		
		if(month > 9)
		{
			disp_S4;//disp_1;
			month -= 10;
		}
		LCD_Num_Data( 1, month);
	}
}


void Lcd_Show_ZF( void )
{
	/*
	if(dl_back_flag) disp_fan;
	else disp_zheng;
	disp_xiang;
	*/
}

//????numflag numflag=0????? numflag=1?????
unsigned char Lcd_Show_N( unsigned char start, unsigned char len, unsigned char *Show,unsigned char numflag)
{
	unsigned char i, k, dotnum=0, Lcddot = 0;
	
	i = 0;
	if(start & 0xf0 )	//?????????
	{
		dotnum = (start & 0xf0)>>4;	//start?????????,F??????
		if(dotnum == 0x0f) dotnum = Display_Para.Dl_Dot;
		if(dotnum == 0x0e) dotnum = Display_Para.P_Dot;
		start &= 0x0f;
		if(numflag==0)
		{
			switch(dotnum)
			{
				case 0x01:	disp_dp4;	break;
				case 0x02:	disp_dp3;	break;
				case 0x03:	disp_dp2;	break;
				case 0x04:	disp_dp1;	break;
				default:			break;
			}
		}
		else
		{
			switch(dotnum)
			{
				case 0x01:	disp_dp10;	break;
				case 0x02:	disp_dp9;	break;
				case 0x03:	disp_dp8;	break;
				case 0x04:	disp_dp7;	break;
				default:			break;
			}
		}
		disp_0_flag = 1;
	}
	if( disp_0_flag )//????
	{		
		for(i=0; i<(7-dotnum); i++)	
		{
			if((i%2) == 0)	//??????
			{
				if(((Show[i/2]>>4)&0x0F) != 0) break;
			}
			else						//??????
			{
				if(((Show[i/2]>>0)&0x0F) != 0) break;
			}
			
			Lcddot++;
		}
	}
	
	for( k=i; k<len*2; k++ )//????
	{
		if(k%2)	//??????
		{
			if(numflag==0)
			{
					LCD_Num_Data( start+k+1, (Show[k/2]>>0)&0x0F);
			}
			else
			{
					LCD_Num_Data( start+k+9, (Show[k/2]>>0)&0x0F);
			}
		}
		else		//??????
		{
			if(numflag==0)
			{
					LCD_Num_Data( start+k+1, (Show[k/2]>>4)&0x0F);
			}
			else
			{
					LCD_Num_Data( start+k+9, (Show[k/2]>>4)&0x0F);
			}
		}
	}
	
	return( Lcddot );
}

//?????????(0~4?????)
void Disp_Dl_Dot(unsigned char *Ee_Buf)
{
	union B32_B08 Temp32;
	
	//disp_dian;			//?
	//disp_liang;            	 	//?
	//disp_kw;                	//KW
	//disp_h;                 	//h
	disp_S16;
	disp_S39;
	disp_S40;
	disp_S41;
	memcpy( Temp32.B08, Ee_Buf, 4 );
	
	if(Display_Para.Dl_Dot < 2)
	{
		Temp32.B32 >>= (2-Display_Para.Dl_Dot)*4;
	}
//	else
//	{
//		Temp32.B32 <<= (Display_Para.Dl_Dot-2)*4;
//	}
	
	Lcd_Show_N( 0xf1, 4, Temp32.B08,1);
}


//?????????(0~4?????)
void Disp_Pw_Dot(unsigned char *Ee_Buf)
{
	union B32_B08 Temp32;
	
	memcpy( Temp32.B08, Ee_Buf, 4 );
	
	Temp32.B32 >>= (4-Display_Para.P_Dot)*4;

	Lcd_Show_ZF();
	Lcd_Show_N( 0xe1, 4, Temp32.B08,1);
}


void Lcd_Show_Err( unsigned char Error)
{
	LCD_Num_Data( 3, 0x0E);		//Err-xx
	LCD_Num_Data( 4, 0x0A);
	LCD_Num_Data( 5, 0x0A);
	LCD_Num_Data( 6, 0x0B);
	LCD_Num_Data( 7, Error>>4);
	LCD_Num_Data( 8, Error>>0);
}

void Disp_Time_Dot( unsigned char Type )
{
	if(Type == 0)
	{
		disp_dp3;
		disp_dp1;
	}
	else
	{
		disp_dp9;
		disp_dp7;
		disp_dp5;
		disp_dp6;
	}
}

void TokenChange(unsigned char *Buf, unsigned char *Ee_Buf )
{
	unsigned char i;
	for( i=0; i<20; i++ )
	Buf[i]=(i%2)?((Ee_Buf[i/2]&0xf0)>>4):(Ee_Buf[i/2]&0x0f);
	
}
unsigned char GetDispItem( unsigned char Status, unsigned char Sn )
{
	unsigned char i, j, Temp08;
	
	i = Sn/62;
	j = Sn%62;
	Temp08 = Item[i+(Status&1)*2].Display[j];
	//if( ((Temp08<227)&&(Temp08!=215)&&(Temp08!=216)&&(!BDMODE)) || ((Temp08<=232)&&(BDMODE)) ) return (Temp08);//?????????????
	if( ((Temp08<255)&&(Temp08!=215)&&(Temp08!=216)&&(!BDMODE)) || ((Temp08<=255)&&(BDMODE)) ) return (Temp08);//?????????????
	return 0;
}


//??????, ?????????????
unsigned char Get_Show_Cnt( void )
{
	unsigned char n, Temp08=0, Total;

	if( Display.Status&0x01 ) 
		Total = Display_Para.K_Number;
	else
		Total = Display_Para.S_Number;
	
	for( n=0; n<Total; n++ )
	{
		Temp08 = GetDispItem( Display.Status, (Display.Number-1) );
		if( Temp08 ) break;

		Display.Number++;
		if( Display.Number > Total ) Display.Number = 1;
	}
	
	if( Temp08 == 0 )
	{
		if( BDMODE )
			Temp08 = 229;	//????
		else
			Temp08 = 1;	//?????????
	}
	
	return (Temp08);
}


//????????????????
unsigned char Get_Valid_Num( unsigned char Status )
//Status: 0-??, 2-??
{
	unsigned char n, Temp08, Result, Total;
	
	if( Status&0x01 ) 
		Total = Display_Para.K_Number;
	else
		Total = Display_Para.S_Number;
	
	Result = 0x00;
	for( n=0; n<Total; n++ )
	{
		Temp08 = GetDispItem( Status, n );
		if( Temp08 ) Result++;
	}
	
	return (Result);
}

void CalculateUseDay(void)
{
  unsigned char i;
	unsigned char buf[6];
	union B32_B08 Tempa,Tempb;
	float  Tempc;
	
	memset( buf, 0, 6 );

	if( ChkBufZero( &Money.Remain[2], 4 ) )
	{
		I2C_Read_Eeprom( ee_Pre7DayMon_page, ee_Pre7DayMon_inpage, &Ic_Comm_TxBuf[0], ee_Pre7DayMon_lenth );
	 
		for(i=0;i<7;i++)
	  {
		  Add_Bcd( buf,&Ic_Comm_TxBuf[i*6+2], 4 );		
	  }
	  Bcd2Hex( buf, Tempa.B08, 4 );
		
	  Tempc = Tempa.B32/7;
		Bcd2Hex(  &Money.Remain[2],Tempb.B08, 4 ); 
		
		if(Tempc)
		Tempc= Tempb.B32*10/Tempc;
		else Tempc = 0x12c;
		
		Tempb.B32 = Tempc;
		
		Hex2Bcd( Tempb.B08, Money_UserINFO.Day_Moneyviable.B08, 4 ); 
	}
	else
	Money_UserINFO.Day_Moneyviable.B32 = 0;	
	
}
void Lcd_Display( void )
{
	uint08 Disp_cnt,i,j, Temp08,flash;
	uint08 Ee_Buf[80];
	uint08 Token_Buf[16];

	
	memset( DispBuf, 0x00, 160 );	//??????
	memset( Token_Buf, 0x00, 16 );	//??????
	
	Disp_cnt = 0;
	display_rtc_flag=0;
	disp_0_flag = 0;

	if(Display.Timer == 0) 
	{		
		Display.Timer = Display_Para.Loop_Time;//????????
		
		if( Display.Status & B0000_0001 )	//????
		{
			Display.Status &= B1111_1110;	//???????
			Display.Number = 0;
		}
		
		Display.Number++;
		if(Display.Number > Display_Para.S_Number) Display.Number = 0;
		
		disp_remain_flag = 0;		//?????????
		ic_display = 0;
	}
	
	if( ic_display && Ic_Err_Reg && (CardCheck || CardCheck1) ) Disp_cnt = 255;//Err-xx:????????
	else
	{	
		//???????
		if( disp_remain_flag ) Disp_cnt = 229;	//????
		else
		{
			if(Display.Number == 0) Display.Number = 1;
			if( Display.Number == 0xfc )
			{
				Disp_cnt = Ext_Disp_Cnt;
			}
			else if( Display.Number == 0xff )
			{
				Disp_cnt = 200; 		//??
			}
			else
			{
				Disp_cnt = Get_Show_Cnt();
			}
		}		
	}

	if( (FLAG_EA==0) && (Stop_Disptimer==0) && (Display.Number != 0xff)) Disp_cnt = 1;	//??????????
	
	switch( Disp_cnt )
		{
//--------------????????
			case 0:
				Display.Number++;
				return;
				
//--------------??
			case 200:
				memset( DispBuf, 0xff, 160 ); 
				break;		
				
//--------------???????
//--------------?1??????
			case 201:
			case 202:
				i = Disp_cnt-201;
				if(i== 0)	disp_S2;//disp_dangqian;
				else	disp_S3;//disp_shang;

				//disp_yong;
				disp_S5;//disp_yue;
				Cal_Comb_Usedl( 0x01, i, Ee_Buf );
						
				Inverse( Ee_Buf+4, 4 );
				if(Ee_Buf[4] & B1000_0000) 
				{
					Ee_Buf[4] &= B0111_1111;
					//disp_neg;
				}	
				Disp_Dl_Dot(Ee_Buf+4);
				//LCD_Num_Data( 9, 1);
				if(i==1)
				LCD_Num_Data( 1, 1);
				break;

//--------------?????				
			case 203:			
				Calculate_Program_Cs( Ee_Buf );
				Lcd_Show_N( 1, 4, Ee_Buf,0);		//NNNNNNNN
				break;
				
//--------------??
			case 204:
			  DispBuf[18]|= 0x00000000+((0x7d&0xf0)<<20);//显示A
		    DispBuf[19]|= 0x00000000+(0x7d&0x0f);
				disp_S37;//disp_v;
				//Lcd_Show_N( 0x11, 4, Emu_Data.U.B08);
				Emu_Data.U.B08[0] = (Real.U[0]&0xff);
				Emu_Data.U.B08[1] = ((Real.U[0]>>8)&0xff);
				Emu_Data.U.B08[2] = ((Real.U[0]>>16)&0xff);
				Emu_Data.U.B08[3] = ((Real.U[0]>>24)&0xff);
				Exchange( Ee_Buf,Emu_Data.U.B08, 4 );
				Lcd_Show_N( 0x11, 4,Ee_Buf ,0);				
				//??????,????????? yang 20180903
				disp_S44;//disp_a;
				Emu_Data.I1.B08[0] = ((Real.I[0])&0xff);
				Emu_Data.I1.B08[1] = ((Real.I[0]>>8)&0xff);
				Emu_Data.I1.B08[2] = ((Real.I[0]>>16)&0xff);
				Emu_Data.I1.B08[3] = ((Real.I[0]>>24)&0xff);
				Exchange( Ee_Buf,Emu_Data.I1.B08, 4 );
				Lcd_Show_N( 0x31, 4,Ee_Buf ,1);				
				break;

//--------------??
			case 205:
				DispBuf[18]|= 0x00000000+((0x37&0xf0)<<20);//显示B
		    DispBuf[19]|= 0x00000000+(0x37&0x0f);
				disp_S37;//disp_v;
				//Lcd_Show_N( 0x11, 4, Emu_Data.U.B08);
				Emu_Data.U.B08[0] = (Real.U[1]&0xff);
				Emu_Data.U.B08[1] = ((Real.U[1]>>8)&0xff);
				Emu_Data.U.B08[2] = ((Real.U[1]>>16)&0xff);
				Emu_Data.U.B08[3] = ((Real.U[1]>>24)&0xff);
				Exchange( Ee_Buf,Emu_Data.U.B08, 4 );
				Lcd_Show_N( 0x11, 4,Ee_Buf ,0);				
				//??????,????????? yang 20180903
				disp_S44;//disp_a;
				Emu_Data.I1.B08[0] = ((Real.I[1])&0xff);
				Emu_Data.I1.B08[1] = ((Real.I[1]>>8)&0xff);
				Emu_Data.I1.B08[2] = ((Real.I[1]>>16)&0xff);
				Emu_Data.I1.B08[3] = ((Real.I[1]>>24)&0xff);
				Exchange( Ee_Buf,Emu_Data.I1.B08, 4 );
				Lcd_Show_N( 0x31, 4,Ee_Buf ,1);							
				break;

//--------------??
			case 206:
				DispBuf[18]|= 0x00000000+((0x0f&0xf0)<<20);//显示C
		    DispBuf[19]|= 0x00000000+(0x0f&0x0f);
				disp_S37;//disp_v;
				//Lcd_Show_N( 0x11, 4, Emu_Data.U.B08);
				Emu_Data.U.B08[0] = (Real.U[2]&0xff);
				Emu_Data.U.B08[1] = ((Real.U[2]>>8)&0xff);
				Emu_Data.U.B08[2] = ((Real.U[2]>>16)&0xff);
				Emu_Data.U.B08[3] = ((Real.U[2]>>24)&0xff);
				Exchange( Ee_Buf,Emu_Data.U.B08, 4 );
				Lcd_Show_N( 0x11, 4,Ee_Buf ,0);				
				//??????,????????? yang 20180903
				disp_S44;//disp_a;
				Emu_Data.I1.B08[0] = ((Real.I[2])&0xff);
				Emu_Data.I1.B08[1] = ((Real.I[2]>>8)&0xff);
				Emu_Data.I1.B08[2] = ((Real.I[2]>>16)&0xff);
				Emu_Data.I1.B08[3] = ((Real.I[2]>>24)&0xff);
				Exchange( Ee_Buf,Emu_Data.I1.B08, 4 );
				Lcd_Show_N( 0x31, 4,Ee_Buf ,1);				
				break;
			case 207:
				DispBuf[18]|= 0x00000000+((0x7d&0xf0)<<20);//显示A
		    DispBuf[19]|= 0x00000000+(0x7d&0x0f);
				disp_S40;//disp_kw;	
				Emu_Data.P1.B08[0] = ((Real.PKw[1])&0xff);
				Emu_Data.P1.B08[1] = ((Real.PKw[1]>>8)&0xff);
				Emu_Data.P1.B08[2] = ((Real.PKw[1]>>16)&0xff);
				Emu_Data.P1.B08[3] = ((Real.PKw[1]>>24)&0xff);
			  Exchange( Ee_Buf,Emu_Data.P1.B08, 4 );
				Disp_Pw_Dot(Ee_Buf);
				disp_S36;//disp_cos;
				Emu_Data.Cosa.B08[0] = ((Real.PF[1])&0xff);
				Emu_Data.Cosa.B08[1] = ((Real.PF[1]>>8)&0xff);
				Emu_Data.Cosa.B08[2] = ((Real.PF[1]>>16)&0xff);
				Emu_Data.Cosa.B08[3] = ((Real.PF[1]>>24)&0xff);
			  Exchange( Ee_Buf,Emu_Data.Cosa.B08, 4 );
				Lcd_Show_N( 0x31, 4, Ee_Buf,0);				
				break;	
			case 208:
				DispBuf[18]|= 0x00000000+((0x37&0xf0)<<20);//显示B
		    DispBuf[19]|= 0x00000000+(0x37&0x0f);
				disp_S40;//disp_kw;	
				Emu_Data.P1.B08[0] = ((Real.PKw[2])&0xff);
				Emu_Data.P1.B08[1] = ((Real.PKw[2]>>8)&0xff);
				Emu_Data.P1.B08[2] = ((Real.PKw[2]>>16)&0xff);
				Emu_Data.P1.B08[3] = ((Real.PKw[2]>>24)&0xff);
			  Exchange( Ee_Buf,Emu_Data.P1.B08, 4 );
				Disp_Pw_Dot(Ee_Buf);
				disp_S36;//disp_cos;
				Emu_Data.Cosa.B08[0] = ((Real.PF[2])&0xff);
				Emu_Data.Cosa.B08[1] = ((Real.PF[2]>>8)&0xff);
				Emu_Data.Cosa.B08[2] = ((Real.PF[2]>>16)&0xff);
				Emu_Data.Cosa.B08[3] = ((Real.PF[2]>>24)&0xff);
			  Exchange( Ee_Buf,Emu_Data.Cosa.B08, 4 );
				Lcd_Show_N( 0x31, 4, Ee_Buf,0);				
				break;	
			case 209:
//				disp_S39;
				DispBuf[18]|= 0x00000000+((0x0f&0xf0)<<20);//显示C
		    DispBuf[19]|= 0x00000000+(0x0f&0x0f);
				disp_S40;//disp_kw;	
				Emu_Data.P1.B08[0] = ((Real.PKw[3])&0xff);
				Emu_Data.P1.B08[1] = ((Real.PKw[3]>>8)&0xff);
				Emu_Data.P1.B08[2] = ((Real.PKw[3]>>16)&0xff);
				Emu_Data.P1.B08[3] = ((Real.PKw[3]>>24)&0xff);
			  Exchange( Ee_Buf,Emu_Data.P1.B08, 4 );
				Disp_Pw_Dot(Ee_Buf);
				disp_S36;//disp_cos;
				Emu_Data.Cosa.B08[0] = ((Real.PF[3])&0xff);
				Emu_Data.Cosa.B08[1] = ((Real.PF[3]>>8)&0xff);
				Emu_Data.Cosa.B08[2] = ((Real.PF[3]>>16)&0xff);
				Emu_Data.Cosa.B08[3] = ((Real.PF[3]>>24)&0xff);
			  Exchange( Ee_Buf,Emu_Data.Cosa.B08, 4 );
				Lcd_Show_N( 0x31, 4, Ee_Buf,0);				
				break;	

//--------------????
//			
//				disp_S36;//disp_cos;
//			  Exchange( Ee_Buf,Emu_Data.Cosa.B08, 4 );
//				Lcd_Show_N( 0x31, 4, Emu_Data.Cosa.B08,0);				
//				break;

//--------------????
			case 210:
				//disp_N;
			  disp_S37;//disp_v;
				//Lcd_Show_N( 0x11, 4, Emu_Data.U.B08);	
			  Exchange( Ee_Buf,Emu_Data.U.B08, 4 );
				Lcd_Show_N( 0x11, 4,Ee_Buf ,0);		
				disp_S44;//disp_a;
			  Exchange( Ee_Buf,Emu_Data.I2.B08, 4 );
				Lcd_Show_N( 0x31, 4,Ee_Buf,1);				
				break;

//--------------??
			case 211:
				disp_S14;//disp_const;
			  Exchange( Ee_Buf,Meter.Const.B08, 2 );				
				disp_0_flag = 1;
				Lcd_Show_N( 5, 2, Ee_Buf,0);	
				break;
				
//--------------?????
			case 212:
			case 213:
			case 214:
				disp_dp9;//disp_dot2;
				I2C_Read_Eeprom(ee_histroyparameter_page, ee_histroyparameter_inpage+(Disp_cnt-212)*2, &Ee_Buf[1], 2);

				Ee_Buf[0] = Ee_Buf[2];
				Lcd_Show_N( 5, 2, Ee_Buf,0);
				break;					
				
//--------------???4?
			case 216:
			case 215:
				//disp_yong;
				//disp_hu;
				//disp_hao;
				Lcd_Show_N( 5, 2, Esam_Para.User_Id,0);				
			
				
//--------------???8?
			
				//disp_yong;
				//disp_hu;
				//disp_hao;
				Lcd_Show_N( 1, 4, Esam_Para.User_Id+2,1);	
				break;
				
//--------------???4?
			case 218:
			case 217:
				disp_S13;
				//disp_biao;
				//disp_hao;
				Lcd_Show_N( 5, 2, Esam_Para.Meter_Id,0);				
			
//--------------???8?
			
				disp_S13;
				//disp_biao;
				//disp_hao;
				Lcd_Show_N( 1, 4, Esam_Para.Meter_Id+2,1);	
				break;
				
//--------------?????4?
			case 220:
			case 219:
//				disp_biao;
//				disp_hao;
				Lcd_Show_N( 5, 2, Comm_Addr.Comm_Id,0);				
			
				
//--------------?????8?
			
//				disp_biao;
//				disp_hao;
				Lcd_Show_N( 1, 4, Comm_Addr.Comm_Id+2,1);	
				break;					

//--------------??
//--------------??
			case 222:
				//display_rtc_flag = 1;
			case 221:			
				display_rtc_flag = 1;//???????? yang20180903
				//i = Disp_cnt - 221;
				//Disp_Time_Dot(i);
				Disp_Time_Dot(0);
				Disp_Time_Dot(1);
				disp_S2;//disp_dangqian;
				disp_S18;//disp_shi;
				//disp_tjian;
				Read_Rtc(Ee_Buf);
				Inverse( Ee_Buf, 7 );
				//Lcd_Show_N( 3, 3, Ee_Buf+i*4);	
				//???? yang 20180903
				Lcd_Show_N( 3, 3, Ee_Buf,0);	
				Lcd_Show_N( 3, 3, Ee_Buf+4,1);	
				break;

////--------------??

//			case 223:
				
				
//			break;	
//			
//			
////--------------??
//			case 224:
//			
//				break;				
								
//--------------????????
//--------------????????				
			case 225:			
			case 226:
				i = Disp_cnt-225;
				Disp_Time_Dot(i);
				disp_S18;//disp_shi;
				//disp_tjian;
				if( RdRecTimes(ee_clearn_inpage) )
				{
					Temp08 = Read_Event_Point( ee_clearn_inpage );
					I2C_Read_Eeprom( ee_cleardl_note_page+Temp08, ee_cleardlnote_inpage+3*(1-i), Ee_Buf, 3 );
				}
				else memset(Ee_Buf, 0x00, 3);
				Inverse(Ee_Buf,3);//?????? 				
				Lcd_Show_N( 3, 3, Ee_Buf,i );
				break;
											
//--------------??????????
//--------------?1 ????????
			case 227:			
			case 228:
				i = Disp_cnt-227;
				if( i == 0)	disp_S2;//disp_dangqian;
				else	disp_S3;//disp_shang;
//				disp_zong;
				//disp_yong;
				Cal_Comb_Usedl( 0x02, i, Ee_Buf );
			
				Inverse( Ee_Buf+4, 4 );
				if(Ee_Buf[4] & B1000_0000) 
				{
					Ee_Buf[4] &= B0111_1111;
					//disp_neg;
				}	
				Disp_Dl_Dot(Ee_Buf+4);
				//LCD_Num_Data( 9, 1);
				LCD_Num_Data( 1, 1);
				break;	
		
//--------------????
			case 229:
				Lcd_Disp_Money();
				if( disp_remain_flag && (Display.Timer > (Display_Para.Loop_Time)))
				{
					if( disp_over_flag )
					{
						disp_S15;
						//disp_she;	//??
						//disp_qian;
						Temp08 = Lcd_Show_N( 0x21, 4, Remain_back+20,0);//????
						if( Temp08 ) LCD_Num_Data( Temp08, 0x0b );//??????"-"
						//else disp_neg;
					}
					else
					{	
						
						Lcd_Show_N( 0x21, 4, Remain_back+16,0);//????
					}
				}
				else
				{
					if( remainder_tick_flag )
					{
						disp_S15;
						//disp_she;	//??
						//disp_qian;
						Exchange( Ee_Buf,Money.Tick+2, 4 );
						Temp08 = Lcd_Show_N( 0x21, 4, Ee_Buf,0);//????
						if( Temp08 ) LCD_Num_Data( Temp08, 0x0b );//??????"-"
						//else disp_neg;
					}
					else
					{	
						Exchange( Ee_Buf,Money.Remain+2, 4 );
						Lcd_Show_N( 0x21, 4, Ee_Buf,0);//????
					}
				}
				//??????? yang 20180903
				disp_S2;//disp_dangqian;
				//disp_S5;//disp_yue;
				Cal_Comb_Usedl( 0x01, 0, Ee_Buf );
						
				Inverse( Ee_Buf+4, 4 );
				if(Ee_Buf[4] & B1000_0000) 
				{
					Ee_Buf[4] &= B0111_1111;
				}	
				Disp_Dl_Dot(Ee_Buf+4);
				break;

//--------------??????
//--------------??????
//--------------????
			case 230:
				//disp_jieti;
			case 231:
			case 232:
				i = Disp_cnt-230;
				if( Disp_cnt == 231 ) 
				{					
					Lcd_Disp_Zedl(0, Tariff.Current+1);//?????????
				}
				disp_S17;
				disp_S34;
				Exchange( Ee_Buf,Price.Current_L.B08+4*i, 4 );
				Lcd_Show_N( 0x41, 4, Ee_Buf,0);	
				//??1.00KWH  ????? ??
				disp_S39;
				disp_S40;
				disp_S41;
				memset( Ee_Buf,0, 4 );
				Ee_Buf[0]=1;
				Lcd_Show_N( 5, 2, Ee_Buf,1);
				disp_dp9;
				break;
				
//--------------????????
			case 234:
				memset( (unsigned char xdata *)(0x2093), 0x00, 42 );	//????
 				LCDTranslate64();
 				
 				run_display_flag = 0;
				return;							

//--------------???/????
			case 235:
			case 236:
				if( Disp_cnt == 235 ) Temp08 = 0x55;	//??????4?
				else Temp08 = 0xaa;//??????4?

				memset( (unsigned char xdata *)(0x2093), Temp08, 42 );	//????
			 	LCDTranslate64();
			
			 	run_display_flag = 0;
				return;	

			//--------------?5???TOKEN??
			//			???I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, &Ic_Comm_TxBuf[10], ee_TOKEN_lenth );
			//			??????10??bcd   ??????BCD??20??   ??12  ?? 01 02
			case 237:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf,10);
				//?1?TOKEN?4??
				Lcd_Show_N( 1, 4, Token_Buf,0 );
				Lcd_Show_N( 1, 4, Token_Buf+4,1 );
				disp_S3;
				LCD_Num_Data(1,1);
				break;
			case 238:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf,10);
				//?1?TOKEN?16??
				Lcd_Show_N( 1, 4, Token_Buf+8,0 );
				Lcd_Show_N( 1, 4, Token_Buf+12,1 );
				disp_S3;
				LCD_Num_Data(1,1);
				break;
			case 239:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf+10,10);
				//?2?TOKEN?4??
				Lcd_Show_N( 1, 4, Token_Buf,0 );
				Lcd_Show_N( 1, 4, Token_Buf+4,1 );
				disp_S3;
				LCD_Num_Data(1,2);
				break;

			case 240:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf+10,10);
				//?2?TOKEN?16??
				Lcd_Show_N( 1, 4, Token_Buf+8,0 );
				Lcd_Show_N( 1, 4, Token_Buf+12,1 );
				disp_S3;
				LCD_Num_Data(1,2);
				break;
			case 241:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf+20,10);
				//?3?TOKEN?4??
				Lcd_Show_N( 1, 4, Token_Buf,0 );
				Lcd_Show_N( 1, 4, Token_Buf+4,1 );
				disp_S3;
				LCD_Num_Data(1,3);
				break;

			case 242:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf+20,10);
				//?3?TOKEN?16??
				Lcd_Show_N( 1, 4, Token_Buf+8,0 );
				Lcd_Show_N( 1, 4, Token_Buf+12,1 );
				disp_S3;
				LCD_Num_Data(1,3);
				break;
			case 243:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf+30,10);
				//?4?TOKEN?4??
				Lcd_Show_N( 1, 4, Token_Buf,0 );
				Lcd_Show_N( 1, 4, Token_Buf+4,1 );
				disp_S3;
				LCD_Num_Data(1,4);
				break;

			case 244:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf+30,10);
				//?4?TOKEN?16??
				Lcd_Show_N( 1, 4, Token_Buf+8,0 );
				Lcd_Show_N( 1, 4, Token_Buf+12,1 );
				disp_S3;
				LCD_Num_Data(1,4);
				break;
			case 245:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf+40,10);
				//?5?TOKEN?4??
				Lcd_Show_N( 1, 4, Token_Buf,0 );
				Lcd_Show_N( 1, 4, Token_Buf+4,1 );
				disp_S3;
				LCD_Num_Data(1,5);
				break;

			case 246:
				disp_S38;
				I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ee_Buf, ee_TOKEN_lenth );
				memcpy(&Token_Buf[6],Ee_Buf+40,10);
				//?4?TOKEN?16??
				Lcd_Show_N( 1, 4, Token_Buf+8,0 );
				Lcd_Show_N( 1, 4, Token_Buf+12,1 );
				disp_S3;
				LCD_Num_Data(1,5);
				break;
			
 //--------------?1?????			
			case 247:
				disp_S3;
				disp_S6;
				disp_S34;
				//?1???????? ??? Money_UserINFO		
				//Exchange( Ee_Buf,Money_UserINFO.Day_Moneyused.B08, 4 );
				I2C_Read_Eeprom( ee_Pre7DayMon_page, ee_Pre7DayMon_inpage, Ee_Buf, ee_Pre7DayMon_lenth );
				// CalculateUseDay(); 
				Inverse( Ee_Buf+2, 4);	
				Lcd_Show_N( 0x21, 4,Ee_Buf+2 ,0);	
				LCD_Num_Data(1,1);
				break;
//			
 //-------------??????
			case 248:
				//???Buy_Total_Money;  4??BCD
				disp_S34;
				Exchange( Ee_Buf,Buy_Total_Money.B08, 4 );
				Lcd_Show_N( 0x21, 4,Ee_Buf ,0); 
				break;
//			
//-------------????+????
			case 249:
				//	???????????? ??0.01?
				//	??????????,??0.1? 
				//	??????? ??? Money_UserINFO					
				Lcd_Disp_Money();
				if( disp_remain_flag && (Display.Timer > (Display_Para.Loop_Time)))
				{
					if( disp_over_flag )
					{
						disp_S15;
						//disp_she;	//??
						//disp_qian;
						Temp08 = Lcd_Show_N( 0x21, 4, Remain_back+20,0);//????
						if( Temp08 ) LCD_Num_Data( Temp08, 0x0b );//??????"-"
						//else disp_neg;
					}
					else
					{	
						
						Lcd_Show_N( 0x21, 4, Remain_back+16,0);//????
					}
				}
				else
				{
					if( remainder_tick_flag )
					{
						disp_S15;
						//disp_she;	//??
						//disp_qian;
						Exchange( Ee_Buf,Money.Tick+2, 4 );
						Temp08 = Lcd_Show_N( 0x21, 4, Ee_Buf,0);//????
						if( Temp08 ) LCD_Num_Data( Temp08, 0x0b );//??????"-"
						//else disp_neg;
					}
					else
					{	
						Exchange( Ee_Buf,Money.Remain+2, 4 );
						Lcd_Show_N( 0x21, 4, Ee_Buf,0);//????
					}
				}
				//????????
				disp_S45;
				CalculateUseDay(); 
				Exchange( Ee_Buf,Money_UserINFO.Day_Moneyviable.B08, 4 );
				Lcd_Show_N( 0x11, 4,Ee_Buf ,1); 
				break;
//-------------?5??????
//?1?????
//		???I2C_Read_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, &Ic_Comm_TxBuf[10], ee_Pre12monthMon_lenth );
//    ?48?? 12?  4??????,	Ic_Comm_TxBuf[0]???1????		
			case 250:
				I2C_Read_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, Ee_Buf, ee_Pre12monthMon_lenth );
				Inverse( Ee_Buf+2, 4);	
				disp_S34;
				Lcd_Show_N( 0x21, 4,Ee_Buf+2 ,0); 
				disp_S3;
				disp_S5;
				LCD_Num_Data(1,1);
				break;
			case 251:
				I2C_Read_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, Ee_Buf, ee_Pre12monthMon_lenth );
				disp_S34;
			  Inverse( Ee_Buf+8, 4);	
				Lcd_Show_N( 0x21, 4,Ee_Buf+8 ,0); 
				disp_S3;
				disp_S5;
				LCD_Num_Data(1,2);
				break;
			case 252:
				I2C_Read_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, Ee_Buf, ee_Pre12monthMon_lenth );
				Inverse( Ee_Buf+14, 4);	
				disp_S34;
				Lcd_Show_N( 0x21, 4,Ee_Buf+14 ,0); 
				disp_S3;
				disp_S5;
				LCD_Num_Data(1,3);
				break;
			case 253:
				I2C_Read_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, Ee_Buf, ee_Pre12monthMon_lenth );
				Inverse( Ee_Buf+20, 4);	
				disp_S34;
				Lcd_Show_N( 0x21, 4,Ee_Buf+20 ,0); 
				disp_S3;
				disp_S5;
				LCD_Num_Data(1,4);
				break;
			case 254:
				I2C_Read_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, Ee_Buf, ee_Pre12monthMon_lenth );
				Inverse( Ee_Buf+26, 4);	
				disp_S34;
				Lcd_Show_N( 0x21, 4,Ee_Buf+26 ,0); 
				disp_S3;
				disp_S5;
				LCD_Num_Data(1,5);
				break;

//--------------??????(?????)
			case 255:
				Temp08 = Hex_To_Bcd(Ic_Err_Reg);
				Lcd_Show_Err(Temp08);		//??ERR-xx
				if(Temp08 == 0x36) disp_S46;//disp_max;	//??
				Display.Number = 0;
				break;

//--------------????12???,??,????
			default:
				if( Disp_cnt <= 195 )	//1~65,66~130,131~195
				{
					Temp08 = (Disp_cnt-1)/65;	//??,??,??
					i = (Disp_cnt-1-65*Temp08)/5;	//0:??; 1~12??
					j = (Disp_cnt-1)%5;		//0,1~4??
					Lcd_Disp_Zedl(i, j);

					if( (FLAG_EA == 0) && (Temp08 == 0) && ((i == 1)||(i == 2)) ) //???????????(????????)
					{
						Exchange( Ee_Buf, Ic_Comm_TxBuf+(i-1)*20+j*4, 4);//???????
					}
					else
					{	
						Cal_Comb( Temp08, j, i, Ee_Buf );	//??????
						Inverse( Ee_Buf, 4 );			//???????
					}
					
					if( (Ee_Buf[0]&B1000_0000) && (Temp08 == 0) )
					{
						Ee_Buf[0] &= B0111_1111;
						//disp_neg;
					}
					if( Temp08 == 1 )
					{
						//disp_zheng;
						//disp_xiang;
					}
					if ( Temp08 == 2 )
					{
						//disp_fan;
						//disp_xiang;
					}
					Disp_Dl_Dot(Ee_Buf);
				}
				else
				{
					Display.Number++;
					return;
				}
				
				break;
		}
	
	if( FLAG_EA )
	{
		//????
		switch( Tariff.Current&3 )
		{
			
			case 0:	disp_S23;	break;//disp_jianf;
			case 1: disp_S24;	break;//disp_fengf;
			case 2:	disp_S25;	break;//disp_pingf;
			case 3: disp_S26;	break;//disp_guf;
			default:		break;
		}				
		
		flash = 0;
		if(Sclock.Clockmain[0]%2) flash = 1;
		if( relay_commu_alarm_flag && flash ) disp_S19;//disp_pbuy; //???(????)
		
		if( BDMODE ) 
		{
//			if(Tariff.Current_Ladder !=0xff)
//			{
//				//????
//				switch( Tariff.Current_Ladder )
//				{
//					case 0:	disp_S23;	break;//disp_jt1
//					case 1:	disp_S24;	break;//disp_jt2
//					case 2:	disp_S25;	break;//disp_jt3
//					case 3:	disp_S26;	break;//disp_jt4
//					case 4:	disp_S23;	disp_S26;	break;//disp_jt1;	disp_jt4;
//					case 5:	disp_S24;	disp_S26;	break;//disp_jt2;	disp_jt4;
//					case 6:	disp_S25;	disp_S26;	break;//disp_jt3;	disp_jt4;
//					default: break;
//				}
//				disp_S23;
//				//disp_ladder1;
//			}

			//??????
	//		if(Tariff.Status & B0000_0001) disp_S24;//disp_ladder2;	//?2????
			
			if( remainder_tick_flag ) 
			{
				if( remainder_maxtick_flag && (relay_commu_close_flag==0) ) disp_S15;//disp_over;		//??(????)
				else
				{
					if(flash) disp_S15;//disp_over;	//??(????)(????)
				}
			}
			
			if(remainder_alarm1_flag || remainder_alarm2_flag || remainder_zero_flag || charge_relay_open_flag) 
			{
				if(flash) disp_S19;//disp_pbuy; //???(????)
			}
		}
	/*
		//?????????????????
		if( (BDMODE) && (Mode.Feature1&B0000_0010) )
		{
			if(Tariff.Status & B0000_0010) 
			{
				disp_S22;//disp_shiduan2;	//?2?????
				disp_S24;
			}
			disp_S22;//disp_shiduan1;
			disp_S23;
		}
		else
		{
			if(Tariff.Status & B0000_0100) //disp_shiduan2;	//?2????
			{
				disp_S22;//disp_shiduan2;	//?2?????
				disp_S24;
			}
			else //disp_shiduan1;
			{
				disp_S22;//disp_shiduan1;
				disp_S23;
			}
		}
	*/
		if( disp_readic_flag)	//?????
		{
			disp_S30;
			//disp_readic;	//??
			//disp_zhong;		//?
		}
		if( ic_display ) 
		{
			disp_S30;//disp_readic;
			//if( Ic_Err_Reg ) disp_err;		//????
			//else	disp_ok;			//????
		}
		
		//????
		if( dl_back_flag ) disp_S31;//disp_fanxiang;		//??
		
		if( low_voltage_flag || Bat_Delay_Timer ) disp_S32;//disp_baterry;		//??
		
		if( progenable ) 
		{
			 if( flash ) 
			 	disp_S36;//disp_prog; 		//??????, ???????
		}
		else if( IR_EnTimer ) 
			disp_S30;//disp_prog;		//??
		
		if( relay_status_flag && (relay_commu_disable_close_flag ==0) && (relay_commu_open_flag || (BDMODE&&( charge_relay_open_flag || remainder_maxtick_flag))) ) 
		{
			  disp_S27;	
        disp_S29;//??
		}
		if( Opendelaytimer.B32 || relay_delay_flag ) 
		{
			if(Sclock.Clockmain[0]%2)
			{
				disp_S27;	
        disp_S29;					//??????,???????
			}
		}

		if( Successled.Timer ) 
		{
			if(display_zaibo_flag)
				disp_S1;//disp_zbcomm;	//????
			else 
				disp_S30;//disp_comm;	//??
		}
		
		
//		if( Fun_Para.Temper_M.B16 > 0x8180 )
//		{
//			if( Disp_cnt == 200 ) LCDBIAS   = Fulllcdbias2;	//??????
//			else LCDBIAS   = Runlcdbias2;	
//		}
//		else
//		{
//			if( Disp_cnt == 200 ) LCDBIAS   = Fulllcdbias;	//??????
//			else LCDBIAS   = Runlcdbias;	
//		}	
	}
//	else
//	{
//		if( (Disp_cnt != 205) && (Disp_cnt != 210) )	//????LN(??????????)
//		{
//			disp_L;
//			disp_N;
//		}
//	}
	
	//if( Key_Status != 1 ) disp_key;				//????(??)
	
	if( (Password_Err[0]>=Defpassworderrcnt) || (Password_Err[1]>=Defpassworderrcnt) || hangup_comm_flag )
	{
		//disp_lock;					//?????
 	}
	/*
	////debug yang 20180830///////	
	memset( DispBuf, 0x00000000, 160 );	//??????
	LCD_Num_Data( 1, 1);
	LCD_Num_Data( 2, 2);
	LCD_Num_Data( 3, 3);
	LCD_Num_Data( 4, 4);
	LCD_Num_Data( 5, 5);
	LCD_Num_Data( 6, 6);
	LCD_Num_Data( 7, 7);
	LCD_Num_Data( 8, 8);
	LCD_Num_Data( 9, 9);
	LCD_Num_Data( 10, 1);
	LCD_Num_Data( 11, 2);
	LCD_Num_Data( 12, 3);
	LCD_Num_Data( 13, 4);
	LCD_Num_Data( 14, 5);
	LCD_Num_Data( 15, 6);
	LCD_Num_Data( 16, 7);
	LCD_Num_Data( 17, 8);
	*/
	if( FLAG_EA)
	{
		if( Real.I[0] )disp_Ia;
	  if( Real.I[1] )disp_Ib;
	  if( Real.I[2] )disp_Ic;
	
	  if( Real.I[0]&0x800000 )disp_negative_Ia;
	  if( Real.I[1]&0x800000 )disp_negative_Ib;
	  if( Real.I[2]&0x800000 )disp_negative_Ic;
	
	  if( Real.U[0]>=MeasPara.Un60 )disp_Ua;
	  if( Real.U[1]>=MeasPara.Un60 )disp_Ub;
	  if( Real.U[2]>=MeasPara.Un60 )disp_Uc;
	  if( Real.PKw[0]&0x800000 )disp_S31;
	}
//	if( Flag.EvenRevP & 0x01 )	
//	if( Flag.EvenRevP & 0x02 )
//	if( Flag.EvenRevP & 0x04 )
//	if( Flag.EvenRevP & 0x07 ) 
	
 	memcpy( (unsigned char xdata *)(0x40002000), DispBuf, 160 );	//????
 	//memcpy( (unsigned char xdata *)(0x2093), DispBuf, 42 );	//????
 	LCDTranslate64();

 	run_display_flag = 0;
}


