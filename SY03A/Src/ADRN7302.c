/************************************************************************
* 版权所有 (C)2016, 杭州西力智能科技股份有限公司。
*
* 文件名称： // RN8302.c
* 文件标识： // 
* 内容摘要： //计量参数测量程序
* 其它说明： // 
* 当前版本： // V01.00
* 作 者：    // yangxing
* 完成日期： // 2016年08月06日
*
* 修改记录1：//
* 修改日期： //
* 版 本 号： // V01
* 修 改 人： //
* 修改内容： //
************************************************************************/
//#define  RN8302_H

#include "ex_func.h"
#include "ex_var.h"
#include <string.h>
//#include <absacc.h>
//#include <stdio.h>


//RN8302寄存器定义//
#define  UA       0x0007     //28 有符号 Phase A 电压有效值
#define  UB       0x0008     //28 有符号 Phase B 电压有效值
#define  UC       0x0009     //28 有符号 Phase C 电压有效值
#define  IA       0x000B     //28 有符号 Phase A 电流有效值
#define  IB       0x000C     //28 有符号 Phase B 电流有效值
#define  IC       0x000D     //28 有符号 Phase C 电流有效值

#define  PA	  0x0014     //32 有符号 phase A 有功功率瞬时值
#define  PB	  0x0015     //32 有符号 phase B 有功功率瞬时值
#define  PC	  0x0016     //32 有符号 phase C 有功功率瞬时值
#define  PTP	  0x0017     //32 有符号 phase 合相有功功率瞬时值

#define  QA	  0x0018     //32 有符号 phase A 无功功率瞬时值
#define  QB	  0x0019     //32 有符号 phase B 无功功率瞬时值
#define  QC	  0x001A     //32 有符号 phase C 无功功率瞬时值
#define  QTQ	  0x001B     //32 有符号 phase 合相无功功率瞬时值

#define  SA	  0x001C     //32 有符号 phase A 视在功率瞬时值
#define  SB	  0x001D     //32 有符号 phase B 视在功率瞬时值
#define  SC	  0x001E     //32 有符号 phase C 视在功率瞬时值
#define  STS	  0x004C     //32 有符号 phase 合相视在功率瞬时值

#define  PfA	  0x0020     //24 有符号 phase A 功率因数
#define  PfB	  0x0021     //24 有符号 phase A 功率因数
#define  PfC	  0x0022     //24 有符号 phase A 功率因数
#define  PfTV	  0x004D     //24 有符号 phase 合相功率因数

#define  EPA      0x0030    //24 无符号  Phase A 有功能量寄存器
#define  EPB      0x0031    //24 无符号  Phase B 有功能量寄存器
#define  EPC      0x0032    //24 无符号  Phase C 有功能量寄存器
#define  EPT      0x0033    //24 无符号  Phase 合相有功能量寄存器

#define  POSEPA   0x0034    //24 无符号  Phase A 正向有功能量寄存器
#define  POSEPB   0x0035    //24 无符号  Phase B 正向有功能量寄存器
#define  POSEPC   0x0036    //24 无符号  Phase C 正向有功能量寄存器
#define  POSEPT   0x0037    //24 无符号  Phase 合相正向有功能量寄存器

#define  NEGEPA   0x0038    //24 无符号  Phase A 反向有功能量寄存器
#define  NEGEPB   0x0039    //24 无符号  Phase B 反向有功能量寄存器
#define  NEGEPC   0x003A    //24 无符号  Phase C 反向有功能量寄存器
#define  NEGEPT   0x003B    //24 无符号  Phase 合相反向有功能量寄存器

#define  EQA      0x003C    //24 无符号 Phase A 无功能量寄存器
#define  EQB      0x003D    //24 无符号 Phase B 无功能量寄存器
#define  EQC      0x003E    //24 无符号 Phase C 无功能量寄存器
#define  EQT      0x003F    //24 无符号  Phase 合相无功能量寄存器

#define  POSEQA   0x0040    //24 无符号 Phase A 正向无功能量寄存器
#define  POSEQB   0x0041    //24 无符号 Phase B 正向无功能量寄存器
#define  POSEQC   0x0042    //24 无符号 Phase C 正向无功能量寄存器
#define  POSEQT   0x0043    //24 无符号  Phase 合相正向无功能量寄存器

#define  NEGEQA   0x0044    //24 无符号 Phase A 反向无功能量寄存器
#define  NEGEQB   0x0045    //24 无符号 Phase B 反向无功能量寄存器
#define  NEGEQC   0x0046    //24 无符号 Phase C 反向无功能量寄存器
#define  NEGEQT   0x0047    //24 无符号  Phase 合相反向无功能量寄存器

#define  ESA      0x0048    //24 无符号 Phase A 视在能量寄存器
#define  ESB      0x0049    //24 无符号 Phase B 视在能量寄存器
#define  ESC      0x004A    //24 无符号 Phase C 视在能量寄存器
#define  ESTV     0x004F    //24 无符号 Phase 合相视在能量寄存器

#define  YUA	  0x0050    //24 无符号 Phase UA 相角寄存器
#define  YUB	  0x0051    //24 无符号 Phase UB 相角寄存器
#define  YUC	  0x0052    //24 无符号 Phase UC 相角寄存器
#define  YIA	  0x0053    //24 无符号 Phase IA 相角寄存器
#define  YIB	  0x0054    //24 无符号 Phase IB 相角寄存器
#define  YIC	  0x0055    //24 无符号 Phase IC 相角寄存器
#define  YIN	  0x0056    //24 无符号 Phase IN 相角寄存器

#define  UFreq	  0x0057    //24 无符号 电压线频率

//---------------配置和状态寄存器----------------------//
//EMM校表寄存器
#define	HFCONST1	0x0100  //16 无符号 高频脉冲计数寄存器1
#define	HFCONST2	0x0101  //16 无符号 高频脉冲计数寄存器2
#define	IStart_PS	0x0102  //16 无符号 有功视在启动电流发值寄存器
#define	IStart_Q	0x0103  //16 无符号 无功启动电流发值寄存器
#define	LostVoltT	0x0104  //16 无符号 失压阀值寄存器
#define	ZXOT		0x0105  //16 无符号 过零阀值寄存器
#define	PRTH1L		0x0106  //16 无符号 相位分段校正电流阀值1下限
#define	PRTH1H		0x0107  //16 无符号 相位分段校正电流阀值1下限
#define	PRTH2L		0x0108  //16 无符号 相位分段校正电流阀值2下限
#define	PRTH2H		0x0109  //16 无符号 相位分段校正电流阀值2下限

#define	PHSUA		0x010C  //8 无符号 采样通道UA相位校正寄存器
#define	PHSUB		0x010D  //8 无符号 采样通道UB相位校正寄存器
#define	PHSUC		0x010E  //8 无符号 采样通道UC相位校正寄存器
#define	PHSIA		0x010F  //24 无符号 采样通道IA相位校正寄存器
#define	PHSIB		0x0110  //24 无符号 采样通道IV相位校正寄存器
#define	PHSIC		0x0111  //24 无符号 采样通道IC相位校正寄存器

