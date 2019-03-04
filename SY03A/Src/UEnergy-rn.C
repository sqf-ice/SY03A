#include "ex_func.h"
#include "ex_var.h"
//#include <intrins.h>
#include <string.h>
#include <absacc.h>
#include <stdio.h>
#include <math.h>

#define	chk_reg_num	25			//??????

/*********************************************/
#define	SPIWrite		0x02
#define	SPIRead			0x01

#define	IARMS				0x00000022
#define	IBRMS				0x00000023
#define	URMS				0x00000024
#define	Freq_U			0x00000025
#define	PowerPA			0x00000026
#define	EmuStatus		0x0000002D
//********************************************************/

const union B32_B08_2 code Dl_Cpu_Init[chk_reg_num-1]=
{
  0x00001a00,//0x0002,
  0x00000060,//0x0003,
	0x00000000,//0x0005,    
	0x00000000,//0x0007,
	0x00000000,//0x000A,
  0x00000000,//0x000E,        
	0x00000000,//0x000F,        
	0x00000000,//0x0017,     
	
	0x00000120,//0x0004,       
	0x00000000,//0x0006,        
	0x00000000,//0x0008,        
	0x00000000,//0x0009,        
	0x00000000,//0x000B,        
	0x00000000,//0x000C,        
	0x00000000,//0x000D,        
	0x00000000,//0x0010,       
	0x00000000,//0x0011,        
	0x00000000,//0x0012, 
	0x00000000,//0x0013,
	0x00000000,//0x0014,
	0x00000000,//0x0015,        
	0x00000000,//0x0016,        
	
	0x00001663,//0x0000,        
  0x00000003,//0x0001, 
};

const union B32_B08_2 code ATT7051Addr[chk_reg_num-1]=
{       
  0x00000002, 0x00000003, 0x00000005, 0x00000007, 0x0000000A, 
  0x0000000E, 0x0000000F, 0x00000017, 0x00000004, 0x00000006, 
	0x00000008, 0x00000009, 0x0000000B, 0x0000000C, 0x0000000D, 
	0x00000010, 0x00000011, 0x00000012, 0x00000013, 0x00000014, 
	0x00000015, 0x00000016, 0x00000000, 0x00000001,
};
/********************************************/
//  float		Pav_Temp;	
//	unsigned char	Pav_Cnt;
//	uint08		Back_Num;	
// 	uint08	 	EnergyErrCnt0;	

struct B08ADDB32
{
	union B32_B08     E16;	
	union B32_B08     E32;
} SPI_OpData,SPI_OpDatatemp,SPI_OpData1;

void ATT7051SPIOpDelay(void)
{
	unsigned char  i;
	for(i = 0; i < 12; i++);
}
void ATT7051SPIOpDelay1(void)
{
	unsigned char  i;
	for(i = 0; i < 5; i++);
}

void Delay_20ms( void )//??100ms
{
	unsigned int Temp16;

	for( Temp16=0; Temp16<0x300; Temp16++ )//??100ms
	{
		Clear_Wdt();
	}
}
/********************************************************/

unsigned char SPIReadByte(uint08 *Buf,uint08 Lenth)		//???????
{
	unsigned char i; 
  unsigned int  Temp08;
//	UART4->INTSTS = UART_INTSTS_RXIF;
//	Clear_Wdt();
	UART4->STATE = 0x0000003f;		
	for(i = 0; i < Lenth; i++)
	{
		for(Temp08 = 0; Temp08 < 6000; Temp08++)
	  {
		    if((UART4->STATE)&UART_INTSTS_RXIF)
		    {
		         UART4->STATE = UART_INTSTS_RXIF; 	
	           *(Buf+i)=(UART4->DATA);
		   
			        break;   
		    }	
		    
		  	 
		if( FLAG_EA == 0 ) return 1;
		}
	        
		if(Temp08==6000)
	  {
		    UART4->STATE = 0x0000003f;		
		    return 1;
		}
}
	return 0;
}


				
unsigned char SPIWriteByte(uint08 *Buf,uint08 Lenth)	//???????
{
	unsigned int  j;
  unsigned char i;
	
//	UART4->INTSTS = UART_INTSTS_TXDONE;
	UART4->STATE = 0x0000003f;	
	UART4->DATA =*Buf;	
	
	for(i = 1; i < (Lenth+1); i++)
	{
		for(j = 0; j < 6000; j++)
	  {
		    if((UART4->STATE)&UART_INTSTS_TXDONE)
		    {
		       if(i==Lenth) break;
		       
		       UART4->STATE = UART_INTSTS_TXDONE;
		    
		       UART4->DATA=*(Buf+i);	
	                
		        break;   
		    }
		   if( FLAG_EA == 0 ) return 1;
		}
		if(j==6000)
		{
		    UART4->STATE = 0x0000003f;		
		    return 1;
		}
	 
	}
	return 0;			
}

