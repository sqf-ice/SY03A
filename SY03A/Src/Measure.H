/************************************************************************
* 版权所有 (C)2016, 杭州西力智能科技股份有限公司。
*
* 文件名称： // Measure.H
* 文件标识： // 电量累加文件头
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

#ifdef	Measure_H
#define	Measure_EXT
#else
#define	Measure_EXT extern
#endif


Measure_EXT signed short  g_PerKwh;//0.01Kwh
Measure_EXT signed short  g_Per1Kwh;//0.1Kwh
Measure_EXT unsigned char FramGrpWriteFlag;
Measure_EXT unsigned char E2PGrpWriteFlag;
Measure_EXT unsigned char FramWriteFlag;
Measure_EXT unsigned char E2PWriteFlag;

//函数声明
extern void MonthEngry_Resume(void);
extern void CycleEngry_Resume(void);
extern void EC_ActGroup(void);
extern void EC_RactGroup(unsigned char MeasureSta, unsigned char Mode);
extern void APulseCnt(void);
extern void RPulseCnt( unsigned short* CntAdd, unsigned char Num );
extern void PulseCount(void);
extern void ECMeasure(void);
extern void PowerMeasure(void);
Measure_EXT void Y_M_C_CycleEngry(unsigned char Mode, unsigned char *ReturnBuf);