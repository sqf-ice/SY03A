//定义全局函数及其变量主要标注为extern

#ifndef __EX_VAR_H__
#define __EX_VAR_H__

#include "define4.h"

extern const float ecode const_KL;
extern const float ecode const_KH;
extern const float ecode const_xtl_top;
extern const float ecode const_TmpsL;
extern const float ecode const_TmpsH;
extern const uint08 ecode constF_XcapH;
extern const uint08 ecode constF_XcapL;
extern const uint08 ecode constF_Volt;

extern const uint16 ecode BPS_SPBRGL1[];
extern const struct HisEvent_STR ecode EventTable[];

extern const uint08 ecode runwdtcon;
extern const uint08 ecode rundivsel;
extern const uint08 ecode clkmode;
extern const uint08 ecode init_tl0;
extern const uint08 ecode init_th0;
extern const uint08 ecode runoscen;
extern const uint08 ecode sleeposcen;
extern const uint08 ecode sleeptmr0ie;
extern const uint08 ecode sleepwakeuptimer;

extern const uint08 ecode Relay_Mode;
extern const uint08 ecode Correction_Mode;
extern const uint08 ecode Lasttesting_Valid;	
extern const uint08 ecode Compile_Polar_485;
extern const uint08 ecode Compile_Lvd4x;
extern const uint08 ecode Memoryerr_Ctrl;	
extern const uint08 ecode Beep_Io_Ctrl;

extern const unsigned char ecode Verdate[18];
extern const unsigned char ecode RatedVolt[6];
extern const unsigned char ecode RatedCurr[6];
extern const unsigned char ecode MaxCurr[6];
extern const unsigned char ecode ActiveClass[4];
extern const unsigned char ecode MaterType[10];
extern const unsigned char ecode Facdate[10];
extern const unsigned char ecode ProtocolVer[16];
extern const unsigned char ecode SoftwareVer1[32];
extern const unsigned char ecode HardwareVer1[32];
extern const unsigned char ecode FactoryNum[32];
extern const unsigned char ecode Softbk[8];
extern const unsigned char ecode DispVer[3];
extern const unsigned long ecode Def_Meterconst_Bcd;
extern const unsigned char ecode Def_Meterconst_Hex;
extern const float         ecode default_meterconst_dm;
extern const unsigned char ecode Def_Plusevalidtimer;
extern const unsigned char ecode Runet1Outcnt[2];
extern const unsigned char ecode Runrtctmls;
extern const unsigned char ecode resetworking_timer_delay;

extern const uint08 ecode Relaytimer_Delay_ZB;
extern const uint08 ecode Relaytimer_Delay_N;
extern const uint08 ecode Powertimer_Delay_N;	//150ms内置继电器电平延时
extern const uint08 ecode Relaytimer_Delay_W;
extern const uint08 ecode Defaultspacetimer;
extern const uint08 ecode Defrelayerrorcnt_s;
extern const uint08 ecode Defrelayerrorcount;
extern const uint08 ecode Defrelayerrorcount_r;

extern const uint08 ecode Def_Dl_Difference;
extern const uint08 ecode Adjclk_timer_delay_s;
extern const uint08 ecode Adjclk_timer_delay;
extern const uint16 ecode Defendenergyvoltage;
extern const uint08 ecode Runcalbps;
extern const uint08 ecode Def_Shielding_I1;
extern const uint08 ecode Def_Shielding_I2;
extern const uint08 ecode Def_Shielding_Cos;
extern const uint08 ecode Defcurrovercount;
extern const uint08 ecode Deflowvoltagecount;
extern const uint16 ecode Uoffset;
extern const uint08 ecode Def_Powerdetcnt2;
extern const uint08 ecode Def_PowerDetCnt0;

extern unsigned char MterTX_Buff[50];
extern unsigned char MterRX_Buff[50];

extern unsigned char CoverDetCnt;

extern const uint08 ecode Fulllcdbias;
extern const uint08 ecode Fulllcdbias2;
extern const uint08 ecode Runlcdbias;
extern const uint08 ecode Runlcdbias2;
extern const uint08 ecode Sleeplcdbias;
extern const uint08 ecode Sleeplcdfreq;
extern const uint08 ecode Sleeplcdset;
extern const uint08 ecode Sleepenmode;
extern const uint08 ecode Sleeplvdefcfg;
extern const uint08 ecode Defaultauthtimer;

