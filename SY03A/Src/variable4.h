//variable.h


#include "define4.h"

const uint16 ecode BPS_SPBRGL1[4] = { 1200, 2400, 4800, 9600 };		//T2初始化字低位@8MHz

//---------------------- bit variable --------------------
B08_Bit			bdata	Meter_Status_S;   		//
B08_Bit			bdata	Meter_Status1_S;		//(改变需要保存)
B08_Bit			bdata	Meter_Status2_S;		//(改变需要保存)
B08_Bit			bdata	System_Status_S;		//(停电不清)
B08_Bit			bdata	System_Status1_S;				
B08_Bit			bdata	System_Status2_S;
B08_Bit			bdata	System_Status3_S;
B08_Bit			bdata	System_Status4_S;
B08_Bit			bdata	System_Status5_S;
B08_Bit			bdata	Ic_Type_Reg_S;

B08_Bit bdata			        System_Status6;		      //需量操作相关标志
B08_Bit			bdata	AT7051_Flag_S;

//;=============================< IDATA 存储定义 >================================
uint08			bdata	DisKeySamp;	//显示按键去抖寄存器
uint08			bdata	DisKeySamp1;	//显示按键去抖寄存器
uint08			bdata	CardKeySamp;	//插卡去抖寄存器
uint08			bdata	Uart1KeySamp;	//485极性检测去抖寄存器
uint08			idata	VectorTimer;	//毫秒定时器
struct 	SYS_CLOCK_FRM	idata	Sclock;		//时钟数据
uint08			idata	UpdateMoneytimer;//更新ESAM钱包定时器
uint08			idata	Key_Status;//密钥状态,00表示全部公钥,01表示全部私钥,FF表有公有私

//;=============================< XDATA 存储定义 >================================
struct  TIMER_FRM	near	Progkey	;	//240min编程(停电运行)
struct  TIMER_FRM	near	Cover		;	//开盖参数(停电保存) 
uint08			near 	Mac_Err_Times	;	//MAC校验失败次数
uint08			near 	System_St_Bk	;	//系统状态字备份(停电运行)
uint08			near 	Report_Data[5]	;	//有效上报状态字
uint08			near 	Report_Data2[5]	;	//有效上报状态字备份
uint08			near 	Report_Num[40]	;	//事件上报新增次数
uint16			near 	Reset_Rep_Timer;	//复位上报状态字定时器
uint08			near 	RReport_Status;	//之前已读上报状态允许复位上报状态
uint08			near	Password_Err[2];	//密码错误次数
struct	LOADREC_FRM	near	Load	;	//负荷指针共18字节
uint08			near	Wr_Ext_Number;	//内部扩展命令写ee允许次数
uint16			near	Self_Timer24;	//自保电24小时定时器
struct	MONEY_FRM	near	Money	;	//剩余金额(6)+透支金额(6)+购电次数(4)+Crc(2)

struct USERINFOR_FRM  Money_UserINFO;	//上一天使用金额 BCD 4 ＋  //剩余金额可用天数BCD	
struct	FWDL_FRM	near	Fwdl;	//正向有功总电量(4)＋尖电量(4)＋峰电量(4)＋平电量(4)＋谷电量(4)＋本月正向有功用电量(4)＋Crc(2)
struct	BKDL_FRM	near	Bkdl;	//反向有功总电量(4)＋尖电量(4)＋峰电量(4)＋平电量(4)＋谷电量(4)＋本月反向有功用电量(4)＋Crc(2)
uint08			near	RAMREG0	;	//定义特殊寄存器
uint08			near	RAMREG1	;	//
uint08			near	RAMREG2		;	//
uint08			near	RAMREG3		;	//
uint08			near	RAMREG4	;	//
uint08			near	RAMREG5		;	//
uint08			near	RAMREG6		;	//
uint08			near	RAMREG7		;	//
uint08			near	RAMREG8		;	//
uint08			near	RAMREG9		;	//
uint08			near	RAMREG10	;	//
uint08			near	RAMREG11	;	//
uint08			near	RAMREG12	;	//
uint32			RTC_Status;
uint32			KEY_Status;