#define	GSUA		0x0113  //16 有符号 采样通道UA增益
#define	GSUB		0x0114  //16 有符号 采样通道UB增益
#define	GSUC		0x0115  //16 有符号 采样通道UC增益
#define	GSIA		0x0116  //16 有符号 采样通道IA增益
#define	GSIB		0x0117  //16 有符号 采样通道IB增益
#define	GSIC		0x0118  //16 有符号 采样通道IC增益
#define	GSIN		0x0119  //16 有符号 采样通道IN增益

#define	DCOS_UA		0x011A  //16 有符号 采样通道UA直流Offset校正
#define	DCOS_UB		0x011B  //16 有符号 采样通道UB直流Offset校正
#define	DCOS_UC		0x011C  //16 有符号 采样通道UC直流Offset校正
#define	DCOS_IA		0x011D  //16 有符号 采样通道IA直流Offset校正
#define	DCOS_IB		0x011E  //16 有符号 采样通道IB直流Offset校正
#define	DCOS_IC		0x011F  //16 有符号 采样通道IC直流Offset校正

#define	UA_OS		0x0121  //16 有符号 A相电压有效值Offset
#define	UB_OS		0x0122  //16 有符号 B相电压有效值Offset
#define	UC_OS		0x0123  //16 有符号 C相电压有效值Offset
#define	IA_OS		0x0124  //16 有符号 A相电流有效值Offset
#define	IB_OS		0x0125  //16 有符号 B相电流有效值Offset
#define	IC_OS		0x0126  //16 有符号 C相电流有效值Offset

#define	GPA		0x0128  //16 有符号 A相有功功率增益
#define	GPB		0x0129  //16 有符号 B相有功功率增益
#define GPC		0x012A  //16 有符号 C相有功功率增益
#define	GQA		0x012B  //16 有符号 A相无功功率增益
#define	GQB		0x012C  //16 有符号 B相无功功率增益
#define	GQC		0x012D  //16 有符号 C相无功功率增益
#define	GSA		0x012E  //16 有符号 A相视功功率增益
#define	GSB		0x012F  //16 有符号 B相视功功率增益
#define	GSC		0x0130  //16 有符号 C相视功功率增益

#define	PA_PHSL		0x0131  //16 有符号 A相有功分段相位校正寄存器-低段
#define	PB_PHSL		0x0132  //16 有符号 B相有功分段相位校正寄存器-低段
#define	PC_PHSL		0x0133  //16 有符号 C相有功分段相位校正寄存器-低段
#define	PA_PHSM		0x01B0  //16 有符号 A相有功分段相位校正寄存器-中段
#define	PB_PHSM		0x01B2  //16 有符号 B相有功分段相位校正寄存器-中段
#define	PC_PHSM		0x01B4  //16 有符号 C相有功分段相位校正寄存器-中段
#define	PA_PHSH		0x01B1  //16 有符号 A相有功分段相位校正寄存器-高段
#define	PB_PHSH		0x01B3  //16 有符号 B相有功分段相位校正寄存器-高段
#define	PC_PHSH		0x01B5  //16 有符号 C相有功分段相位校正寄存器-高段

#define	QA_PHSL		0x0134  //16 有符号 A相无功分段相位校正寄存器-低段
#define	QB_PHSL		0x0135  //16 有符号 B相无功分段相位校正寄存器-低段
#define	QC_PHSL		0x0136  //16 有符号 C相无功分段相位校正寄存器-低段
#define	QA_PHSM		0x01B6  //16 有符号 A相无功分段相位校正寄存器-中段
#define	QB_PHSM		0x01B8  //16 有符号 B相无功分段相位校正寄存器-中段
#define	QC_PHSM		0x01BA  //16 有符号 C相无功分段相位校正寄存器-中段
#define	QA_PHSH		0x01B7  //16 有符号 A相无功分段相位校正寄存器-高段
#define	QB_PHSH		0x01B9  //16 有符号 B相无功分段相位校正寄存器-高段
#define	QC_PHSH		0x01BB  //16 有符号 C相无功分段相位校正寄存器-高段

#define	PA_OS		0x0137  //16 有符号 A相有功功率Offset
#define	PB_OS		0x0138  //16 有符号 B相有功功率Offset
#define	PC_OS		0x0139  //16 有符号 C相有功功率Offset
#define	QA_OS		0x013A  //16 有符号 A相无功功率Offset
#define	QB_OS		0x013B  //16 有符号 B相无功功率Offset
#define	QC_OS		0x013C  //16 有符号 C相无功功率Offset
//EMM配置和状态寄存器
#define	CFCFG		0x0160  //24 CF引脚配置寄存器
#define EMUCFG          0x0161  //24 EMU配置寄存器
#define EMUCON          0x0162  //24 EMU控制寄存器
#define	PQSign		0x0166  //16 有功无功功率方向寄存器
#define	PHASES		0x0169  //16 相电压电流状态寄存器
#define	CheckSum1	0x016A  //24 EMM校表和配置寄存器校验和
//系统配置寄存器
#define	WREN		0x0180  //8  写使能寄存器
#define	WMSW		0x0181  //8  工作模式切换寄存器
#define	SOFTRST		0x0182  //8  软件复位寄存器
#define	ADCCFG		0x0183  //16 ADC配置寄存器
#define	MODSEL		0x0186  //8  三相四线/三线模式选择寄存器
//系统状态寄存器
#define	RData		0x018C  //32 上次SPI读出的数据
#define	WData		0x018D  //32 上次SPI写入的数据
#define	DeviceID	0x018f  //24 RN8302 Device ID

#define	ZXOTU		0x0190  //16 无符号 过零阀值电压和电流分开寄存器
#define	ZXOTCFG		0x0192  //16 无符号 过零阀值选择控制寄存器






//unsigned char  RN8302Status;
//unsigned char  RN8302Errcum;
//unsigned char  Second_8302;
//unsigned char  Second_Power;
//unsigned char  PowerRead_Flag;
//unsigned char  PowerReact1_Flag;
//unsigned char  PowerReact2_Flag;
//unsigned char  Quadrant[4];
//unsigned char  Angle_I;
//unsigned char  U_InvOrder_Flag;
//unsigned char  I_InvOrder_Flag;
//unsigned char  FactoryCFDEN;
//unsigned long  RN8302Checksum;


/**************************************************************************/
unsigned char BYTE_BCD( unsigned char HEX )
{
    if( HEX > 99 )
    {
        return 0x00;
    }

    return( (HEX/10)*0x10 + HEX%10 );
}

/**************************************************************************/
unsigned char BCD_BYTE( unsigned char BCD )
{
    return( ((BCD>>4) & 0x0F)*10 + (BCD & 0x0F) );
}

