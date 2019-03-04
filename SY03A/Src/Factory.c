#include "ex_var.h"
#include "ex_func.h"
#include <string.h>

const union B32_B08_2 code SysATT7051[2]=
{     
      0x00001643,//     80
      0x00000003,// MT0 83     
};
//主频选择
#define	MAINCLOCK	4	//系统主频,单位M
#define  MeterPara_Mode 5

#if( MAINCLOCK == 8 )
const uint08 ecode runwdtcon = 0x07;//12分频,cpuwdt定时1.57s
const uint08 ecode rundivsel = 0x00;//时钟1分频,寄存器bit2~4控制
const uint08 ecode clkmode  = 0x01;//定时1分频
const uint08 ecode init_tl0 = 0x55;//4ms定时(8M时钟)
const uint08 ecode init_th0 = 0xF5;
#else
const uint08 ecode runwdtcon = 0x06;//12分频,cpuwdt定时0.784s*2
const uint08 ecode rundivsel = 0x04;//时钟2分频,寄存器bit2~4控制
const uint08 ecode clkmode  = 0x02;//定时2分频
const uint08 ecode init_tl0 = 0xAB;//4ms定时(4M时钟)
const uint08 ecode init_th0 = 0xFA;
#endif
const uint08 ecode runoscen = 0x03;//运行振荡器使能寄存器,bit4~bit5为xtlf振荡强度选择控制,00弱振~11强振
const uint08 ecode sleeposcen = 0x02;//停电振荡器使能寄存器,bit4~bit5为xtlf振荡强度选择控制,00弱振~11强振
const uint08 ecode sleeptmr0ie = 0x20;//0x20打开tmr0停电唤醒,0x00关闭tmr0停电唤醒仅支持1h唤醒
const uint08 ecode sleepwakeuptimer = 10;//停电唤醒清syswdt时间,16s-6,32s-12,65s-25
const uint16 code Rly_voltage = 0x1710;		//?????????
//功能编译
#define RELAYTYPE 0x00 //0x55:外置继电器，其它:内置继电器

#if( RELAYTYPE == 0x55 )
const uint08 ecode Relay_Mode = 0x55;		//0x55-外置继电器表(最大电流修改成80A),其他-内置继电器表(最大电流修改成60A)
#else
const uint08 ecode Relay_Mode = 0x00;		//0x55-外置继电器表(最大电流修改成80A),其他-内置继电器表(最大电流修改成60A)
#endif
const uint08 ecode Correction_Mode = 0x00;	//0x55-开启电压补偿功能,其他-关闭(目前rn8209c和ht7017支持)
const uint08 ecode Lasttesting_Valid = 0x00;	//0x55-使用成测温度数据,其他-使用非成测温度数据
const uint08 ecode Compile_Polar_485 = 0x00;	//0x55-兼容无极性485,其他-只支持有极性485
const uint08 ecode Compile_Lvd4x = 0x00;	//0x55-掉电阀值4.2v,0xaa-掉电阀值4.6v,其他-掉电阀值4.4v,
const uint08 ecode Memoryerr_Ctrl = 0x00;	//0x55-允许存储器上报,其他-禁止存储器上报(建议关闭)
const uint08 ecode Beep_Io_Ctrl = 0x55;		//0x55-蜂鸣器控制脚为GPIO42,其他-蜂鸣器控制脚为GPIO57(CALCOMMMODE==0有效)

//版本和规格
const unsigned char ecode Verdate[18] =  "XLJN-181120-01";   //厂家,版本号及版本日期 
const unsigned char ecode RatedVolt[6] = "230V";	//额定电压
const unsigned char ecode RatedCurr[6] = "5A";		//额定电流/基本电流
#if( RELAYTYPE == 0x55 )
const unsigned char ecode MaxCurr[6] = "80A";	//最大电流
#else
const unsigned char ecode MaxCurr[6] = "60A";	//最大电流
#endif
const unsigned char ecode ActiveClass[4] = "1.0";	//有功准确度等级
const unsigned char ecode MaterType[10] = "DDSY311";	//电表型号
const unsigned char ecode ProtocolVer[16] = "DL/T645-2007-15";//协议版本号
const unsigned char ecode Softbk[8] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};	//软件备案号
const unsigned char ecode Facdate[10] = "";	//生产日期

const unsigned long ecode Def_Meterconst_Bcd = 0x1000;	//脉冲常数默认1200
const unsigned char ecode Def_Meterconst_Hex = 10;	//脉冲常数默认1200
const unsigned char ecode Def_Plusevalidtimer = 20;	//80ms脉冲宽度(同步通讯)
const	float         ecode default_meterconst_dm	=	1000.0; 		//默认的表常数为1200(需量计算专用)

const unsigned char ecode Runet1Outcnt[2] = {0x0a,0x3e};//80ms脉冲宽度输出配置寄存器(暂不支持)
const unsigned char ecode Runrtctmls = 0x04;		//使能秒时标输出,负脉冲切换信号	
const unsigned char ecode resetworking_timer_delay=60;	//240ms复位载波芯片时间

//温度曲线相关参数
const float ecode const_xtl_top	= 25.2;		//计算温度曲线使用的顶点
const float ecode const_KL	= 0.0283+0.003;	//温度补偿系数
const float ecode const_KH	= 0.0381;
const float ecode const_TmpsL	= 5.1273;	//温度ADC斜率
const float ecode const_TmpsH	= 5.1273;
const uint08 ecode constF_XcapH = 0x13;
const uint08 ecode constF_XcapL = 0x80;
const uint08 ecode constF_Volt	= 0;		//电压补偿量(兼容新硬件电池增加二极管),3表示0.03v/10表示0.1v

//继电器相关
const uint08 ecode Relaytimer_Delay_ZB= 250;	//1000ms载波拉合闸延时时间
const uint08 ecode Relaytimer_Delay_N = 16;	//90ms内置继电器电平延时
const uint08 ecode Powertimer_Delay_N = 30;	//150ms内置继电器电平延时

const uint08 ecode Relaytimer_Delay_W = 98;	//400ms外置置继电器电平延时
const uint08 ecode Defaultspacetimer = 5;	//默认预跳闸间隔时间(5min)
const uint08 ecode Defrelayerrorcnt_s = 0;	//继电器检测显示去抖开始次数
const uint08 ecode Defrelayerrorcount = 3;	//继电器检测显示去抖次数(每次0.5s)
const uint08 ecode Defrelayerrorcount_r = 8;	//继电器检测故障记录去抖次数(每次0.5s)(西力要求缩短)