APDU_ANALYZE_VAR      gs_APDU_analyze_var;
APDU_KEY_EXRAM_VAR    gs_APDU_key_var;
STS_KAY_CHANHE_DATA   sts_kay_change_data;
APDU_KEY_EXRAM_VAR   sts_kay_process_data;

uint08			near 	PLC_RST1_Timer;
uint08			near 	Outputtype;	//0-秒脉冲,1-需量,2-时段投切信号,0x50-FOUT输出
struct  TIMER_FRM	near	Successled,Shiduanctrl,Relayctrl,Powertimer;
struct  TIMER_FRM	near	Adcctrl;	//adc转换时间间隔定时器
union   B16_B08		near	Lcdled;		//背光定时器
union   B32_B08			Current_Demand;//当前需量(低字节在前)
union   B32_B08		near	Identitytimer;	//安全认证定时器
uint16			near 	IR_EnTimer;	//红外认证定时器
union   B32_B08		near	Opendelaytimer;//跳闸延时定时器
union	B32_B08		near	CurrentTimer;	//超拉闸电流定时器
uint08			near	Relayerror_Count,Relayerror_Count1,Relayerror_Count2;//继电器错误计数器
uint08			near	Relay_Space_Timer;//上电延时控制继电器定时器
uint08			near	Keyclose_timer;//按键闭合继电器定时器
uint08			near	Display_Space_Timer;//红外控制显示间隔定时器
uint16		near	Stop_Disptimer;//停电显示时间定时
signed char		near	Temperature_Hex;//备份温度(hex码)
union   B16_B08 	near	Temperature_Adc;//当前温度ADC值(hex码)
//uint08			near	Low_Voltage_Num;//电池欠压去抖次数
//unsigned long int	near	Voltage_Time_Inc;//电池工作时间
float			near	Rtcadj_Offset;//温度补偿调校值
uint08			near	RtcStopProcFlag;//停止温补标志
uint16		near	Hw_Lcdled_Timer;//红外点亮背光时间
uint08			near	Beep_Timer;//蜂鸣器控制定时器
uint08			near	Ext_Disp_Cnt;	//扩展LCD查询命令
uint08			near	Init_Led_Timer;//点亮指示灯定时器
uint08			near	Cover_Space_Timer;//开盖延时定时器
uint08			near	RdLoadPara[3][22]; //数据标识(4)+剩余记录块数(1)+后续帧序号(1)+起始地址(8)+剩余有效记录数(8)
uint08			near	PowerDetCnt0,PowerDetCnt1,PowerDetCnt2,PowerDetCnt3;	//电源检测计数器
uint08			near	Resetworking_Timer;//载波复位时间定时器
uint08			near	Current_Over_Count;//超拉闸电流限值时间计数器
uint08			near	Current_Over_Count2;//过流持续时间计数器
uint08			near	Voltage_Low_Count;//低压持续时间计数器
//uint08			near	Last_Timer;	//预跳闸持续时间
//unsigned long int	near	CloseDelayTimer;//跳闸后延时合闸定时器
union   B32_B08		near	Loadtime;	//总过载累计时间(低3字节有效)
uint08			near	Adj_Clk_Timer_S;//定时8M时钟调校定时器(计量uart专用)
uint08			near	Adj_Clk_Timer;	//定时8M时钟调校定时器

