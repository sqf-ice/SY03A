#include "ex_var.h"
#include "ex_func.h"
//#include <intrins.h>
#include <string.h>

const uint08 code Reg_Addr_Int[IntRegLen] = {COMMAND, STATUS2, TXMODE, RXMODE, TXCONTROL, MODWIDTH, RXTRESHOLD, RFCFG, WATERLEVEL, TXAUTO, BITFRAMING, COLL, COMMIEN};
const uint08 code Reg_Data_Int[IntRegLen] = {0x20, 0x00, 0x08, 0x08, 0x0b, 0x26, 0x55, 0x48, 0x20, 0x40, 0x00, 0x00, 0x00};

uint08	near Pcb_Flag;

//==================== �ײ����д�Ĵ��� ==================
//********************************************************
unsigned char RF_Spi_Com( unsigned char Reg_Addr ) //��/дһ�ֽڼĴ���
{
	unsigned char i, Temp08;
	
	Temp08 = 0x00;
	for(i=0; i<8; i++)	//��ȡ�Ĵ������ݷ���RF_Rx_Buf
	{
		RFSPICLK0;
		if( Reg_Addr&0x80 ) RFSPIMO1;		//������һ����ַ
		else RFSPIMO0;
		Reg_Addr <<= 1;

		RFSPICLK1;
		Temp08 <<= 1;
		if(RFSPIMIGPIO & BIT12)	Temp08 |= 1;
	}
	
	return Temp08;
}


//*********************************************************
void RF_Reg_RW( unsigned char OpMode, unsigned char Reg_Addr, unsigned char *Reg_Data, unsigned char Reg_Len )
{	
	unsigned char k, Addr_Temp;
 	
	Clear_Wdt();
 	RFSPICS0;
 	
 	Addr_Temp = (Reg_Addr<<1)|OpMode;
	
	RF_Spi_Com(Addr_Temp);	//��д��Ĵ�����ַ

	for( k=0; k<Reg_Len; k++ )
	{		
		if( FLAG_EA==0 ) break;
			
		if( OpMode == RD )
		{
			if( k == (Reg_Len - 1) ) Addr_Temp = 0;	//��ȡ���һ�ֽڣ���ַ=0
			Reg_Data[k] = RF_Spi_Com( Addr_Temp );
		}
		else
			RF_Spi_Com( Reg_Data[k] );
	}

	RFSPICS1;
	RFSPICLK0;
}

//===================== ��Ƶ��д1���ֽ�===================
unsigned char RF_Get_Reg( unsigned char Reg_Addr )
{
	unsigned char Reg_Data;
	
	RF_Reg_RW(RD, Reg_Addr, &Reg_Data, 1);
	
	return (Reg_Data);
}

void RF_Set_Reg( unsigned char Reg_Addr, unsigned char Reg_Data )
{
	RF_Reg_RW(WR, Reg_Addr, &Reg_Data, 1);
}

/*********************************************************************************************************
** ��������:	FM175xx_ReadFIFO
** ��������:	��FIFO
** �������:	*Buf: ��ȡ�����ݴ�ŵ�ַ
** �������:	
** ����ֵ:		Len: FIFO��ȡ�����ݳ���
*********************************************************************************************************/
unsigned char RF_Read_FIFO( unsigned char CH )
{
	unsigned char i, Temp, Len;
	
	Len = 0;

	i = 2; //�����ʱ: 0.6ms@1��
	if( CH == CARD ) i = 30; //�����ʱ��5.9ms@10��
	
	for( ; i>0; i-- ) //0.6ms@one loop, ����֡����Сʱ��0.610ms
	{
		if( FLAG_EA == 0 ) break;

		Temp = RF_Get_Reg( FIFOLEVEL );	//��ȡFIFO�����ݳ���
		if( Temp > 64 ) break;	//����ȡ64�ֽ�
	
		if( Temp )
		{
			if( (Len+Temp) > u7816_buf_size ) break;
			RF_Reg_RW(RD, FIFODATA, Ic_Comm_RxBuf+Len, Temp);
			Len += Temp;
		}
	}
	
	return (Len);
}