extern const unsigned char code UartErrCnt_const;
extern const unsigned long ecode disp_table[];
extern const unsigned char ecode Def_Limitmenory_table[];
extern const unsigned char ecode Def_DisplayPara_table[];
extern const unsigned char ecode Def_featureword_table[];
extern const unsigned char ecode Def_weekloadpara_table[];
extern const unsigned char Def_Demand_table[];
extern const unsigned char ecode Def_feik_table[];
extern const unsigned char ecode Def_Autodisp_table[16*2];
extern const unsigned char ecode Def_Keydisp_table[32*2];

extern const uint08 code BACKBUF[];
extern const uint16 code BACKADDR[];
extern const union B32_B08_2 code SysATT7051[2];
//南网新增
extern const uint08 ecode Keyswitch_Valid;
extern const uint08 ecode METERTYPE;
extern const uint08 ecode SELFCLOSE;
extern const uint08 ecode default_hearttime;
extern const uint08 ecode Sleepdispdays;
extern const uint08 ecode MAX_PSW_NUM;
extern const uint08 ecode Terminal_delay;
extern const uint08 ecode Defpassworderrcnt;
extern const uint08 ecode Def_GSN;
extern const uint08 ecode Def_CWGSP;
extern const uint08 ecode Def_eventthreshold_table[];
extern uint32				RTC_Status;
extern uint32				KEY_Status;
//---------------------- bit variable ------------------------------------------------------------------------


extern 	B08_Bit bdata			        Meter_Status_S;			      //
#define Meter_Status			        Meter_Status_S.B08      	
#define	remainder_alarm1_flag		  Meter_Status_S.Bit.bit0		//处于报警1状态
#define	remainder_alarm2_flag		  Meter_Status_S.Bit.bit1		//处于报警2状态
#define	remainder_zero_flag		    Meter_Status_S.Bit.bit2		//剩余为零标志
#define	remainder_maxtick_flag		Meter_Status_S.Bit.bit3		//剩余金额到赊欠门限
#define	first_alaram2_flag		    Meter_Status_S.Bit.bit4		//刚处于报警2状态
#define	first_eqzero_flag		      Meter_Status_S.Bit.bit5		//刚处于过零状态
#define	relay_dl_status_flag		  Meter_Status_S.Bit.bit6		//继电器电量控制状态(0表示闭合1表示断开)
#define	relay_commu_close2_flag		Meter_Status_S.Bit.bit7	        //保电解除标志(南网新增)

extern 	B08_Bit bdata			        Meter_Status1_S;		      //(改变需要保存)
#define Meter_Status1			        Meter_Status1_S.B08
#define	relay_status_flag		      Meter_Status1_S.Bit.bit0	//继电器状态(0表示闭合1表示断开)
#define	card_closerelay_flag		  Meter_Status1_S.Bit.bit1	//插卡闭合继电器标志(报警2)
#define	card_closerelay_flag1		  Meter_Status1_S.Bit.bit2	//插卡闭合继电器标志(过零)
#define	charge_relay_open_flag		Meter_Status1_S.Bit.bit3	//购电不闭合继电器标志
#define	relay_commu_alarm_flag		Meter_Status1_S.Bit.bit4	//通讯报警继电器
#define	relay_commu_open_flag		  Meter_Status1_S.Bit.bit5	//通讯断开继电器
#define relay_commu_disable_close_flag	Meter_Status1_S.Bit.bit6	//不允许闭合继电器标志
#define	relay_commu_close_flag		Meter_Status1_S.Bit.bit7	//通讯闭合继电器

extern 	B08_Bit bdata		        	Meter_Status2_S;		      //(改变需要保存)
#define Meter_Status2		        	Meter_Status2_S.B08
#define	relay_success_flag		    Meter_Status2_S.Bit.bit0	//继电器操作成功标志
#define	relay_failed_flag		      Meter_Status2_S.Bit.bit1	//继电器错误标志(上报)
#define	relay_commu_open_1a_flag	Meter_Status2_S.Bit.bit2	//通讯跳闸命令标志
#define	relay_commu_open_1d_flag	Meter_Status2_S.Bit.bit3	//通讯预跳闸命令标志
#define	relay_commu_open_1e_flag	Meter_Status2_S.Bit.bit4	//通讯预跳闸命令标志
#define	relay_cleardl_flag		    Meter_Status2_S.Bit.bit5	//外置表清零标志
#define	Relay_modechange_flag		  Meter_Status2_S.Bit.bit6  //模式改变标志(拉合闸成功不上报)
#define	self_close_flag			      Meter_Status2_S.Bit.bit7  //自保电标志