//energy


const unsigned long int code METER_PWZEROVAUL  = 0x00000035;
const unsigned long int code METER_I1ZEROVAUL  = 0x00000020;
const unsigned char code METER_BACKCNT  = 2;   //功率反向延时判断次数  

const uint16 code Deflowledvoltage = 0x1160;	//
const unsigned char code UartErrCnt_const = 30;	     

const uint08 ecode Def_Dl_Difference    = 0x00;	//此值<0x03则电量仅支持crc和bcd容错,否则电量还支持总电量大于4费率电量累加和0.xxkwh的容错处理和总电量小于4费率电量累加和的容错处理
const uint08 ecode Adjclk_timer_delay_s = 90;	//无计量通讯90s后启动8M时钟调校功能,如果为0则关闭定时8M时钟调校功能
const uint08 ecode Adjclk_timer_delay   = 2;	//无通讯2h后启动8M时钟调校功能,如果为0则关闭定时8M时钟调校功能
const uint16 ecode Defendenergyvoltage  = 0;	//如果为0x1250表示125v关闭计量,如果为0则禁止关闭计量(建议设置成0)
const uint08 ecode Runcalbps = 25;		//计量uart通讯波特率(BRGH=0):25/4800bps,51/2400bps
const uint08 ecode Def_Shielding_I1     = 0x15;	//允许火线清零最大值15mA
const uint08 ecode Def_Shielding_I2 = 0x18;	//允许零线清零最大值18mA
const uint08 ecode Def_Shielding_Cos = 0x50;	//允许功率因数置位最小值0.05
const uint08 ecode Defcurrovercount = 3;	//拉闸电流判断超过限制去抖时间
const uint16 ecode Defaultpdvoltage = 0x1320;	//默认掉电电压值(60%Un)
const uint08 ecode Deflowvoltagecount = 1;	//默认掉电记录上电去抖时间(或者高于60%去抖时间)(西力要求缩短)
const uint16 ecode Uoffset = 0x0000;		//默认电压系数偏移量
const uint08 ecode Def_Powerdetcnt2 = 11;	//低于170v在8s以内才进行电源异常检测,上电默认低于170v约11s
const uint08 ecode Def_PowerDetCnt0 = 4;	//如果为4则上电如果出现继电器动作则立刻进行继电器检测，如果为2表示需要去抖2s才进行检测

//interrupt
const uint08 ecode Fulllcdbias = 0x09;		//全屏显示偏置电压(3.5v液晶)
const uint08 ecode Fulllcdbias2 = 0x0b;		//低温全屏显示偏置电压
const uint08 ecode Runlcdbias = 0x08;		//运行液晶显示偏置电压(3.5v液晶)
const uint08 ecode Runlcdbias2 = 0x0a;		//低温运行液晶显示偏置电压
const uint08 ecode Sleeplcdbias = 0x0f;		//停电液晶显示偏置电压(3.5v液晶)
const uint08 ecode Sleeplcdfreq = 0x15;		//A波形64hz,B波形32hz
const uint08 ecode Sleeplcdset = 0x81;		//停电液晶显示配置
const uint08 ecode Sleepenmode = 0x47;		//Biasl_en宽度为2个周期,电流次大,片内电阻驱动模式(LCD模块关断时配置为00,否则会产生额外的功耗)
const uint08 ecode Sleeplvdefcfg = 0x5b;	//低压检测电路间歇使能(0x47更频繁)(客户修改此值请与复旦微确认)32T占空比1/512

//其他
const uint08 ecode Defaultauthtimer = 0x30;	//默认身份认证时效(30min)
const uint08 ecode Defaultstartup = 0x00;	//默认启动时间全0x00或者全0x99

//南网新增
const uint08 ecode Keyswitch_Valid = 0x00;	//0xaa-无卡槽；0x55-卡座有触点；其它-卡座无触点（远程表需配置为0xaa）
const uint08 ecode METERTYPE = 0;		//00：本地模式含显示; 01：远程模式含显示
const uint08 ecode SELFCLOSE = 0x00;		//自保电功能：00关闭；55开启
const uint08 ecode default_hearttime = 30;	//默认心跳时间30分钟
const uint08 ecode Sleepdispdays = 7*24;	//停电显示时间(天)
const uint08 ecode MAX_PSW_NUM = 32;		//最大密钥数默认32条
const uint08 ecode Terminal_delay = 120;	//交互终端身份认证有效时长2分钟
const uint08 ecode Defpassworderrcnt = 5;	//密码错误5次闭锁
const uint08 ecode Def_GSN = 0xf4;		//默认GSN的值
const uint08 ecode Def_CWGSP = 0x3f;		//默认CWGSP的值


const  signed short  ecode  g_PerKwh = 100;
const  signed short  ecode g_Per1Kwh = 1000;

const uint08 ecode Def_eventthreshold_table[6] = //事件判断设定范围
{
	0x60,			//停电事件电压触发定值---0x040014F1,1字节,例如0x60表示60%Un
	0x05,			//停电事件电流触发定值---0x040014F2,1字节,例如0x05表示5%Ib
	0x60,			//停电事件判定延时时间定值----0x040014F3,1字节,例如0x60表示60S
	0x05,			//开表盖事件判定延时时间定值----0x040014F4,1字节,例如0x05表示5S
	0x30,			//时钟电池欠压事件触发下限定值---0x040014F5,1字节,例如0x30表示3.0v
	0x60,			//时钟电池欠压事件判定延时时间定值----0x040014F6,1字节,例如0x60表示60S
};