void Tx_Head( unsigned char OpMode ,unsigned char *OpData ,unsigned char Lenth)
{       
        unsigned char r;
        
	memset( MterTX_Buff , 0x00, 8 );
	
	MterTX_Buff[0] =  *(OpData + 0);
	
	
	if(OpMode == SPIWrite)
	{
      for(r = 0; r < Lenth ; r++)
	    MterTX_Buff[r+1] = *(OpData + Lenth+3-r);
        
	    for(r = 0; r < (Lenth+1); r++)
	    MterTX_Buff[Lenth+1] += MterTX_Buff[r];
	
	    MterTX_Buff[Lenth+1] = ~MterTX_Buff[Lenth+1]; 
  }
}

unsigned char EmuRegOp( unsigned char   OpMode   ,unsigned char  Len  )	//???????
{
	unsigned char i,CRC,temp,temp1;
	 
  CRC = 0;
	temp = MterTX_Buff[0];
	if(SPIWriteByte(MterTX_Buff,1)) return 1;
	
	if(OpMode == SPIRead)
	{
	       if(SPIReadByte(&MterRX_Buff[1],(Len+1))) return 1 ;
	       MterRX_Buff[0] = temp;
	       for(i = 0; i < Len+1 ; i++)
	       {
	             CRC+= MterRX_Buff[i];
	       }
	       temp= (~CRC);
				 temp1= MterRX_Buff[Len+1] ;
//	       if( MterRX_Buff[Len+1] !=  (~CRC)) return 1;
				if( temp !=  temp1) return 1; 
        }
        else
	      {
               if(SPIWriteByte(&MterTX_Buff[1],(Len+1))) return 1; 
        }
	       
        return 0;	
}
/********************************************************/
//OpMode:????: Write(0x80); Read(0x00)
//OpData:????????: ?????,?????,????????
/********************************************************/
unsigned char ATT7051SPIOp_rel(	unsigned char OpMode, unsigned char *OpData)
{
	unsigned char i,Lenth;	
	
	if(*OpData <= 0x06) Lenth = 2;
	else if(*OpData <= 0x08) Lenth = 1;
	else if(*OpData <= 0x21) Lenth = 2;
	else if(*OpData <= 0x24) Lenth = 3;	
	else if(*OpData == 0x25) Lenth = 2;
	else if(*OpData <= 0x28) Lenth = 4;
	else if(*OpData <= 0x35) Lenth = 3;
	else if(*OpData <= 0x43) Lenth = 1;
	else if(*OpData == 0x44) Lenth = 4;
	else if(*OpData == 0x45) Lenth = 2;
	else if(*OpData == 0x7f) Lenth = 3;
	else Lenth = 1;

  if(OpMode == SPIWrite)
	{
		*OpData = ((*OpData) )|0x80;	//??????
		
	}
	else
	{       
		*OpData = ((*OpData))&0x7f;
		*(OpData + 4) = *(OpData + 5) = *(OpData + 6) = *(OpData + 7) =0;	        
	}
	
  Tx_Head( OpMode,OpData,Lenth );
	
	UART4->CTRL |= (BIT0|BIT1);		
	
	for(i = 0; i<3; i++)
 	{
 		if( FLAG_EA== 0 ) return 1;//?????,??????
 			
		Clear_Wdt();
		
		if(EmuRegOp(OpMode,Lenth) == 0) 
		{		
		    if(OpMode == SPIRead) 
	      {
		        for(i = 0; i <Lenth; i++) 
		        *(OpData +3  + Lenth-i) = MterRX_Buff[i+1];   	
		    }
		    UART4->CTRL |= (BIT0|BIT1);
		    return 0 ;
		 }  
	}
	
	UART4->CTRL |= (BIT0|BIT1);
	return 1;			//????
}
	