//================== ��ƵCommand_Send����=================
unsigned char Test_Command_Send( unsigned char CH, unsigned char *TxData, unsigned char RF_Len )
{
	unsigned char i, Result;
	
	RF_Set_Reg(COMMAND, 0x00);
	RF_Set_Reg(COMMIRQ, 0x00);				//���TXI�ж�
	RF_Set_Reg(BITFRAMING, 0x00);	//���1�ֽڵ�����λ������
	RF_Set_Reg(FIFOLEVEL, 0x80);			//��FIFO
	
	if(Ic_Comm_TxBuf[0]==0x60)
	{			
		RF_Reg_RW( WR, FIFODATA, TxData, RF_Len ); //������д��FIFO	  
		RF_Set_Reg(COMMAND, 0x0e);	
		I2c_Wait_5ms(  ) ;	
	}
	else		
	  RF_Set_Reg(COMMAND, 0x0c);				//����FIFO�������е����ݵ����ߣ�������Զ����������
	
	if(CH == CARD)
	{
		RF_Set_Reg(FIFODATA, Pcb_Flag);	//�Կ�Ƭ������дPcb_Flag
	}
	
	
	if(Ic_Comm_TxBuf[0]!=0x60) RF_Reg_RW( WR, FIFODATA, TxData, RF_Len ); //������д��FIFO

	if(Ic_Comm_TxBuf[0]!=0x60)
	{
		if((Ic_Comm_TxBuf[0]==0x52) && (RF_Len==1))
		  RF_Set_Reg(BITFRAMING, 0x87);	//�������ݿ��������1�ֽڷ���7bit(Request)		
	  else 
		  RF_Set_Reg(BITFRAMING, 0x80);	//�������ݿ��������1�ֽڵ�����λ������
  }
	for(i=0; i<20; i++)
  {		
		if(Ic_Comm_TxBuf[0]==0x60)
		{			   
			  Result = RF_Get_Reg(STATUS2);	
		    if((Result & 0x08)) break; 				
	  }
		else	
		{	
			  Result = RF_Get_Reg(COMMIRQ);
		    if((Result & 0x02) ) return 0xff;		//���մ����˳�
	  }
	}
	if((Ic_Comm_TxBuf[0]==0x60)&&(i==20))	return 0xff;		//���մ����˳�
	
	return 0;
}

//================== ��ƵCommand_Send����=================
unsigned char RF_Command_Send( unsigned char CH, unsigned char RF_Len )
{
	unsigned char i, Result;
	
	Result = Test_Command_Send(CH, Ic_Comm_TxBuf, RF_Len);
	if( Result ) return 0;

	Result = RF_Read_FIFO(CH);	//��ȡFIFO����
	if( CH == CARD )
	{
		for(i=0; i<10; i++)
		{
			if( Result && (Result != 2) ) return(Result);
			if(Result == 2) 
			{
				Result = Test_Command_Send(FM17520, Ic_Comm_RxBuf, 2);		//Transceive = 93 20
				if( Result ) return 0;
			}
			Result = RF_Read_FIFO(CH);			//��ȡFIFO����
		}
	}
	
	return (Result);
}

//================== ��Ƶ�����ա��������� =================
unsigned char RF_TxRxFrame( uint08 Lc, uint08 Le )
{
	unsigned char Result;

	Pcb_Flag ^= 0x01;

	Result = RF_Command_Send(CARD, Lc+5);	//Transceive = 52
	if( Result != Le+3 ) 
	{
		Err_Sw12[0] = Ic_Comm_RxBuf[1];
		Err_Sw12[1] = Ic_Comm_RxBuf[2];
		return(1); 	//�жϷ��ص����ݳ���
	}
	
	memcpy( Ic_Comm_RxBuf, Ic_Comm_RxBuf+1, Le+2 );
	Ic_Comm_RxPtr = Le + 2;
	
	if( ((Ic_Comm_RxBuf[Le] == 0x90)&&(Ic_Comm_RxBuf[Le+1] == 0)) || (Ic_Comm_RxBuf[Le] == 61) ) return 0;
	else return 1;
}

//***********************************************
//�������ƣ�FM175xx_Initial
//�������ܣ�FM175xxоƬ��ʼ��
//��ڲ�����NA
//���ڲ�����uchar  0���ɹ�   others:ʧ��
/***********************************************
COMMAND =0x20:��λ���мĴ���
STATUS2 =0x00:�����Ĵ���
TXMODE =0x00:��������ʱ����ʹ��CRCУ��
RXMODE =0x08:��������ʱ����ʹ��CRCУ�飬���յ�����Ч�������ᱻ����
RXTRESHOLD =0x55:������������ֵѡ���ܽ��յ�����С�ź�ǿ��
GSN =0xf4:����λ�ı������ȣ���ֵԽС���������ԽС��
RFCFG=0x48:�������Ŵ����ͽ���������
WaterLevel =0x20:�յ�һ������ʱ�����ж�
TXAUTO =0x00:Force100ASK����ǿ�Ʋ���100%ASK����
************************************************/
void RF_Fm17xx_Init(void)
{
	unsigned char i;
	
	Pcb_Flag = 0x03;

	RF_Set_Reg(GSN, Def_GSN);	//GSN��CWGSP ������������
	RF_Set_Reg(CWGSP, Def_CWGSP);
//	i=RF_Get_Reg(GSN);
//	i=RF_Get_Reg(CWGSP);
	for(i=0; i<IntRegLen; i++)
	{
		RF_Set_Reg(Reg_Addr_Int[i], Reg_Data_Int[i]);
//		TEMP=RF_Get_Reg(Reg_Addr_Int[i]);
	}
}

//***********************************************
//�������ƣ�RF_Request
//�������ܣ�ReqA
//��ڲ�����Type��ReqA��0x26 WupA��0x52
//���ڲ�����uchar  0���ɹ�   others:ʧ��
//          CardA_Sel_Res.ATQA[0]��[1]
//***********************************************
unsigned char RF_Request(void)
{
	unsigned char result;

	Ic_Comm_TxBuf[0] = 0x52; 
	result = RF_Command_Send(FM17520, 1);
	if(result != 2) return 1;
	
	return 0;
}

