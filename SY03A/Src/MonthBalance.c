/************************************************************************
* 版权所有 (C)2016, 杭州西力智能科技股份有限公司。
*
* 文件名称： // MonthBalance.c
* 文件标识： // 
* 内容摘要： //月结算程序
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
#include "ex_func.h"
#include "ex_var.h"

unsigned char  Hour_Balance;

/************************************************************************
* 函数名 ：//EngRecombination
* 功能 ：//电量重新组合
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void EngRecombination(void)
{
    unsigned char i;
    
    for( i = 0; i < 3; i++ )//组合、正向、反向有功总尖峰平谷电量
    {
        memcpy( g_Save_Buffer+20*i, AEngy_Blk.AEngyBuff+22*i, 20 );
    }
    for( i = 0; i < 6; i++ )//组合1、2、四象限无功总尖峰平谷电量
    {
        memcpy( g_Save_Buffer+60+20*i, REngy_Blk.REngyBuff+22*i, 20 );
    }
    memcpy( g_Save_Buffer+180, (unsigned char*)&PEngy_Blk.PEngy.PhAPp[0], 24 );//分向有功电能
    memcpy( g_Save_Buffer+204, (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 24 );//分向组合无功电能
    memcpy( g_Save_Buffer+228, (unsigned char*)&PEngy_Blk.PEngy.PhAQur1[0], 48 );//分向4象限无功电能
    for( i = 0; i < 2; i++ )
    {
        memcpy( g_Save_Buffer+276+20*i, VEngy_Blk.VEngyBuff+22*i, 20 );//视在电能
    }
}

/************************************************************************
* 函数名 ：//DmdRecombination
* 功能 ：//需量重新组合
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void DmdRecombination(void)
{
    unsigned char i;
    
    for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
    {
        memcpy( (unsigned char*)&g_Save_Buffer[40*i],  (unsigned char*)&MaxDmd_Blk[i].MaxDmdBuff[0], 40 );
    }
}




unsigned char TimedateFormatCheck(unsigned char *buf)
{
    return 0;

}
/************************************************************************
* 函数名 ：//DmdSet0
* 功能 ：//结算数据补零
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void DmdSet0(void)
{
    unsigned char i;

    for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
    {
        memset( (unsigned char*)&g_Save_Buffer[40*i],  0x00, 40 );
    }
}

/************************************************************************
* 函数名 ：//PowerOffTimeCheck
* 功能 ：//最近1次掉电时间合法性判断
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
unsigned char PowerOffTimeCheck(void)
{
    unsigned char TempBuffer[6];

    memset( (unsigned char*)&TempBuffer[0], 0x00, 6 );
    MEM_Read( (unsigned char*)&TempBuffer[0], E2A_PowerOffTim, 6, Mem_E2PROMA );//读取最近一次掉电时间

    if( TimedateFormatCheck((unsigned char*)&TempBuffer[1]) != 0  )//判断年、月、日、时、分秒是否合法
        return 1;

    return 0;
}

/************************************************************************
* 函数名 ：//EngryBalanceProc
* 功能 ：//电量结算功能
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void EngryBalanceProc(void)
{
    unsigned char Ptr;
    unsigned short Addr;
    unsigned long Num;
    
    if( Timer.EngSaveCnt < 5 )
    {
        MEM_Read( (unsigned char*)&Ptr, FM1_LastM_Engy_Ptr, 1, Mem_FRAM );//读出电量结算指针
        Addr = LastM_Act_Pz0 + EngyBalan_Len*Ptr;//该次结算的起始地址

        if( Timer.EngSaveCnt <= 3 )//前4次每次写64字节
        {
            MEM_Write( Addr+64*Timer.EngSaveCnt, (unsigned char*)&g_Save_Buffer[64*Timer.EngSaveCnt], 64, Mem_E2PROMA);
        }
        else if( Timer.EngSaveCnt == 4 )//最后一次写60字节
        {
            MEM_Write( Addr+64*Timer.EngSaveCnt, (unsigned char*)&g_Save_Buffer[64*Timer.EngSaveCnt], 60, Mem_E2PROMA);

            if(( Flag.Power & F_PwrOffTim )== 0 )//掉电后上电过结算日补结算
            {
                Addr = E2A_LAST_1M_DATA + 4*Ptr;//该次结算的起始地址
                if( Flag.BalanBDong & F_BalanBD )         
                    MEM_Read( (unsigned char*)&Clockbak.Hour, E2A_PowerOffTim+2, 4, Mem_E2PROMA );//读取掉电时间
                MEM_Write( Addr, (unsigned char*)&Clockbak.Hour, 4, Mem_E2PROMA);//补结算时间
                MEM_Write( E2A_LAST1M_DATA, (unsigned char*)&Clockbak.Hour, 4, Mem_E2PROMA);//补结算时间
            }
            else
            {
                Addr = E2A_LAST_1M_DATA + 4*Ptr;//该次结算的起始地址
                MEM_Write( Addr, (unsigned char*)&ClockBlock.Clock.Hour, 4, Mem_E2PROMA);//结算时间
                MEM_Write( E2A_LAST1M_DATA, (unsigned char*)&ClockBlock.Clock.Hour, 4, Mem_E2PROMA);//结算时间
            }
            Ptr++;//指针增加
            if( Ptr >= EngyBalan_MNum )	Ptr = 0;
            MEM_DBWrite( FM1_LastM_Engy_Ptr, (unsigned char*)&Ptr, 1, Mem_FRAM );

            MEM_Read( (unsigned char*)&Num, LastM_Engy_Num, 4, Mem_E2PROMA );
            if( Num < 0x00999999 )//结算次数累加
            {
                BCDX_INC( (unsigned char*)&Num, 4 );
                MEM_Write( LastM_Engy_Num, (unsigned char*)&Num, 4, Mem_E2PROMA );
            }
            Request.Flag_EngSave = 0x00;//电量转存标志
        }
        Timer.EngSaveCnt++;
        if( Timer.EngSaveCnt >= 5 )
        {
            Timer.EngSaveCnt = 0;
            Flag.BalanRequest |= F_BalanEngEnd;//电量转存结束
        }
    }
}

/************************************************************************
* 函数名 ：//BalanceCheck
* 功能 ：//结算上电补冻确认
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void BalanceCheck(void)
{
    unsigned char TempBuffer[6];
    unsigned char TempBuffer2[4];
    MEM_Read( (unsigned char*)&Flag.BalanBDong, BalanSave_Flag, 1, Mem_FRAM );
    MEM_Read( (unsigned char*)&TempBuffer[0], E2A_PowerOffTim, 6, Mem_E2PROMA );//读取最近一次掉电时间
     
    MEM_Read( (unsigned char*)&TempBuffer2[0], E2A_LAST1M_DATA, 4, Mem_E2PROMA );//上次结算时间
    if( memcmp((unsigned char*)&TempBuffer2[0],(unsigned char*)&TempBuffer[2],4)==0 )
        return;
     
    if( (TempBuffer[0] == 0x00) &&(TempBuffer[1] ==0x00) &&(Flag.BalanBDong & F_ReBalanBD) )//零分零秒
    {
        //月结算
        MEM_Read( g_Buffer, Balance_Time1, 6, Mem_E2PROMA );//读出结算日1、2、3
        if( (TempBuffer[2] == g_Buffer[0]) && (TempBuffer[3] == g_Buffer[1]) )//结算日1
        {           
            Request.Flag_EngSave = 0x5A;//电量转存标志
            Request.Flag_Dmd1Save = 0x5A;//需量转存标志
        }
        else if( (TempBuffer[2] == g_Buffer[2]) && (TempBuffer[3] == g_Buffer[3]) )//结算日2
        {
            Request.Flag_EngSave = 0x5A;//电量转存标志            
        }
        else if( (TempBuffer[2] == g_Buffer[4]) && (TempBuffer[3] == g_Buffer[5]) )//结算日3
        {
            Request.Flag_EngSave = 0x5A;//电量转存标志
        }
        memcpy( g_Buffer,(unsigned char*)&Request.Flag_EngSave,3 );
        MEM_DBWrite( EngSave_Flag, (unsigned char*)&g_Buffer[0], 3,Mem_FRAM );
    }
}

/************************************************************************
* 函数名 ：//EngryBalanceAgain
* 功能 ：//电量结算上电补冻功能
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void EngryBalanceCheck(void)
{
    MEM_Read( (unsigned char*)&Request.Flag_EngSave, EngSave_Flag, 1, Mem_FRAM );
    if( Request.Flag_EngSave == 0x5A )
    {
        if( PowerOffTimeCheck() != 0 )//如果上1次掉电时间非法退出
            return;

        EngRecombination();//电量重新组合
        Timer.EngSaveCnt = 0;//电量转存计数器
    }
    while( Request.Flag_EngSave == 0x5A )
    {
        Flag.BalanBDong |= F_BalanBD;
        EngryBalanceProc();//电量转存
        if( Flag.BalanRequest & F_BalanEngEnd )//电量转存数据分5次写完，总共写316个字节
        {
            Flag.BalanBDong &= ~F_BalanBD;
            Flag.BalanRequest &= ~F_BalanEngEnd;
            Flag.BalanBDong &= ~F_ReBalanBD;
            Flag.BalanEngNum =0x00;
            MEM_Write( E2A_EngNum, (unsigned char*)&Flag.BalanEngNum, 1, Mem_E2PROMA );            
            CycleEngry_Resume();//电量转存后重新计算周期累计用电量 
            MEM_Write( E2A_LastZ_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 4, Mem_E2PROMA );//当前组合有功总电能写入上1周期组合有功总电能    
            memcpy( (unsigned char*)&CycEnery.LastPz0[0], (unsigned char*)&CycEnery.CurntPz0[0], 4 );//当前累计组合有功总电能写入上1周期累计组合有功总电能
            MEM_Write( E2A_LastZ2_Act_Pz0, (unsigned char*)&CycEnery.LastPz0[0], 4, Mem_E2PROMA );//转存累计用电量
            memset( (unsigned char*)&CycEnery.CurntPz0[0], 0x00, 4 );               
        }
    }
}


/************************************************************************
* 函数名 ：//DmdBalanceProc_ADD
* 功能 ：//需量补结算功能
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void DmdBalanceProc_ADD(void)
{
    unsigned char Ptr;
    unsigned short Addr;
    unsigned long Num;
    MEM_Read( (unsigned char*)&Ptr, FM1_LastM_Dmd_Ptr, 1, Mem_FRAM );//读出需量结算指针
    Addr = LastM_TotalDmdF + DmdBalan_Len*Ptr;//该次结算的起始地址

    MEM_Write( Addr, (unsigned char*)&g_Save_Buffer[0], 192, Mem_E2PROMA);
    MEM_Write( Addr+192, (unsigned char*)&g_Save_Buffer[192], 128, Mem_E2PROMA);

    Ptr++;//指针增加
    if( Ptr >= DmdBalan_MNum )	Ptr = 0;
    MEM_DBWrite( FM1_LastM_Dmd_Ptr, (unsigned char*)&Ptr, 1, Mem_FRAM );
    
    MEM_Read( (unsigned char*)&Num, LastM_Dmd_Num, 4, Mem_E2PROMA );
    if( Num < 0x00999999 )//结算次数累加
    {
        BCDX_INC( (unsigned char*)&Num, 4 );
        MEM_Write( LastM_Dmd_Num, (unsigned char*)&Num, 4, Mem_E2PROMA );
    }
}
/************************************************************************
* 函数名 ：//DmdBalanceProc
* 功能 ：//需量结算功能
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void DmdBalanceProc(void)
{
    unsigned char Ptr;
    unsigned short Addr;
    unsigned long Num;

    if( Timer.DmdSaveCnt < 5 )
    {
        MEM_Read( (unsigned char*)&Ptr, FM1_LastM_Dmd_Ptr, 1, Mem_FRAM );//读出需量结算指针
        Addr = LastM_TotalDmdF + DmdBalan_Len*Ptr;//该次结算的起始地址

        if( Timer.DmdSaveCnt <= 4 )//每次写64字节
        {
            MEM_Write( Addr+64*Timer.DmdSaveCnt, (unsigned char*)&g_Save_Buffer[64*Timer.DmdSaveCnt], 64, Mem_E2PROMA);
        }

        if( Timer.DmdSaveCnt == 4 )
        {
            Ptr++;//指针增加
            if( Ptr >= DmdBalan_MNum )	Ptr = 0;
            MEM_DBWrite( FM1_LastM_Dmd_Ptr, (unsigned char*)&Ptr, 1, Mem_FRAM );
            
            MEM_Read( (unsigned char*)&Num, LastM_Dmd_Num, 4, Mem_E2PROMA );
            if( Num < 0x00999999 )//结算次数累加
            {
                BCDX_INC( (unsigned char*)&Num, 4 );
                MEM_Write( LastM_Dmd_Num, (unsigned char*)&Num, 4, Mem_E2PROMA );
            }
        }

        Timer.DmdSaveCnt++;
        if( Timer.DmdSaveCnt >= 5 )
        {
            Timer.DmdSaveCnt = 0;
            Flag.BalanRequest |= F_BalanDmdEnd;//需量转存结束
        }
    }
}

/************************************************************************
* 函数名 ：//DmdBalanceAgain
* 功能 ：//需量结算上电补冻功能
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void DmdBalanceCheck(void)
{
    MEM_Read( (unsigned char*)&Request.Flag_Dmd1Save, Dmd1Save_Flag, 2, Mem_FRAM );

//    if( (Request.Flag_Dmd1Save == 0x5A) || (Request.Flag_Dmd2Save == 0x5A) )
    if( Request.Flag_Dmd1Save == 0x5A )
    {
        if( PowerOffTimeCheck() != 0 )//如果上1次掉电时间非法退出
            return;
    }

    if( Request.Flag_Dmd1Save == 0x5A )//结算日1需量补存
    {
        DmdRecombination();//需量重新组合
        DmdBalanceProc_ADD();
        Demand_ClearRAM();//RAM中最大需量清零
        Demand_ClearE2();//E2中当前最大需量清零
        
        Request.Flag_Dmd1Save = 0x00;//需量转存标志
        MEM_DBWrite( Dmd1Save_Flag, (unsigned char*)&Request.Flag_Dmd1Save, 1,Mem_FRAM );
    }

//    if( Request.Flag_Dmd2Save == 0x5A )//结算日2需量补存
//    {
//        for( i = 0; i < 320; i++ )
//        {
//            g_Save_Buffer[i] = 0xFF;
//        }
//        DmdBalanceProc_ADD();
//        Request.Flag_Dmd2Save = 0x00;//需量转存标志
//        MEM_DBWrite( Dmd2Save_Flag, (unsigned char*)&Request.Flag_Dmd2Save, 1,Mem_FRAM );
//    }
}

/************************************************************************
* 函数名 ：//BalanceTotalNum
* 功能 ：//计算需要补冻结的总数目
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：// 
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
unsigned char BalanceTotalNum(void)
{
    unsigned char TempBuffer[6];
    unsigned char TempBuffer2[6];
    unsigned char BalanBuffer[6];
    unsigned char LastMonth = 0;
    unsigned char CurtMonth = 0;
    unsigned char BalanMonth = 0;
    unsigned char BalanNum = 0;
    unsigned char LastYear = 0;
    unsigned char Num = 0;
    unsigned char EngCnt0 = 0;
    unsigned char EngCnt1 = 0;
    unsigned char LastEngCnt = 0;
    unsigned char BalanceEng1_Cnt = 0;
    unsigned char i;
    unsigned int Tmp0 = 0;
    unsigned int Tmp1 = 0;
    unsigned long NowTmr,BalanceTmr,PowOffTmr;

    memset( (unsigned char*)&TempBuffer[0], 0x00, 6 );
    memset( (unsigned char*)&TempBuffer2[0], 0x00, 6 );
    memset( (unsigned char*)&BalanBuffer[0], 0x00, 6 );
    if(Flag.TimeChgBill == 0x5A)
      {
        MEM_Read( (unsigned char*)&TempBuffer[0], E2A_Timechange, 6, Mem_E2PROMA );//校时前记录时间
        MEM_Read( (unsigned char*)&Clockbak.Sec, E2A_TimechangeNOW, 6, Mem_E2PROMA );//校时后记录时间
      }
    else
      {
        MEM_Read( (unsigned char*)&TempBuffer[0], E2A_PowerOffTim, 6, Mem_E2PROMA );//读取最近一次掉电时间
        memcpy( (unsigned char*)&Clockbak.Sec, (unsigned char*)&ClockBlock.Clock.Sec, 6);//当前时间
      }

    if( TimedateFormatCheck((unsigned char*)&TempBuffer[1]) != 0  )//判断年、月、日、时、分、秒是否合法
        return 1;

     if( TempBuffer[5] > Clockbak.Year )
         return 1;
     if(( TempBuffer[5] == Clockbak.Year )&&( TempBuffer[4] > Clockbak.Mon ))
         return 1;
     if(( TempBuffer[5] == Clockbak.Year )&&( TempBuffer[4] == Clockbak.Mon )&&( TempBuffer[3] > Clockbak.Day ))
       return 1;
     if(( TempBuffer[5] == Clockbak.Year )&&( TempBuffer[4] == Clockbak.Mon )&&( TempBuffer[3] == Clockbak.Day )&&( TempBuffer[2] > Clockbak.Hour ))
       return 1;
     /*
     MEM_Read( (unsigned char*)&TempBuffer2[0], E2A_LAST1M_DATA, 4, Mem_E2PROMA );//上次结算时间
     if( memcmp((unsigned char*)&TempBuffer2[0],(unsigned char*)&TempBuffer[2],4)==0 )
         return 1;
     */
    //计算出跨月个数
    LastMonth = BCD_BYTE(TempBuffer[4]);
    CurtMonth = BCD_BYTE(Clockbak.Mon);
    if( TempBuffer[5] == Clockbak.Year )//同年
        BalanMonth = CurtMonth - LastMonth;//计算跨月个数
    else
    {
        LastYear = TempBuffer[5];
        BCDX_INC((unsigned char*)&LastYear,1);
        if( LastYear == Clockbak.Year )//去年掉电
            BalanMonth = CurtMonth + 12 - LastMonth;
        else//前年或者更早
            BalanMonth = 13;
    }
    Flag.MonitorNum = BalanMonth;//电压合格率需要补的个数

    //计算结算日有效个数
    NowTmr = ((unsigned long)BCD_BYTE(Clockbak.Day))*86400+((unsigned long)BCD_BYTE(Clockbak.Hour))*3600+((unsigned long)BCD_BYTE(Clockbak.Min))*60+(unsigned long)BCD_BYTE(Clockbak.Sec);//当前月份时间总时间秒
    PowOffTmr = ((unsigned long)BCD_BYTE(TempBuffer[3]))*86400+((unsigned long)BCD_BYTE(TempBuffer[2]))*3600+((unsigned long)BCD_BYTE(TempBuffer[1]))*60+(unsigned long)BCD_BYTE(TempBuffer[0]);//最近一次掉电总时间秒
    MEM_Read( (unsigned char*)&BalanBuffer[0], Balance_Time1, 6, Mem_E2PROMA );
    for( i = 0; i < 3; i++ )
    {
        Tmp0 = ((unsigned int)(BalanBuffer[2*i+1]) << 8) + (unsigned int)BalanBuffer[2*i];//结算日时
        Tmp1 = ((unsigned int)(BalanBuffer[2*((i+1)%3)+1]) << 8) + (unsigned int)BalanBuffer[2*((i+1)%3)];

        if((BalanBuffer[2*i+1] > 0x28) || (BalanBuffer[2*i+1] == 0) ||(BalanBuffer[2*i] > 0x23))//判断结算时间格式
            continue;
        if(( BalanMonth > 1 )&&(BalanBuffer[1] <= 0x28)&&(BalanBuffer[0] <= 0x23))
        {
          Flag.BalanDmdClr = 0x5A;
        }
        BalanceTmr = ((unsigned long)BCD_BYTE(BalanBuffer[2*i+1]))*86400+((unsigned long)BCD_BYTE(BalanBuffer[2*i]))*3600;//结算日时

        if( Tmp0 != Tmp1 )
        {
            BalanNum++;//计算有效结算日数
            if((TempBuffer[4] == Clockbak.Mon) && (TempBuffer[5] == Clockbak.Year))//在同个月上下电
            {
                if( (BalanceTmr > PowOffTmr) && (NowTmr >= BalanceTmr) )//计算本月跨结算日数
                {
                    Num++;
                    if( i == 0 )//跨结算日1，需量结算后当前需量清零
                    {
                        Flag.BalanDmdClr = 0x5A;
                        BalanceEng1_Cnt++;
                    }
                }
            }
            else
            {
                if( NowTmr >= BalanceTmr )//上电这个月跨结算日数
                {
                    Num++;
                    if( i == 0 )//跨结算日1，需量结算后当前需量清零
                    {
                        Flag.BalanDmdClr = 0x5A;
                        BalanceEng1_Cnt++;
                    }
                }

                if( PowOffTmr < BalanceTmr )//掉电那个月跨结算日数
                {
                    Num++;
                    if( i == 0 )//跨结算日1，需量结算后当前需量清零
                    {
                        Flag.BalanDmdClr = 0x5A;
                        BalanceEng1_Cnt++;
                    }
                }
            }
        }
        else
        {
            EngCnt0++;//两个结算日相同，若三个结算日都相同则EngCnt0＝3
            if((TempBuffer[4] == Clockbak.Mon) && (TempBuffer[5] == Clockbak.Year))//在同个月上下电
            {
                if( (BalanceTmr > PowOffTmr) && (NowTmr >= BalanceTmr) )
                {
                    EngCnt1++;
                    if( i == 0 )//跨结算日1，需量结算后当前需量清零
                    {
                        Flag.BalanDmdClr = 0x5A;
                        BalanceEng1_Cnt++;
                    }
                }
            }
            else
            {
                if( PowOffTmr < BalanceTmr )//掉电那个月跨结算日数
                {
                    LastEngCnt++;
                    if( i == 0 )//跨结算日1，需量结算后当前需量清零
                    {
                        Flag.BalanDmdClr = 0x5A;
                        BalanceEng1_Cnt++;
                    }
                }
                if( NowTmr >= BalanceTmr )//上电这个月跨结算日数
                {
                    EngCnt1++;
                    if( i == 0 )//跨结算日1，需量结算后当前需量清零
                    {
                        Flag.BalanDmdClr = 0x5A;
                        BalanceEng1_Cnt++;
                    }
                }
            }
            continue;
        }
    }

    if( EngCnt0 == 3 )//3个结算日都相等
        BalanNum = 1;//有效结算日数为1

    if( EngCnt1 == 3 )//本月跨结算日数为1
        Num++;

    if( LastEngCnt == 3 )//掉电那个月跨结算日数为1
        Num++;

    g_BalanNum = BalanNum;
    //计算需要补结算的总个数
    if( BalanMonth >= 1 )//掉电那个月已经计算过跨结算日数
        BalanMonth--;
    BalanceEng1_Cnt += BalanMonth;
    Flag.BalanceEng1_Num += BalanceEng1_Cnt;//过结算日1的总数
    if( Flag.BalanceEng1_Num > 2 ) 
      Flag.BalanceEng1_Num = 2;
    Flag.BalanceEng1_MDNum = Flag.BalanceEng1_Num;
    
    Num += BalanMonth*BalanNum;
    if( Num > 12 )
        Num = 12;

    Flag.BalanEngNum += Num;//电量补冻结总数
    if( Flag.BalanEngNum > 12 )
        Flag.BalanEngNum = 12;

    Flag.BalanDmdNum += Num;//需量补冻结总数
    if( Flag.BalanDmdNum > 12 )
        Flag.BalanDmdNum = 12;
    
    MEM_Write( E2A_EngNum, (unsigned char*)&Flag.BalanEngNum, 5, Mem_E2PROMA );
    return 0;
}