unsigned char ATT7051SPIOp(	unsigned char OpMode, unsigned char *OpData)
{
	unsigned char i;
	//unsigned char  TempBuf[5];
	for(i = 0; i<3; i++)
  {

       if(ATT7051SPIOp_rel( OpMode,OpData  )==0)
       {
//           if(*OpData == 0xea)return 0;

//	         if(OpMode == SPIWrite)
//	         {
//		            SPI_OpDatatemp.E16.B32=0x00000045;//??????
//                Lenth=2;
//           }
//	         else
//	         {      
//	              SPI_OpDatatemp.E16.B32=0x00000044;
//                Lenth=4;
//	         }
//	         
//					 if(ATT7051SPIOp_rel( SPIRead,SPI_OpDatatemp.E16.B08  )==0)
//           {	
//			         for(j = 0; j <Lenth; j++) 
//		           {
//                    if( (*(OpData + 4 +j)) != SPI_OpDatatemp.E32.B08[j])break;   	
//		           }  
//               if( j ==Lenth)return 0;  
//	         }

     return 0; 
		 }
     Clear_Wdt();
	}
  if( i ==3)return 1;			//????
  
	return 0;

}

/**********************************************************/

/**********************************************************/
//CH=0xe5,???/
//CH=0xdc,???
//CH=0x5a,????A
//CH=0xa5,????B
void ATT7051WRPRO( unsigned char CH)
{       
	unsigned char i;

  SPI_OpData.E16.B32= 0x000000ea;
	SPI_OpData.E32.B32=( unsigned long int)CH;
	
	for(i=0; i<3; i++)
	{
       if(ATT7051SPIOp(SPIWrite,SPI_OpData.E16.B08)==0)break;
  }
}

/**********************************************************/
unsigned char Read_UIPF( unsigned char num, unsigned char *RtBuf )
{
	unsigned char i,SPIOpResult ;
	const uint16 code RegAddr[5]={URMS,IARMS,PowerPA,IBRMS,Freq_U};
	
	for(i=0; i<num; i++)
	{
	
		SPI_OpData1.E16.B32 = RegAddr[i];
		SPIOpResult = ATT7051SPIOp(SPIRead,SPI_OpData1.E16.B08);
		if(SPIOpResult) return 0x01;
		
		if( ( i == 0 )||( i == 1 )||( i == 3 ))
		{
			if(SPI_OpData1.E32.B08[2] & B1000_0000) SPI_OpData1.E32.B32 = 0;	//????????????
			
		}
		
		memcpy(RtBuf+4*i, SPI_OpData1.E32.B08, 4);
	}

	return 0x00;

}

void Clr7051Flg( void )	
{			        
//	Flg_182u = 0;			
//	Flg_200u = 0;			
//	Flg_230u = 0;
//	Flg_250u = 0;
//	Flg_255u = 0; 
}
	