/************************************************************************
* 函数名 ：//WORD_BCD2
* 功能 ：//将双字节HEX码转换为双字节BCD码
* 输入参数 ：//BCD：双字节BCD码数据地址,WORD：双字节HEX码数据
* 输出参数 ：//2字节BCD码
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void WORD_BCD2( unsigned char *BCD, unsigned short WORD )
{
    if( WORD > 9999 )
    {
        *BCD = 0x00;
        *(BCD+1) = 0x00;
        return;
    }
    
    *(BCD+1) = BYTE_BCD( (unsigned char)(WORD/100) );
    *BCD = BYTE_BCD( (unsigned char)(WORD%100) );
}

/************************************************************************
* 函数名 ：//BCD2_WORD
* 功能 ：//将双字节BCD码转换为双字节HEX码
* 输入参数 ：//BCD：双字节BCD码数据地址
* 输出参数 ：//双字节HEX码数据
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
unsigned int BCD2_WORD( unsigned char *BCD )
{
    return ( BCD_BYTE( *(BCD+1) )*100 + BCD_BYTE( *BCD ) );
}

/************************************************************************
* 函数名 ：//HEX4_BCD4
* 功能 ：//将4字节HEX码转换为4字节BCD码
* 输入参数 ：//BCD：4字节BCD码数据地址，HEX：4字节HEX码数据
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void HEX4_BCD4( unsigned char *BCD, unsigned long HEX )
{
    unsigned char i;

    if( HEX > 99999999 )
    {
        *BCD = 0x00;
        *(BCD+1) = 0x00;
        *(BCD+2) = 0x00;
        *(BCD+3) = 0x00;
    }
    else
    {
        for( i = 0; i < 4; i++ )
        {
            *BCD = HEX%10;
            HEX = HEX/10;
            *BCD += ((HEX%10)<<4) & 0xF0;
            HEX = HEX/10;
            BCD++;
        }
    }
}

/************************************************************************
* 函数名 ：//BCD4_HEX4
* 功能 ：//将4字节BCD码转换为4字节HEX码
* 输入参数 ：//BCD：4字节BCD码数据地址
* 输出参数 ：//4字节HEX码数据
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
unsigned long BCD4_HEX4( unsigned char *BCD )
{
    unsigned char i,Tmp;
    unsigned long HEX=0;

    BCD = BCD + 3;
    for( i=0;i<3;i++ )
    {
        Tmp = BCD_BYTE( *BCD );
        HEX = (HEX + Tmp) * 100;
        BCD--;
    }
    Tmp = BCD_BYTE( *BCD );
    HEX = HEX + Tmp;
    return HEX;
}
/**************************************************************************/
void Delay_Xms(unsigned long mSec)
{   
	while (mSec--)	
	{           
      I2c_Wait_1ms();
	}
}
/************************************************************************
* 函数名 ：//RN8302_Reset
* 功能 ：//RN8302复位
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/06 V1.0 yangxing XXXX
**************************************************************************/
void RN8302_Reset(void)
{
    unsigned char Temp=0;
    Clear_Wdt();//清看门狗
    
    Temp = 0xE5;//写使能
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );
    Temp = 0xA2;//切换工作模式到EMM
    RN8302_Write( WMSW, (unsigned char*)&Temp, 1 ); 
    Temp = 0xFA;//软件复位
    RN8302_Write( SOFTRST, (unsigned char*)&Temp, 1 );
    Delay_Xms(50);
    Temp = 0xDC;//写使能
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );
    Clear_Wdt();//清看门狗
}

/************************************************************************
* 函数名 ：//Enable_SPI
* 功能 ：//RN8302锁定SPI通讯
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/06 V1.0 yangxing XXXX
**************************************************************************/
void Enable_SPI(void)
{
    unsigned char i;

    for( i = 0; i < 3; i++ )
    {
        P_RN8302_CS_H;// CS=1
        P_RN8302_CS_PORT;
        Delay_Xms(1);
        P_RN8302_CS_L;// CS=0
        Delay_Xms(1);
         Clear_Wdt();//清看门狗
    }
    Delay_Xms(1);
    P_RN8302_CS_H;// CS=1
    Delay_Xms(10);
    Clear_Wdt();//清看门狗
}

/************************************************************************
* 函数名 ：//RN8302_BasicPara
* 功能 ：//基本参数设置
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/06 V1.0 yangxing XXXX
**************************************************************************/
void RN8302_BasicPara(void)
{
    unsigned long Temp=0;
    
    RN8302Checksum = 0;
    Temp = 0xE5;//写使能
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );
          
    RN8302_Read( (unsigned char*)&Temp, EMUCON, 3 );
    Temp = 0x000777;//使能分相有功、无功、视在电能计量且分相参与合相电能计算
    RN8302_Write( EMUCON, (unsigned char*)&Temp, 3 );
//	  Temp = 0x000000;
//	  RN8302_Read( (unsigned char*)&Temp, EMUCON, 3 );
    RN8302Checksum +=Temp;

    Temp = MeterParameter.MeterHFConst1;//高频脉冲计数寄存器1,默认值：0x1000
    RN8302_Write( HFCONST1, (unsigned char*)&Temp, 2 );
    RN8302Checksum +=Temp;
    
    Temp = MeterParameter.MeterHFConst2;//高频脉冲计数寄存器2,默认值：0x1000
    RN8302_Write( HFCONST2, (unsigned char*)&Temp, 2 );
    RN8302Checksum +=Temp;
    
    Temp = 0x0000;//电流增益1倍、电压增益1倍
    RN8302_Write( ADCCFG, (unsigned char*)&Temp, 2 ); 
    
    if( MeterParameter.Meter_Type == 0x34 )//三相四线模式
        Temp = 0x00;
    else//三相三线模式
        Temp = 0x33;
    RN8302_Write( MODSEL, (unsigned char*)&Temp, 1 ); 
    
    Temp = Factor.IStart_P;//视在、有功启动电流门限值寄存器,默认值：0x0250
    RN8302_Write( IStart_PS, (unsigned char*)&Temp, 2 );   
    RN8302Checksum +=Temp;
    
    //Temp = Factor.IStart_Q;//无功启动电流门限值寄存器
    RN8302_Write( IStart_Q, (unsigned char*)&Temp, 2 ); //默认值：0x0250
    RN8302Checksum +=Temp;
    
    Temp = (unsigned long)(MeterParameter.RN8302Irms*0.34);//相位分段校正电流阀值1下限
    Temp = (unsigned long)(Temp >> 12);//取高16位
    RN8302_Write( PRTH1L, (unsigned char*)&Temp, 2 );
    RN8302Checksum +=Temp;
    
    Temp = (unsigned long)(MeterParameter.RN8302Irms*0.35);//相位分段校正电流阀值1上限
    Temp = (unsigned long)(Temp >> 12);//取高16位
    RN8302_Write( PRTH1H, (unsigned char*)&Temp, 2 );
    RN8302Checksum +=Temp;
    if(MeterParameter.CT == 0x03 )//直接式
        Temp = (unsigned long)(MeterParameter.RN8302Irms*5.4);//相位分段校正电流阀值2下限
    else
        Temp = (unsigned long)(MeterParameter.RN8302Irms*2.4);//相位分段校正电流阀值2下限
    Temp = (unsigned long)(Temp >> 12);//取高16位
    RN8302_Write( PRTH2L, (unsigned char*)&Temp, 2 );
    RN8302Checksum +=Temp;
    
    if(MeterParameter.CT == 0x03 )
        Temp = (unsigned long)(MeterParameter.RN8302Irms*5.5);//相位分段校正电流阀值2上限
    else
        Temp = (unsigned long)(MeterParameter.RN8302Irms*2.5);//相位分段校正电流阀值2下限
    Temp = (unsigned long)(Temp >> 12);//取高16位
    RN8302_Write( PRTH2H, (unsigned char*)&Temp, 2 );
    RN8302Checksum +=Temp;  

    Temp = (unsigned long)(MeterParameter.RN8302Irms*0.001/4096);//过零阀值寄存器默认值：0x0073
    RN8302_Write( ZXOT, (unsigned char*)&Temp, 2 );
    RN8302Checksum +=Temp;
    
    Temp = (unsigned long)(MeterParameter.RN8302Urms*0.05/4096);//过零阀值寄存器默认值：0x0000
    RN8302_Write( ZXOTU, (unsigned char*)&Temp, 2 );
    RN8302Checksum +=Temp;

    Temp = 0x0010;//电压夹角和频率采用ZXOTU作为判断阀值：0x0010
    RN8302_Write( ZXOTCFG, (unsigned char*)&Temp, 2 );
    
    Temp = 0x047798;//CF1使能输出有功电量，CF2使能输出无功电量，CF3、CF4禁止输出默认值：0x043210
    RN8302_Write( CFCFG, (unsigned char*)&Temp, 3 );
    RN8302Checksum +=Temp;
    
    Temp = 0x400000;//合相有、无功能量累加寄存器采用代数和累加模式、读后清零、零线不参与计算、全波启动电流判断、高通滤波使能
    RN8302_Write( EMUCFG, (unsigned char*)&Temp, 3 );//默认值：0x400000
    RN8302Checksum +=Temp;

    Temp = 0xDC;//写保护
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );   

}

