//定义全局函数
#ifndef	__EX_FUNC_H__
#define __EX_FUNC_H__

#include "define4.h"

//iic
extern unsigned char I2C_Write_Eeprom( uint16 Page, unsigned char offset, unsigned char *IIC_Buf, unsigned char Len );
extern unsigned char I2C_Read_Eeprom( uint16 Page, unsigned char offset, unsigned char *IIC_Buf, unsigned char Len );
extern unsigned char ReadE2WithBackup( uint16 Page, uint08 inpage, uint08 *Buf, uint08 Len );
extern unsigned char WriteE2WithBackup( uint16 Page, uint08 inpage, uint08 *Buf, uint08 Len );
extern unsigned char I2C_Read_Eeprom44( uint16 Page, uint08 *Buf, uint08 Len );
extern unsigned char I2C_Write_Eeprom44( uint16 Page, uint08 *Buf, uint08 Len );

//rtc
extern unsigned char Add_Bcd( unsigned char  *sr, unsigned char  *de, unsigned char len );
extern void Sub_Bcd(unsigned char  *sr,unsigned char  *de,unsigned char len);
extern void Bcd2Hex( unsigned char *BCDopt, unsigned char *HEXopt, unsigned char Len );
extern void Hex2Bcd( unsigned char *Hexop, unsigned char *BCDopt, unsigned char Len );
extern unsigned char Hex_To_Bcd( unsigned char value );
extern unsigned char Bcd_To_Hex( unsigned char value );
extern unsigned char ChkBufZero( unsigned char *Buf,unsigned char Len );
extern unsigned char Cmp_Data( unsigned char *BufA, unsigned char *BufB, unsigned char Len );
extern unsigned char Cmp_Bcd_Inv( uint08 *BcdA, uint08 *BcdB, uint08 Len );
extern unsigned char Cmp_DataLH( unsigned char *BufA, unsigned char *BufB, unsigned char Len );
extern unsigned char Cmp_Clock( unsigned char *Buf, unsigned char Len );
extern unsigned char Judge_Clock( uint08 Start, uint08 *Ck, uint08 Len );
extern unsigned char JudgeClockNoWeek( uint08 Start, uint08 *Ck, uint08 Len );
extern unsigned char Read_Rtc( unsigned char *RTC_Buf );
extern unsigned char Set_Rtc( unsigned char Start, unsigned char *Ck, unsigned char Len );
extern void Syn_Clock( void );
extern void Read_Sysclock( void );

//verify
extern void Veri_Etmr1_Pcount(unsigned char Result);
extern void Veri_Clk_Gen(void);
extern void Veri_Pad(void);
extern void Veri_Sys_Reg(void);
extern void Veri_Uart_All(void);
extern void Starttimer_Verify(void);
extern void Remainmoney_Verify(void);
extern void Esam_Para_Verify(void);
extern void Money_Para_Verify(void);
extern unsigned char Fwdl_Verify(void);
extern unsigned char Bkdl_Verify(void);
extern void Mode_Verify(void);
extern void Commaddr_Verify(void);
extern void Display_Para_Verify(void);
extern void Displayitem_Verify(void);
extern void Week_Holiday_Verify(void);
extern void Temperpara_Verify(void);
extern unsigned char Verify_para(unsigned short Page, unsigned char Inpage, unsigned char *Buf, unsigned char Len );
extern uint16 Crc_16Bits(unsigned char *buf,unsigned char len) ;
extern unsigned char Crc_Check( unsigned char *buf, unsigned char len ); 
extern void feik_Num_Verify( void ); 
extern void RdMeterMode( void );



//Demand
extern void Demand_Process(void);

//tariff   
extern  void Festival_holiday_Judge(void);
extern void Sequence( unsigned char *Buf, unsigned char Num );
extern void Timezone_Judge(void);
extern void Price_Judge(void);
extern unsigned char Freeze_Change_Judge( unsigned char Change );
extern unsigned char Year_Ladder_Judge(void);
extern void ReadE2WithJieti( uint08 Ch, uint08 *Buf );
extern void WriteE2WithJieti( uint08 Ch, uint08 *Buf );

//event
extern void Exchange(unsigned char *BufA, unsigned char *BufB, unsigned char Len );
extern void Inverse( unsigned char *Buf, unsigned char Len );
extern void Number_Add1_Direct(unsigned char *BufA);
extern void Number_Add1(unsigned char Offset);
extern unsigned char Read_Event_Number( unsigned char *BufA, unsigned char Offset );
extern unsigned char Read_Event_Point( unsigned char Inpage );
extern unsigned char Write_Event_Point( unsigned char Inpage );
extern unsigned char Write_DmEvent_Point( void );
extern unsigned char RdRecTimes( unsigned char Inpage );
extern void Note_Powerd_Event(void);
extern void Note_Relay_Event(void);
extern void ClrRecTimes( unsigned char Inpage, unsigned char mode );
extern void Note_Run_Event( unsigned char Type );
extern void Init_Demand(void);
extern void Init_Demand1(void);
extern void Demand_Para_Verify(void);
extern unsigned char ReadLoadRec( unsigned char Ch, unsigned char *InBuf, unsigned char *OutBuf );
extern void ClearPloadRec( unsigned char *Clock );
extern void Load_Judge(void);
extern unsigned char Event_Record( unsigned char Inpage );
extern void DltoEepromBuf( unsigned char st );