void Write7051reg( unsigned short Adress,unsigned long int VuleAdress )	
{
  unsigned  short tempcrc;
        unsigned char SPIOpResult, i;
	Att7051_Calib.A25.CKSUM.B32 =0;
	for(i=0; i<(chk_reg_num-1); i++)	
	tempcrc +=(unsigned short )Att7051_Calib.E32[i];
	Att7051_Calib.A25.CKSUM.B32 = (unsigned long int )(~tempcrc);
	ATT7051WRPRO(0xe5);
	for( i=0; i<3; i++ )
	{
		  SPI_OpData.E16.B32 = Adress;
		  SPI_OpData.E32.B32 = VuleAdress;
		  SPIOpResult =  ATT7051SPIOp(SPIWrite,SPI_OpData.E16.B08) ;
		  if(SPIOpResult==0)break;
	}
	
	ATT7051WRPRO(0xdc);
   
}			
/**********************************************************/
void Read_UIP(void)					//瞬时量
{
	union B32_B08  RegData[10]; //U, I1, P1, I2, F
	unsigned char SPIOpResult, i, j;
	unsigned long int P_flag;
	
	float temp;
	if( FLAG_EA == 0 ) return;
	
	memset( RegData[0].B08, 0x00, 40);
	
	if(read_back_flag) 
	{
	  read_back_flag = 0;	
	  Clear_Wdt();
        
	  SPI_OpData.E16.B32= 0x0000002d;
	  SPIOpResult = ATT7051SPIOp(SPIRead,SPI_OpData.E16.B08);
	  if(SPIOpResult==0) P_flag = SPI_OpData.E32.B32;
	  else {P_flag = 0xffffffff;}
	  if( P_flag != 0xffffffff )
	  {
		  if((P_flag & 0x00010000))
      { 
		    if( ( P_flag & 0x0000ffff) != Att7051_Calib.A25.CKSUM.B32 )init_7051_flag = 1; 
		  }
	  }
	
	  SPIOpResult = Read_UIPF( 5, RegData[0].B08 );
	  if(SPIOpResult==1)	
	  {
		   if(UartErrCnt < UartErrCnt_const) UartErrCnt++;
		   else 
		   {
			     init_7051_flag = 1;  //连续60次不成功，初始化计量芯片
           UartErrCnt = 0;
	     }
		   return;
	  }
	  else
    {
       UartErrCnt = 0;
    }
    Clear_Wdt();
	  RegData[6].B32 = RegData[4].B32;		
	  read_uip_flag1=0;
	  j = 7;
					 
		for( i=0; i<j; i++ )
	  {		    
		  if( FLAG_EA == 0 ) return ;
		  switch(i)
		  {				
			case 0:	//U
				RegData[i].B32 = RegData[i].B32*10/Emu_Para1.rate[i].B32;	
			  break;
			case 1:	////IA,IB
		  case 3:
        RegData[i].B32 = RegData[i].B32*1000/Emu_Para1.rate[i].B32;			
				break;
			case 2:	//PI
				if(RegData[i].B32&0x80000000)
				{
					 dl_back_flag1 = 1;RegData[i].B32 = ~RegData[i].B32+1;
				}
				else dl_back_flag1 = 0;
							
				RegData[i].B32 = (unsigned long int )(((float )RegData[i].B32/Emu_Para1.rate[i].B32)*10000);
				
				Pav_Temp += RegData[i].B32;					//累计每分钟平均功率值
		    Pav_Cnt ++;
        
				break;
	    case 4: //Ps		
			  temp = (float)RegData[0].B32*RegData[1].B32/1000;			      
			  RegData[i].B32 = (unsigned long int )temp;			       
			  break;				
			case 5:	
			  if(RegData[4].B32)
				{
				   temp=(float )RegData[2].B32/RegData[4].B32;
				   RegData[i].B32 =(unsigned long int )(temp*1000 );
				}
				else 
					RegData[i].B32 = 1000;
				break;
			
			case 6:				
        RegData[i].B32=44744312.5/(RegData[i].B32);
        break;
      default:
				break;
		  }
		  
			Hex2Bcd(RegData[i].B08, Emu_Data.U.B08+4*i, 4);
		  if( FLAG_EA == 0 ) return;
	  }
             
   	if(Emu_Data.U.B32 < Rly_voltage ) below_70u_flag = 1;
	  else 
	  {
	      if(below_70u_flag == 1)run_alarm_flag = 1; //补充继电器动作
	      below_70u_flag = 0;		     
	  }
	     
	  if(Emu_Data.U.B32 > 0x4500)      Emu_Data.U.B32 = 0;		//>450V，不合法数值	
	  if((Emu_Data.I1.B32 > 0x160000)) Emu_Data.I1.B32 = 0;	  //>160A，不合法数值	
	  if((Emu_Data.I2.B32 > 0x160000)||(Emu_Data.I2.B32 < Def_Shielding_I2)) Emu_Data.I2.B32 = 0;//>160A不合法数值	
    if(Emu_Data.Cosa.B32 > 0x0995)	 Emu_Data.Cosa.B32 =0x1000;
	
	  Clear_Wdt();
	
	  if(Emu_Data.P1.B32 <= METER_PWZEROVAUL)//
	  {       
		  if(Emu_Data.I1.B32 < METER_I1ZEROVAUL )
		  {
		    Emu_Data.I1.B32 = 0;
			  Emu_Data.Cosa.B32 = 0x1000;
			  Emu_Data.P2.B32 = 0;
		  } 			
		  Emu_Data.P1.B32 = 0;	
		  dl_back_flag = 0;	
		  power_zero_flag = 1;
			
			Pav_Temp = 0;
			Pav_Cnt = 0;
			
	  }
	  else	
	  {
		  power_zero_flag = 0;	        
	  }
          
	  if( ( Emu_Data.P1.B32 )&&( dl_back_flag1))
	  {
		  if(Back_Num < METER_BACKCNT) Back_Num++;
		  else
		  {
				if( dl_back_flag == 0 )
				{
					Init_Demand();
				}	
			  dl_back_flag = 1;	//功率反向
		  }			
	  }
	  else
	  {
		  Back_Num = 0;
			if( dl_back_flag == 1 )
			{
					Init_Demand();
			}	
			dl_back_flag1=0;
		  dl_back_flag = 0;		
	  } 
		
	  if(Emu_Data.P1.B32 == 0) power_zero_flag = 1;	//功率为0标志 
	}	
}

