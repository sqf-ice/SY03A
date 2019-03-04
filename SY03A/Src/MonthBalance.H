/************************************************************************
* 版权所有 (C)2016, 杭州西力智能科技股份有限公司。
*
* 文件名称： // MonthBalance.H
* 文件标识： //月结算文件头
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
#ifdef	MonthBalance_H
#define	MonthBalance_EXT
#else
#define	MonthBalance_EXT extern
#endif

MonthBalance_EXT unsigned char  Hour_Balance;

extern unsigned char PowerOffTimeCheck(void);
extern void EngryBalanceCheck(void);
extern void BalanceCheck(void);
extern void DmdBalanceCheck(void);
//extern unsigned char PowOnBalance(void);
extern void MonthBalanceProc(void);
MonthBalance_EXT void YearBalanceProc(void);
MonthBalance_EXT unsigned char BalanceMode_Judge(void);
MonthBalance_EXT void ClearYearBalance(void);