extern 	B08_Bit bdata			        System_Status_S;		      //(停电不清)
#define System_Status			        System_Status_S.B08
#define	real_relaystatus_flag		  System_Status_S.Bit.bit0	//继电器实际状态标志
#define	remainder_tick_flag		    System_Status_S.Bit.bit1	//处于赊欠状态(上报)
#define	clock_error			          System_Status_S.Bit.bit2	//时钟错误标志
#define	low_voltage_flag		      System_Status_S.Bit.bit3	//电池低压标志
#define	esam_error			          System_Status_S.Bit.bit4	//esam错误标志
#define	eeprom_error_flag		      System_Status_S.Bit.bit5	//存储器故障标志
#define	hangup_comm_flag		      System_Status_S.Bit.bit6	//挂起通讯标志
#define	already_setclock_flag		  System_Status_S.Bit.bit7	//已经校时标志

extern 	B08_Bit bdata			        System_Status1_S;		      //
#define System_Status1			      System_Status1_S.B08		  //(停电清零)
#define	identity_prog_flag		    System_Status1_S.Bit.bit0	//认证编程标志
#define	identity_ir_flag		      System_Status1_S.Bit.bit1	//红外请求标志
#define	display_zaibo_flag		    System_Status1_S.Bit.bit2 //显示载波标志
#define	dl_back_flag			        System_Status1_S.Bit.bit3	//电流反向标志
#define	first_prog_flag			      System_Status1_S.Bit.bit4	//首次编程标志
#define	progenable			          System_Status1_S.Bit.bit5	//编程使能
#define	comm_relaystatus_flag		  System_Status1_S.Bit.bit6	//继电器实际状态标志(通讯专用)
#define	relay_error			          System_Status1_S.Bit.bit7	//继电器错误标志

extern 	B08_Bit bdata			        System_Status2_S;		      //
#define System_Status2			      System_Status2_S.B08
#define	esam_biaohao_flag		      System_Status2_S.Bit.bit0	//已采用esam表号标志
#define	display_rtc_flag		      System_Status2_S.Bit.bit1	//显示时间标志
#define	disp_0_flag			          System_Status2_S.Bit.bit2	//需要灭零处理标志
#define	terminal_auth_flag		    System_Status2_S.Bit.bit3	//交互终端身份认证标志
#define	below_60u_flag			      System_Status2_S.Bit.bit4	//低于60%电压标志
#define	below_70u_flag			      System_Status2_S.Bit.bit5	//低于70%电压标志
#define	relay_direct_flag		      System_Status2_S.Bit.bit6	//继电器直接动作标志
#define	relay_delay_flag		      System_Status2_S.Bit.bit7	//大电流延时跳闸标志

extern 	B08_Bit bdata			        System_Status3_S;
#define System_Status3			      System_Status3_S.B08
#define	run_readrtc_flag		      System_Status3_S.Bit.bit0	//读取实时时钟标志
#define	run_display_flag		      System_Status3_S.Bit.bit1	//刷新LCD显示标志
#define	run_relaychk_flag		      System_Status3_S.Bit.bit2	//继电器检测标志
#define	run_freeze_flag			      System_Status3_S.Bit.bit3 //冻结和结算标志
#define	run_loadchk_flag		      System_Status3_S.Bit.bit4 //负荷记录标志
#define	run_timezone_flag		      System_Status3_S.Bit.bit5	//时区时段表标志
#define	run_price_flag			      System_Status3_S.Bit.bit6	//电价更新标志
#define	run_alarm_flag			      System_Status3_S.Bit.bit7	//报警状态标志