/**********************************************************/
void Judge_Uip_Data(void)
{
	union B32_B08  SPIOpData;
	union B16_B08  SPIOpData1;
	if( FLAG_EA == 0 ) return;
  #if(RELAYMODE == 0)//内置继电器
	
	SPIOpData.B08[3] = 0;
	SPIOpData.B08[2] = Mode.Limit_I[0];
	SPIOpData.B08[1] = Mode.Limit_I[1];
	SPIOpData.B08[0] = Mode.Limit_I[2];
	
	if( (Emu_Data.I1.B32 > SPIOpData.B32) && SPIOpData.B32 )
	{
		if( Current_Over_Count >= Defcurrovercount )
		{
			if( over_xxx_flag == 0 ) CurrentTimer.B32 = Calculate_Timer(1);
			over_xxx_flag = 1;	//拉闸控制电流门限值 
		}
		else Current_Over_Count++;
	}
	else
	{
		if( SPIOpData.B32 == 0 ) Current_Over_Count = 0;
		if( Current_Over_Count == 0 )
		{
			CurrentTimer.B32 = 0;
			if( over_xxx_flag ) run_alarm_flag = 1;
			over_xxx_flag = 0;
		}
		else Current_Over_Count--;
	}
#else
	over_xxx_flag = 0;	
	relay_delay_flag = 0;	
#endif
	
	//过流事件电流触发下限
	SPIOpData.B08[0] = 0;
	SPIOpData.B08[1] = Week_Holiday.Max_I.B08[1];
	SPIOpData.B08[2] = Week_Holiday.Max_I.B08[0];
	SPIOpData.B08[3] = 0;
	
	SPIOpData1.B08[0] = Week_Holiday.Max_I.B08[1];
	SPIOpData1.B08[1] = Week_Holiday.Max_I.B08[0];
	
	if( (Emu_Data.I1.B32 > SPIOpData.B32) && SPIOpData1.B16 )//?????????????
	{
		if( Current_Over_Count2 > Bcd_To_Hex(Week_Holiday.Max_I_Period) )
		{
			if( load_over_flag==0 ) Note_Run_Event(ee_maxi_inpage);//????????(??)	
			load_over_flag = 1;
		}
		else Current_Over_Count2++;
	}
	else
	{
		if( SPIOpData1.B16 == 0 ) Current_Over_Count2 = 0;
		if( Current_Over_Count2 == 0 )
		{
			if( load_over_flag ) Note_Run_Event(ee_maxi_inpage|0x80);//????????(??)
			load_over_flag = 0;
		}
		else Current_Over_Count2--;
	}
	
	//60%Un
	if(Emu_Data.U.B32 < Defaultpdvoltage)
	{
		if( Voltage_Low_Count >= 60 )	//
		{
			if( below_60u_flag == 0 ) Note_Run_Event( ee_powerdownn_inpage ); //????????
			below_60u_flag = 1;
		}
		else
		{
			Voltage_Low_Count++;
		}
			
		Flg_60lev=0;
	}
	else
	{
		if( Voltage_Low_Count == 0 )
		{
			if( below_60u_flag )
			{
				Note_Run_Event( ee_powerdownn_inpage|0x80 ); //????????
				Set_Reportdata(2*8+7);	//?????A???
			             
			}
			below_60u_flag = 0;
		}
		else 
		{
			if( Voltage_Low_Count>Deflowvoltagecount ) Voltage_Low_Count = Deflowvoltagecount;
			if( Voltage_Low_Count != 0 ) Voltage_Low_Count--;
		}
			
		Flg_60lev = 1;//
	}
	
	//0.8Un~1.15Un?,??????
//	if( (Emu_Data.U.B32 > 0x1740) && (Emu_Data.U.B32 < 0x2550)  )
	if( Emu_Data.U.B32 > 0x1650 )		//????????75%		
	{
		if(PowerDetCnt0 < 4) PowerDetCnt0++;
		
	}
	else
	{
		if(PowerDetCnt0) PowerDetCnt0--;
	        
	}
	if( Emu_Data.U.B32 > Deflowledvoltage )			
	{
		Flg_53lev= 1;	
	}
	else
	{
		Flg_53lev= 0;
	}
		
	if( Emu_Data.U.B32 > 0x770 )			
	{
		Flg_35lev= 1;	
	}
	else
	{
		Flg_35lev= 0;
	}
	
	if( Emu_Data.U.B32 > 0x2860  )	
	{
		if(PowerDetCnt1 < 10) PowerDetCnt1++;
		
	}
	else
	{
		if(PowerDetCnt1) PowerDetCnt1--;
	}	
	
	if( Emu_Data.U.B32 < 0x1700  )	
	{
		if(PowerDetCnt2 < 8) PowerDetCnt2++;

	}
	else
	{
		if(PowerDetCnt2) PowerDetCnt2--;
	}	
	
	if( Emu_Data.U.B32 > 0x3300  )	
	{
		if(PowerDetCnt3 < 2) PowerDetCnt3++;
		else Over_voltage = 1;
	}
	else
	{
		 Over_voltage = 0;
		 PowerDetCnt3 = 0;
	}
}
	