//lcd
const unsigned long ecode disp_table[disp_num]=
{
	//1~65：当前及上12月组合电量
	//66~130：当前及上12月正向电量
	//131~195：当前及上12月反向电量
	0xffffffff,		//全显200
	0x00000c00,		//本月用电量201
	0x01000c00,		//上月用电量202
	0xefefefef,		//程序累加和203(与西力厂内显示命令冲突)
	0x00010102,		//电压(A相)204
	0x00010202,		//电流(A相)205
	0x00000302,		//功率(总)206
	0x00010302,		//功率(A相)207
	0x00000602,		//功率因数(总)208	
	0x00010602,		//功率因数(A相)209
	0x01008002,		//零线电流210
	0x09040004,		//常数211
	0x010b0004,		//第1结算日212
	0x020b0004,		//第2结算日213
	0x030b0004,		//第3结算日214
	0x0e040004,		//户号低215
	0x0e040004,		//户号高216
	0x02040004,		//表号低217
	0x02040004,		//表号高218
	0x01040004,		//通讯地址低219
	0x01040004,		//通讯地址高220
	0x01010004,		//日期221
	0x02010004,		//时间222
	0x01003003,		//最近一次编程日期223
	0x01003003,		//最近一次编程时间224
	0x01013003,		//最近一次清零日期225
	0x01013003,		//最近一次清零时间226
	0x00000d00,		//当前年用电量227
	0x01000d00,		//上次年用电量228
	0x00029000,		//剩余金额(DI0~DI3)229
	0x0b008002,		//当前阶梯电价230
	0x21008002,		//当前费率电价231
	0x20008002,		//当前电价232
};


//factory
const unsigned char ecode  Def_Limitmenory_table[]=
{
	0x00,0x00,0x10,0x00,		//透支金额限值xxxxxx.xx
	0x99,0x99,0x99,0x99,		//囤积金额限值xxxxxx.xx
	0x00,0x00,0x02,0x00,		//合闸允许金额限值xxxxxx.xx
};const unsigned char ecode Def_DisplayPara_table[8*2]=
{
//本地
	2,	//自动循环显示屏数
	5, 	//每屏显示时间    
	2, 	//显示电能小数位数
	4, 	//显示功率小数位数
	20,	//按键循环显示屏数
	0,	//预留
	0,	//费控模式: 00本地; 01远程
	5,	//上电全显5s      
	
//远程
	7,	//自动循环显示屏数
	5,	//每屏显示时间
	2,	//显示电能小数位数
	4,	//显示功率小数位数  
	23,	//按键循环显示屏数
	0,	//预留
	1,	//费控模式: 00本地; 01远程
	5,	//上电全显5s    
};


const unsigned char ecode Def_featureword_table[]=
{
	0x08,		//通信口1 波特率特征字	04000703	08H（2400bps）        
	0x05,		//有功组合方式特征字	04000601	05H（正向有功+反向有功
	0x0f,		//负荷模式字
	0x83,		//定时冻结数据模式字（定时冻结正向有功电能、反向有功电能、变量）
	0x83,		//瞬时冻结数据模式字（瞬时冻结正向有功电能、反向有功电能、变量）
	0x83,		//约定冻结数据模式字（约定冻结正向有功电能、反向有功电能、变量）
	0x03,		//整点冻结数据模式字（整点冻结正向有功总电能、反向有功总电能） 
	0x83,		//日冻结数据模式字	（日冻结正向有功电能、反向有功电能、变量） 
	0x00,		//电表运行特征字1
	0xbb,		//上报模式字1/除内卡初始化错误	
	0x04,		//上报模式字2/开盖
	0x00,		//上报模式字3/null
	0x00,		//上报模式字4/null
	0x00,		//上报模式字5/null
	0x00,		//上报模式字6/null
	0x00,		//上报模式字7/null
	0x00,		//上报模式字8/null
	0x00,0x00,0x00,	//继电器拉闸控制电流门限值(XXX.XXX)
	0x00,0x00,  	//跳闸延时时间(bcd码)
	0x00,0x00,	//大电流拉闸延时(bcd码)
	0x30,		//红外认证(bcd码)
	0x30,		//上报复位延时(bcd码)
	0x00,		//内置继电器0x55表示自动合闸,外置继电器0x55表示手动合闸
	0x00,		//扩展模式字,0x0A或者0xAA每天20:00复位载波一次(主要针对瑞斯康载波),0xA0或者0xAA反向点亮背光
	0x00,		//预留
	0x08,		//通信口3 波特率特征字	04000705	08H（2400bps） (载波) 
};


const unsigned char ecode Def_weekloadpara_table[]=
{
	0x7F,				//周休日特征字7F（周六、周日无休息）
	0x01,				//周休日采用的日时段表号1（第一套第1日时段表）
	0x13,0x01,0x01,0x00,0x00,	//负荷记录起始时间(年月日时分)
	0x00,0x01,			//第1~4类负荷记录间隔时间	
	0x00,0x05,
	0x00,0x02,
	0x00,0x10, 
#if(RELAYTYPE == 0x55)
	0x09,0x60,   			//过流触发下限1.2Imax,96.0A
#else
	0x07,0x20,   			//过流触发下限1.2Imax,72.0A
#endif
	0x60,				//过流延时时间,60s          
};

//循环显示默认支持16项初始化,如果超过不能修改表格实现,只能通讯设置实现
const unsigned char ecode Def_Autodisp_table[16*2]=
{    
	// 本地  
	229, //剩余金额 
	232,//232, //当前电价
	1,	 // 预留3   
	1,	 // 预留4   
	1,	 // 预留5   
	1,	 // 预留6   
	1,	 // 预留7   
	1,	 // 预留8	  
	1,	 // 预留9	  
	1,	 // 预留10      
	1,	 // 预留11      
	1,	 // 预留12      
	1,	 // 预留13      
	1,	 // 预留14      
	1,	 // 预留15      
	1,	 // 预留16      

// 远程
	 221, // 日期      
	 1,	  // 组合总电量
	 2,	  // 组合尖电量
	 3,	  // 组合峰电量
	 4,	  // 组合平电量
	 5,	  // 组合谷电量
	 1,	 // 预留7 	
	 1,	  // 预留8     
	 1,	  // 预留9     
	 1,	  // 预留10    
	 1,	  // 预留11    
	 1,	  // 预留12    
	 1,	  // 预留13    
	 1,	  // 预留14    
	 1,	  // 预留15    
	 1,	  // 预留16   
};