/************************************************************************
* 函数名 ：//ADE7858_AdjPara
* 功能 ：//ADE7858校表参数设置
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------

**************************************************************************/
void RN8302_AdjPara(void)
{
    unsigned char i;
    unsigned long Temp=0;
    
    Meter_RN7302Para_Verify();
    
    Temp = 0xE5;//写使能
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );
    
    for( i = 0; i < 3; i++ )//注意，校表参数如果是负数，最高字节补0F
    {   //电压电流增益
        RN8302_Write( (GSIA+i), (unsigned char*)&Adj_Block[i].Adj.Igain, 2 );//电流增益
        RN8302Checksum +=Adj_Block[i].Adj.Igain;
        RN8302_Write( (GSUA+i), (unsigned char*)&Adj_Block[i].Adj.Ugain, 2 );//电压增益
        RN8302Checksum +=Adj_Block[i].Adj.Ugain;
        //电压电流偏移
        RN8302_Write( (IA_OS+i), (unsigned char*)&Adj_Block[i].Adj.Iofst, 2 );//电流偏移
        RN8302Checksum +=Adj_Block[i].Adj.Iofst;
        RN8302_Write( (UA_OS+i), (unsigned char*)&Adj_Block[i].Adj.Uofst, 2 );//电压偏移
        RN8302Checksum +=Adj_Block[i].Adj.Uofst;
        //功率增益
        RN8302_Write( (GSA+i), (unsigned char*)&Adj_Block[i].Adj.Pgain, 2 );//视在功率增益
        RN8302Checksum +=Adj_Block[i].Adj.Pgain;
        RN8302_Write( (GPA+i), (unsigned char*)&Adj_Block[i].Adj.Pgain, 2 );//有功率增益
        RN8302Checksum +=Adj_Block[i].Adj.Pgain;
        
        Adj_Block2[i].Adj2.Iofst2=0x0000;
        RN8302_Write( (DCOS_IA+i), (unsigned char*)&Adj_Block2[i].Adj2.Iofst2, 2 );//直流OFFSET
        
        if( Meterspecial.AdjCheck_Mode == 0x5A )//无功校表
        {
            RN8302_Write( (GQA+i), (unsigned char*)&Adj_Block[i].Adj.Qgain, 2 );//无功率增益
            RN8302Checksum +=Adj_Block[i].Adj.Qgain;
        }
        else
        {
            RN8302_Write( (GQA+i), (unsigned char*)&Adj_Block[i].Adj.Pgain, 2 );//无功率增益
            RN8302Checksum +=Adj_Block[i].Adj.Pgain;
        }

        //功率偏移
        RN8302_Write( (PA_OS+i), (unsigned char*)&Adj_Block[i].Adj.Pofst, 2 );//有功率偏移
        RN8302Checksum +=Adj_Block[i].Adj.Pofst;
        RN8302_Write( (QA_OS+i), (unsigned char*)&Adj_Block[i].Adj.Pofst, 2 );//无功功率偏移
        RN8302Checksum +=Adj_Block[i].Adj.Pofst;
        //通道相位
        RN8302_Write( (PHSUA+i), (unsigned char*)&Adj_Block[i].Adj.UTofst, 1 );//采样通道U相位偏移
        RN8302Checksum +=Adj_Block[i].Adj.UTofst;
        RN8302_Write( (PHSIA+i), (unsigned char*)&Adj_Block[i].Adj.ITofst, 3 );//采样通道I相位偏移 
        memcpy( (unsigned char*)&Temp, (unsigned char*)&Adj_Block[i].Adj.ITofst, 3 );
        RN8302Checksum += Temp;
        //功率相位
        RN8302_Write( (PA_PHSL+i), (unsigned char*)&Adj_Block[i].Adj.PTofst[1], 2 );//低段有功相位偏移
        RN8302Checksum +=Adj_Block[i].Adj.PTofst[1];
        
        Temp=0x00;
        memcpy( (unsigned char*)&Temp, (unsigned char*)&Adj_Block[i].Adj.PTofst[0], 2 );
        RN8302_Write( (PA_PHSM+i*2), (unsigned char*)&Temp, 3 );//中段有功相位偏移（寄存器为3个字节，只是高字节未用）
        RN8302Checksum +=Temp;
        
        Temp=0x00;
        memcpy( (unsigned char*)&Temp, (unsigned char*)&Adj_Block[i].Adj.PTofst[2], 2 );
        RN8302_Write( (PA_PHSH+i*2), (unsigned char*)&Temp, 3 );//高段有功相位偏移
        RN8302Checksum +=Temp;
        
        if( Meterspecial.AdjCheck_Mode == 0x5A )//无功校表
        {
            RN8302_Write( (QA_PHSL+i), (unsigned char*)&Adj_Block[i].Adj.QTofst[1], 2 );//低段无功相位偏移
            RN8302Checksum +=Adj_Block[i].Adj.QTofst[1];
        }
        else
        {
            RN8302_Write( (QA_PHSL+i), (unsigned char*)&Adj_Block[i].Adj.PTofst[1], 2 );//低段无功相位偏移
            RN8302Checksum +=Adj_Block[i].Adj.PTofst[1];
        }

        Temp=0x00;
        if( Meterspecial.AdjCheck_Mode == 0x5A )//无功校表
        {
            memcpy( (unsigned char*)&Temp, (unsigned char*)&Adj_Block[i].Adj.QTofst[0], 2 );
        }
        else
            memcpy( (unsigned char*)&Temp, (unsigned char*)&Adj_Block[i].Adj.PTofst[0], 2 );
        RN8302_Write( (QA_PHSM+i*2), (unsigned char*)&Temp, 3 );//中段无功相位偏移
        RN8302Checksum +=Temp;
        
        Temp=0x00;
        if( Meterspecial.AdjCheck_Mode == 0x5A )//无功校表
        {
            memcpy( (unsigned char*)&Temp, (unsigned char*)&Adj_Block[i].Adj.QTofst[2], 2 );
        }
        else
            memcpy( (unsigned char*)&Temp, (unsigned char*)&Adj_Block[i].Adj.PTofst[2], 2 );
        RN8302_Write( (QA_PHSH+i*2), (unsigned char*)&Temp, 3 );//高段无功相位偏移
        RN8302Checksum +=Temp;
        
        memcpy( (unsigned char*)&Temp, (unsigned char*)&Adj_Block[i].Adj.Igain, 2 );
        if(Temp<0x8000)
            Nvm1Ki.Ki[i]= (float)((float)Factor.Ki*16*(float)(Temp/32768+1))/0.87909;
        else
        {
            Temp=65536-Temp;
            Nvm1Ki.Ki[i]= (float)((float)Factor.Ki*16*(float)(1-(float)Temp/32768))/0.87909;
        }
    }
    RN8302Checksum += 0x480;//失压阀值寄存器0x400+采样通道IN相位寄存器0x80
    RN8302Checksum = ((~RN8302Checksum) & 0x00FFFFFF);//校验和
    RN8302_Read( (unsigned char*)&Temp, CheckSum1, 3 );//读校验和
    if(Temp == RN8302Checksum)
        WriteE2WithBackup( ee_Chk_RN8302_page, ee_Chk_RN8302_inpage, (unsigned char*)&RN8302Checksum, ee_Chk_RN8302_lenth);

    
    Temp = 0xDC;//写保护
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );
}