extern 	B08_Bit bdata			        System_Status4_S;
#define System_Status4			      System_Status4_S.B08
#define	fwdl_veri_flag			      System_Status4_S.Bit.bit0	//正向电量校验标志
#define	bkdl_veri_flag			      System_Status4_S.Bit.bit1	//反向电量校验标志
#define	mode_veri_flag			      System_Status4_S.Bit.bit2	//特征模式字校验标志
#define	reg_veri_flag			        System_Status4_S.Bit.bit3 //寄存器校验标志
#define	addr_veri_flag			      System_Status4_S.Bit.bit4	//通信地址校验标志
#define	disppm_veri_flag		      System_Status4_S.Bit.bit5	//显示参数校验标志
#define	dispitem_veri_flag		    System_Status4_S.Bit.bit6	//显示项目校验标志
#define	temperpara_veri_flag		  System_Status4_S.Bit.bit7	//温度系数校验标志

extern 	B08_Bit bdata			        System_Status5_S;		      //在中断里置位和清零,主循环不操作
#define System_Status5			      System_Status5_S.B08
#define	BDMODE		                System_Status5_S.Bit.bit0
#define	rf_card_flag		          System_Status5_S.Bit.bit1 //射频寻卡标志
#define Festival_holiday_flag	    System_Status5_S.Bit.bit2 //接地故障标志
#define	KeySTS_OK_flag		        System_Status5_S.Bit.bit3 //STS_Key更新成功标志
#define Over_voltage              System_Status5_S.Bit.bit4 //接地故障标志
#define dl_back_flag1             System_Status5_S.Bit.bit5 //首次反向标志
#define UartErrCnt                System_Status5_S.Bit.bit6

extern 	B08_Bit bdata			        System_Status6;		      //需量操作相关标志
#define Dm_process_flag           System_Status6.Bit.bit0 //需量处理标志
#define Dm_veri_flag              System_Status6.Bit.bit1 //需量处理标志





extern B08_Bit bdata			        Ic_Type_Reg_S;
#define Ic_Type_Reg			          Ic_Type_Reg_S.B08
#define	ic_display			          Ic_Type_Reg_S.Bit.bit4		//显示有卡插入
#define disp_remain_flag	        Ic_Type_Reg_S.Bit.bit5		//显示购电后剩余金额标志
#define	disp_readic_flag	        Ic_Type_Reg_S.Bit.bit6		//显示读卡中
#define	disp_over_flag		        Ic_Type_Reg_S.Bit.bit7		//显示购电前剩余金额透支标志（负号）

#define	UserCardSerialBit		      Ic_Type_Reg_S.Bit.bit0		//表卡序列号一致, 0:一致; 1:不一致
#define	UserCardNoNull 		        Ic_Type_Reg_S.Bit.bit1		//返写文件不为空, 0:空 ； 1：不空
#define	card_open_flag		        Ic_Type_Reg_S.Bit.bit2		//本地已开户标志
#define	comm_open_flag  	        Ic_Type_Reg_S.Bit.bit3		//远程已开户标志

extern B08_Bit bdata			        AT7051_Flag_S;	
#define AT7051_Flag			          AT7051_Flag_S.B08
#define	read_back_flag		        AT7051_Flag_S.Bit.bit0		//读反向电量标志
#define	init_7051_flag		        AT7051_Flag_S.Bit.bit1		//初始化AT7051标志1
#define	read_uip_flag1		        AT7051_Flag_S.Bit.bit2		//读AT7051瞬时值标志1
#define	check_7051_flag1	        AT7051_Flag_S.Bit.bit3		//校验AT7051调教值标志1
#define	load_over_flag		        AT7051_Flag_S.Bit.bit4		//过流标志
#define	over_xxx_flag			        AT7051_Flag_S.Bit.bit5		//拉闸电流超标志
#define	power_zero_flag		        AT7051_Flag_S.Bit.bit6		//功率等于0标志
#define	energy_rst_flag		        AT7051_Flag_S.Bit.bit7		//计量芯片复位标志



//;=============================< IDATA 存储定义 >================================
extern 	uint08			bdata	DisKeySamp;	//显示按键去抖寄存器
extern 	uint08			bdata	DisKeySamp1;	//显示按键去抖寄存器
extern 	uint08			bdata	CardKeySamp;	//插卡去抖寄存器
extern 	uint08			bdata	Uart1KeySamp;	//485极性检测去抖寄存器
extern 	uint08		  idata	VectorTimer;	//毫秒定时器
extern 	struct 	    SYS_CLOCK_FRM	idata	Sclock;		//时钟数据
extern 	uint08			idata	UpdateMoneytimer;//更新ESAM钱包定时器
extern 	uint08			idata	Key_Status;//密钥状态,00表示全部公钥,01表示全部私钥,FF表有公有私