//按键显示默认支持32项初始化,如果超过不能修改表格实现,只能通讯设置实现
const unsigned char ecode Def_Keydisp_table[32*2]=
{
	//本地	
	221, // 日期+时间			   
	247, //上1日用电金额
	250, //上1月用电金额
	249, //剩余金额+可用天数
	237, //上1次充值Token 高4位
	238, //上1次充值Token 低16位
	239, //上2次充值Token 高4位
	240, //上2次充值Token 低16位
	241, //上3次充值Token 高4位
	242, //上3次充值Token 低16位
	243, //上4次充值Token 高4位
	244, //上4次充值Token 低16位
	245, //上5次充值Token 高4位
	246, //上5次充值Token 低16位
	204, // 电压+电流 A
  205, // 电压+电流 B//	207, // 功率+功率因数
  206, // 电压+电流 C//	1,	// 预留17 		  
	207, //功率+功率因数 A//1,	// 预留18 		  
	208, //功率+功率因数 B//1,	// 预留19 		  
	209, //功率+功率因数 C//1,	// 预留20 		  
	1,	// 预留21 		  
	1,	// 预留22 		  
	1,	// 预留23 		  
	1,	// 预留24 		  
	1,	// 预留25 		  
	1,	// 预留26 		  
	1,	// 预留27 		  
	1,	// 预留28 	   
	1,	// 预留29 	   
	1,	// 预留30 	   
	1,	// 预留31 	   
	1,	// 预留32 	   
	//远程
	1,	 //组合总电量  当前  
	2,	 //组合尖电量  当前 
	3,	 //组合峰电量  当前 
	4,	 //组合平电量  当前 
	5,	 //组合谷电量  当前 
	6,	 //组合总电量  上1月
	7,	 //组合尖电量  上1月
	8,	 //组合峰电量  上1月
	9,	 //组合平电量  上1月
	10,	 //组合谷电量  上1月
	11,	 //组合总电量  上2月
	12,	 //组合尖电量  上2月
	13,	 //组合峰电量  上2月
	14,	 //组合平电量  上2月
	15,	 //组合谷电量  上2月
	219, // 通讯地址低8位   
	220, // 通讯地址高4位   
	221, // 日期            
	222, // 时间            
	204, // 电压
	205, // 电流
	207, // 功率A相
	209, // 功率因数A相       
	1,	 // 预留24          
	1,	 // 预留25          
	1,	 // 预留26          
	1,	 // 预留27          
	1,	 // 预留28          
	1,	 // 预留29          
	1,	 // 预留30          
	1,	 // 预留31          
	1,	 // 预留32          
};
const unsigned char ecode Def_freezeparameter_table[]=
{
	0x10,0x01,0x01,0x00,0x00,	//整点冻结起始时间	10年01月01日00时00分
	0x60,				//整点冻结时间间隔	60分钟
	0x00,0x00,			//日冻结时间		00时00分
	0x99,0x99,0x12,0x00,		//定时冻结时间		12时00分
};

const unsigned char Def_Demand_table[]=
{
	0x05,  //最大需量周期 15                                           
	0x01,  //滑差时间	1        
};

const unsigned char ecode Def_feik_table[]=
{
	0x02,  //年时区数	2                                               
	0x02,  //日时段表数	2                                               
	0x08,  //日时段数 	8                                               
	0x04,  //费率数	4                                               
	0x00,  //公共假日数	0（不启用，启用时至少可设置1～14日公共假日参数）
	0x00,  //谐波分析次数	0                                               
	0x00,  //阶梯数	0        
	0x11,	//密钥允许设置的条数                                
};

//冻结日
const unsigned char ecode Def_histroyparameter_table[]=
{
	0x00,0x01,								//01日00点
	0x99,0x99,
	0x99,0x99,
};

//密码
const uint08  Def_Password3_table[4]=
{
	0x03,0x00,0x00,0x00,
};

const uint08  Def_Password4_table[4]=
{
	0x04,0x11,0x11,0x11,
};

//时区表1
const unsigned char ecode Def_sqb_table1[]=
{
	0x01,0x28,0x12,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
};

//时区表2
const unsigned char ecode Def_sqb_table2[]=
{
	0x01,0x28,0x12,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
	0x02,0x22,0x06,	   //时段表号、日、月
};

//时段表1
const unsigned char ecode Def_sdb_table1[]=
{										//费率号,分,时,最多14个
	0x01,0x00,0x00,        
	0x02,0x00,0x08,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
	0x03,0x00,0x16,
};

//时段表2
const unsigned char ecode Def_sdb_table2[]=
{										//费率号,分,时,最多14个
	0x01,0x00,0x00,        
	0x02,0x00,0x06,
	0x03,0x00,0x12,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
	0x04,0x00,0x18,
};

//当前阶梯时区表
const unsigned char ecode Def_jtshiqu_table[]=
{										//阶梯表张号+日+月
	0x01,0x01,0x01,        
	0x01,0x01,0x01,
	0x01,0x01,0x01,
	0x01,0x01,0x01,
};