/************************************************************************
* 函数名 ：//PowOnBalance
* 功能 ：//上电/修改时钟补结算
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
unsigned char PowOnBalance(void)
{
    unsigned char j=0,i=0,Tempnum=0,CNT = 0;
    unsigned char TempBufferA[6];
    unsigned char TempBufferB[6];
    unsigned char TempBuffer[4];
    unsigned char TempBuffer2[4];
    unsigned short CurrentTime;
    unsigned short LastBalanceTmr;
    unsigned short BillTime1;
    unsigned short BillTime2;
    unsigned short BillTime3;
    unsigned short Lng2;
    unsigned char DMDREC_CNT;
    unsigned char Temp,lastmonth;

    MEM_Read( (unsigned char*)&Flag.BalanEngNum, E2A_EngNum, 3, Mem_E2PROMA );
    MEM_Read( (unsigned char*)&Flag.BalanceEng1_Num, E2A_Eng1_Num, 1, Mem_E2PROMA );

    Flag.BalanBDong |= F_ReBalanBD;
    Flag.Power &=~ F_PwrOffTim3;
    if( (Flag.BalanEngNum < 12) || (Flag.BalanDmdNum < 12) )
    {
        if( BalanceTotalNum() == 1 )//计算要冻结的个数
        {
            return 0;
        }
    }

    if( (Flag.BalanEngNum > 12) || (Flag.BalanDmdNum > 12) )
    {
        Flag.BalanEngNum = 0;
        Flag.BalanDmdNum = 0;
        Flag.BalanDmdClr = 0;
        Flag.BalanceEng1_Num = 0;
        MEM_Write( E2A_EngNum, (unsigned char*)&Flag.BalanEngNum, 3, Mem_E2PROMA );
        MEM_Write( E2A_Eng1_Num, (unsigned char*)&Flag.BalanceEng1_Num, 1, Mem_E2PROMA );
        return 0;
    }

    MEM_Read( TempBufferA, Balance_Time1, 6, Mem_E2PROMA );//读出结算日1、2、3         
    BillTime1 = BCD_BYTE(TempBufferA[1])*24 + BCD_BYTE(TempBufferA[0]);
    BillTime2 = BCD_BYTE(TempBufferA[3])*24 + BCD_BYTE(TempBufferA[2]);
    BillTime3 = BCD_BYTE(TempBufferA[5])*24 + BCD_BYTE(TempBufferA[4]);
    EngRecombination();//电量重新组合
    Timer.EngSaveCnt = 0;//电量转存计数器
    while( Flag.BalanEngNum > 0 )//电量补冻结总数
    {
        WatchDog_Clear;//清看门狗
        Request.Flag_EngSave = 0x5A;//电量转存标志
        if( PowerOff_Ack() == 1 )
            return 1;
        if( Timer.EngSaveCnt == 4 )
        {
            j++;
            if( j==1 )
            {
                MEM_Read( (unsigned char*)&TempBufferB[2], E2A_PowerOffTim+2, 4, Mem_E2PROMA );//读取最近一次掉电时间
                if( Flag.BalanEngNum ==12)
                {
                    Lng2 = ((unsigned short)BCD_BYTE(ClockBlock.Clock.Day))*24+((unsigned short)BCD_BYTE(ClockBlock.Clock.Hour));//当前时间
                    memcpy( (unsigned char*)&TempBufferB[2], (unsigned char*)&ClockBlock.Clock.Hour, 4);//当前时间  
                    switch( g_BalanNum )//有效结算日数
                     {
                       case 3:                            
                           if(( Lng2 >= BillTime1 )&&( Lng2 >= BillTime2 )&&( Lng2 >= BillTime3 ))
                               lastmonth=3;
                           else
                           {
                               if( Lng2 < BillTime3 ) {TempBufferB[2]=TempBufferA[2];TempBufferB[3]=TempBufferA[3];}
                               if( Lng2 < BillTime2 ) {TempBufferB[2]=TempBufferA[0];TempBufferB[3]=TempBufferA[1];}
                               if( Lng2 < BillTime1 ) {TempBufferB[2]=0;TempBufferB[3]=0;}
                               lastmonth=4;   
                           }
                            break;
                       case 2: 
                           if((( Lng2 >= BillTime1 )&&( Lng2 >= BillTime2 ))||(( Lng2 >= BillTime2 )&&( Lng2 >= BillTime3 ))||(( Lng2 >= BillTime1 )&&( Lng2 >= BillTime3 )))
                               lastmonth=5;
                           else
                           {
                               if(BillTime3 ==2475 )
                               {
                                   if(Lng2 >=BillTime1)
                                   {
                                       TempBufferB[2]=TempBufferA[0];
                                       TempBufferB[3]=TempBufferA[1];
                                   }
                                   else
                                   {
                                       TempBufferB[2]=0;
                                       TempBufferB[3]=0;
                                   }
                               }
                               lastmonth=6;   
                           }                               
                            break;
                       case 1: 
                           if(( Lng2 >= BillTime1 )||( Lng2 >= BillTime2 )||( Lng2 >= BillTime3 ))
                               lastmonth=11;
                           else
                               lastmonth=12;                                         
                            break;                              
                     }
                    if((lastmonth==3)||(lastmonth==5)||(lastmonth==11)||(lastmonth==12))
                    {
                        TempBufferB[2]=0;
                        TempBufferB[3]=0;
                    }
                    LastBalanceTmr = (unsigned short)BCD_BYTE(TempBufferB[4]);//月
                    if(LastBalanceTmr <= lastmonth)
                    {
                        Temp = BYTE_BCD(BCD_BYTE(TempBufferB[5])-1);  
                        TempBufferB[5] = Temp;
                        Temp = (12-(lastmonth - LastBalanceTmr));
                        TempBufferB[4] = BYTE_BCD(Temp);
                    }
                    else 
                    {
                        Temp = ( LastBalanceTmr - lastmonth );
                        TempBufferB[4] = BYTE_BCD(Temp);
                    }
                    memcpy( (unsigned char*)&TempBuffer2, (unsigned char*)&TempBufferB[2], 4 );//用于需量补结算
                }
            }
            else 
                MEM_Read( (unsigned char*)&TempBufferB[2], E2A_LAST1M_DATA, 4, Mem_E2PROMA );//上次结算时间
            LastBalanceTmr = ((unsigned short)BCD_BYTE(TempBufferB[3]))*24+(unsigned short)BCD_BYTE(TempBufferB[2]);//结算日时

             switch( g_BalanNum )//有效结算日数
             {
               case 2:
                   memcpy( (unsigned char*)&Clockbak.Hour, (unsigned char*)&TempBufferB[2], 4 );
                   if((( TempBufferA[3] ==0x99)||( TempBufferA[2] ==0x99))||( BillTime1 == BillTime2 ))//结算日2无效
                   {
                       if( BillTime1 < BillTime3 )//1小于等于3
                       {
                           if( LastBalanceTmr < BillTime1 )
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else if(( LastBalanceTmr >= BillTime1 )&&( LastBalanceTmr < BillTime3 ))//大于结算日1，则补结算日3
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else 
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }
                       }
                       else
                       {
                           if( LastBalanceTmr < BillTime3 )
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else if(( LastBalanceTmr >= BillTime3 )&&( LastBalanceTmr < BillTime1 ))//大于结算日3，则补结算日1
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else 
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }
                       }
                   } 
                   else if((( TempBufferA[5] ==0x99)||( TempBufferA[4] ==0x99))||( BillTime1 == BillTime3 )||( BillTime2 == BillTime3 ))//结算日3无效
                   {
                       if( BillTime1 < BillTime2 )//1小于等于2
                       {
                           if( LastBalanceTmr < BillTime1 )
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else if(( LastBalanceTmr >= BillTime1 )&&( LastBalanceTmr < BillTime2 ))//大于结算日1，则补结算日2
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else 
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }
                       }
                       else
                       {
                           if( LastBalanceTmr < BillTime2 )
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else if(( LastBalanceTmr >= BillTime2 )&&( LastBalanceTmr < BillTime1 ))//大于结算日2，则补结算日1
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else 
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }
                       }
                   } 
                   else if(( TempBufferA[1] ==0x99)||( TempBufferA[0] ==0x99))//结算日1无效
                   {
                       if( BillTime2 <= BillTime3 )//2小于等于3
                       {                       
                           if( LastBalanceTmr < BillTime2 )
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else if(( LastBalanceTmr >= BillTime2 )&&( LastBalanceTmr < BillTime3 ))//大于结算日2，则补结算日3
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else 
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }
                       }
                       else
                       {
                           if( LastBalanceTmr < BillTime3 )
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else if(( LastBalanceTmr >= BillTime3 )&&( LastBalanceTmr < BillTime2 ))//大于结算日2，则补结算日3
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else 
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           } 
                       }
                   }  
                    break;
                   case 3:
                       memcpy( (unsigned char*)&Clockbak.Hour, (unsigned char*)&TempBufferB[2], 4 );
                       if(( BillTime1 < BillTime2 )&&( BillTime2 < BillTime3 ))//1<2<3
                       {  
                           if( LastBalanceTmr < BillTime1 )
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else if(( LastBalanceTmr >= BillTime1 )&&( LastBalanceTmr < BillTime2 ))//大于结算日1，则补结算日3
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else if(( LastBalanceTmr >= BillTime2 )&&( LastBalanceTmr < BillTime3 ))
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }
                       }
                       else if(( BillTime1 < BillTime3 )&&( BillTime3 < BillTime2 ))//1<3<2
                       {
                           if( LastBalanceTmr < BillTime1 )
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else if(( LastBalanceTmr >= BillTime1 )&&( LastBalanceTmr < BillTime3 ))//大于结算日1，则补结算日3
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else if(( LastBalanceTmr >= BillTime3 )&&( LastBalanceTmr < BillTime2 ))
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }                           
                       }
                       else if((BillTime2 < BillTime1)&&(BillTime1 < BillTime3))//2<1<3
                       {
                           if( LastBalanceTmr < BillTime2 )
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else if(( LastBalanceTmr >= BillTime2 )&&( LastBalanceTmr < BillTime1 ))//大于结算日1，则补结算日3
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else if(( LastBalanceTmr >= BillTime1 )&&( LastBalanceTmr < BillTime3 ))
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }                           
                       } 
                       else if((BillTime2 < BillTime3)&&(BillTime3 < BillTime1))//2<3<1
                       {
                           if( LastBalanceTmr < BillTime2 )
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else if(( LastBalanceTmr >= BillTime2 )&&( LastBalanceTmr < BillTime3 ))//大于结算日1，则补结算日3
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else if(( LastBalanceTmr >= BillTime3 )&&( LastBalanceTmr < BillTime1 ))
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }                           
                       }  
                       else if((BillTime3 < BillTime1)&&(BillTime1 < BillTime2))//3<1<2
                       {
                           if( LastBalanceTmr < BillTime3 )
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else if(( LastBalanceTmr >= BillTime3 )&&( LastBalanceTmr < BillTime1 ))//大于结算日1，则补结算日3
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else if(( LastBalanceTmr >= BillTime1 )&&( LastBalanceTmr < BillTime2 ))
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }                           
                       } 
                       else if((BillTime3 < BillTime2)&&(BillTime2 < BillTime1))//3<2<1
                       {
                           if( LastBalanceTmr < BillTime3 )
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                           }
                           else if(( LastBalanceTmr >= BillTime3 )&&( LastBalanceTmr < BillTime2 ))//大于结算日1，则补结算日3
                           {
                               Clockbak.Hour = TempBufferA[2];
                               Clockbak.Day = TempBufferA[3];
                           }
                           else if(( LastBalanceTmr >= BillTime2 )&&( LastBalanceTmr < BillTime1 ))
                           {
                               Clockbak.Hour = TempBufferA[0];
                               Clockbak.Day = TempBufferA[1];
                           }
                           else
                           {
                               Clockbak.Hour = TempBufferA[4];
                               Clockbak.Day = TempBufferA[5];
                               Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                               if(Clockbak.Mon > 0x12)
                               {
                                  Clockbak.Mon = 1; 
                                  Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                               }
                           }                           
                       }                       
                 break;
               default:
                      memcpy( (unsigned char*)&Clockbak.Hour, (unsigned char*)&TempBufferB[2], 4 );
                      if(( TempBufferA[1] !=0x99)&&( TempBufferA[0] !=0x99))                                 
                        {
                            Clockbak.Hour = TempBufferA[0];
                            Clockbak.Day = TempBufferA[1];
                            if( LastBalanceTmr >= BillTime1 )
                            {
                                 Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                                 if(Clockbak.Mon > 0x12)
                                 {
                                    Clockbak.Mon = 1; 
                                    Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                                 }
                            }
                         }
                       else if(( TempBufferA[3] !=0x99)&&( TempBufferA[2] !=0x99))                                  
                        {
                            Clockbak.Hour = TempBufferA[2];
                            Clockbak.Day = TempBufferA[3];
                            if( LastBalanceTmr >= BillTime2 )
                            {
                                 Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                                 if(Clockbak.Mon > 0x12)
                                 {
                                    Clockbak.Mon = 1; 
                                    Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1); 
                                 }
                            }
                         }
                       else if(( TempBufferA[5] !=0x99)&&( TempBufferA[4] !=0x99))                                 
                        {
                            Clockbak.Hour = TempBufferA[4];
                            Clockbak.Day = TempBufferA[5];
                            if( LastBalanceTmr >= BillTime3 )
                            {
                                 Clockbak.Mon = BYTE_BCD(BCD_BYTE(TempBufferB[4])+1);     
                                 if(Clockbak.Mon > 0x12)
                                 {
                                    Clockbak.Mon = 1; 
                                    Clockbak.Year = BYTE_BCD(BCD_BYTE(TempBufferB[5])+1); 
                                 }
                            }
                         }           
                    break;
             }
        }
        //Request.Flag_EngSave = 0x5A;//电量转存标志
        EngryBalanceProc();//电量转存
        if( Flag.BalanRequest & F_BalanEngEnd )//电量转存数据分5次写完，总共写316个字节
        {
            //Request.Flag_EngSave = 0x00;//电量转存标志
            Flag.BalanRequest &= ~F_BalanEngEnd;
            Timer.EngSaveCnt = 0;
            Flag.BalanEngNum--;
            MEM_Write( E2A_EngNum, (unsigned char*)&Flag.BalanEngNum, 1, Mem_E2PROMA );             
            CycleEngry_Resume();//计算周期累计用电量  
            MEM_Write( E2A_LastZ_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 4, Mem_E2PROMA );//当前组合有功总电能写入上1周期组合有功总电能    
            memcpy( (unsigned char*)&CycEnery.LastPz0[0], (unsigned char*)&CycEnery.CurntPz0[0], 4 );//当前累计组合有功总电能写入上1周期累计组合有功总电能
            MEM_Write( E2A_LastZ2_Act_Pz0, (unsigned char*)&CycEnery.LastPz0[0], 4, Mem_E2PROMA );//转存累计用电量
            memset( (unsigned char*)&CycEnery.CurntPz0[0], 0x00, 4 );                
            if( Flag.BalanEngNum ==0 )
                Flag.BalanBDong &= ~ F_ReBalanBD;
        }
        //MEM_DBWrite( EngSave_Flag, (unsigned char*)&Request.Flag_EngSave, 1,Mem_FRAM );
    }
    while( Flag.BalanceEng1_Num > 0 )//过结算日1的冻结总数
    {
        Flag.BalanceEng1_Num--;
        MonthEngry_Resume();//计算月度累计用电量  
        MEM_Write( E2A_Eng1_Num, (unsigned char*)&Flag.BalanceEng1_Num, 1, Mem_E2PROMA );
        MEM_Write( E2A_LastM_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 4, Mem_E2PROMA );//当前组合有功总电能写入上1月组合有功总电能 
        memcpy( (unsigned char*)&MonthEnery.LastPz0[0], (unsigned char*)&MonthEnery.CurntPz0[0], 4 );//当前累计组合有功总电能写入上1月累计组合有功总电能
        MEM_Write( E2A_LastM2_Act_Pz0, (unsigned char*)&MonthEnery.LastPz0[0], 4, Mem_E2PROMA );//转存累计用电量
        memset( (unsigned char*)&MonthEnery.CurntPz0[0], 0x00, 4 );        
    }

    Timer.DmdSaveCnt = 0;//需量转存计数器
        
    if(Flag.TimeChgBill == 0x5A)
      {
        MEM_Read( (unsigned char*)&TempBufferB[0], E2A_Timechange, 6, Mem_E2PROMA );//校时前记录时间
      }
    else if( Flag.BalanDmdNum == 12 )
      {
        memcpy( (unsigned char*)&TempBufferB[2], (unsigned char*)&TempBuffer2[0], 4 );//第一个补电量结算时间
      }
    else
      {
        MEM_Read( (unsigned char*)&TempBufferB[0], E2A_PowerOffTim3, 6, Mem_E2PROMA );//读取最近一次掉电时间
      }
    CurrentTime = BCD_BYTE(TempBufferB[3])*24 + BCD_BYTE(TempBufferB[2]);
    while( Flag.BalanDmdNum > 0 )
    {
        WatchDog_Clear;//清看门狗
       if( PowerOff_Ack() == 1 )
            return 1;
         Flag.Power |= F_PwrOffTim3;
         Tempnum++;
         switch( g_BalanNum )//有效结算日数
         {
           case 2:
               CNT++;
               if( TempBufferA[0] != 0x99 )//结算日1有效
               {
                 if(( (BillTime1 < BillTime2)&&((TempBufferA[5] == 0x99)||(BillTime2 == BillTime3))) || ( (BillTime1 < BillTime3)&&((TempBufferA[3] == 0x99)||(BillTime2 == BillTime3))) )//1小于2或3  
                 {
                   if((CurrentTime < BillTime1)||(CurrentTime >= BillTime2) ||(CurrentTime >= BillTime3) )//补时间>=结算2 3时间
                   {
                     DMDREC_CNT=1;//第一个补结算日1数据
                     if(( CNT % 2 )==0)
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                    else 
                      DmdRecombination();//需量重新组合
                   }
                   else//补时间<结算2 3时间
                   {
                     
                     DMDREC_CNT=2;
                     if(( CNT % 2 )==1)
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                    else 
                      DmdRecombination();//需量重新组合
                   }
  
                 }
                 else//1大于2或3
                 {
                  if( ((CurrentTime >= BillTime2)||(CurrentTime >= BillTime3)) && (CurrentTime < BillTime1) )
                   {
                     DMDREC_CNT=1;
                     if(( CNT % 2 )==0)
                       {
                         for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                            {
                                memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                            }
                       }
                       else
                        DmdRecombination();//需量重新组合
                   }
                   else
                   {
                     DMDREC_CNT=2;
                      if(( CNT % 2 )==1)
                       {
                         for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                            {
                                memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                            }
                       }
                       else
                        DmdRecombination();//需量重新组合                   
                   }
                 }
               }
               else
               {
                 for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                    {
                        memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                    }
               }
             break;
           case 3:
              CNT++;
             if( (BillTime1 < BillTime2) &&  (BillTime1 < BillTime3) )//1<2 / 3
             {
                if((CurrentTime < BillTime1)||(CurrentTime >= BillTime3) )
                {
                   DMDREC_CNT=1;

                   if((CNT != 1 )&&(CNT != 4 )&&(CNT != 7 )&&(CNT != 10 ))
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                   else 
                      DmdRecombination();//需量重新组合
                }
                else if((CurrentTime >= BillTime2)&&(CurrentTime < BillTime3) )
                {
                   DMDREC_CNT=2;
                  if((CNT != 2 )&&(CNT != 5 )&&(CNT != 8 )&&(CNT != 11 ))
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                   else 
                      DmdRecombination();//需量重新组合
                }
                 else if((CurrentTime >= BillTime1)&&(CurrentTime < BillTime2) )
                {
                   DMDREC_CNT=3;
                   if((CNT != 3 )&&(CNT != 6 )&&(CNT != 9 )&&(CNT != 12 ))
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                   else 
                      DmdRecombination();//需量重新组合
                }
             }
             else if( (BillTime1 > BillTime2) &&  (BillTime1 < BillTime3) )
             {
                if((CurrentTime >= BillTime2)&&(CurrentTime < BillTime1) )
                {
                   DMDREC_CNT=1;
                   if((CNT != 1 )&&(CNT != 4 )&&(CNT != 7 )&&(CNT != 10 ))
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                   else 
                      DmdRecombination();//需量重新组合
                }
                 else if((CurrentTime < BillTime2)||(CurrentTime >= BillTime3) )
                {
                   DMDREC_CNT=2;
                  if((CNT != 2 )&&(CNT != 5 )&&(CNT != 8 )&&(CNT != 11 ))
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                   else 
                      DmdRecombination();//需量重新组合
                }
                else if((CurrentTime >= BillTime1)&&(CurrentTime < BillTime3) )
                {
                   DMDREC_CNT=3;
                   if((CNT != 3 )&&(CNT != 6 )&&(CNT != 9 )&&(CNT != 12 ))
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                   else 
                      DmdRecombination();//需量重新组合
                }
             }
             else if( (BillTime1 > BillTime2) &&  (BillTime1 > BillTime3) )//1大于 2 / 3
             {
                if((CurrentTime >= BillTime3)&&(CurrentTime < BillTime1) )
                {
                   DMDREC_CNT=1;
                   if((CNT != 1 )&&(CNT != 4 )&&(CNT != 7 )&&(CNT != 10 ))
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                   else 
                      DmdRecombination();//需量重新组合
                }
                else if((CurrentTime >= BillTime2)&&(CurrentTime < BillTime3) )
                {
                   DMDREC_CNT=2;
                  if((CNT != 2 )&&(CNT != 5 )&&(CNT != 8 )&&(CNT != 11 ))
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                   else 
                      DmdRecombination();//需量重新组合
                }
                else if((CurrentTime >= BillTime1)||(CurrentTime < BillTime2) )
                {
                   DMDREC_CNT=3;
                   if((CNT != 3 )&&(CNT != 6 )&&(CNT != 9 )&&(CNT != 12 ))
                     {
                       for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                          {
                              memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                          }
                     }
                   else 
                      DmdRecombination();//需量重新组合
                }
               
             }
             break;
           default:
             DMDREC_CNT=1;
             if( TempBufferA[0] == 0x99 )
              {
               for( i = 0; i < 8; i++ )//正向、反向、组合无功1、2、4象限无功总尖峰平谷最大需量
                  {
                      memset( (unsigned char*)&g_Save_Buffer[40*i],  0xFF, 40 );
                  }
              }
             else
             {
               DmdRecombination();//需量重新组合
             }
             break;
         }
          //修改为一次性结算完成1次
        DmdBalanceProc_ADD();//需量结算

        Flag.BalanDmdNum--;
        MEM_Write( E2A_DmdNum, (unsigned char*)&Flag.BalanDmdNum, 1, Mem_E2PROMA );
        if(( Flag.BalanDmdClr == 0x5A )&&(DMDREC_CNT==Tempnum))//第一次过结算日1时，当前需量要清零
        {
           if( Flag.BalanceEng1_MDNum == 1 )
             {
                memcpy( (unsigned char*)&MD_Freeze_Buffer[0],  (unsigned char*)&MaxDmd_Blk[0].MaxDmdBuff[0], 40 );//用于补日冻结最大需量
                memcpy( (unsigned char*)&MD_Freeze_Buffer[40],  (unsigned char*)&MaxDmd_Blk[1].MaxDmdBuff[0], 40 );
             }
            Demand_ClearRAM();//RAM中最大需量清零
            Demand_ClearE2();//E2中最大需量清零
            Request.Flag_Dmd1Save = 0x00;//需量转存标志
            Flag.BalanDmdClr = 0;
            MEM_Write( E2A_DmdClr, (unsigned char*)&Flag.BalanDmdClr, 1, Mem_E2PROMA );
        }
//        else
//            Request.Flag_Dmd2Save = 0x00;//需量转存标志
    }
    if( ( Flag.BalanDmdNum == 0) && ( Flag.BalanEngNum == 0 ) )
      Flag.TimeChgBill = 0x00;
    
    return 0;
}



/************************************************************************
* 函数名 ：//MonthBalanceProc
* 功能 ：//月结算功能
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void MonthBalanceProc(void)
{  
    unsigned char i;
    unsigned char LadderNO;
    
    if( Hour_Balance != ClockBlock.Clock.Hour )
    {
        Hour_Balance = ClockBlock.Clock.Hour;
        if( ClockBlock.Clock.Min != 0 )
            return;
        
        if(Flag.CpuCard & F_CardBDMODE) 
        {
            MeterPrice.Ladder_Mode = BalanceMode_Judge();//年结算模式
            LadderNO = Fee_JtChange_Judge();
	    MEM_Read( &g_Buffer[0], Set1Step_Table0_kWh+70*4*Fee.StepTable+70*LadderNO+52, 18, Mem_E2PROMA );  
            for( i=0; i<18; i+=3 )
	    {
	        if( Cmp_Bcd_Inv( g_Buffer+i, &ClockBlock.Clock.Hour, 3 ) == 0x03 ) break;
	    }
            if( (i<18) && (MeterPrice.Ladder_Mode == 0x55)&& (ClockBlock.Clock.Day <=0x28)) YearBalanceProc(); 
        
        }
        else MeterPrice.Ladder_Mode = 0x00;//月结算模式  
        
        //月结算
        MEM_Read( g_Buffer, Balance_Time1, 6, Mem_E2PROMA );//读出结算日1、2、3
        if( (ClockBlock.Clock.Hour == g_Buffer[0]) && (ClockBlock.Clock.Day == g_Buffer[1]) )//结算日1
        {           
            Flag.BalanRequest |= ( F_BalanEng + F_BalanDmd1 + F_BalanEngStar + F_BalanDmd1Star );
            Request.Flag_EngSave = 0x5A;//电量转存标志
            Request.Flag_Dmd1Save = 0x5A;//需量转存标志
        }
        else if( (ClockBlock.Clock.Hour == g_Buffer[2]) && (ClockBlock.Clock.Day == g_Buffer[3]) )//结算日2
        {
            Flag.BalanRequest |= ( F_BalanEng + F_BalanEngStar );
//            Flag.BalanRequest |= ( F_BalanEng + F_BalanDmd2 + F_BalanEngStar + F_BalanDmd2Star );
            Request.Flag_EngSave = 0x5A;//电量转存标志
//            Request.Flag_Dmd2Save = 0x5A;//需量转存标志
            
        }
        else if( (ClockBlock.Clock.Hour == g_Buffer[4]) && (ClockBlock.Clock.Day == g_Buffer[5]) )//结算日3
        {
            Flag.BalanRequest |= ( F_BalanEng + F_BalanEngStar );
//            Flag.BalanRequest |= ( F_BalanEng + F_BalanDmd2 + F_BalanEngStar + F_BalanDmd2Star );//因为结算日3和结算日2需量转存一致，所以用同一个标志
            Request.Flag_EngSave = 0x5A;//电量转存标志
//            Request.Flag_Dmd2Save = 0x5A;//需量转存标志
        }
        
       
          
    }
    if(Flag.TimeChgBill == 0x5A)//修改了时间如果立刻过结算，则先不结算
          return;
    Flag.BalanBDong &= ~F_ReBalanBD;
    if( (Flag.BalanRequest != 0)&& (g_SysCtlTime == 0) )//系统空闲时，执行电量结算
    {
        g_SysCtlTime = 1;//有结算事件就不执行转存电压合格率

        if( Flag.BalanRequest & F_BalanEng )//电量转存完成后再清该标志
        {
            if( Flag.BalanRequest & F_BalanEngStar )
            {
                Flag.BalanRequest &= ~F_BalanEngStar;
                EngRecombination();//电量重新组合
                Timer.EngSaveCnt = 0;//电量转存计数器
            }
            EngryBalanceProc();//电量转存
            if( Flag.BalanRequest & F_BalanEngEnd )//电量转存数据分5次写完，总共写316个字节
            {
                Flag.BalanRequest &= ~F_BalanEngEnd;
                Flag.BalanRequest &= ~F_BalanEng;//电量转存结束

                if( Flag.BalanRequest & F_BalanDmd1 )//结算日1电量转存完，再转存月累计用电量
                {
                    MEM_Write( E2A_LastM_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 4, Mem_E2PROMA );//当前组合有功总电能写入上1月组合有功总电能 
                    memcpy( (unsigned char*)&MonthEnery.LastPz0[0], (unsigned char*)&MonthEnery.CurntPz0[0], 4 );//当前累计组合有功总电能写入上1月累计组合有功总电能
                    MEM_Write( E2A_LastM2_Act_Pz0, (unsigned char*)&MonthEnery.LastPz0[0], 4, Mem_E2PROMA );//转存累计用电量
                    memset( (unsigned char*)&MonthEnery.CurntPz0[0], 0x00, 4 ); //月结算,月用电量转存和清零 
                    if( MeterPrice.Ladder_Mode == 0x00 ) //月结算清阶梯电价判断电量
                    {     
                        memset( (unsigned char*)&AEngy_Blk.AEngy.ActUsedPz0[0], 0x00, 4 );//清电价判断使用电量寄存器
                        MEM_CRCWrite( FM1_EC_ActUsed_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActUsedPz0[0], 4, Mem_FRAM );
                       
                    }
                }
                
                MEM_Write( E2A_LastZ_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 4, Mem_E2PROMA );//当前组合有功总电能写入上1周期组合有功总电能    
                memcpy( (unsigned char*)&CycEnery.LastPz0[0], (unsigned char*)&CycEnery.CurntPz0[0], 4 );//当前累计组合有功总电能写入上1周期累计组合有功总电能
                MEM_Write( E2A_LastZ2_Act_Pz0, (unsigned char*)&CycEnery.LastPz0[0], 4, Mem_E2PROMA );//转存累计用电量
                memset( (unsigned char*)&CycEnery.CurntPz0[0], 0x00, 4 );
            }
        }
        else if( Flag.BalanRequest & F_BalanDmd1 )//结算日1需量转存完，再清该标志
        {
            if( Flag.BalanRequest & F_BalanDmd1Star )
            {
                Flag.BalanRequest &= ~F_BalanDmd1Star;
                DmdRecombination();//需量重新组合
                Timer.DmdSaveCnt = 0;//需量转存计数器
                Demand_ClearRAM();//RAM中最大需量清零
//                MaxDemInt();//最大需量初始化
 //               GrpDemInt();
 //               CurrDemInt();
            }

            DmdBalanceProc();//需量结算

            if( Flag.BalanRequest & F_BalanDmdEnd )//需量转存数据分5次写完，总共写320个字节
            {
                Flag.BalanRequest &= ~F_BalanDmdEnd;
                Flag.BalanRequest &= ~F_BalanDmd1;//需量转存结束
                Request.Flag_Dmd1Save = 0x00;//需量转存标志
            }

            if( ( Flag.BalanRequest & F_BalanDmd1 ) == 0 )//结算日1需量转存结束，最大需量清零
                Demand_ClearE2();//E2中最大需量清零
        }
//        else if( Flag.BalanRequest & F_BalanDmd2 )//结算日2、3的需量转存用0XFF补
//        {
//            if( Flag.BalanRequest & F_BalanDmd2Star )
//            {
//                Flag.BalanRequest &= ~F_BalanDmd2Star;
//                for( i = 0; i < 320; i++ )
//                {
//                    g_Save_Buffer[i] = 0xFF;
//                }
//                Timer.DmdSaveCnt = 0;//需量转存计数器
//            }
//
//            DmdBalanceProc();//需量结算
//
//            if( Flag.BalanRequest & F_BalanDmdEnd )//需量转存数据分5次写完，总共写320个字节
//            {
//                Flag.BalanRequest &= ~F_BalanDmdEnd;
//                Flag.BalanRequest &= ~F_BalanDmd2;//需量转存结束
//                Request.Flag_Dmd2Save = 0x00;//需量转存标志
//            }
//        }
    }
}









/*


//offset=0结算周期,offset=3清结算周期
//offset=1月结算,offset=4清月结算
//offset=2年结算,offset=5清年结算,offset=6年结算时间清零
void Process_Usedl( unsigned char offset )
{
	unsigned char i, len;
	unsigned char  Buf[ee_yhistroyusedl_lenth+8];	
		
	Y_M_C_CycleEngry( offset,(unsigned char*)&Buf[4]);//计算当前年度累计用电量
        memcpy( (unsigned char*)&MonthEnery.LastPz0[0], (unsigned char*)&MonthEnery.CurntPz0[0], 4 );//当前累计组合有功总电能写入上1月累计组合有功总电能
        MEM_Write( E2A_LastM2_Act_Pz0, (unsigned char*)&MonthEnery.LastPz0[0], 4, Mem_E2PROMA );//转存累计用电量
        memset( (unsigned char*)&MonthEnery.CurntPz0[0], 0x00, 4 );
         if( (ClockBlock.Clock.Hour == g_Buffer[0]) && (ClockBlock.Clock.Day == g_Buffer[1]) )//结算日1
        {  
            if( Price.Ladder_Mode == 0x55 )
            { 
                 Y_M_C_CycleEngry( offset,(unsigned char*)&MonthEnery.CurntPz0);//计算当前月度累计用电量
                 memcpy( (unsigned char*)&MonthEnery.LastPz0[0], (unsigned char*)&MonthEnery.CurntPz0[0], 4 );//当前累计组合有功总电能写入上1月累计组合有功总电能
                 MEM_Write( E2A_LastM2_Act_Pz0, (unsigned char*)&MonthEnery.LastPz0[0], 4, Mem_E2PROMA );//转存累计用电量
               //年结算,月转存
            }
            else  
            {
                 Y_M_C_CycleEngry( offset,(unsigned char*)&MonthEnery.CurntPz0);//计算当前年度累计用电量
                 memcpy( (unsigned char*)&MonthEnery.LastPz0[0], (unsigned char*)&MonthEnery.CurntPz0[0], 4 );//当前累计组合有功总电能写入上1月累计组合有功总电能
                 MEM_Write( E2A_LastM2_Act_Pz0, (unsigned char*)&MonthEnery.LastPz0[0], 4, Mem_E2PROMA );//转存累计用电量
                 memset( (unsigned char*)&MonthEnery.CurntPz0[0], 0x00, 4 );
	         //月结算,月用电量转存和清零 
            }
        } 
	
}

*/
/************************************************************************
* 函数名 ：//YearBalanceProc
* 功能 ：//年结算功能
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2016/12/12 V1.0 yangxing XXXX
**************************************************************************/
void YearBalanceProc(void)
{
    unsigned char Ptr;
    unsigned short Addr;
    unsigned long Num;
    unsigned char  TempBuf[8];

    MEM_Read( (unsigned char*)&Ptr, FM1_CurrentY_Ptr, 1, Mem_FRAM );//读出电量年结算指针
    Y_M_C_CycleEngry( 2,(unsigned char*)&YearEnery.CurntPz0);//计算当前年度累计用电量
    memcpy( (unsigned char*)&TempBuf[0], (unsigned char*)&ClockBlock.Clock.Hour,4 );
    memcpy( (unsigned char*)&TempBuf[4], (unsigned char*)&YearEnery.CurntPz0,4 );
    memcpy( (unsigned char*)&YearEnery.LastPz0, (unsigned char*)&YearEnery.CurntPz0,4 );
    
    Addr = E2A_CurrentY_DATA + 8*Ptr;//该次年结算的起始地址
    MEM_Write( Addr, (unsigned char*)&TempBuf[0], 8, Mem_E2PROMA);//转存
    Ptr++;//指针增加
    if( Ptr >= CurrentY_MNum )	Ptr = 0;
    MEM_DBWrite( FM1_CurrentY_Ptr, (unsigned char*)&Ptr, 1, Mem_FRAM );
    
    MEM_Read( (unsigned char*)&Num, CurrentY_Num, 4, Mem_E2PROMA );
    if( Num < 0x00999999 )//结算次数累加
    {
        BCDX_INC( (unsigned char*)&Num, 4 );
        MEM_Write( CurrentY_Num, (unsigned char*)&Num, 4, Mem_E2PROMA );
    }
    MEM_Write( E2A_LastY_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 4, Mem_E2PROMA );//当前组合有功总电能转入上1年度组合有功总电能  
    memset( (unsigned char*)&YearEnery.CurntPz0[0], 0x00, 4 );//当前年度累计用电量清零   
    memset( (unsigned char*)&AEngy_Blk.AEngy.ActUsedPz0[0], 0x00, 4 );//清电价判断使用电量寄存器
    MEM_CRCWrite( FM1_EC_ActUsed_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActUsedPz0[0], 4, Mem_FRAM );
}