//iokey
extern void Enable_Closerelay(void);
extern void Store_Meterstatus(void);
extern void Displaykey_Process(void);
extern void Displaykey_Check(void);
extern void Cover_Check(void);
extern void Relaystatus_Check(void);

//init
extern void Init_Int(void);
extern void I2c_Wait_5ms( void );
extern void Delay_Initial(unsigned char Num);
extern void Clear_Wdt(void);
extern void Clear_SYS_Wdt(void);
extern void Init_Dw80251(void);
extern void Clk_Adj_Proc(void);
extern void Adc_Proc(void);
extern void Init_System(void);

extern void Clock_Init(void);

//money
extern void Remainmoney_Process(unsigned char *BcdA, unsigned char Style );
extern void Calc_Pwr_Proc(void);
extern void Process_Usedl( unsigned char offset );
extern void Freeze_Judge(void);
extern void Histroy_Freeze_Dl(unsigned char status);
extern void Freeze_Usedl(unsigned char type);

extern void Freeze_Dl(unsigned short page, unsigned char offset);
extern void Pd_Histroyfreeze_Judge(void);
extern void FreezeMonth_Demand(unsigned  char mode);

extern void Init_Dayfreeze_Judge(void);
extern void Wr_Pretime_Dayfreeze(void);
extern void Judge_Relay_Delay(void);
extern void Init_Metererrstatus(void);
extern void Alarmstatus_Judge(void);
extern void Mt_Alarmstatus_Judge(void);

//interrupt
extern void Judge_645_Frame( uint08 CH, uint08 RcvData );
extern void Tx_645_Frame( uint08 CH );
extern void Lcdled_Control(void);
extern void Sleep(void);
extern void Beep_Ctrl(void);
extern void Wr_Powerdown_Data( unsigned char mode );
extern void Powerd_Hourwakeup( unsigned char Status );
extern void Save_Keep_Timer( unsigned char Store );
extern void Get_Keep_Timer(unsigned char Ch, unsigned char *Buf);
extern void Lcd_Disp_On(void);
extern void Lcd_Disp_Off(void);
	//7816

extern void PowerError_Check(void);
extern void api_Change_Key(void);
extern void Card_Proc( void );
extern unsigned char EsamReset( void );
 
extern unsigned char RF_Command_Send( unsigned char CH, unsigned char RF_Len );
extern void CloseEsam( void );
extern unsigned char Clear_Rand_Gene( void );
extern unsigned char ReadRecFile( unsigned char P1 );
extern unsigned char ReadBinFile( unsigned char CH, unsigned char P1, unsigned char P2, unsigned char Len );
extern unsigned char ReadM1File( unsigned char  section,unsigned char Block );

extern unsigned char RF_Anticoll(void);
extern unsigned char RF_Select_Tag(void);
extern unsigned char Inter_Authrf08(unsigned char  section);

extern void sub_delay(unsigned char PB_delay);

extern unsigned char Cal_Mac_Pro(unsigned char P2, unsigned char *BufA , unsigned char Len);
extern unsigned char SelectFile( unsigned char CH, unsigned char F1, unsigned char F2 );
extern void Moneybag_Process1(void);
extern void Moneybag_Process2(void);
extern unsigned char Ready_Remainback( void );
extern unsigned char JudgeBuyTimes( void );
extern unsigned char MoneyIn( void );
extern unsigned char Init_Money(void);
extern unsigned char OpenAddMoney( unsigned char Type );
extern void UpdataRunFile(void);
extern void Get_Usercard_Serial( void );
extern void Updata_Usercard_Serial( unsigned char Type );
extern void Buy_Record(unsigned char Inpage);
extern unsigned char Update_ESAM_Bin_Mac( unsigned char P1, unsigned char P2, unsigned char *Buf, unsigned char Len );
extern void Mac_Err_Proc( void );
extern unsigned char UpdateEepromKey( unsigned char Ch );
extern unsigned char UpdateEsamKey( void );
extern void GetKeyStatus(void);
extern void FailKey( void );
extern void Clear_Meter(unsigned char Ch, unsigned char *Opt);

extern unsigned char Inter_Auth(void);
	//factory
extern void Factory_Init( void );