//;=============================< XDATA 存储定义 >================================
extern 	struct      TIMER_FRM	near	Progkey;	//240min编程(停电运行)
extern 	struct      TIMER_FRM	near	Cover;		//开盖参数(停电保存)
extern 	uint08			near	Mac_Err_Times;	//MAC校验失败次数
extern 	uint08			near	System_St_Bk;	//系统状态字备份(停电运行)
extern 	uint08			near	Report_Data[5];	//有效上报状态字
extern 	uint08			near	Report_Data2[5];//有效上报状态字备份
extern 	uint08			near	Report_Num[40];	//事件上报新增次数
extern 	uint16			near	Reset_Rep_Timer;//复位上报状态字定时器
extern 	uint08			near	RReport_Status;	//之前已读上报状态允许复位上报状态
extern  uint08			near	Password_Err[2];//密码错误次数
extern 	struct	    LOADREC_FRM	near	Load;		//负荷指针共18字节
extern 	uint08			near	Wr_Ext_Number;	//内部扩展命令写ee允许次数
extern 	uint16			near	Self_Timer24;	//自保电24小时定时器
extern 	struct	    MONEY_FRM	near	Money;		//剩余金额(6)+透支金额(6)+购电次数(4)+Crc(2)
extern  struct      USERINFOR_FRM  Money_UserINFO;	//上一天使用金额 BCD 4 ＋  //剩余金额可用天数BCD	
extern 	struct	    FWDL_FRM	near	Fwdl;		//正向有功总电量(4)＋尖电量(4)＋峰电量(4)＋平电量(4)＋谷电量(4)＋本月正向有功用电量(4)＋Crc(2)
extern 	struct	    BKDL_FRM	near	Bkdl;		//反向有功总电量(4)＋尖电量(4)＋峰电量(4)＋平电量(4)＋谷电量(4)＋本月反向有功用电量(4)＋Crc(2)
extern	uint08			near	RAMREG0;	//定义特殊寄存器 
extern	uint08			near	RAMREG1;	//
extern	uint08			near	RAMREG2;	//
extern	uint08			near	RAMREG3;	//
extern	uint08			near	RAMREG4;	//
extern	uint08			near	RAMREG5;	//
extern	uint08			near	RAMREG6;	//
extern	uint08			near	RAMREG7;	//
extern	uint08			near	RAMREG8;	//
extern	uint08			near	RAMREG9;	//
extern	uint08			near	RAMREG10;	//
extern	uint08			near	RAMREG11;	//
extern	uint08			near	RAMREG12;	//

extern APDU_ANALYZE_VAR      gs_APDU_analyze_var;
extern APDU_KEY_EXRAM_VAR    gs_APDU_key_var;
extern APDU_KEY_EXRAM_VAR   sts_kay_process_data;