/************************************************************************
* 函数名 ：//BalanceMode_Judge
* 功能 ：//结算模式判断
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//0x00 月计算；0x55年结算
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
*************************************************************************/
unsigned char BalanceMode_Judge(void )
{
    unsigned char LadderNO;
 
    LadderNO = Fee_JtChange_Judge();
    
    MEM_Read( &g_Buffer[0], Set1Step_Table0_kWh+70*4*Fee.StepTable+70*LadderNO+52, 18, Mem_E2PROMA );    
    if( JudgeYearClock( g_Buffer ) == 0 ) return 0x55;//年结算模式
    else return 0x00;   
 
    
}
/************************************************************************
* 函数名 ：//ClearYearBalance
* 功能 ：//年电量清理
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
*************************************************************************/
void ClearYearBalance(void)
{
    unsigned long Num;
    unsigned char  TempBuf[48];    
    Num=0;
    MEM_Write( CurrentY_Num, (unsigned char*)&Num, 4, Mem_E2PROMA );
    MEM_DBWrite( FM1_CurrentY_Ptr, (unsigned char*)&Num, 1, Mem_FRAM );
    memset( (unsigned char*)&TempBuf[0], 0x00, 48 );
    MEM_Write( E2A_CurrentY_DATA, (unsigned char*)&TempBuf[0], 48, Mem_E2PROMA);//转存
    MEM_Write( E2A_LastY_Act_Pz0, (unsigned char*)&TempBuf[0], 4, Mem_E2PROMA );
}