/**********************************************************/
void Emu_JBReg_Verify()				//?????????
{
	if( Verify_para( ee_7051_page, 0, Att7051EepD.HFConstD.B08, ee_7051_lenth )) 
	memcpy(&Att7051EepD.HFConstD.B08, &Dl_Cpu_Init[0],8*4);
}

unsigned long int Sum()
{
     unsigned char i; 
     unsigned  int Temp=0,Temp1=0;
     for(i=0; i<(chk_reg_num-1); i++)	
    {
       	 Temp =(unsigned int )(Att7051_Calib.E32[i]);  
         Temp1 += Temp;
    }
    
     return Temp1;
}
/**********************************************************/
void Emu_Reg_Verify()			//计量芯片参数校验
{
  unsigned char i;
	unsigned int  temp;
  temp=0;
	
	if((Sum() != (unsigned int)Att7051_Calib.E32[24]) || (Att7051_Calib.E32[0]!=SysATT7051[0].B32 ))
	{
		Emu_JBReg_Verify();
    memcpy(&Att7051_Calib.A25.SYSCON.B08, &SysATT7051[0].B08, 2*4);
		memcpy(&Att7051_Calib.A25.DStart.B08, &Dl_Cpu_Init[8].B08, 14*4);
		memcpy(Att7051_Calib.A25.HFConst.B08, Att7051EepD.HFConstD.B08, 8*4);
		Att7051_Calib.A25.CKSUM.B32 =0;
		for(i=0; i<(chk_reg_num-1); i++)	
		temp +=(unsigned int)Att7051_Calib.E32[i];
    Att7051_Calib.A25.CKSUM.B32= (unsigned  long int)(~temp);
		
	}
}