//备用阶梯时区表
const unsigned char ecode Def_jtshiqu_table2[]=
{										//阶梯表张号+日+月
	0x01,0x01,0x01,        
	0x01,0x01,0x01,
	0x01,0x01,0x01,
	0x01,0x01,0x01,
};
const FACTOR ecode Factor =
{
    #if MeterPara_Mode == 0           
     ///////电流互感器为1.5(10)A/5mA，采用电阻为2×4.7欧姆//////
    //电压系数
    0.00000677424168,//(float)MeterUn/(float)RN8302Urms/10,
    //电流系数
    0.000000200000002,//(float)MeterIb/(float)RN8302Irms/10,
    //功率系数
    0.000113652924,//8388608/(((float)RN8302Urms/(float)MeterUn)*((float)RN8302Irms/(float)MeterIb))/10,
    //0.0001kwh需要的能量
    64,//MeterCP2*0.0001,一个脉冲代表0.0001KWH
    //能量上限值
    11000,//(signed long)((float)MeterUn*MeterImax*3*(MeterCP2/MeterCP)/6000),
    //视在、有功启动电流门限值寄存器
    234,//(unsigned short)(RN8302Irms*0.0006/16),
    //电流显示上限
    500000,//MeterImax*1000*5,//5Imax以上不显示 xxx.xxxx
    //电压显示上限值
    660000,//MeterUn*100*3,//3Un以上不显示 xxx.xxx校表电压是3位小数，所以再乘以100补成3位小数格式
    //电压显示下限值
    440,//MeterUn/5,//0.02Un以下电压不显示
    //常数
    6400//MeterCP 
#elif MeterPara_Mode == 1    
 ///////电流互感器为1.5(6)A/5mA，采用电阻为2×4.7欧姆//////
    //电压系数
    0.00000677424168,//(float)MeterUn/(float)RN8302Urms/10,
    //电流系数
    0.000000200000002,//(float)MeterIb/(float)RN8302Irms/10,
    //功率系数
    0.000113652924,//8388608/(((float)RN8302Urms/(float)MeterUn)*((float)RN8302Irms/(float)MeterIb))/10,
    //0.0001kwh需要的能量
    64,//MeterCP2*0.0001,一个脉冲代表0.0001KWH
    //能量上限值
    6600,//(signed long)((float)MeterUn*MeterImax*3*(MeterCP2/MeterCP)/6000),
    //视在、有功启动电流门限值寄存器
    281,//(unsigned short)(RN8302Irms*0.0006/16),
    //电流显示上限
    300000,//MeterImax*1000*5,//5Imax以上不显示 xxx.xxxx
    //电压显示上限值
    660000,//MeterUn*100*3,//3Un以上不显示 xxx.xxx校表电压是3位小数，所以再乘以100补成3位小数格式
    //电压显示下限值
    440,//MeterUn/5,//0.02Un以下电压不显示
    //常数
    6400//MeterCP              
#elif MeterPara_Mode == 3
    //电压系数
    0.00000667642007,//(float)MeterUn/(float)RN8302Urms/10,
    //电流系数
    0.00000120481922,//(float)MeterIb/(float)RN8302Irms/10,
    //功率系数
    0.000674769573,//8388608/(((float)RN8302Urms/(float)MeterUn)*((float)RN8302Irms/(float)MeterIb))/10,
    //0.0001kwh需要的能量
    4,//MeterCP2*0.0001,一个脉冲代表0.0001KWH
    //能量上限值
    66000,//(signed long)((float)MeterUn*MeterImax*3*(MeterCP2/MeterCP)/6000),
    //视在、有功启动电流门限值寄存器
    518,//(unsigned short)(RN8302Irms*0.002/16),
    //电流显示上限
    3000000,//MeterImax*1000*5,//5Imax以上不显示 xxx.xxxx
    //电压显示上限值
    660000,//MeterUn*100*3,//3Un以上不显示 xxx.xxx校表电压是3位小数，所以再乘以100补成3位小数格式
    //电压显示下限值
    440,//MeterUn/5,//0.02Un以下电压不显示
    //常数
    400//MeterCP  
#elif MeterPara_Mode == 4
    //电压系数
    0.00000673145768,//(float)MeterUn/(float)RN8302Urms/10,
    //电流系数
    0.00000166666666,//(float)MeterIb/(float)RN8302Irms/10,
    //功率系数
    0.000941125967,//8388608/(((float)RN8302Urms/(float)MeterUn)*((float)RN8302Irms/(float)MeterIb))/10,
    //0.0001kwh需要的能量
    8,//MeterCP2*0.0001,一个脉冲代表0.0001KWH
    //能量上限值
    66000,//(signed long)((float)MeterUn*MeterImax*3*(MeterCP2/MeterCP)/6000),
    //视在、有功启动电流门限值寄存器
    375,//(unsigned short)(RN8302Irms*0.002/16),
    //电流显示上限
    3000000,//MeterImax*1000*5,//5Imax以上不显示 xxx.xxxx
    //电压显示上限值
    660000,//MeterUn*100*3,//3Un以上不显示 xxx.xxx校表电压是3位小数，所以再乘以100补成3位小数格式
    //电压显示下限值
    440,//MeterUn/5,//0.02Un以下电压不显示
    //常数
    800//MeterCP          
#elif MeterPara_Mode == 5    
    //电压系数
    0.00000668746088,//(float)MeterUn/(float)RN8302Urms/10,
    //电流系数
    0.00000166666666,//(float)MeterIb/(float)RN8302Irms/10,
    //功率系数
    0.000934974872,//8388608/(((float)RN8302Urms/(float)MeterUn)*((float)RN8302Irms/(float)MeterIb))/10,
    //0.0001kwh需要的能量
    3,//MeterCP2*0.0001,一个脉冲代表0.0001KWH
    //能量上限值
    110000,//(signed long)((float)MeterUn*MeterImax*3*(MeterCP2/MeterCP)/6000),
    //视在、有功启动电流门限值寄存器
    750,//(unsigned short)(RN8302Irms*0.002/16),
    //电流显示上限
    5000000,//MeterImax*1000*5,//5Imax以上不显示 xxx.xxxx
    //电压显示上限值
    660000,//MeterUn*100*3,//3Un以上不显示 xxx.xxx校表电压是3位小数，所以再乘以100补成3位小数格式
    //电压显示下限值
    440,//MeterUn/5,//0.02Un以下电压不显示
    //常数
    300//MeterCP 
#elif MeterPara_Mode == 11    
    //电压系数
    0.0000017869936,//(float)MeterUn/(float)RN8302Urms/10,
    //电流系数
    0.000000142857146,//(float)MeterIb/(float)RN8302Irms/10,
    //功率系数
    0.0000214148422,//8388608/(((float)RN8302Urms/(float)MeterUn)*((float)RN8302Irms/(float)MeterIb))/10,
    //0.00001kwh需要的能量
    2,//MeterCP2*0.00001,一个脉冲代表0.00001KWH
    //能量上限值
    300,//(signed long)((float)MeterUn*MeterImax*3*(MeterCP2/MeterCP)/6000),
    //视在、有功启动电流门限值寄存器
    392,//(unsigned short)(RN8302Irms*0.0006/16),
    //电流显示上限
    300000,//MeterImax*1000*5,//5Imax以上不显示 xxx.xxxx
    //电压显示上限值
    300000,//MeterUn*100*3,//3Un以上不显示 xxx.xxx校表电压是3位小数，所以再乘以100补成3位小数格式
    //电压显示下限值
    200,//MeterUn/5,//0.02Un以下电压不显示
    //常数
    20000//MeterCP
#elif MeterPara_Mode == 13    
    //1.5(6)A 5mA 12.4欧
    //电压系数
    0.00000181968266,//(float)MeterUn/(float)RN8302Urms/10,
    //电流系数
    0.000000142857146,//(float)MeterIb/(float)RN8302Irms/10,
    //功率系数
    0.0000218065779,//8388608/(((float)RN8302Urms/(float)MeterUn)*((float)RN8302Irms/(float)MeterIb))/10,
    //0.00001kwh需要的能量
    2,//MeterCP2*0.00001,一个脉冲代表0.00001KWH
    //能量上限值
    173,//(signed long)((float)MeterUn*MeterImax*3*(MeterCP2/MeterCP)/6000),
    //视在、有功启动电流门限值寄存器
    393,//(unsigned short)(RN8302Irms*0.0006/16),
    //电流显示上限
    300000,//MeterImax*1000*5,//5Imax以上不显示 xxx.xxxx
    //电压显示上限值
    173100,//MeterUn*100*3,//3Un以上不显示 xxx.xxx校表电压是3位小数，所以再乘以100补成3位小数格式
    //电压显示下限值
    115,//MeterUn/5,//0.02Un以下电压不显示
    //常数
    20000//MeterCP         
#endif  
};
const METERSPECIAL ecode Meterspecial =
{
    0x5A, //0x5A-????7???
    0xAA, //0x00-????,0x5A-??,0xAA-?? 
    0x00 //0x5A-??????  
};