/************************************************************************
* 函数名 ：//ADE7858_Run
* 功能 ：//ADE7858测量开启
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/06 V1.0 yangxing XXXX
**************************************************************************/
void RN8302_Run(void)
{
    unsigned int Temp=0;

    Temp = 0xE5;//写使能
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );
    
    Temp = 0xA2;//切换工作模式到EMM
    RN8302_Write( WMSW, (unsigned char*)&Temp, 1 );    
    
    Temp = 0xDC;//写保护
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );
}

/************************************************************************
* 函数名 ：//ADE7858Init
* 功能 ：//ADE7858初始化
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/06 V1.0 
**************************************************************************/
void RN8302Init(void)
{
    Enable_SPI();//选择SPI通讯模式
    RN8302_SPI_Init();
	  RN8302_Reset();//复位
    RN8302_BasicPara();//基本参数配置
    RN8302_AdjPara();//校表参数配置
    Timer.PowerRead_Tmr = 0;
    Flag.PhOfset0 = 0;
    Flag.PhOfset1 = 0;
    RN8302Errcum = 0;
}

/************************************************************************
* 函数名 ：//UIMeas
* 功能 ：//电压电流测量
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/06 V1.0 yangxing XXXX
**************************************************************************/
void UIMeas(void)
{
    unsigned char i;
    unsigned long Volt,Curnt;

    for( i = 0; i < 3; i++ )//读取电压、电流有效值
    {
        if( ( i == 1 )&&(MeterParameter.Meter_Type == 0x33) )
            continue;
        RN8302_Read( (unsigned char*)&ChipReg.Urms[i], (UA+i), 4 );//电压有效值
        if(( ChipReg.Urms[i] & 0XF8000000 ) != 0)
        {
            ChipReg.Urms[i] = 0;
            RN8302Errcum++;
        }
        RN8302_Read( (unsigned char*)&ChipReg.Irms[i], (IA+i), 4 );//电流有效值
        if(( ChipReg.Irms[i] & 0XF8000000 ) != 0)
        {
            ChipReg.Irms[i] = 0;
            RN8302Errcum++; 
        }
        Volt = (unsigned long)(((float)(ChipReg.Urms[i]*Factor.Ku))*1000);//XXX.XXX
        Curnt = (unsigned long)(((float)(ChipReg.Irms[i]*Factor.Ki))*10000);//XXX.XXXX

        if( (Volt > Factor.DispUnDown) && (Volt < Factor.DispUnUp) )
        {
            HEX4_BCD4((unsigned char*)&RealAdj.UAdj[i],Volt);//校表电压
            Real.U[i] = (unsigned short)(RealAdj.UAdj[i] >> 8);//显示电压
            if( Real.U[i] < MeasPara.Un75 )//为了切换相电压用于电网频率检测
                Flag.LineV |= ( F_LineA << i );
            else
                Flag.LineV &= ~( F_LineA << i );
        }
        else
        {
            RealAdj.UAdj[i] = 0;
            Real.U[i] = 0;
            Flag.LineV |= ( F_LineA << i );
        }

        if( Curnt < Factor.ImaxUp )
        {
            HEX4_BCD4((unsigned char*)&RealAdj.IAdj[i],Curnt);//校表电流XX.XXXX
            Real.I[i] = (RealAdj.IAdj[i]>>4);
            
            if( MeterParameter.CT == 3 )
            {
                if(( (Real.I[i]&0x007FFFFF) < MeasPara.StartI )||(Timer.PowerUpTim_Tmr !=0 ))//格式XXX.XXX
                {
                    RealAdj.IAdj[i] = 0;
                    Real.I[i] = 0;
                }
            }
            else
            {
                if(( (RealAdj.IAdj[i]&0x007FFFFF) <= MeasPara.StartI2 )||(Timer.PowerUpTim_Tmr !=0 ))//格式XX.XXXX
                {
                    RealAdj.IAdj[i] = 0;
                    Real.I[i] = 0;
                }
            }
        }
    }
}
/************************************************************************
* 函数名 ：//PowerMeas
* 功能 ：//功率测量
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/06 V1.0 yangxing XXXX
**************************************************************************/
void PowerMeas(void)
{
    signed long Power[16];//用来存放临时有功\无功\视在功率\功率因数
    unsigned long ActPower,RActPower,ApparentPower;
    unsigned char i,Num;
    unsigned short Angle;
    
    memset( (unsigned char*)&Power[0], 0x00, 64 );

    for( i = 0; i < 3; i++ )//分相
    {
        if( ( i == 1 )&&(MeterParameter.Meter_Type == 0x33) )
            continue;
        RN8302_Read( (unsigned char*)&Power[i], (PA+i), 4 );//读取分相有功功率
        RN8302_Read( (unsigned char*)&Power[i+4], (QA+i), 4 );//读取分相无功功率
        RN8302_Read( (unsigned char*)&Power[i+8], (SA+i), 4 );//读取分相视在功率
        RN8302_Read( (unsigned char*)&Power[i+12], (PfA+i), 3 );//读取分相功率因数
        Power[i+12] = (0x007FFFFF&Power[i+12]);

        if( Power[i] < 0 )//有功方向判断
        {
            Power[i] = ~Power[i]+1;
            Power[i+12] = (8388608-Power[i+12]);
            Flag.ECDir |= ( F_ActRevA << i );
        }
        else
            Flag.ECDir &= ~( F_ActRevA << i );

        if( Power[i+4] < 0 )//无功方向判断
        {
            Power[i+4] = ~Power[i+4]+1;
            Flag.ECDir |= ( F_RActRevA << i );
        }
        else
            Flag.ECDir &= ~( F_RActRevA << i );

        ActPower = (unsigned long)(((float)(Power[i]*Factor.Kp))*10);//有功功率
        RActPower = (unsigned long)(((float)(Power[i+4]*Factor.Kp))*10);//无功功率
        ApparentPower = (unsigned long)(((float)(Power[i+8]*Factor.Kp)));//视在功率

        HEX4_BCD4((unsigned char*)&RealAdj.PAdj[i+1],ActPower);//校表功率XX.XXXXX
        Real.PKw[i+1] = (RealAdj.PAdj[i+1] >> 4);//显示有功功率XX.XXXX
        HEX4_BCD4( (unsigned char*)&RealAdj.RAdj[i+1],RActPower );//显示无功功率XX.XXXX
        Real.PKvar[i+1] = (RealAdj.RAdj[i+1] >> 4);//显示无功功率XX.XXXX
        HEX4_BCD4( (unsigned char*)&Real.PKva[i+1],ApparentPower );//显示视在功率XX.XXXX
        
        if((( (RealAdj.PAdj[i+1]&0x07FFFFFF) <= MeasPara.StartP2 )&&( (Real.I[i]&0x007FFFFF) < MeasPara.Ib05 ))||( Timer.PowerUpTim_Tmr !=0 ))
        {
            RealAdj.PAdj[i+1]=0;
            Real.PKw[i+1] = 0;
            Power[i] = 0;
            Flag.ECDir &= ~( F_ActRevA << i );
        }

        if((( (RealAdj.RAdj[i+1]&0x07FFFFFF) <= MeasPara.StartP2 )&&( (Real.I[i]&0x007FFFFF) < MeasPara.Ib05 ))||( Timer.PowerUpTim_Tmr !=0 ))
        {
            Real.PKvar[i+1] = 0;
            Power[i+4] = 0;
            Flag.ECDir &= ~( F_RActRevA << i );
        }

        if(( (Real.PKva[i+1]&0x007FFFFF) <= MeasPara.StartP )||( Timer.PowerUpTim_Tmr !=0 ))
        {
            Real.PKva[i+1] = 0;
            Power[i+8] = 0;
        }
        
        if( ((Real.I[i]&0x007FFFFF) < MeasPara.StartI )||(Real.PKva[i+1] == 0) ) //功率因数测量条件:0.1Ib 
            Real.PF[i+1] = 0x1000;  
        else
        {
            Angle = (unsigned short)((float)(Power[i+12]/(float)8388608)*1000);//功率因数
            if( Angle > 1000 )
                Angle = 1000;
            WORD_BCD2( (unsigned char*)&Real.PF[i+1], Angle );//显示功率因数X.XXX
        }

        if( Flag.ECDir & ( F_ActRevA << i ) )//有功符号
        {
            if( Real.I[i]!= 0 )
                Real.I[i] |= 0x800000;
            if( Real.PKw[i+1]!= 0 )
                Real.PKw[i+1] |= 0x800000;
            if( Real.PF[i+1]!= 0 )
                Real.PF[i+1] |= 0x8000;
        }
        else
        {
            Real.I[i] &= ~0x800000;
            Real.PKw[i+1] &= ~0x800000;
            Real.PF[i+1] &= ~0x8000;
        }

        if( Flag.ECDir & ( F_RActRevA << i ) )//无功符号
        {
            if( Real.PKvar[i+1]!= 0 )
                Real.PKvar[i+1] |= 0x800000;
        }
        else
            Real.PKvar[i+1] &= ~0x800000;
             
    }
    
    RN8302_Read( (unsigned char*)&Power[3], PTP, 4 );//读取合相有功功率(注意：合相不受累加模式的影响，只有代数和模式)
    RN8302_Read( (unsigned char*)&Power[7], QTQ, 4 );//读取合相无功功率
    RN8302_Read( (unsigned char*)&Power[11], STS, 4 );//读取合相视在功率
    RN8302_Read( (unsigned char*)&Power[15], PfTV, 3 );//读取合相功率因数
    Power[15] = (0x007FFFFF&Power[15]);
    
    if(  Power[3] < 0 )//计算总有功功率
    {
        Power[3] = ~Power[3]+1;
        Power[15] = (8388608-Power[15]);
        Flag.ECDir |= F_ActRevZ;
    }
    else
        Flag.ECDir &= ~F_ActRevZ;
    
    if( Power[7] < 0 )//计算总无功功率
    {
        Power[7] = ~Power[7]+1;
        Flag.ECDir |= F_RActRevZ;
    }
    else
        Flag.ECDir &= ~F_RActRevZ;
//需验证精度    
    ActPower = (unsigned long)(((float)(Power[3]*Factor.Kp))*10);//有功功率
    RActPower = (unsigned long)(((float)(Power[7]*Factor.Kp))*10);//无功功率
    ApparentPower = (unsigned long)((float)(Power[11]*Factor.Kp));//视在功率

    HEX4_BCD4( (unsigned char*)&RealAdj.PAdj[0],ActPower );//总有功功率
    Real.PKw[0] = (RealAdj.PAdj[0] >> 4);//显示总有功功率XX.XXXX
    ActPower = (ActPower/10);
    HEX4_BCD4( (unsigned char*)&RealAdj.RAdj[0],RActPower );//总无功功率
    Real.PKvar[0] = (RealAdj.RAdj[0] >> 4);//显示总无功功率XX.XXXX
    RActPower = (RActPower/10);
    HEX4_BCD4( (unsigned char*)&Real.PKva[0],ApparentPower );//总视在功率

    Angle = (unsigned short)((float)(Power[15]/(float)8388608)*1000);//功率因数
    if(( Angle > 1000 )||( Real.PKva[0]==0 ))
        Angle = 1000;
    WORD_BCD2( (unsigned char*)&Real.PF[0], Angle );//显示功率因数X.XXX

    if( MeterParameter.Meter_Type == 0x34 )
    {
        if((Timer.PowerUpTim_Tmr !=0)||(( (RealAdj.PAdj[0]&0x07FFFFFF) <= MeasPara.StartP2 )&&(Real.PKw[1]== 0)&&(Real.PKw[2]== 0)&&(Real.PKw[3]== 0)))//如果有功功率过小
        {
           Real.PKw[0] = 0;
           ActPower = 0;
           Flag.ECDir &= ~F_ActRevZ;
        }
        if((Timer.PowerUpTim_Tmr !=0)||(( (RealAdj.RAdj[0]&0x07FFFFFF) <= MeasPara.StartP2 )&&(Real.PKvar[1]== 0)&&(Real.PKvar[2]== 0)&&(Real.PKvar[3]== 0)))//如果无功功率过小
        {
           Real.PKvar[0] = 0;
           RActPower = 0;
           Flag.ECDir &= ~F_RActRevZ;
        }
        if((Timer.PowerUpTim_Tmr !=0)||(( (Real.PKva[0]&0x007FFFFF) <= MeasPara.StartP )&&(Real.PKva[1]== 0)&&(Real.PKva[2]== 0)&&(Real.PKva[3]== 0)))//如果视在功率过小
        {
           Real.PKva[0] = 0;
           Real.PF[0] = 0x1000;
           ApparentPower = 0;
        }
    }
    else
    {
        if((Timer.PowerUpTim_Tmr !=0)||(( (RealAdj.PAdj[0]&0x07FFFFFF) <= MeasPara.StartP2 )&&(Real.PKw[1]== 0)&&(Real.PKw[3]== 0)))//如果有功功率过小
        {
           Real.PKw[0] = 0;
           ActPower = 0;
           Flag.ECDir &= ~F_ActRevZ;
        }
        if((Timer.PowerUpTim_Tmr !=0)||(( (RealAdj.RAdj[0]&0x07FFFFFF) <= MeasPara.StartP2 )&&(Real.PKvar[1]== 0)&&(Real.PKvar[3]== 0)))//如果无功功率过小
        {
           Real.PKvar[0] = 0;
           RActPower = 0;
           Flag.ECDir &= ~F_RActRevZ;
        }
        if((Timer.PowerUpTim_Tmr !=0)||(( (Real.PKva[0]&0x007FFFFF) <= MeasPara.StartP )&&(Real.PKva[1]== 0)&&(Real.PKva[3]== 0)))//如果视在功率过小
        {
           Real.PKva[0] = 0;
           Real.PF[0] = 0x1000;
           ApparentPower = 0;
        }
    }
    
    if( Flag.Run & F_Fist )
    {
        Flag.Run &= ~F_Fist;
        if( Flag.ECDir & F_ActRevZ )
            Flag.Run |= F_RevZP0;//上电第一次检查到功率的方向同步
        else 
            Flag.Run &=~ F_RevZP0;
    }
    
    if(( Flag.ECDir & F_ActRevZ )&&(Real.PKw[0] != 0))
    {
        Real.PKw[0] |= 0x800000;
        Real.PF[0] |= 0x8000;
    }
    else
    {
        Real.PKw[0] &= ~0x800000;
        Real.PF[0] &= ~0x8000;
    }

    if(( Flag.ECDir & F_RActRevZ )&&(Real.PKvar[0] != 0))
        Real.PKvar[0] |= 0x800000;
    else
        Real.PKvar[0] &= ~0x800000;

    
    //象限判断
    for( i = 0; i < 4; i++ )
    {
        Num = 0;

        if( (Flag.ECDir & (F_ActRevZ<<i)) == 0 )//有功正向
        {
            Num = 10;
        }
        else
        {
            if( i == 0 )//这里操作保证象限切换和有功功率方向同步
            {
                if( Timer.Rev_Cnt[3] < 2 )
                    Num = 10;
            }
            else
            {
                if( Timer.Rev_Cnt[i-1] < 2 )
                    Num = 10;
            }
        }

        if( (Flag.ECDir & (F_RActRevZ<<i)) == 0 )//无功正向
            Num += 1;

        switch( Num )
        {
            case 0:  //第3象限
                Quadrant[i] = 3;
                break;
            case 1:  //第2象限
                Quadrant[i] = 2;
                break;
            case 10: //第4象限
                Quadrant[i] = 4;
                break;
            default: //第1象限
                Quadrant[i] = 1;
                break;
        }
    }

}