extern STS_KAY_CHANHE_DATA   sts_kay_change_data;
extern  uint08			near 	PLC_RST1_Timer;
extern 	uint08			near	Outputtype;	//0-秒脉冲,1-需量,2-时段投切信号,0x50-FOUT输出
extern 	struct      TIMER_FRM	near	Successled,Shiduanctrl,Relayctrl,Powertimer;
extern 	struct      TIMER_FRM	near	Adcctrl;
extern 	union       B16_B08		near	Lcdled;//背光定时器
extern	union   B32_B08			Current_Demand;//当前需量(低字节在前)
extern 	union       B32_B08		near	Identitytimer;	//安全认证定时器
extern 	uint16			near	IR_EnTimer;	//红外认证定时器
extern 	union       B32_B08		near	Opendelaytimer;	//跳闸延时定时器
extern 	union	      B32_B08		near	CurrentTimer;	//超拉闸电流定时器
extern 	uint08			near	Relayerror_Count,Relayerror_Count1,Relayerror_Count2;//继电器错误计数器
extern 	uint08			near	Relay_Space_Timer;//上电延时控制继电器定时器
extern 	uint08			near	Keyclose_timer;//按键闭合继电器定时器
extern 	uint08			near	Display_Space_Timer;//红外控制显示间隔定时器
extern 	uint16      near	Stop_Disptimer;//停电显示时间定时
extern 	signed char		near	Temperature_Hex;//备份温度(hex码)
extern	union   B16_B08 	near	Temperature_Adc;//当前温度ADC值(hex码)
//extern	uint08			near	Low_Voltage_Num;//电池欠压去抖次数
//extern 	unsigned long int	near	Voltage_Time_Inc;//电池工作时间
extern	float			near	Rtcadj_Offset;//温度补偿调校值
extern	uint08			near	RtcStopProcFlag;//停止温补标志
extern 	uint16		near	Hw_Lcdled_Timer;//红外点亮背光时间
extern 	uint08			near	Beep_Timer;//蜂鸣器控制定时器
extern 	uint08			near	Ext_Disp_Cnt;//扩展LCD查询命令
extern 	uint08			near	Init_Led_Timer;//点亮指示灯定时器
extern 	uint08			near	Cover_Space_Timer;//开盖延时定时器
extern 	uint08			near	RdLoadPara[3][22]; //数据标识(4)+剩余记录块数(1)+后续帧序号(1)+起始地址(8)++剩余有效记录数(8)
extern  uint08			near	PowerDetCnt0,PowerDetCnt1,PowerDetCnt2,PowerDetCnt3;//电源检测计数器
extern	uint08			near	Resetworking_Timer;//载波复位时间定时器
extern	uint08			near	Current_Over_Count;//超拉闸电流限值时间计数器
extern	uint08			near	Current_Over_Count2;//过流持续时间计数器
extern	uint08			near	Voltage_Low_Count;//低压持续时间计数器
//extern	uint08			near	Last_Timer;	//预跳闸持续时间
//extern	unsigned long int	near	CloseDelayTimer;//跳闸后延时合闸定时器
extern	union	B32_B08		near	Loadtime;	//总过载累计时间(低3字节有效)
extern	uint08			near	Adj_Clk_Timer_S;//定时8M时钟调校定时器(计量uart专用)
extern	uint08			near	Adj_Clk_Timer;	//定时8M时钟调校定时器

extern	uint16		near		Self_Timer3;	//心跳帧间隔时间定时器
extern	uint08		near		Self_Cnt;	//心跳帧次数
extern  uint08  	near		Bat_Delay_Timer;//电池欠压延时时间定时器
extern  uint08		near		Find_Card;
extern  uint08		near		Threshold[8];	//事件判断设定范围:停电事件电压触发、停电电流触发、停电延时时间,开表盖延时时间,欠压事件触发下限、欠压延时时间
extern  uint08		near		F_Plcevtout_Sta;//PLCEVTOUT脚状态
extern  uint08		near		Zb_Relayctrl_Timer;//载波拉合闸延时定时器
extern uint32				near	DispBuf[40];	//?????
//extern  uint08				near	DispBuf[160];	//显示缓冲区
extern 	struct	PARA_ESAM_FRM		near	Esam_Para;	//报警金额1限值(4)＋报警金额2限值(4)+变比(6)＋表号(6)+客户编号(6)+电卡类型(1)+身份认证时效(2)+Crc(2)
extern 	struct 	PARA_LIMITMENORY_FRM	near	Money_Para;	//透支金额限值(4)＋囤积金额限值(4)＋合闸允许金额限值(4)＋Crc(2)
extern 	struct	PARA_DISPLAY_FRM	near	Display_Para;	//循显屏数(1)＋每屏显示时间(1)＋显示电能小数位数(1) ＋波特率(1)+ 跳闸延时时间（2）＋crc（2）
extern 	struct 	COMM_ADDRESS_FRM	near	Comm_Addr;	//通信地址(6) ＋crc（2）
extern 	struct 	STARTTIME_FRM		near	Starttime;	//两套时区表切换时间（5）＋两套日时段表切换时间（5）＋两套费率电价切换时间（5）＋两套阶梯切换时间（5）＋crc（2）
extern 	struct 	EMU_FRM			near	Emu_Para;	//增益1(2)＋相位1(1)＋增益2(2)＋相位2(1)+电流2增益(2)+速率(2)＋电压系数(4)＋电流系数(4)＋电流系数(4)＋Crc(2)
extern 	struct	PRICE_FRM		near	Price;		//当前阶梯电量（16）＋crc（2）＋当前阶梯电价（20）＋crc（2）＋当前费率电价表（16）＋crc（2）
extern 	struct 	TARIFF_FRM		near	Tariff;		//当前费率时段表（3*14）＋crc（2）
extern 	struct 	ITEM_FRM		near	Item[4];	//循环/按键显示项目（1*62）＋crc（2）
extern 	struct	UIP_FRM			near	Emu_Data;	//电压（3）+电流1（3）电流2（3）+功率1（3）+功率2（3）+功率因数（2）
extern 	struct  FUNCTION_FRM		near	Fun_Para;	//性能参数
extern 	struct	DECIMAL_FRM		near	Decimal;	//脉冲尾数数据
extern 	struct 	DISPALY_FRM		near	Display;
extern 	struct 	EEPROM_FRM		near	Eeprom;
extern 	struct 	COMMUNICATION_FRM	near	Uart[3];
extern 	struct	WEEK_FRM		near	Week_Holiday;
extern 	struct	TEMPERATURE_FRM		near	Tpara;
extern 	struct 	METERCONST_FRM		near	Meter;
extern 	struct 	PARA_FEATUREWORD_FRM	near	Mode;
extern	struct	ADCDATA_FRM		near	ADCResultArr;
extern	struct	RTC_TRIMA_FRM		near	RtcTadcBuf;
extern	struct	RTC_TRIMC_FRM		near	RtcTcapBuf;
extern 	struct	FEIK_FRM		near	Feik;
extern  struct  TIMER2_FRM		near  	Keeptime;	//过流持续时间，电池欠压累计时间，掉电累计时间
extern struct   Att7051Eep              Att7051EepD;
extern  union   Att7051Para              Att7051_Calib;