const METERPARAMETER ecode MeterParameter =
{
    #if MeterPara_Mode == 0 //220V 1(10)A 6400 imp/kWh        
    1,//1-互感式，3-直接式
    0x34,//0x34-三相四线，0x33三相三线
    0x00,//0-无低频晶振，1-有低频晶振
    183.1806828,//电压采样1：1200
    47,//mv需要乘以增益系数 5ma 9.4      
    32475957.31,//RN8302电压标准值
    7500000,//RN8302电流标准值
    29504299,//RN8302功率标准值1.0 Ib
    0x003B,//HFConst1(计量用)
    0x170C,//HFConst2(脉冲用)
    2200,//电压
    15, //电流IB
    100,//电流IMAX
    6400, //常数  
    640000//计量常数
#elif MeterPara_Mode == 1     //220V 1.5(6)A 6400 imp/kWh

//    1,//1-互感式，3-直接式
//    0x34,//0x34-三相四线，0x33三相三线
//    0x01,//0-无低频晶振，1-有低频晶振
//    183.1806828,//电压采样1：1200
//    20,//mv需要乘以增益系数 1ma 20      
//    32525997.31,//RN8302电压标准值
//    3300000,//RN8302电流标准值
//    12981892,//RN8302功率标准值
//    0x001A,//HFConst1(计量用)
//    0x0A28,//HFConst2(脉冲用)
//    2200,//电压
//    15, //电流IB
//    60,//电流IMAX
//    6400, //常数  
//    640000//计量常数     

    1,//1-互感式，3-直接式
    0x34,//0x34-三相四线，0x33三相三线
    0x00,//0-无低频晶振，1-有低频晶振
    183.1806828,//电压采样1：1200
    47,//mv需要乘以增益系数 5ma 9.4      
    32475957.31,//RN8302电压标准值
    7500000,//RN8302电流标准值
    29504299,//RN8302功率标准值1.0 Ib
    0x003B,//HFConst1(计量用)
    0x170C,//HFConst2(脉冲用)
    2200,//电压
    15, //电流IB
    60,//电流IMAX
    6400, //常数  
    640000//计量常数          
#elif MeterPara_Mode == 3 //220V 5(80)A 400 imp/kWh
    3,//1-互感式，3-直接式
    0x34,//0x34-三相四线，0x33三相三线
    0x00,//0-无低频晶振，1-有低频晶振
    183.1806828,//电压采样1：1200
    18.8,//电流采样mv需要乘以增益系数 2ma 9.4
    32969059.77,//43863183.87,//RN8302电压标准值
    3000000,//RN8302电流标准值
    11801720,//15735626,//RN8302功率标准值
    0x0073,//0x0099,//HFConst1(计量用)
    0x2CEC,//0x3BC4,//HFConst2(脉冲用)
    2200,//电压
    50, //电流IB
    800,//电流IMAX
    400,//常数
    40000//计量常数    
#elif MeterPara_Mode == 4 //220V 5(60)A 800 imp/kWh
    3,//1-互感式，3-直接式
    0x34,//0x34-三相四线，0x33三相三线
    0x00,//0-无低频晶振，1-有低频晶振
    183.1806828,//219.7802198,//电压采样mv
    18.8,//电流采样mv需要乘以增益系数 2ma 9.4 
    32682372.3,//43863183.87,//RN8302电压标准值
    3000000,//RN8302电流标准值
    11801720,//15735626,//RN8302功率标准值
    0x0039,//0x0099,//HFConst1(计量用)
    0x1644,//0x3BC4,//HFConst2(脉冲用)
    2200,//电压
    50, //电流IB
    600,//电流IMAX
    800,//常数
    80000//计量常数            
#elif MeterPara_Mode == 5 //220V 10(100)A 300 imp/kWh
    3,//1-互感式，3-直接式
    0x34,//0x34-三相四线，0x33三相三线
    0x00,//0-无低频晶振，1-有低频晶振
    183.1806828,//电压采样1：1200
    37.6,//电流采样mv需要乘以增益系数 2ma 9.4 
    32897387.9,//RN8302电压标准值
    6000000,//RN8302电流标准值
    23603439,//RN8302功率标准值
    0x0099,//HFConst1(计量用)
    0x3BC4,//HFConst2(脉冲用)
    2200,//电压
    100, //电流IB
    1000,//电流IMAX
    300,//常数
    30000//计量常数      
#elif MeterPara_Mode == 11 //100V 1.5(6)A 20000 imp/kWh     
    //电压1/600K，电流5mA/9.4欧        
    1,//1-互感式，3-直接式
    0x33,//0x34-三相四线，0x33三相三线
    0x00,//0-无低频晶振，1-有低频晶振
    166.3893511,//电压采样mv 1/600K
    47,//电流采样mv需要乘以增益系数 5ma 9.4 
    27912935.19,//RN8302电压标准值
    7500000,//RN8302电流标准值
    12479201,//RN8302功率标准值
    0x0165,//HFConst1(计量用)
    0x0DF2,//HFConst2(脉冲用)
    1000,//电压
    15, //电流IB
    60,//电流IMAX
    20000,//常数
    200000//计量常数        
#elif MeterPara_Mode == 13 //57.7V 1.5(6)A 20000 imp/kWh
    1,//1-互感式，3-直接式
    0x34,//0x34-三相四线，0x33三相三线
    0x00,//0-无低频晶振，1-有低频晶振
    191.6943522,//电压采样mv 2/600
    47,//电流采样mv需要乘以增益系数 5ma 9.4 
    31715271.19,//RN8302电压标准值
    7500000,//RN8302电流标准值
    28373301,//RN8302功率标准值
    0x02BF,//HFConst1(计量用)
    0x1B76,//HFConst2(脉冲用)
    577,//电压
    15, //电流IB
    60,//电流IMAX
    20000,//常数
    200000//计量常数       
#endif        
};