/************************************************************************
* 函数名 ：//ParaMeasure
* 功能 ：//瞬时量测量
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/06 V1.0 yangxing XXXX
**************************************************************************/
void ParaMeasure(void)
{
    unsigned char i;
    unsigned short Freq_Hz = 0;
    unsigned long Angle = 0;
    unsigned long Temp=0;
    unsigned short AngBuf[11];//Ua Ub Uc Ia Ib Ic Iab Ibc Iac Uab Ucb
    unsigned short UFlag = 0;

    if(  Second_8302 != Sclock.Clockbackup[0] )
    {
        Second_8302 = Sclock.Clockbackup[0];
        //PowerRead_Flag = 0;//禁止中断读能量寄存器

        UIMeas();//电压、电流有效值测量
          
        PowerMeas();//有功、无功、视在功率,功率因数测量

        //频率测量
        Temp=0;
        RN8302_Read( (unsigned char*)&Temp, UFreq, 3 );//电网频率
        if( Temp != 0 )
            Freq_Hz = (unsigned short)(6553600000/Temp);//Freq_Hz=8192000*8/Temp*100 XX.XX
        else
            Freq_Hz = 5000;
        WORD_BCD2( (unsigned char*)&Real.Freq, (unsigned short)Freq_Hz );
         
        //角度测量
        for( i = 0; i < 6; i++ )
        {
            if( ( i == 1 )&&(MeterParameter.Meter_Type == 0x33) )
            continue;
            Angle = 0;
            RN8302_Read( (unsigned char*)&Angle, (YUA+i), 3 );//角度
            AngBuf[i] = (unsigned short)(((float)Angle/16777216)*3600);//分辨率为0.07°相角格式为XXX.X所以多乘以10，公式参考手册           
        }
        
        //测量电压和电流的夹角
        for( i = 0; i < 3; i++ )
        {
            if( AngBuf[i] >= AngBuf[i+3] )
                Angle = (3600-(AngBuf[i] - AngBuf[i+3]));
            else
                 Angle = (AngBuf[i+3] - AngBuf[i]);
            
            if(( (Real.I[i]&0x7FFFFF) < MeasPara.StartI )||( Real.PKva[i+1] == 0 ))//最后保留
                Angle = 0;

            if( Angle > 3600 )
                Angle = 3600;
            WORD_BCD2( (unsigned char*)&Real.PhaseAngle[i], (unsigned short)Angle );//A、B、C电压电流相角
        }
        
        if( MeterParameter.Meter_Type == 0x34 )//三相四线电流逆相序检测方法
        {
            if((Real.U[0]>=MeasPara.Un60)&&(Real.U[1]>=MeasPara.Un60)&&(Real.U[2]>=MeasPara.Un60))
            {
              if( AngBuf[4] >= AngBuf[3] ) 
                  AngBuf[6] = (AngBuf[4]-AngBuf[3]);//Iab
              else
                  AngBuf[6] = (3600-(AngBuf[3]-AngBuf[4]));//Iab
              if( AngBuf[5] >= AngBuf[4] ) 
                  AngBuf[7] = (AngBuf[5]-AngBuf[4]);//Ibc
              else
                  AngBuf[7] = (3600-(AngBuf[4]-AngBuf[5]));//Ibc
              if( AngBuf[5] >= AngBuf[3] ) 
                  AngBuf[8] = (AngBuf[5]-AngBuf[3]);//Iac
              else
                  AngBuf[8] = (3600-(AngBuf[3]-AngBuf[5]));//Iac
              AngBuf[7] += AngBuf[6];//Iab、Ibc夹角之和
              AngBuf[8] += 2400;//Iac夹角+240
              if(((Real.I[0]&0x7FFFFF)>=MeasPara.Ib05)&&((Real.I[1]&0x7FFFFF)>=MeasPara.Ib05)&&((Real.I[2]&0x7FFFFF)>=MeasPara.Ib05))
              {
                  if( AngBuf[8] > AngBuf[7] )//逆相序判断，正常情况AC夹角大于AB、BC夹角之和
                      I_InvOrder_Flag = 0;
                  else
                      I_InvOrder_Flag = 1;
              }
              else
                  I_InvOrder_Flag = 0;
            }
            else
                I_InvOrder_Flag = 0;
        }
        
        //电压逆相序检测
        RN8302_Read( (unsigned char*)&UFlag, PHASES, 2 );//读取电压逆相序状态位
        if( MeterParameter.Meter_Type == 0x34 )//三相四线电压逆相序检测方法
        {
            if((Real.U[0]>=MeasPara.Un60)&&(Real.U[1]>=MeasPara.Un60)&&(Real.U[2]>=MeasPara.Un60))
            {
                if( UFlag & 0x0008 )
                    U_InvOrder_Flag = 1;
                else
                    U_InvOrder_Flag = 0;
            }
            else
                U_InvOrder_Flag = 0;
        }
        else//三相三线电压逆相序检测方法
        {
            if( (Real.U[0]>=MeasPara.Un60)&&(Real.U[2]>=MeasPara.Un60) )
            {
              if( AngBuf[2] >= AngBuf[0] ) 
                  AngBuf[10] = (AngBuf[2]-AngBuf[0]);//Uca
              else
                  AngBuf[10] = (3600-(AngBuf[0]-AngBuf[2]));//Uca
              //if( UFlag & 0x0008 )
              if( (AngBuf[10] >= 300) && (AngBuf[10] <= 1800) )//正相序时夹角为300度，夹角为30-180度时为逆相序，缺相不判断
                  U_InvOrder_Flag = 1;
              else
                  U_InvOrder_Flag = 0;
            }
            else
                U_InvOrder_Flag = 0;
        }
        UFlag = 0xFFFF;
        RN8302_Write( PHASES, (unsigned char*)&UFlag, 2 );//清状态位
				
				if( (Real.I[0]<MeasPara.StartI)&&(Real.I[1]<MeasPara.StartI)&&(Real.I[2]<MeasPara.StartI) )
				 power_zero_flag = 1;
    }
}