/**********************************************************/
void Backup_Para_7051()			//????????
{       		
       WriteE2WithBackup( ee_7051_page, 0, Att7051EepD.HFConstD.B08, ee_7051_lenth );	//???????
       init_7051_flag = 1;
}
/**********************************************************/

void Emu_Para_Verify()			//瞬时量系数参数校验
{
	if(Verify_para( ee_emupara_page, ee_emupara_inpage, Emu_Para.rate[0].B08, ee_emupara_lenth ))
	{
		Emu_Para.rate[0].B32 = 1;
		Emu_Para.rate[1].B32 = 1;
		Emu_Para.rate[2].B32 = 1;
		Emu_Para.rate[3].B32 = 1;
		
	}
  if(Verify_para( ee_7051_para_page, ee_7051_para_inpage, Emu_Para1.rate[0].B08, ee_7051_para_lenth ))
	{
		Emu_Para1.rate[0].B32 =  0x2000;  //电压
		Emu_Para1.rate[1].B32 =  0x70b8;  //电流1
		Emu_Para1.rate[2].B32 =  0x70b8;  //电流2
		Emu_Para1.rate[3].B32 =  0xec0000;//功率
	}

}
	
void Init_Meter(void)	//参数设置
{
	unsigned char i,n, SPIOpResult;
	
	if( FLAG_EA == 0 ) return;
	dl_back_flag = 0;	
	
	Emu_Reg_Verify();  //计量运行参数
	Emu_Para_Verify(); //瞬时量系数参数
                
	Clear_Wdt();
	
	ATT7051WRPRO(0xe5);		  //写保护
	
	for(n=0; n<3; n++)			//重复三次
	{	
	    for(i=0; i<chk_reg_num-1; i++)		
	    {
		      SPI_OpData.E16.B32 = ATT7051Addr[i].B32;
		      SPI_OpData.E32.B32 = Att7051_Calib.E32[i];
		      SPIOpResult = ATT7051SPIOp(SPIWrite,SPI_OpData.E16.B08);	
		      if(SPIOpResult==1 )break;
				
				  I2c_Wait_2ms();
		      if( FLAG_EA == 0 ) break;
		      Clear_Wdt();				
	    }		
      if(SPIOpResult==0)break;	
	}
  Clear_Wdt();
	I2c_Wait_5ms();	

	ATT7051WRPRO(0xdc);	
}

void Init_Meter_Chip(void)	//计量芯片初始化
{	
	AT7051_Flag_S.B08 = 0x0f;
  PLUSELED_OFF;             //关闭脉冲信号输出
	Init_Meter();			        //计量参数初始化
	Delay_Initial(20);        //延时稳定
	Clear_Wdt();
	Read_UIP();               //读取瞬时量，判断各种电流阀值
	Judge_Uip_Data();
	dl_back_flag = 0;		    	//反向标志清零，防止电压不稳定情况下的误判
}
	
void ATT7051_Check(void)		//?????????
{
	Clear_Wdt();
	if(init_7051_flag)
	{
    NVIC_DisableIRQ(PMU_IRQn );
		*PMU_CONTROL =0x00000000;       //计量芯片初始化前关闭端口中断
	  GPIOB->OEN &= ~BIT0; GPIOB->IE &= ~BIT0;//配置端口为输出模式
		METER_RST0;		
	  Delay_Initial(20);
    Clear_Wdt();
    METER_RST1;
    GPIOB->OEN |= BIT0; GPIOB->IE |= BIT0;//恢复端口为输入模式	
	
		I2c_Wait_5ms();
		PLUSELED_OFF;
		Init_Meter();
	  init_7051_flag = 0;
//    *PMU_CONTROL =0x00000001; 	    //打开计量
//		NVIC_EnableIRQ(PMU_IRQn );		
		read_back_flag=0;		
	}
	check_7051_flag1=0;
}		