void Factory_Init( void )
{
	unsigned char i;
	unsigned char Buf[72];
	
	//清有效指针页
	ClrRecTimes( 0, 0xff );//清冻结和事件有效指针
	
	//晶振停振, 复位次数, 恢复新时钟起始时间,恢复金额备份
	Clear_Wdt();
	memset( Buf, 0x00, 64 );
	for( i=0; i<3; i++ )
	{
		I2C_Write_Eeprom44( ee_xtlfstop_time_page+i, Buf, 18 );
	}
	Wr_Ext_Number = 0;//清扩展命令允许写ee次数
	Clr_Otherdata(4); //电池工作时间
	relay_commu_close_flag = 0;//默认保电解除状态
	Store_Meterstatus();
	
	//初始化电能表位置信息
	I2C_Write_Eeprom( ee_meterpos_page, ee_meterpos_inpage, Buf, ee_meterpos_lenth );	
		
	//第1~60公共假日日期及日时段表号: 00000000
	for( i=0; i<4; i++ )
	{
		I2C_Write_Eeprom( ee_holiday_page+i, 0x00, Buf, ee_holiday_lenth );
	}
	
	//密码初始化
	WriteE2WithBackup( ee_password_page, ee_password_inpage, (uint08*)Def_Password3_table, ee_password_lenth );
	WriteE2WithBackup( ee_password_page, ee_password4_inpage, (uint08*)Def_Password4_table, ee_password_lenth );
		
	//变比,表号等(双备份)
	ReadE2WithBackup( ee_esampara_page, ee_esampara_inpage, Buf, ee_esampara_lenth );
	if( IsMultiBcd( Buf, 14 ) )//非法数据
	{
		memset( Buf, 0x00, 14 );	//报警金额1限值(4)＋报警金额2限值(4)+变比(6)
		Buf[10] = 0x01;//电流变比1
		Buf[13] = 0x01;//电压变比1
	}
//	memset( Buf+14, 0x00, 6 );	//表号(6),表号不初始化	
//	Buf[19] = 0x01;//表号0x000000000001
	memset( Buf+20, 0x00, 9 );	//客户编号(6)+电卡类型(1)+身份认证时效(2)
	Buf[27] = 0x00;//身份认证有效时长
	Buf[28] = Defaultauthtimer;
	WriteE2WithBackup( ee_esampara_page, ee_esampara_inpage, Buf, ee_esampara_lenth );
	memcpy( Esam_Para.Alarm_Limit1.B08, Buf, ee_esampara_lenth+2 );
	
	//金额参数(双备份)
	WriteE2WithBackup( ee_limitmenory_page, ee_limitmenory_inpage, (uint08*)Def_Limitmenory_table, ee_limitmenory_lenth );
	memcpy( Money_Para.Tick.B08, Def_Limitmenory_table, ee_limitmenory_lenth );//赊欠金额限值/囤积金额限值/合闸允许金额恢复默认
	
	//显示参数(双备份)
	WriteE2WithBackup( ee_displaypara_page, ee_displaypara_inpage, (uint08*)Def_DisplayPara_table+8*METERTYPE, ee_displaypara_lenth );
	memcpy( &Display_Para.S_Number, Def_DisplayPara_table+8*METERTYPE, ee_displaypara_lenth );
	RdMeterMode();
	
	//切换时间(双备份)
	memset( Starttime.Shiqu, Defaultstartup, ee_starttimer_lenth );
	WriteE2WithBackup( ee_starttimer_page, ee_starttimer_inpage, Starttime.Shiqu, ee_starttimer_lenth );
	memset( Buf, 0xff, ee_changecontrol_lenth );	//时区时段均使用第一套
	I2C_Write_Eeprom44( ee_changecontrol_page, Buf, ee_changecontrol_lenth );
	run_timezone_flag = 1;
	
	//特征模式字(双备份)
	WriteE2WithBackup( ee_featureword_page, ee_featureword_inpage, (uint08*)Def_featureword_table, ee_featureword_lenth );
	memcpy( Mode.Bps, Def_featureword_table, ee_featureword_lenth );
	mode_veri_flag = 1;
	reg_veri_flag = 1;	
	
	
	//事件判断设定范围校验(西力新增)
	WriteE2WithBackup( ee_eventthreshold_page, ee_eventthreshold_inpage, (uint08*)Def_eventthreshold_table, ee_eventthreshold_lenth );
	memcpy( Threshold, Def_eventthreshold_table, ee_eventthreshold_lenth );
	
	memset( Buf, 0xff, 72 );			
	I2C_Write_Eeprom( ee_TID_page, ee_TID_inpage, Buf, ee_TID_lenth );
	
	memset( Buf, 0x00, 72 );
  I2C_Write_Eeprom( ee_TIDCNT_page, ee_TIDCNT_inpage, Buf, ee_TIDCNT_lenth );	
	I2C_Write_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Buf, ee_TOKEN_lenth );
	I2C_Write_Eeprom( ee_Pre7DayMon_page, ee_TOKEN_inpage, Buf, ee_Pre7DayMon_lenth );
	I2C_Write_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, Buf, ee_Pre12monthMon_lenth );
	I2C_Write_Eeprom( ee_DayUserINFO_page, ee_UserINFO_inpage, Buf, 16 );
	
	
//	memset( Buf, 0xFF, 72 );
//	I2C_Read_Eeprom( ee_Pre12monthMon_page, ee_Pre12monthMon_inpage, Buf, ee_Pre12monthMon_lenth );
//	
	memset( Buf, 0x01, 8 );
	WriteE2WithBackup( ee_keysts_page, ee_keysts_inpage, Buf, ee_keysts_lenth );
	ReadE2WithBackup( ee_keysts_page, ee_keysts_inpage, &gs_APDU_key_var.secret_key[0], ee_keysts_lenth );
	
	//周休日和负荷参数(双备份)
	memset( Buf, 0x00, 72 );
	
	WriteE2WithBackup( ee_weekholiday_page, ee_weekholiday_inpage, (uint08*)Def_weekloadpara_table, ee_weekholiday_lenth );
	memcpy( &Week_Holiday.Status, Def_weekloadpara_table, ee_weekholiday_lenth );