/************************************************************************
* 函数名 ：//InstantPowerMeas
* 功能 ：//有功、无功、视在电量瞬时值测量
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/06 V1.0 yangxing XXXX
**************************************************************************/
void InstantPowerMeas(void)
{
    unsigned char i;

    memset( (unsigned char*)&ChipReg.EnergyBuf[0], 0x00, 36 );
    ChipReg.ActWatthrP = 0;
    ChipReg.ActWatthrN = 0;
    ChipReg.RactVarhr = 0;
    ChipReg.Vahr = 0;
    
    for( i = 0; i < 3; i++ )//A、B、C相有功电能寄存器值
    {
        if( ( i == 1 )&&(MeterParameter.Meter_Type == 0x33) )
            continue;
        RN8302_Read( (unsigned char*)&ChipReg.EnergyBuf[i], (POSEPA+i), 3 );//分相正向有功能量
        RN8302_Read( (unsigned char*)&ChipReg.EnergyBuf[3+i], (NEGEPA+i), 3 );//分相反向有功能量
        RN8302_Read( (unsigned char*)&ChipReg.EnergyBuf[6+i], (EQA+i), 3 );//分相无功能量

        if( ChipReg.EnergyBuf[i] > Factor.EnergyUp )//有功正向电量过滤
            ChipReg.EnergyBuf[i] = 0;

        if( ChipReg.EnergyBuf[3+i] > Factor.EnergyUp )//有功反向电量过滤
            ChipReg.EnergyBuf[3+i] = 0;
        
        if( ChipReg.EnergyBuf[6+i] > Factor.EnergyUp )//无功电量过滤
            ChipReg.EnergyBuf[6+i] = 0;
        
        PowerValue.ActPowerPp[i] += ChipReg.EnergyBuf[i];//分相正向有功电能
        PowerValue.ActPowerPn[i] += ChipReg.EnergyBuf[3+i];//分相反向有功电能
        PowerValue.RactPowerPp[i] += ChipReg.EnergyBuf[6+i];//分相无功电能
    }
    RN8302_Read( (unsigned char*)&ChipReg.ActWatthrP, POSEPT, 3 );//合相正向总有功能量
    RN8302_Read( (unsigned char*)&ChipReg.ActWatthrN, NEGEPT, 3 );//合相反向总有功能量
    RN8302_Read( (unsigned char*)&ChipReg.RactVarhr, EQT, 3 );//合相总无功能量（无符号：当合相代数和为负时，内部自动完成了补码转换）
    RN8302_Read( (unsigned char*)&ChipReg.Vahr, ESTV, 3 );//合相视在能量
    
    if( ChipReg.ActWatthrP > Factor.EnergyUp )
        ChipReg.ActWatthrP = 0;
    if( ChipReg.ActWatthrN > Factor.EnergyUp )
        ChipReg.ActWatthrN = 0;
    if( ChipReg.RactVarhr > Factor.EnergyUp )
        ChipReg.RactVarhr = 0;
    if( ChipReg.Vahr > Factor.EnergyUp )
        ChipReg.Vahr = 0;
    
    PowerValue.ActPowerP += ChipReg.ActWatthrP;//计算正向总有功电能
    PowerValue.ActPowerN += ChipReg.ActWatthrN;//计算反向总有功电能
    PowerValue.RactPower += ChipReg.RactVarhr;//计算总无功电能
    
    if( Flag.ECDir & F_ActRevZ )
        PowerValue.VahrPower[1] += ChipReg.Vahr;//反向视在电能
    else
        PowerValue.VahrPower[0] += ChipReg.Vahr;//正向视在电能
}