uint16		near		Self_Timer3;	//心跳帧间隔时间定时器
uint08		near		Self_Cnt;	//心跳帧次数
uint08  	near		Bat_Delay_Timer;//电池欠压延时时间定时器
uint08		near		Find_Card;	//寻卡标志：0x55启动寻卡；0xaa禁止寻卡；0x00正常寻卡
uint08		near		Threshold[8];	//事件判断设定范围:停电事件电压触发、停电电流触发、停电延时时间,开表盖延时时间,欠压事件触发下限、欠压延时时间
uint08		near		F_Plcevtout_Sta;//PLCEVTOUT脚状态
uint08		near		Zb_Relayctrl_Timer;//载波拉合闸延时定时器
uint32				near	DispBuf[40];	//?????
//uint08				near	DispBuf[160];	//显示缓冲区
struct	PARA_ESAM_FRM		near	Esam_Para;	//报警金额1限值(4)＋报警金额2限值(4)+变比(6)＋表号(6)+客户编号(6)+电卡类型(1)+身份认证时效(2)+Crc(2)
struct 	PARA_LIMITMENORY_FRM	near	Money_Para;	//透支金额限值(4)＋囤积金额限值(4)＋合闸允许金额限值(4)＋Crc(2)
struct	PARA_DISPLAY_FRM	near	Display_Para;	//循显屏数(1)＋每屏显示时间(1)＋显示电能小数位数(1) ＋波特率(1)+ 跳闸延时时间（2）＋crc（2）
struct 	COMM_ADDRESS_FRM	near	Comm_Addr;	//通信地址(6) ＋crc（2）
struct 	STARTTIME_FRM		near	Starttime;	//两套时区表切换时间（5）＋两套日时段表切换时间（5）＋两套费率电价切换时间（5）＋两套阶梯切换时间（5）＋crc（2）
struct 	EMU_FRM			near	Emu_Para;	//增益1(2)＋相位1(1)＋增益2(2)＋相位2(1)+电流2增益(2)+速率(2)＋电压系数(4)＋电流系数(4)＋电流系数(4)＋Crc(2)
struct	PRICE_FRM		near	Price;		//当前阶梯电量（16）＋crc（2）＋当前阶梯电价（20）＋crc（2）＋当前费率电价表（16）＋crc（2）
struct 	TARIFF_FRM		near	Tariff;		//当前费率时段表（3*14）＋crc（2）
struct 	ITEM_FRM		near	Item[4];	//循环/按键显示项目（1*62）＋crc（2）
struct	UIP_FRM			near	Emu_Data;	//电压（3）+电流1（3）电流2（3）+功率1（3）+功率2（3）+功率因数（2）
struct  FUNCTION_FRM		near	Fun_Para;	//性能参数
struct	DECIMAL_FRM		near	Decimal;	//脉冲尾数数据
struct 	DISPALY_FRM		near	Display;
struct 	EEPROM_FRM		near	Eeprom;
struct 	COMMUNICATION_FRM	near	Uart[3];
struct	WEEK_FRM		near	Week_Holiday;
struct	TEMPERATURE_FRM		near	Tpara;
struct 	METERCONST_FRM		near	Meter;
struct 	PARA_FEATUREWORD_FRM	near	Mode;
struct	ADCDATA_FRM		near	ADCResultArr;
struct	RTC_TRIMA_FRM		near	RtcTadcBuf;
struct	RTC_TRIMC_FRM		near	RtcTcapBuf;
struct	FEIK_FRM		near	Feik;		//年时区数+日时段表数+日时段数+费率数+公共假日数+谐波次数+阶梯数+密钥总条数+CRC(2)
struct  TIMER2_FRM		near	Keeptime;	//过流持续时间，电池欠压累计时间，掉电累计时间

struct 	EMU_FRM			Emu_Para,Emu_Para1;	//??1(2)+??1(1)+??2(2)+??2(1)+??2??(2)+??(2)+????(4)+????(4)+????(4)+Crc(2)
struct   Att7051Eep              Att7051EepD;
union   Att7051Para              Att7051_Calib;
unsigned char MterTX_Buff[50];
unsigned char MterRX_Buff[50];

uint08			near 	RFTESTCNT;
uint08			near 	PowerLow;
unsigned char CoverDetCnt;
uint08 Flg_60lev,Flg_53lev,Flg_35lev;

//uart-u7816
unsigned char 	near	Ic_Comm_RxBuf[u7816_buf_size]	;	//7816接收缓冲区
unsigned char 	near	Ic_Comm_RxPtr			;	//7816接收指针
unsigned char 	near	Ic_Comm_TxBuf[u7816_buf_size]	;	//7816发送缓冲区
unsigned char 	near	Ic_Comm_TxPtr			;	//7816接收指针

unsigned char 	near	UID[4]	;
unsigned char 	Charge_Token[20]	;
unsigned char 	KeyH_Token[20]	;
unsigned char 	KeyL_Token[20]	;

unsigned char 	KeyA_M1[6]	;
unsigned char 	KeyB_M1[6]	;
unsigned char 	Key_User[8]	;