//	ReadE2WithBackup( ee_weekholiday_page, ee_weekholiday_inpage, &Week_Holiday.Status, ee_weekholiday_lenth );
//	ReadE2WithBackup( ee_weekholiday_page, ee_weekholiday_inpage, Buf, ee_weekholiday_lenth );
	//费控数
	WriteE2WithBackup( ee_feik_number_page, ee_feiknumber_inpage, (uint08*)Def_feik_table, ee_feiknumber_lenth );
	memcpy( Feik.Num, Def_feik_table, ee_feiknumber_lenth );
  
	memset( Money.Remain,0, ee_remainmoney_lenth);
	Money.Remain[3] = 0x05;	
	WriteE2WithBackup( ee_remainmoney_page, ee_remainmoney_inpage,  Money.Remain, ee_remainmoney_lenth );
	I2C_Read_Eeprom(ee_remainmoney_page, ee_remainmoney_inpage, Money.Remain, ee_remainmoney_lenth);
  
	memset( Buf, 0x00, 5 );
  I2C_Write_Eeprom44( ee_meterstatus1_page, Buf, ee_meterstatus1_lenth);
	//结算日
	WriteE2WithBackup( ee_histroyparameter_page, ee_histroyparameter_inpage, (uint08*)Def_histroyparameter_table, ee_histroyparameter_lenth );
	
	//冻结参数
	I2C_Write_Eeprom44( ee_freezeparameter_page, (uint08*)Def_freezeparameter_table, ee_freezeparameter_lenth );
  
	WriteE2WithBackup( ee_Demand_Period_page, ee_Demand_Period_inpage,(uint08*)Def_Demand_table, ee_Demand_Period_len );//需量周期，滑差时间
	
	memset( Buf, 0x00, 42 );
	I2C_Write_Eeprom( ee_p_demand_page, ee_p_demand_inpage, Buf, ee_pdemand_lenth );
	I2C_Write_Eeprom( ee_n_demand_page, ee_n_demand_inpage, Buf, ee_ndemand_lenth );
	
	Clear_Wdt();
		
	//第一套，第二套时区表数据
	I2C_Write_Eeprom( ee_shiqu1_page, 0x00, (uint08*)Def_sqb_table1, ee_shiqu_lenth );
	I2C_Write_Eeprom( ee_shiqu2_page, 0x00, (uint08*)Def_sqb_table2, ee_shiqu_lenth );
	
	//第一套，第二套 所有8个日时段表
	I2C_Write_Eeprom( ee_shiduantable1_1_page, 0x00, (uint08*)Def_sdb_table1, ee_shiduantable_lenth );
	for( i=1; i<8; i++ )
	{
		I2C_Write_Eeprom( ee_shiduantable1_1_page+i, 0x00, (uint08*)Def_sdb_table2, ee_shiduantable_lenth );
	}
	
	I2C_Write_Eeprom( ee_shiduantable2_1_page, 0x00, (uint08*)Def_sdb_table2, ee_shiduantable_lenth );
	for( i=1; i<8; i++ )
	{
		I2C_Write_Eeprom( ee_shiduantable2_1_page+i, 0x00, (uint08*)Def_sdb_table1, ee_shiduantable_lenth );
	}
	
	Tariff.Crc16.B16 += 1;
	
	//自动循环显示项目列表
	memset( Buf, 0x01, ee_displayitem_lenth );
	I2C_Write_Eeprom( ee_loop_displayitem_page+1, 0x00, Buf, ee_displayitem_lenth );
	I2C_Write_Eeprom( ee_key_displayitem_page+1, 0x00, Buf, ee_displayitem_lenth );
	
	memcpy( Buf, Def_Autodisp_table+16*METERTYPE, sizeof(Def_Autodisp_table)/2 );
	I2C_Write_Eeprom( ee_loop_displayitem_page, 0x00, Buf, ee_displayitem_lenth );
	memset( Buf, 0x01, ee_displayitem_lenth );
	memcpy( Buf, Def_Keydisp_table+32*METERTYPE, sizeof(Def_Keydisp_table)/2 );
	I2C_Write_Eeprom( ee_key_displayitem_page, 0x00, Buf, ee_displayitem_lenth );
	
	for( i=0; i<4; i++ ) Item[i].Crc16.B16 += 1;
	dispitem_veri_flag = 1;
	
	//当前/备用套阶梯时区表
	I2C_Write_Eeprom( ee_jtshiqu1_page, ee_jtshiqu_inpage, (uint08*)Def_jtshiqu_table, ee_jtshiqu_len );
	I2C_Write_Eeprom( ee_jtshiqu2_page, ee_jtshiqu_inpage, (uint08*)Def_jtshiqu_table2, ee_jtshiqu_len );
	
	//初始化ee里费率电价和阶梯参数
	Clear_Wdt();
	memset( Buf, 0x00, 72 );
	if( Crc_Check( Price.Tariff[0].B08, ee_tariffprice_lenth+2 ) ) 
	{
		I2C_Write_Eeprom( ee_tariffprice1_page, 0x00, Buf, ee_tariffprice_lenth );//费率电价
		I2C_Write_Eeprom( ee_tariffprice2_page, 0x00, Buf, ee_tariffprice_lenth );//费率电价
	}
	if( Crc_Check( Price.Ladder_Dl[0].B08, ee_ladder_lenth+2 ) ) 
	{
		for( i=0; i<8; i++ ) I2C_Write_Eeprom( ee_ladder1_page+i, 0, Buf, ee_ladder_lenth );//阶梯参数
	}
	
	//远程模式，清当前电价参数
	if( !BDMODE )
	{
		memset(Price.Current_L.B08, 0x00, 16);
	}
	
	//初始化温度系数
	Temperpara_Verify();	//校验数据合法性
	WriteE2WithBackup( ee_temperature_page, ee_temperature_inpage, Tpara.Offset, ee_temperature_lenth); 
	
	//初始化心跳时间
	Buf[0] = default_hearttime;
	WriteE2WithBackup( ee_hearttime_page, ee_hearttime_inpage, Buf, ee_hearttime_lenth );
	(*( void (*)( ) )0x00000321) ();	//软复位
	//while(1)
	//{	;}
										
}