//lcd_disiplay
extern void Lcd_Display( void );
extern void Lcd_DispReadIc( void );
extern void LCDTranslate64(void);
extern void I2c_Wait_2ms( void ); 
extern void I2c_Wait_1ms( void );
//energy
extern void ATT7051AInitial(void);
extern void ATT7051_Check(void);
extern void Read_UIP(void);
extern void Judge_Uip_Data(void);
extern void Init_Meter_Chip(void);
extern unsigned char Check_Err_Pro(unsigned char Ch, unsigned char DI0);
extern unsigned char Comm_Read_Reg(unsigned char *Buf);
extern unsigned char Comm_Write_Reg(unsigned char *Buf);

//uart
extern unsigned long int Calculate_Timer(unsigned char Offset);
extern void Clr_Commu_Relaydata(void);
extern void Clr_Reportnum( unsigned char i );
extern void Clr_Reportdata( unsigned char i );
extern void Set_Reportdata( unsigned char Offset );
extern void Clr_Zero_Data(void);
extern void Clr_Otherdata( unsigned char Type );
extern void Uart_Proc( unsigned char Ch );
extern unsigned char IsBCD( uint08 Value );
extern unsigned char IsMultiBcd( unsigned char *Buf, unsigned char Len );
extern void Xor_Data( unsigned char *BufA, unsigned char *BufB, unsigned char Num );
extern void Calculate_Program_Cs( unsigned char *Buf );
extern unsigned char Cal_Comb( unsigned char DI2, unsigned char DI1, unsigned char DI0, unsigned char *ReturnBuf );
extern unsigned char JudgeSwitch( unsigned char *Buf, unsigned char Inv );
extern void ProgramRecord( unsigned char *Dix, unsigned char *Opt );
extern void CalZhuhe( const unsigned char *ZhuBuf, unsigned char *RtBuf, unsigned char Mode, unsigned char Offset );
extern void Cal_Comb_Usedl( unsigned char DI1, unsigned char DI0, unsigned char *ReturnBuf );
extern void FLRecord( unsigned char *InOpt );
extern void JTRecord( unsigned char *InOpt );
extern unsigned char  Cal_ChkSum(unsigned char *Buf,unsigned char Len);
//extern void Send2Uart( unsigned char *Buf, unsigned char Len );

extern void Delay_Initial(unsigned char Num);
extern void LoopExchange( unsigned char *Drc, unsigned char *Src, unsigned char Times, unsigned char Spec );
extern void LoopInverse( unsigned char *Inv, unsigned char Times, unsigned char Spec );
extern unsigned char WriteBinWithMac( uint08 CH, unsigned char P1, unsigned char P2, unsigned char Len );
extern unsigned char Relay_Ctrl(unsigned char Ch);
extern void ClrSelfClose( void );
extern void MacExchange( unsigned char *Drc, unsigned char *Src, unsigned char Len );
extern void GetKey( unsigned char *KeyStatus );
extern unsigned char Moneybag_Process(void);
extern unsigned char Cmp_Meter_Id(unsigned char *Buf);
extern unsigned char Comm_Auth_Judge( void );
extern unsigned char Get_Valid_Num( unsigned char Status );
extern void Pd_Freeze_Judge(unsigned char CH);
extern void RefreshPrice( void );
extern void Release_Rf(void);

extern void api_Reset_APDU_dataOfDefault(void);
extern void api_initialize_APDU_when_reset(void);
extern void api_save_APDU_key_var(void);
extern void api_updata_APDU_key_var(void);
extern void api_set_APDU_TID_Record(INT32U u32_tid_tmp,INT8U mode);
extern INT8U api_Handle_key_sts_code(INT8U *ptr_kay_data);
extern void api_judge_keyChg_TOKEN_interval_pre_second(void);

extern void RN8302_SPI_Init(void);
extern void RN8302_Delay( unsigned long n );
extern void RN8302_Read( unsigned char *RAM_Addr, unsigned int ADE78xx_Addr, unsigned char Lenth );
extern void RN8302_Write( unsigned int ADE78xx_Addr, unsigned char *RAM_Addr, unsigned char Lenth );
extern void RN8302Init(void);
extern void RN8302_Reset(void);
extern void Enable_SPI(void);
extern void RN8302_BasicPara(void);
extern void RN8302_Run(void);
extern void ParaMeasure(void);
extern void InstantPowerMeas(void);
extern unsigned char UMesure_Init(void);
extern void PowerMeasure(void);
extern void Meter_RN7302Para_Verify(void);
extern unsigned long BCD4_HEX4( unsigned char *BCD );

extern void HEX4_BCD4( unsigned char *BCD, unsigned long HEX );
extern void WORD_BCD2( unsigned char *BCD, unsigned short WORD );
extern unsigned char BYTE_BCD( unsigned char HEX );
//extern unsigned char BYTE_BCD( unsigned char HEX );
//extern void RN8302_Init(void);
#endif