//*************************************
//����  ����FM175xx_AntiCol
//��ڲ�����size:����ͻ�ȼ�������0��1��2��ֻ�õ���1��
//���ڲ�����uchar:0:OK  others������
//*************************************
unsigned char RF_Anticoll(void)
{
	unsigned char i, result;
	
	Ic_Comm_TxBuf[0] = 0x93;   					//����ͻ�ȼ�0x93,0x95,0x97
	Ic_Comm_TxBuf[1] = 0x20;
	result = RF_Command_Send(FM17520, 2);//���� 93 20��0����ͻ��
	if(result != 0x05) return 1;	//Ӧ����5�ֽ�ID����(Buf[3]ӦΪ�����ֽ���)
	
	result = 0;
	for(i=0; i<4; i++)
	{
		result ^= Ic_Comm_RxBuf[i];
	}
	if(result != Ic_Comm_RxBuf[4])	return 1;				//BCC ����У��, UID�����У��ֵ��
	memcpy( UID, Ic_Comm_RxBuf, 5);
	return 0;
}

//*************************************
//����  ����FM175xx_Select
//��ڲ�����size:����ͻ�ȼ�������0��1��2
//���ڲ�����uchar:0:OK  others������
//*************************************
unsigned char RF_Select_Tag(void)
{
	unsigned char result;
	
	RF_Set_Reg(TXMODE, 0x88);//��������CRCУ��
	RF_Set_Reg(RXMODE, 0x88);//��������CRCУ��
		
	Ic_Comm_TxBuf[0] = 0x93;   			//����ͻ�ȼ�0x93,0x95,0x97(������ط���ͻ)
	Ic_Comm_TxBuf[1] = 0x70;
	memcpy(Ic_Comm_TxBuf+2, Ic_Comm_RxBuf, 5);//93 70 +4�ֽ�UID+1�ֽ�У��
	result = RF_Command_Send(FM17520, 7);	//��������
	if(result != 1) return 1;

	if( Ic_Comm_RxBuf[0] == 0x08 ) return(0);//RETURN����
	else return(1);
}

//*************************************
//����  ����FM175xx_Rats
//��ڲ�����FSDI��Reader��֡�������ֵ��2^x��CID:0~14,Card���߼���ַ��
//���ڲ�����uchar:0:OK  others������
//*************************************
unsigned char RF_Send_Rats(void)
{
	unsigned char Result;
		
	Ic_Comm_TxBuf[0] = 0xE0;
	Ic_Comm_TxBuf[1] = 0x50;   					//Э�鼤����64�ֽ�
	Result = RF_Command_Send(FM17520, 2);
	if( Result == 0x0e ) 
	{
		memcpy(CardRst8B, Ic_Comm_RxBuf+6, 8);
		return(0);
	}
	
	else return 1;
}


//ϵͳ�ڲ���֤
unsigned char Inter_Authrf08(unsigned char  section)
{
	unsigned char result;
	unsigned char idata  K1[8];
	
	RF_Set_Reg(TXMODE, 0x88);//��������CRCУ��
	RF_Set_Reg(RXMODE, 0x88);//��������CRCУ��
	memset(K1, 0xff, 8);
	Ic_Comm_TxBuf[0] = 0x60;   			//����ͻ�ȼ�0x93,0x95,0x97(������ط���ͻ)
	Ic_Comm_TxBuf[1] = section;
	memcpy(Ic_Comm_TxBuf+2, K1, 6);//93 70 +4�ֽ�UID+1�ֽ�У��
	memcpy(Ic_Comm_TxBuf+8, UID, 4);
	result = RF_Command_Send(FM17520, 12);	//��������
	if(result != 0) return 1;

	return(0);
}

//================ ��Ƶ��Ѱ��������ͻ������ ==============
unsigned char RF_Init(void)
{
	uint16 i;
	unsigned char Result;
	
	RFSPIRST1;
	for(i=0; i<330; i++)		//��ʱ3ms
	{
		Clear_Wdt();
	}

	RF_Set_Reg(COMMAND, 0x0f);			//��λ�Ĵ���
	
	RF_Fm17xx_Init();								//��ʼ��17520��ؼĴ���
	
	Result = RF_Request();					//Ѱ��
	if(Result) 
	{
		if(RFTESTCNT) RFTESTCNT=0;
		return(0xff);
	}	
	
	Result = RF_Anticoll();					//����ͻ
	if(Result) 
	{
		if(RFTESTCNT) RFTESTCNT=0;
		return(0xff);
	}	
	
	Result = RF_Select_Tag();				//ѡ��
	
	if(Result) 
	{
		if(RFTESTCNT) RFTESTCNT=0;
		return(0xff);
	}	
	
	RFTESTCNT++;
	if(RFTESTCNT<2) return(0xff);
	
	RFTESTCNT=0;
	return 0;
}

void Release_Rf(void)
{
	RFSPIRST0;				//RST�ţ�������:����������0:��λ
}