extern 	struct 	EMU_FRM			Emu_Para,Emu_Para1;	//??1(2)+??1(1)+??2(2)+??2(1)+??2??(2)+??(2)+????(4)+????(4)+????(4)+Crc(2)

//demand
extern unsigned char Pav_Cnt ;//计算用需量周期 
extern float		Pav_Temp			;	//计算1分钟平均功率
extern uint08				Slidingtimer,Tar_Slidingtimer;//滑差定时(总和费率)
extern uint08				Demandtimer,Tar_Demandtimer;//最大需量定时(总和费率)
extern unsigned short			Demand[61];//总需量脉冲数脉冲数(滑差时间内)
extern unsigned short			Tar_Demand[61];//费率需量脉冲数
extern struct PARA_DEMAND_FRM    Demand_Para;
extern uint08		Back_Num;	
//uart-u7816
extern  unsigned char 	near	UID[4];

extern  unsigned char 	Charge_Token[20]	;
extern  unsigned char 	KeyH_Token[20]	;
extern  unsigned char 	KeyL_Token[20]	;

extern  unsigned char 	KeyA_M1[6]	;
extern  unsigned char 	KeyB_M1[6]	;

extern const uint16 code Rly_voltage;		//?????????
extern const unsigned long int code METER_PWZEROVAUL;
extern const unsigned long int code METER_I1ZEROVAUL;
extern const unsigned char code METER_BACKCNT ;
extern  unsigned char 	Key_User[8]	;
extern const uint16 code Defaultpdvoltage ;	//???????(60%Un)
extern const uint16 code Deflowledvoltage;	//
extern const uint08 code Deflowvoltagecount;	//????????????(????60%????)
extern uint08 Flg_60lev,Flg_53lev,Flg_35lev;
extern uint08			near 	PowerLow;



extern uint08			near 	RFTESTCNT;
extern 	unsigned char 	near	Ic_Comm_RxBuf[u7816_buf_size]	;	//7816接收缓冲区
extern 	unsigned char 	near	Ic_Comm_RxPtr			;	//7816接收指针
extern 	unsigned char 	near	Ic_Comm_TxBuf[u7816_buf_size]	;	//7816发送缓冲区
extern 	unsigned char 	near	Ic_Comm_TxPtr			;	//7816接收指针

extern union B32_B08 	near	Buy_Total_Money		;	//购电总金额
extern  unsigned char 	near	Refers_File_Mac[180]		;	//参数文件+MAC(480字节)                       
extern  union B32_B08 	near	Buy_Money_Mac[3]		;	//剩余金额+MAC
extern  union B32_B08 	near	Buy_Times_Mac[2]		;	//购电次数+MAC
extern  unsigned char 	near	BuyTimesFlag			;	//购电次数标志（00:相等；01:差1；2:超囤积；其它:非法）
extern  unsigned char 	near	Err_Sw12[2]			;	//暂存错误命令返回的SW1和SW2

