/************************************************************************
* 版权所有 (C)2016, 杭州西力智能科技股份有限公司。
*
* 文件名称： // RN8302Drive.H
* 文件标识： // RN8302计量驱动文件头
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
#ifdef	RN8302Drive_H
#define	RN8302Drive_EXT
#else
#define	RN8302Drive_EXT extern
#endif
#include "V85XX.h"




extern void RN8302_SPI_Init(void);
extern void RN8302_Delay( unsigned long n );
extern void RN8302_Read( unsigned char *RAM_Addr, unsigned int ADE78xx_Addr, unsigned char Lenth );
extern void RN8302_Write( unsigned int ADE78xx_Addr, unsigned char *RAM_Addr, unsigned char Lenth );