union B32_B08 	near	Buy_Total_Money		;	//购电总金额
unsigned char 	near	Refers_File_Mac[180]		;	//参数文件+MAC(180字节)
union B32_B08 	near	Buy_Money_Mac[3]		;	//剩余金额+MAC
union B32_B08 	near	Buy_Times_Mac[2]		;	//购电次数+MAC
unsigned char 	near	BuyTimesFlag			;	//购电次数标志（00:相等；01:差1；2:超囤积；其它:非法）
unsigned char 	near	Err_Sw12[2]			;	//暂存错误命令返回的SW1和SW2

unsigned char	near	PcRst8B[8]			;	//PC远程下发的 分散因子 8字节
unsigned char	near	PcRand[8]			;	//PC远程下发的 随机数1  8字节
unsigned char	near	EsamRst8B[8]			;	//ESAM复位信息中后8字节
unsigned char	near	EsamRand[8]			;	//ESAM 8字节随机数
unsigned char	near	Esam_Timer			;	//ESAM上电延时复位定时器

//demand
unsigned char Pav_Cnt ;//计算用需量周期 
float		Pav_Temp			;	//计算1分钟平均功率
uint08				Slidingtimer,Tar_Slidingtimer;//滑差定时(总和费率)
uint08				Demandtimer,Tar_Demandtimer;//最大需量定时(总和费率)
unsigned short			Demand[61];//总需量脉冲数脉冲数(滑差时间内)
unsigned short			Tar_Demand[61];//费率需量脉冲数(滑差时间内)		
struct PARA_DEMAND_FRM    Demand_Para;
uint08		Back_Num;		
//u7816
unsigned char 	near	Remain_back[24]			;	//表内剩余金额和购电次数 BCD+HEX+BCD
unsigned char 	near	Buy_Money_Bcd[4]		;	//充值金额     BCD
union B32_B08 	near	Buy_Times			;	//购电次数     HEX

unsigned char	near	CardRst8B[8]			;	//CARD复位信息中后4字节
unsigned char	near	CardRand[8]			;	//CARD 8字节随机数
unsigned char	near	CardCheck			;	//卡检测
unsigned char	near	CardCheck1			;	//寻卡命令检测
unsigned char	near	CardType			;	//卡类型
unsigned char	near  	UpdataFlag			;	//更新标志
unsigned char	near	Ic_Err_Reg			;	//IC卡操作错误标识
unsigned char	near	UserCardFlag			;	//用户卡类型标志
unsigned char	near	InsertCardSta			;	//插卡状态字
uint32 FLAG_EA;
unsigned char	near	RTCIF		;

//unsigned char  EA;
unsigned char  IICPowerON;



unsigned char  RN8302Status;
unsigned char  RN8302Errcum;

unsigned char  Second_8302;
 unsigned char  Second_Power;
 unsigned char  PowerRead_Flag;
 unsigned char  Quadrant[4];//象限指示
 unsigned char  Angle_I;//角度测量
 unsigned char  U_InvOrder_Flag;//电压逆相序状态位
 unsigned char  I_InvOrder_Flag;//电流逆相序状态位
 unsigned char  FactoryCFDEN;
 unsigned long  RN8302Checksum;
 unsigned char  PowerReact1_Flag;
 unsigned char  PowerReact2_Flag;
CHIPREG ChipReg;
REALADJ RealAdj;
REAL    Real;
MEASPARA             MeasPara;
TIMER	Timer;
FLAG	     Flag;

ADJ_BLOCK Adj_Block[3];
POWERVALUE PowerValue;
CHIPREG ChipReg;
ADJ_BLOCK2 Adj_Block2[3];
//const METERPARAMETER MeterParameter;

MAXDMD_BLOCK  MaxDmd_Blk[8];
CURRENTDEMAND  CurrDmd;
PULSECOUNT_BLOCK  PCnt_Blk;
VARENERY_BLOCK  VEngy_Blk;
PHASEENERGY_BLOCK  PEngy_Blk;
RACTENERGY_BLOCK  REngy_Blk;
ACTENERGY_BLOCK  AEngy_Blk;
CYCENERY CycEnery;
MONTHENERY MonthEnery;
YEARENERY YearEnery;
MEASUREMODE  MeasureMode;

NVM1KI Nvm1Ki;
unsigned char FramGrpWriteFlag;
unsigned char E2PGrpWriteFlag;
unsigned char FramWriteFlag;
unsigned char E2PWriteFlag;