/**********************************************************/
//????????
unsigned char Comm_Read_Reg(unsigned char *Buf)
{
	unsigned char result, Lenth;
  struct B08ADDB32  SPIOpDataTemp;
	unsigned char buff[4];
	
	if(*Buf <= 0x06) Lenth = 2;
	else if(*Buf <= 0x08) Lenth = 1;
	else if(*Buf <= 0x21) Lenth = 2;
	else if(*Buf <= 0x24) Lenth = 3;	
	else if(*Buf == 0x25) Lenth = 2;
	else if(*Buf <= 0x28) Lenth = 4;
	else if(*Buf <= 0x35) Lenth = 3;
	else if(*Buf <= 0x43) Lenth = 1;
	else if(*Buf == 0x44) Lenth = 4;
	else if(*Buf == 0x45) Lenth = 2;
	else if(*Buf == 0x7f) Lenth = 3;
	else Lenth = 1;
	
	
	SPIOpDataTemp.E16.B32=0; 
	memset(buff, 0, 4);
	
	memcpy(SPIOpDataTemp.E16.B08, Buf, 2);
	
	result = ATT7051SPIOp(SPIRead, SPIOpDataTemp.E16.B08);
	
	if(result) return 1;
  memcpy(buff, SPIOpDataTemp.E32.B08, 4);	
	
	Inverse( buff,Lenth);
	memcpy(Buf+1, buff, 4);	
	
//	if( (Buf[0] == 0x26) || (Buf[0] == 0x27) || (Buf[0] == 0x26)) 
//	{
//		Temp[3]=Buf[0];
//		result = ATT7051SPIOp(SPIRead, Temp);
//		if(result) return 1;
//		Exchange( Buf+1,Temp, 4 );
//	}
//	else
//	{
//		Temp[3] = *Buf;
//		result = ATT7051SPIOp(SPIRead, Temp);
//		if(result) return 1;
//		Exchange( Buf,Temp, 4 );
//	}
//	
//	
//	
//	
//	Inverse(buff, Lenth);	
////	for(i=0; i<4; i++)
////	{
////	  if(buff[3-i]) break;	
////	}
////	Inverse(buff, 4-i);	
//	memcpy(Buf+2, buff, 4);	
//	memcpy(Buf, buff, 3);	
	return 0;
}
//????????
unsigned char Comm_Write_Reg(unsigned char *Buf)
{
	 unsigned char i;
   union B32_B08 SPIOpData2;
        
	SPIOpData2.B32 = 0;

	if(Buf[0] == 0xff)	//???????
	{
    memcpy( &Att7051_Calib.A25.SYSCON.B08, &SysATT7051[0].B08, 2*4 );
		memcpy( &Att7051_Calib.A25.DStart.B08, &Dl_Cpu_Init[8].B08, 14*4 );
		memcpy( Att7051_Calib.A25.HFConst.B08, &Dl_Cpu_Init[0].B08 , 8*4 );
    memcpy( Att7051EepD.HFConstD.B08, Att7051_Calib.A25.HFConst.B08, 8*4 );
    Backup_Para_7051();//????????

//		Emu_Para.rate[0].B32 = 1;
//		Emu_Para.rate[1].B32 = 0;
//		Emu_Para.rate[2].B32 = 0;
//	  Emu_Para.rate[3].B32 = 0;
//		WriteE2WithBackup( ee_emupara_page, ee_emupara_inpage, Emu_Para.rate[0].B08, ee_emupara_lenth );
    Emu_Para1.rate[0].B32 = 0x000020bb;
		Emu_Para1.rate[1].B32 = 0x000070bb;
		Emu_Para1.rate[2].B32 = 0x000070bb;
	  Emu_Para1.rate[3].B32 = 0x00e020bb;
    WriteE2WithBackup( ee_7051_para_page, ee_7051_para_inpage, Emu_Para1.rate[0].B08, ee_7051_para_lenth );


	}    
	else	//??????
	{
		for(i=0; i<chk_reg_num-1; i++)
		{
			if( Buf[0] == ATT7051Addr[i].B08[0])
			{       
			  if( Buf[0] == 0x07 )
				Exchange( (SPIOpData2.B08), Buf+1, 1 );
				else	
			  Exchange( (SPIOpData2.B08), Buf+1, 2 );
				Att7051_Calib.E32[i] = SPIOpData2.B32;
				memcpy(Att7051EepD.HFConstD.B08, Att7051_Calib.A25.HFConst.B08, ee_7051_lenth);
				Backup_Para_7051();
				return 0;
			}
		}
		
	}
	return 0;
}