extern 	unsigned char	near	PcRst8B[8]			;	//PC远程下发的 分散因子 8字节
extern 	unsigned char	near	PcRand[8]			;	//PC远程下发的 随机数1  8字节
extern 	unsigned char	near	EsamRst8B[8]			;	//ESAM复位信息中后8字节
extern 	unsigned char	near	EsamRand[8]			;	//ESAM 8字节随机数
extern 	unsigned char	near	Esam_Timer			;	//ESAM上电延时复位定时器

//u7816
extern 	unsigned char 	near	Remain_back[24]			;	//表内剩余金额和购电次数 BCD+HEX+BCD
extern 	unsigned char 	near	Buy_Money_Bcd[4]		;	//充值金额     BCD
extern 	union B32_B08 	near	Buy_Times			;	//购电次数     HEX

extern 	unsigned char	near	CardRst8B[8]			;	//CARD复位信息中后8字节
extern 	unsigned char	near	CardRand[8]			;	//CARD 8字节随机数
extern 	unsigned char	near	CardCheck			;	//卡检测
extern  unsigned char	near	CardCheck1			;	//寻卡命令检测
extern 	unsigned char	near	CardType			;	//卡类型
extern  unsigned char	near  	UpdataFlag			;	//更新标志
extern 	unsigned char	near	Ic_Err_Reg			;	//IC卡操作错误标识
extern  unsigned char	near	UserCardFlag			;	//用户卡类型标志
extern  unsigned char	near	InsertCardSta			;	//插卡状态字

extern uint32  FLAG_EA;
extern unsigned char	near	RTCIF	;
extern unsigned char  IICPowerON;

extern unsigned char  RN8302Status;
extern unsigned char  RN8302Errcum;
extern unsigned char  Second_8302;
extern unsigned char  Second_Power;
extern unsigned char  PowerRead_Flag;
extern unsigned char  Quadrant[4];//象限指示
extern unsigned char  Angle_I;//角度测量
extern unsigned char  U_InvOrder_Flag;//电压逆相序状态位
extern unsigned char  I_InvOrder_Flag;//电流逆相序状态位
extern unsigned char  FactoryCFDEN;
extern unsigned long  RN8302Checksum;
extern unsigned char  PowerReact1_Flag;
extern unsigned char  PowerReact2_Flag;

extern ADJ_BLOCK Adj_Block[3];
extern POWERVALUE PowerValue;
extern CHIPREG ChipReg;
extern REALADJ RealAdj;
extern	REAL Real;
extern	MEASPARA  MeasPara;
extern	   FLAG	     Flag;
extern	TIMER	Timer;
extern const FACTOR ecode Factor;
extern const METERSPECIAL ecode Meterspecial;
extern const METERPARAMETER ecode MeterParameter;
extern const unsigned char FirmWareVersion[50];
extern const unsigned char FirmWareRecord[8];
extern const unsigned char HardWareVersion[32];



extern	MAXDMD_BLOCK  MaxDmd_Blk[8];//正向、反向、组合无功1、2，4象限无功有功最大需量
extern	CURRENTDEMAND  CurrDmd;//当前需量
extern	PULSECOUNT_BLOCK  PCnt_Blk;
extern	VARENERY_BLOCK  VEngy_Blk;
extern	PHASEENERGY_BLOCK  PEngy_Blk;
extern	RACTENERGY_BLOCK  REngy_Blk;
extern	ACTENERGY_BLOCK  AEngy_Blk;
extern CYCENERY CycEnery;
extern MONTHENERY MonthEnery;
extern YEARENERY YearEnery;
extern	MEASUREMODE  MeasureMode;
extern NVM1KI Nvm1Ki;
extern ADJ_BLOCK2 Adj_Block2[3];


extern const  signed short  ecode  g_PerKwh;
extern const  signed short  ecode g_Per1Kwh;
 
extern  unsigned char FramGrpWriteFlag;
extern  unsigned char E2PGrpWriteFlag;
extern  unsigned char FramWriteFlag;
extern  unsigned char E2PWriteFlag;


//extern 	unsigned char 	bdata	Ic_Type_Reg			;		
//extern 	unsigned char 	bdata	AT7051_Flag			;	

#endif