/************************************************************************
* 函数名 ：//UMesure_Init
* 功能 ：//电压测量初始化
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//0电压正常，1电压异常
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/03/09 V1.0 
**************************************************************************/
unsigned char UMesure_Init(void)
{
    unsigned char Temp = 0;
    unsigned char i;
    unsigned short Adj[3];
    unsigned long Volt = 0;
    
    for( i = 0; i < 3; i++ )
    {
        Adj[i] = 0;
    }
    Clear_Wdt();//清看门狗
    Enable_SPI();//选择SPI通讯模式
    RN8302_Reset();//复位
    RN8302_BasicPara();//基本参数配置
    Temp = 0xE5;//写使能
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );
    
//33    MEM_Read( (unsigned char*)&Adj[0], ADJ_AVgain, 2, Mem_E2PROMA );
//33    MEM_Read( (unsigned char*)&Adj[1], ADJ_BVgain, 2, Mem_E2PROMA );
//33    MEM_Read( (unsigned char*)&Adj[2], ADJ_CVgain, 2, Mem_E2PROMA );

    for(i=0;i<3;i++)
    {
        RN8302_Write( (GSUA+i), (unsigned char*)& Adj[i], 2 );//电压增益
    }
    //RN8302_Run();//测量开启
    Temp = 0xDC;//写保护
    RN8302_Write( WREN, (unsigned char*)&Temp, 1 );
    for( i = 0; i < 5; i++ )
    {
        Clear_Wdt();//清看门狗
        Delay_Xms(100);
    }

    Clear_Wdt();//清看门狗
    for( i = 0; i < 3; i++ )//读取电压有效值
    {
        if( ( i == 1 )&&(MeterParameter.Meter_Type == 0x33) )
            continue;
        RN8302_Read( (unsigned char*)&ChipReg.Urms[i], (UA+i), 4 );
        Volt = (unsigned long)(((float)(ChipReg.Urms[i]*Factor.Ku))*1000);
        if( (Volt > Factor.DispUnDown) && (Volt < Factor.DispUnUp) )
        {
            HEX4_BCD4((unsigned char*)&RealAdj.UAdj[i],Volt);//校表电压
            Real.U[i] = (unsigned short)(RealAdj.UAdj[i] >> 8);//显示电压
        }
        else
        {
            Real.U[i] = 0;
        }
    }
   if( FLAG_EA == 1 )
       return 0;
    else
       return 1;
//		 return 0;
}



