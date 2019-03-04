/************************************************************************
* 版权所有 (C)2016, 杭州西力智能科技股份有限公司。
*
* 文件名称： // ADE7858.H
* 文件标识： // ADE7858计量文件头
* 内容摘要： // 
* 其它说明： //
* 当前版本： // V01.00
* 作 者： // yangxing
* 完成日期： // 2016年08月06日
*
* 修改记录1：//
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
************************************************************************/









//校表瞬时量结构体定义
typedef struct
{
    unsigned long  UAdj[3]; //XXXX.XXXX A、B、C相电压 BCD
    unsigned long  IAdj[3]; //XX.XXXX A、B、C相电流 BCD
    unsigned long  PAdj[4]; //XX.XXXXXX 总、A、B、C相有功功率 BCD
    unsigned long  RAdj[4]; //XX.XXXXXX 总、A、B、C相无功功率 BCD
} REALADJ;
RN8302_EXT REALADJ RealAdj;

//瞬时量寄存器结构体定义
typedef struct
{
    unsigned long  Urms[3];//电压寄存器有效值
    unsigned long  Irms[3];//电流存器有效值
    unsigned long  DmdP[2]; //正向、反向、当前有功总需量一分钟累计功率
    unsigned long  DmdTouP[2];//正向、反向分时费率一分钟累计功率
    unsigned long  CurDmdP[3];//当前有、无功、视在一分钟累计功率
    unsigned long  QuadrDmdP[4];//4象限总无功需量一分钟累计功率
    unsigned long  QuadrDmdTouP[4];//4象限分时费率无功需量一分钟累计功率
    signed long    GrpDmdP[2]; //组合无功1、2总需量一分钟累计功率
    signed long    GrpDmdTouP[2];//组合无功1、2分时需量一分钟累计功率
    unsigned long    ActWatthrP;//有功正向总能量
    unsigned long    ActWatthrN;//有功反向总能量
    unsigned long    RactVarhr;//无功总能量
    unsigned long    Vahr;//视在总能量
    unsigned long    EnergyBuf[9];
} CHIPREG;
RN8302_EXT CHIPREG ChipReg;

//各相电量结构体定义
typedef struct
{
    unsigned long    ActPowerP;//总有功正向能量瞬时值
    unsigned long    ActPowerN;//总有功反向能量瞬时值
    unsigned long    ActPowerPp[3]; //A、B、C正向有功能量瞬时值
    unsigned long    ActPowerPn[3]; //A、B、C反向有功能量瞬时值
    unsigned long    RactPower;//总无功能量瞬时值
    unsigned long    RactPowerPp[3]; //A、B、C无功能量瞬时值
    unsigned long    VahrPower[2];//正、反向视在能量瞬时值
} POWERVALUE;
#define  LenValue  56
RN8302_EXT POWERVALUE PowerValue;

//校表参数结构体定义
typedef struct
{
    unsigned short  Igain; //电流增益
    unsigned short  Ugain; //电压增益
    unsigned short  Pgain; //功率增益
    unsigned short  Iofst; //电流偏移
    unsigned short  Uofst; //电压偏移
    unsigned short  Pofst; //功率偏移
    unsigned char  UTofst; //采样通道U相位偏移
    unsigned char  ITofst[3]; //采样通道I相位偏移
    unsigned short PTofst[3]; //中、低、高功率相位偏移
    unsigned short AdjChk;
} ADJ;
#define  LenAdjBuff  24
typedef union
{
    unsigned char   AdjBuff[LenAdjBuff];
    ADJ 	    Adj;
} ADJ_BLOCK;
RN8302_EXT ADJ_BLOCK Adj_Block[3];


RN8302_EXT unsigned char  RN8302Status;
RN8302_EXT unsigned char  RN8302Errcum;

RN8302_EXT unsigned char  Second_8302;
RN8302_EXT unsigned char  Second_Power;
RN8302_EXT unsigned char  PowerRead_Flag;
RN8302_EXT unsigned char  Quadrant[4];//象限指示
RN8302_EXT unsigned char  Angle_I;//角度测量
RN8302_EXT unsigned char  U_InvOrder_Flag;//电压逆相序状态位
RN8302_EXT unsigned char  I_InvOrder_Flag;//电流逆相序状态位
RN8302_EXT unsigned char  FactoryCFDEN;
RN8302_EXT unsigned long  RN8302Checksum;
RN8302_EXT unsigned char  PowerReact1_Flag;
RN8302_EXT unsigned char  PowerReact2_Flag;

extern void RN8302Init(void);
extern void RN8302_Reset(void);
extern void Enable_SPI(void);
extern void RN8302_BasicPara(void);
extern void RN8302_Run(void);
extern void ParaMeasure(void);
extern void InstantPowerMeas(void);
extern unsigned char UMesure_Init(void);
