/************************************************************************
* 版权所有 (C)2016, 杭州西力智能科技股份有限公司。
*
* 文件名称： // Measure.c
* 文件标识： // 
* 内容摘要： //有功、无功电能累加
* 其它说明： // 
* 当前版本： // V01.00
* 作 者：    // yangxing
* 完成日期： // 2013年3月05日
*
* 修改记录1：//
* 修改日期： //
* 版 本 号： // V01
* 修 改 人： //
* 修改内容： //
************************************************************************/
#include "ex_func.h"
#include "ex_var.h"
#include <string.h>
#define	 CHECKWORD   0xA55A
#define   MaxFeeNum      4



void BCDX_INC( unsigned char *BCD, unsigned char Len )
{
    unsigned char i;
    
    for( i=0;i<Len;i++ )
    {
        *BCD += 1;
        if( (*BCD & 0x0F) > 0x09 )
        {
            *BCD += 0x06;
            if( (*BCD & 0xF0) > 0x90 )
                *BCD += 0x60;
            else
                break;
        }
        else
            break;
        BCD++;
    }
}

unsigned char BCD_Check( unsigned char *Addr, unsigned int Len )
{
    while( Len > 0 )
    {
        if( (*Addr&0x0F) > 0x09 ) return 1;
        if( (*Addr&0xF0) > 0x90 ) return 1;
        Addr++;
        Len--;
    }
    
    return 0;
}

void BCD4_DEC( unsigned char *BCD )
{
    signed long Lng;
    
    if( (*(BCD+3)) & 0x80 )//??
    {
        *(BCD+3) &= ~0x80;
        Lng = BCD4_HEX4( BCD );
        Lng = ~Lng+1;
    }
    else
    	Lng = BCD4_HEX4( BCD );
    if( Lng <= (-79999999) )
        Lng = 0;
    else
        Lng--;
    if( Lng < 0)
    {
        Lng = ~Lng+1;
        HEX4_BCD4( BCD, Lng );
        *(BCD+3) |= 0x80;
    }
    else
        HEX4_BCD4( BCD, Lng );
}

void BCD4_INC( unsigned char *BCD )
{
    signed long Lng;
    
    if( (*(BCD+3)) & 0x80 )//??
    {
        *(BCD+3) &= ~0x80;
        Lng = BCD4_HEX4( BCD );
        Lng = ~Lng+1;
    }
    else
    {
    	Lng = BCD4_HEX4( BCD );
    }
    if( Lng >= 79999999 )
    {
        Lng = 0;
    }
    else
    {
        Lng++;
    }
    if( Lng < 0)
    {
        Lng = ~Lng+1;
        HEX4_BCD4( BCD, Lng );
        *(BCD+3) |= 0x80;
    }
    else
    {
        HEX4_BCD4( BCD, Lng );
    }
}
unsigned short CheckNum( unsigned char* Addr, unsigned int Lenth )
{
    unsigned short Num = CHECKWORD;

    while( Lenth > 0 )
    {
        Num += *Addr++;
        Lenth--;
    }

    return Num;
}
/************************************************************************
* 函数名 ：//Y_M_C_CycleEngry
* 功能 ：//当前年度、月度、周期组合有功累计用电量计算
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//DI2：0：当前周期；1：当前月；2：当前年；
             //DI1：0：当前；1：上1；
             //ReturnBuf:返回数据
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/04/06 V1.0 yangxing XXXX
**************************************************************************/
void Y_M_C_CycleEngry(unsigned char Mode, unsigned char *ReturnBuf)
{
    unsigned long Tmp0 = 0;
    unsigned long Tmp1 = 0;
    unsigned long CurEng = 0;
    unsigned long LastEng = 0;
    unsigned long MonEng = 0;
    
    if( Mode == 0 )
    {
//33        MEM_Read( (unsigned char*)&Tmp0, E2A_LastZ_Act_Pz0, 4, Mem_E2PROMA );//上周期组合有功总电能
    }
    else if( Mode == 1 )
    {
//33        MEM_Read( (unsigned char*)&Tmp0, E2A_LastM_Act_Pz0, 4, Mem_E2PROMA );//上1月组合有功总电能(结算日1结算)
    }
    else
    {
//33          MEM_Read( (unsigned char*)&Tmp0, E2A_LastY_Act_Pz0, 4, Mem_E2PROMA );//上1年组合有功总电能
    }
    memcpy( (unsigned char*)&Tmp1, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 4 );//当前组合有功总电量
        
    if( Tmp0 & 0x80000000 )//上周期为负电量
    {
        Tmp0 &= 0x7FFFFFFF;
        LastEng = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 )//当前周期为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if( CurEng > LastEng )
            {
              MonEng = CurEng - LastEng;
              HEX4_BCD4( ReturnBuf, MonEng );
              ReturnBuf[3] |= 0x80;
            }
            else//如果在79999999翻转后，该方式不正确
            {//本月改为正-反，则本月小于上月处理会有问题
              MonEng = CurEng + (79999999-LastEng);
              HEX4_BCD4( ReturnBuf, MonEng );
            }
        }
        else//当前周期为正电量
        {
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = CurEng + LastEng;
            HEX4_BCD4( ReturnBuf, MonEng );
        }
    }
    else//上1月为正电量
    {
        LastEng = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 )//当前月为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = CurEng + LastEng;
            HEX4_BCD4( ReturnBuf, MonEng );
            ReturnBuf[3] |= 0x80;
        }
        else//当前月为正电量
        {
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if( CurEng >= LastEng )
            {
              MonEng = CurEng - LastEng;
              HEX4_BCD4( ReturnBuf, MonEng );
            }
            else
            {
              MonEng = LastEng - CurEng;
              HEX4_BCD4( ReturnBuf, MonEng );
              ReturnBuf[3] |= 0x80;
            }
        }
    }
}

/************************************************************************
* 函数名 ：//Current_CycleEngry
* 功能 ：//当前周期组合有功累计用电量
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/04/06 V1.0 yangxing XXXX
**************************************************************************/
void Current_CycleEngry(void)
{
    unsigned long Tmp0 = 0;
    unsigned long Tmp1 = 0;
    unsigned long CurEng = 0;
    unsigned long LastEng = 0;
    unsigned long MonEng = 0;

//33    MEM_Read( (unsigned char*)&Tmp0, E2A_LastZ_Act_Pz0, 4, Mem_E2PROMA );//上周期组合有功总电能
    memcpy( (unsigned char*)&Tmp1, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 4 );//当前累计组合用电量

    if( Tmp0 & 0x80000000 )//上周期为负电量
    {
        Tmp0 &= 0x7FFFFFFF;
        LastEng = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 )//当前周期为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if( CurEng > LastEng )
            {
              MonEng = CurEng - LastEng;
              HEX4_BCD4( (unsigned char*)&CycEnery.CurntPz0[0], MonEng );
              CycEnery.CurntPz0[3] |= 0x80;
            }
            else
            {
              MonEng = LastEng - CurEng;
              HEX4_BCD4( (unsigned char*)&CycEnery.CurntPz0[0], MonEng );
            }
        }
        else//当前周期为正电量
        {
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = CurEng + LastEng;
            HEX4_BCD4( (unsigned char*)&CycEnery.CurntPz0[0], MonEng );
        }
    }
    else//上1月为正电量
    {
        LastEng = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 )//当前月为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = CurEng + LastEng;
            HEX4_BCD4( (unsigned char*)&CycEnery.CurntPz0[0], MonEng );
            CycEnery.CurntPz0[3] |= 0x80;
        }
        else//当前月为正电量
        {
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if( CurEng >= LastEng )
            {
              MonEng = CurEng - LastEng;
              HEX4_BCD4( (unsigned char*)&CycEnery.CurntPz0[0], MonEng );
            }
            else
            {
              MonEng = LastEng - CurEng;
              HEX4_BCD4( (unsigned char*)&CycEnery.CurntPz0[0], MonEng );
              CycEnery.CurntPz0[3] |= 0x80;
            }
        }
    }
}

/************************************************************************
* 函数名 ：//Last_CycleEngry
* 功能 ：//上1周期组合有功累计用电量
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void Last_CycleEngry(void)
{
    unsigned long LastEngB = 0; //上上次电量
    unsigned long LastEng = 0; //上1次电量
    unsigned long MonEng = 0;
    unsigned long Tmp0 = 0;
    unsigned long Tmp1 = 0;

//33     MEM_Read( (unsigned char*)&Tmp0, E2A_LastZ2_Act_Pz0, 4, Mem_E2PROMA ); //上2月组合有功总电能
//33     MEM_Read( (unsigned char*)&Tmp1, E2A_LastZ_Act_Pz0, 4, Mem_E2PROMA ); //上1月组合有功总电能
    if( Tmp0 & 0x80000000 ) //上2月为负电量
    {
        Tmp0 &= 0x7FFFFFFF;
        LastEngB = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 ) //上1月为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            LastEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if(LastEng > LastEngB )
            {
              MonEng = LastEng - LastEngB;
              HEX4_BCD4( (unsigned char*)&CycEnery.LastPz0[0], MonEng );           
              CycEnery.LastPz0[3] |= 0x80;
            }
            else
            {
              MonEng = LastEngB - LastEng;
              HEX4_BCD4( (unsigned char*)&CycEnery.LastPz0[0], MonEng );  
            }
              
        }
        else//上1月为正电量
        {
            LastEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = LastEng + LastEngB;
            HEX4_BCD4( (unsigned char*)&CycEnery.LastPz0[0], MonEng );
        }
    }
    else//上2月为正电量
    {
        LastEngB = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 )//上1月为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            LastEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = LastEng + LastEngB;
            HEX4_BCD4( (unsigned char*)&CycEnery.LastPz0[0], MonEng );
            CycEnery.LastPz0[3] |= 0x80;
        }
        else//上1月为正电量
        {
            LastEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if(LastEng >= LastEngB )
            {
              MonEng = LastEng - LastEngB;
              HEX4_BCD4( (unsigned char*)&CycEnery.LastPz0[0], MonEng );
            }
            else
            {
              MonEng = LastEngB - LastEng;
              HEX4_BCD4( (unsigned char*)&CycEnery.LastPz0[0], MonEng );
              CycEnery.LastPz0[3] |= 0x80;
            }
        }
    }
}
/************************************************************************
* 函数名 ：//CycleEngry_Resume
* 功能 ：//当前、上1周期组合有功累计用电量恢复
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2013/04/06 V1.0 yangxing XXXX
**************************************************************************/
void CycleEngry_Resume(void)
{
//    Current_CycleEngry();
    Y_M_C_CycleEngry( 0,(unsigned char*)&CycEnery.CurntPz0);
//    Last_CycleEngry();
}


/************************************************************************
* 函数名 ：//Current_MonthEngry
* 功能 ：//当前月组合有功累计用电量
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void Current_MonthEngry(void)
{
    unsigned long Tmp0 = 0;
    unsigned long Tmp1 = 0;
    unsigned long CurEng = 0;
    unsigned long LastEng = 0;
    unsigned long MonEng = 0;

//33     MEM_Read( (unsigned char*)&Tmp0, E2A_LastM_Act_Pz0, 4, Mem_E2PROMA );//上1月组合有功总电能(结算日1结算)
    memcpy( (unsigned char*)&Tmp1, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 4 );//当前组合有功总电量

    if( Tmp0 & 0x80000000 )//上1月为负电量
    {
        Tmp0 &= 0x7FFFFFFF;
        LastEng = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 )//当前月为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if( CurEng > LastEng )
            {
              MonEng = CurEng - LastEng;
              HEX4_BCD4( (unsigned char*)&MonthEnery.CurntPz0[0], MonEng );
              MonthEnery.CurntPz0[3] |= 0x80;
            }
            else
            {
              MonEng = LastEng - CurEng;
              HEX4_BCD4( (unsigned char*)&MonthEnery.CurntPz0[0], MonEng );
            }
        }
        else//当前月为正电量
        {
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = CurEng + LastEng;
            HEX4_BCD4( (unsigned char*)&MonthEnery.CurntPz0[0], MonEng );//当前月度组合有功总累计用电量
        }
    }
    else//上1月为正电量
    {
        LastEng = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 )//当前月为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = CurEng + LastEng;
            HEX4_BCD4( (unsigned char*)&MonthEnery.CurntPz0[0], MonEng );
            MonthEnery.CurntPz0[3] |= 0x80;
        }
        else//当前月为正电量
        {
            CurEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if( CurEng >= LastEng )
            {
              MonEng = CurEng - LastEng;
              HEX4_BCD4( (unsigned char*)&MonthEnery.CurntPz0[0], MonEng );
            }
            else
            {
              MonEng = LastEng - CurEng;
              HEX4_BCD4( (unsigned char*)&MonthEnery.CurntPz0[0], MonEng );
              MonthEnery.CurntPz0[3] |= 0x80;
            }
        }
    }
}

/************************************************************************
* 函数名 ：//Last_MonthEngry
* 功能 ：//上1月组合有功累计用电量
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void Last_MonthEngry(void)
{
    unsigned long LastEngB = 0; //上上次电量
    unsigned long LastEng = 0; //上1次电量
    unsigned long MonEng = 0;
    unsigned long Tmp0 = 0;
    unsigned long Tmp1 = 0;

 //33    MEM_Read( (unsigned char*)&Tmp0, E2A_LastM2_Act_Pz0, 4, Mem_E2PROMA );//上2月组合有功总电能
//33     MEM_Read( (unsigned char*)&Tmp1, E2A_LastM_Act_Pz0, 4, Mem_E2PROMA );//上1月组合有功总电能
    if( Tmp0 & 0x80000000 )//上2月为负电量
    {
        Tmp0 &= 0x7FFFFFFF;
        LastEngB = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 )//上1月为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            LastEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if(LastEng > LastEngB )
            {
              MonEng = LastEng - LastEngB;
              HEX4_BCD4( (unsigned char*)&MonthEnery.LastPz0[0], MonEng );           
              MonthEnery.LastPz0[3] |= 0x80;
            }
            else
            {
              MonEng = LastEngB - LastEng;
              HEX4_BCD4( (unsigned char*)&MonthEnery.LastPz0[0], MonEng );  
            }
              
        }
        else//上1月为正电量
        {
            LastEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = LastEng + LastEngB;
            HEX4_BCD4( (unsigned char*)&MonthEnery.LastPz0[0], MonEng );
        }
    }
    else//上2月为正电量
    {
        LastEngB = BCD4_HEX4( (unsigned char*)&Tmp0 );
        if( Tmp1 & 0x80000000 )//上1月为负电量
        {
            Tmp1 &= 0x7FFFFFFF;
            LastEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            MonEng = LastEng + LastEngB;
            HEX4_BCD4( (unsigned char*)&MonthEnery.LastPz0[0], MonEng );
            MonthEnery.LastPz0[3] |= 0x80;
        }
        else//上1月为正电量
        {
            LastEng = BCD4_HEX4( (unsigned char*)&Tmp1 );
            if(LastEng >= LastEngB )
            {
              MonEng = LastEng - LastEngB;
              HEX4_BCD4( (unsigned char*)&MonthEnery.LastPz0[0], MonEng );
            }
            else
            {
              MonEng = LastEngB - LastEng;
              HEX4_BCD4( (unsigned char*)&MonthEnery.LastPz0[0], MonEng );
              MonthEnery.LastPz0[3] |= 0x80;
            }
        }
    }
}
/************************************************************************
* 函数名 ：//MonthEngry_Resume
* 功能 ：//当前、上1月组合有功累计用电量恢复
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void MonthEngry_Resume(void)
{
    //Current_MonthEngry();
    Y_M_C_CycleEngry( 1,(unsigned char*)&CycEnery.CurntPz0);//当前月累计用电量
//    Last_MonthEngry();
}

/************************************************************************
* 函数名 ：//EC_ActGroup
* 功能 ：//组合有功电量重新计算
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------

**************************************************************************/
void EC_ActGroup(void)
{
    unsigned char i,j;
    unsigned char EC_Flag = 0;
    unsigned long EC_Pp = 0;
    unsigned long EC_Pn = 0;
    signed long EC_Grp = 0;
    unsigned short EC_Check = CHECKWORD;

    for( i = 0; i < 5; i++ )
    {
        EC_Pp = 0;
        EC_Pn = 0;
        EC_Grp = 0;
        EC_Pp = BCD4_HEX4( (unsigned char*)(&AEngy_Blk.AEngy.ActPp0[0]+4*i) );
        EC_Pn = BCD4_HEX4( (unsigned char*)(&AEngy_Blk.AEngy.ActPn0[0]+4*i) );

        for( j = 0; j < 4; j++ )
        {
            switch( MeasureMode.ActMode & (F_IncPp<<j) )
            {
                case F_IncPp://加正向有功
                    EC_Grp += EC_Pp;
                    break;
                case F_DecPp://减正向有功
                    EC_Grp -= EC_Pp;
                    break;
                case F_IncPn://加反向有功
                    EC_Grp += EC_Pn;
                    break;
                case F_DecPn://减反向有功
                    EC_Grp -= EC_Pn;
                    break;
                default:
                    break;
            }
        }
        if( EC_Grp < 0 )
        {
            EC_Grp = ~EC_Grp+1;
            EC_Flag = 1;
        }
        HEX4_BCD4( (unsigned char*)(&AEngy_Blk.AEngy.ActPz0[0]+4*i), EC_Grp );
        if( EC_Flag == 1 )//组合有功电量为负数
        {
            EC_Flag = 0;
            *(&AEngy_Blk.AEngy.ActPz0[3]+4*i) |= 0x80;
        }
    }

///33    EC_Check += ( CheckNum( (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 20 ) - CHECKWORD );//待验证
    memcpy( (unsigned char*)&AEngy_Blk.AEngy.ActPzChk[0], (unsigned char*)&EC_Check, 2 );

 ///33    MEM_CRCWrite( FM1_EC_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 20, Mem_FRAM );//写FM
 ///33    MEM_CRCWrite( E2A_EC_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 20, Mem_E2PROMA );//写E2
}

/************************************************************************
* 函数名 ：//EC_RactGroup
* 功能 ：//组合无功电量重新计算
* 输入参数 ：//mode:0 组合1 1 组合2
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------

**************************************************************************/
void EC_RactGroup(unsigned char MeasureSta, unsigned char Mode)
{
    unsigned char i,j,k;
    unsigned char EC_Flag = 0;
    unsigned char Dmd_Flag = 0;
    unsigned char Dmd_Tim[5];
    unsigned long Dmd_Use = 0;
    unsigned long Dmd_Grp = 0;
    signed long EC_Grp[4];//总、A、B、C组合无功
    unsigned short EC_Check = CHECKWORD;
    unsigned short Ph_Check = CHECKWORD;
    unsigned short Dmd_Check = CHECKWORD;
    unsigned long EC_Qur[4];
    unsigned long Dmd_Qur[4];
    unsigned long Phs_Qur[12];//分相4象限无功

    for( i = 0; i < 12; i++ )//分相4象限无功
    {
        Phs_Qur[i] = BCD4_HEX4( (unsigned char*)(&PEngy_Blk.PEngy.PhAQur1[0]+4*i) );
    }
    for( i = 0; i < 3; i++ )
    {
        EC_Grp[i+1] = 0;
    }
    for( i = 0; i < 5; i++ )//总、尖、峰、平、谷
    {
        EC_Grp[0] = 0;
        Dmd_Grp = 0;
        for( k = 0; k < 4; k++ )
        {
            EC_Qur[k] = BCD4_HEX4( (unsigned char*)(&REngy_Blk.REngy.Quadr1P0[0]+4*i+22*k) );
            memcpy( (unsigned char*)&Dmd_Use,(unsigned char*)(&MaxDmd_Blk[k+4].MaxDmd.TotalDmd[0]+8*i),3 );
            Dmd_Qur[k] = BCD4_HEX4( (unsigned char*)&Dmd_Use );
        }
        memset( (unsigned char*)&Dmd_Tim[0], 0x00, 5 );
        for( j = 0; j < 8; j++ )
        {
            switch( MeasureSta & (F_IncQuadr1<<j) )
            {
                case F_IncQuadr1://加1象限无功
                    EC_Grp[0] += EC_Qur[0];//总电量
                    if( Dmd_Qur[0] > Dmd_Grp )//需量
                    {
                        Dmd_Grp = Dmd_Qur[0];
                        memcpy( (unsigned char*)&Dmd_Tim[0],(unsigned char*)(&MaxDmd_Blk[4].MaxDmd.TotalDmdTime[0]+8*i),5 );
                        Dmd_Flag = 0;
                    }
                    if( i == 0 )//分相电量
                    {
                        EC_Grp[1] += Phs_Qur[0];//A相电量
                        EC_Grp[2] += Phs_Qur[1];//B相电量
                        EC_Grp[3] += Phs_Qur[2];//C相电量
                    }
                    break;
                case F_DecQuadr1://减1象限无功
                    EC_Grp[0] -= EC_Qur[0];//电量
                    if( Dmd_Qur[0] > Dmd_Grp )//需量
                    {
                        Dmd_Grp = Dmd_Qur[0];
                        memcpy( (unsigned char*)&Dmd_Tim[0],(unsigned char*)(&MaxDmd_Blk[4].MaxDmd.TotalDmdTime[0]+8*i),5 );
                        Dmd_Flag = 1;
                    }
                    if( i == 0 )//分相电量
                    {
                        EC_Grp[1] -= Phs_Qur[0];//A相电量
                        EC_Grp[2] -= Phs_Qur[1];//B相电量
                        EC_Grp[3] -= Phs_Qur[2];//C相电量
                    }
                    break;
                case F_IncQuadr2://加2象限无功
                    EC_Grp[0] += EC_Qur[1];//电量
                    if( Dmd_Qur[1] > Dmd_Grp )//需量
                    {
                        Dmd_Grp = Dmd_Qur[1];
                        memcpy( (unsigned char*)&Dmd_Tim[0],(unsigned char*)(&MaxDmd_Blk[5].MaxDmd.TotalDmdTime[0]+8*i),5 );
                        Dmd_Flag = 0;
                    }
                    if( i == 0 )
                    {
                        EC_Grp[1] += Phs_Qur[3];//A相电量
                        EC_Grp[2] += Phs_Qur[4];//B相电量
                        EC_Grp[3] += Phs_Qur[5];//C相电量
                    }
                    break;
                case F_DecQuadr2://减2象限无功
                    EC_Grp[0] -= EC_Qur[1];//电量
                    if( Dmd_Qur[1] > Dmd_Grp )//需量
                    {
                        Dmd_Grp = Dmd_Qur[1];
                        memcpy( (unsigned char*)&Dmd_Tim[0],(unsigned char*)(&MaxDmd_Blk[5].MaxDmd.TotalDmdTime[0]+8*i),5 );
                        Dmd_Flag = 1;
                    }
                    if( i == 0 )
                    {
                        EC_Grp[1] -= Phs_Qur[3];//A相电量
                        EC_Grp[2] -= Phs_Qur[4];//B相电量
                        EC_Grp[3] -= Phs_Qur[5];//C相电量
                    }
                    break;
                case F_IncQuadr3://加3象限无功
                    EC_Grp[0] += EC_Qur[2];//电量
                    if( Dmd_Qur[2] > Dmd_Grp )//需量
                    {
                        Dmd_Grp = Dmd_Qur[2];
                        memcpy( (unsigned char*)&Dmd_Tim[0],(unsigned char*)(&MaxDmd_Blk[6].MaxDmd.TotalDmdTime[0]+8*i),5 );
                        Dmd_Flag = 0;
                    }
                    if( i == 0 )
                    {
                        EC_Grp[1] += Phs_Qur[6];//A相电量
                        EC_Grp[2] += Phs_Qur[7];//B相电量
                        EC_Grp[3] += Phs_Qur[8];//C相电量
                    }
                    break;
                case F_DecQuadr3://减3象限无功
                    EC_Grp[0] -= EC_Qur[2];//电量
                    if( Dmd_Qur[2] > Dmd_Grp )//需量
                    {
                        Dmd_Grp = Dmd_Qur[2];
                        memcpy( (unsigned char*)&Dmd_Tim[0],(unsigned char*)(&MaxDmd_Blk[6].MaxDmd.TotalDmdTime[0]+8*i),5 );
                        Dmd_Flag = 1;
                    }
                    if( i == 0 )
                    {
                        EC_Grp[1] -= Phs_Qur[6];//A相电量
                        EC_Grp[2] -= Phs_Qur[7];//B相电量
                        EC_Grp[3] -= Phs_Qur[8];//C相电量
                    }
                    break;
                case F_IncQuadr4://加4象限无功
                    EC_Grp[0] += EC_Qur[3];//电量
                    if( Dmd_Qur[3] > Dmd_Grp )//需量
                    {
                        Dmd_Grp = Dmd_Qur[3];
                        memcpy( (unsigned char*)&Dmd_Tim[0],(unsigned char*)(&MaxDmd_Blk[7].MaxDmd.TotalDmdTime[0]+8*i),5 );
                        Dmd_Flag = 0;
                    }
                    if( i == 0 )
                    {
                        EC_Grp[1] += Phs_Qur[9];//A相电量
                        EC_Grp[2] += Phs_Qur[10];//B相电量
                        EC_Grp[3] += Phs_Qur[11];//C相电量
                    }
                    break;
                case F_DecQuadr4://减4象限无功
                    EC_Grp[0] -= EC_Qur[3];//电量
                    if( Dmd_Qur[3] > Dmd_Grp )//需量
                    {
                        Dmd_Grp = Dmd_Qur[3];
                        memcpy( (unsigned char*)&Dmd_Tim[0],(unsigned char*)(&MaxDmd_Blk[7].MaxDmd.TotalDmdTime[0]+8*i),5 );
                        Dmd_Flag = 1;
                    }
                    if( i == 0 )
                    {
                        EC_Grp[1] -= Phs_Qur[9];//A相电量
                        EC_Grp[2] -= Phs_Qur[10];//B相电量
                        EC_Grp[3] -= Phs_Qur[11];//C相电量
                    }
                    break;
                default:
                    break;
            }
        }
        if( EC_Grp[0] < 0 )
        {
            EC_Grp[0] = ~EC_Grp[0]+1;
            EC_Flag = 1;
        }

        HEX4_BCD4( (unsigned char*)(&REngy_Blk.REngy.Grp1Pz0[0]+4*i+22*Mode), EC_Grp[0] );//电量
        if( EC_Flag == 1 )//组合无功电量为负数
        {
            EC_Flag = 0;
            *(&REngy_Blk.REngy.Grp1Pz0[3]+4*i+22*Mode) |= 0x80;
        }

        HEX4_BCD4( (unsigned char*)&Dmd_Use, Dmd_Grp );//需量
        memcpy( (unsigned char*)(&MaxDmd_Blk[2+Mode].MaxDmd.TotalDmd[0]+8*i),(unsigned char*)&Dmd_Use, 3 );
        memcpy( (unsigned char*)(&MaxDmd_Blk[2+Mode].MaxDmd.TotalDmdTime[0]+8*i),(unsigned char*)&Dmd_Tim[0], 5 );
        if( Dmd_Flag == 1 )
        {
            Dmd_Flag = 0;
            *(&MaxDmd_Blk[2+Mode].MaxDmd.TotalDmd[2]+8*i) |= 0x80;
        }
    }

    for( i = 0; i < 3; i++ )//分相电量
    {
        if( EC_Grp[i+1] < 0 )
        {
            EC_Grp[i+1] = ~EC_Grp[i+1]+1;
            EC_Flag = 1;
        }
        HEX4_BCD4( (unsigned char*)(&PEngy_Blk.PEngy.PhAGrp1[0]+4*i+12*Mode), EC_Grp[i+1] );//电量
        if( EC_Flag == 1 )//组合无功电量为负数
        {
            EC_Flag = 0;
            *(&PEngy_Blk.PEngy.PhAGrp1[3]+4*i+12*Mode) |= 0x80;
        }
    }
    Ph_Check += ( CheckNum( (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 24 ) - CHECKWORD );
    memcpy( (unsigned char*)&PEngy_Blk.PEngy.PhGrpChk[0], (unsigned char*)&Ph_Check, 2 );
//33    MEM_CRCWrite( FM1_EC_PhA_Grp1, (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 24, Mem_FRAM );//写FM
//33    MEM_CRCWrite( E2A_EC_PhA_Grp1, (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 24, Mem_E2PROMA );//写E2

    EC_Check += ( CheckNum( (unsigned char*)(&REngy_Blk.REngy.Grp1Pz0[0]+22*Mode), 20 ) - CHECKWORD );//总电量
    memcpy( (unsigned char*)(&REngy_Blk.REngy.Grp1PzChk[0]+22*Mode), (unsigned char*)&EC_Check, 2 );
//33    MEM_CRCWrite( FM1_EC_Group1_Pz0+22*Mode, (unsigned char*)&REngy_Blk.REngy.Grp1Pz0[0], 20, Mem_FRAM );//写FM
// 33   MEM_CRCWrite( E2A_EC_Group1_Pz0+22*Mode, (unsigned char*)&REngy_Blk.REngy.Grp1Pz0[0], 20, Mem_E2PROMA );//写E2

    Dmd_Check += ( CheckNum( (unsigned char*)&MaxDmd_Blk[2+Mode].MaxDmdBuff[0], 40 ) - CHECKWORD );
    memcpy( (unsigned char*)&MaxDmd_Blk[2+Mode].MaxDmdBuff[40], (unsigned char*)&Dmd_Check, 2 );
//33    MEM_CRCWrite( E2A_TotalDemandGrp1+42*Mode, (unsigned char*)&MaxDmd_Blk[2].MaxDmdBuff[0], 40, Mem_E2PROMA );
}

/************************************************************************
* 函数名 ：//PzEC_INC
* 功能 ：//组合有功、无功电能量加1
* 输入参数 ：//RamAddr：电量RAM地址，OffAddr,偏移地址
             //Len: 数据长度，TouFee: 0-有费率, 1-无费率
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void PzEC_INC( unsigned char* RamAddr, unsigned char OffAddr, unsigned char Len, unsigned char TouFee )
{
    unsigned short EC_Check = 0;

    if( BCD_Check((RamAddr),4) == 0 )
    {
        BCD4_INC( RamAddr+OffAddr );
        EC_Check = CheckNum( RamAddr, Len );//从起始地址开始算校验和
        memcpy( RamAddr+Len, (unsigned char*)&EC_Check, 2 );//更新校验和

        if( FramGrpWriteFlag != 0x5A )
            FramGrpWriteFlag = 0x5A;

        if( (*(RamAddr+OffAddr) == 0x00) &&
            (((*(RamAddr+OffAddr+1)&0x0F) == 0x00) || ((*(RamAddr+OffAddr+1)&0x0F) == 0x05)) )//5kWh写E2
        {
            if( E2PGrpWriteFlag != 0x5A )
                E2PGrpWriteFlag = 0x5A;
        }
      /*
        if( (Fee.TOUNo > 0) && (Fee.TOUNo <= MaxFeeNum) && (TouFee == 0) )//费率电量累加
        {
            if( BCD_Check((RamAddr+OffAddr+4*Fee.TOUNo),4) == 0 )//分时电量
            {
                BCD4_INC( RamAddr+OffAddr+4*Fee.TOUNo );
                EC_Check = CheckNum( RamAddr, Len );//从起始地址开始算校验和
                memcpy( RamAddr+Len, (unsigned char*)&EC_Check, 2 );//更新校验和
            }
        }*/
    }
}

/************************************************************************
* 函数名 ：//PzEC_DEC
* 功能 ：//组合有功、无功电能量减1
* 输入参数 ：//RamAddr：电量RAM地址，OffAddr,偏移地址
             //Len: 数据长度，TouFee: 0-有费率, 1-无费率
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void PzEC_DEC( unsigned char* RamAddr, unsigned char OffAddr, unsigned char Len, unsigned char TouFee )
{
    unsigned short EC_Check = 0;

    if( BCD_Check((RamAddr),4) == 0 )
    {
        BCD4_DEC( RamAddr+OffAddr );
        EC_Check = CheckNum( RamAddr, Len );//从起始地址开始算校验和
        memcpy( RamAddr+Len, (unsigned char*)&EC_Check, 2 );//更新校验和

        if( FramGrpWriteFlag != 0x5A )
            FramGrpWriteFlag = 0x5A;

        if( (*(RamAddr+OffAddr) == 0x00) &&
            (((*(RamAddr+OffAddr+1)&0x0F) == 0x00) || ((*(RamAddr+OffAddr+1)&0x0F) == 0x05)) )//5kWh写E2
        {
            if( E2PGrpWriteFlag != 0x5A )
                E2PGrpWriteFlag = 0x5A;
        }
        /*
        if( (Fee.TOUNo > 0) && (Fee.TOUNo <= MaxFeeNum) && (TouFee == 0) )//费率电量累加
        {
            if( BCD_Check((RamAddr+OffAddr+4*Fee.TOUNo),4) == 0 )//分时电量
            {
                BCD4_DEC( RamAddr+OffAddr+4*Fee.TOUNo );
                EC_Check = CheckNum( RamAddr, Len );//从起始地址开始算校验和
                memcpy( RamAddr+Len, (unsigned char*)&EC_Check, 2 );//更新校验和
            }
        }*/
    }
}

/************************************************************************
* 函数名 ：//EC_INC
* 功能 ：//有功、无功电能量加1
* 输入参数 ：//RamAddr：电量起始地址，OffAddr,偏移地址
             //Len: 数据长度，TouFee: 0-有费率, 1-无费率
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void EC_INC( unsigned char* RamAddr, unsigned char OffAddr, unsigned char Len, unsigned char TouFee )
{
    unsigned short EC_Check = 0;

    if( BCD_Check((RamAddr),4) == 0 )
    {
        BCDX_INC( (RamAddr+OffAddr), 4 );
        EC_Check = CheckNum( RamAddr, Len );//从起始地址开始算校验和
        memcpy( RamAddr+Len, (unsigned char*)&EC_Check, 2 );//更新校验和

        if( FramWriteFlag != 0x5A )
            FramWriteFlag = 0x5A;

        if( (*(RamAddr+OffAddr) == 0x00) &&
            (((*(RamAddr+OffAddr+1)&0x0F) == 0x00) || ((*(RamAddr+OffAddr+1)&0x0F) == 0x05)) )//5kWh写E2
        {
            if( E2PWriteFlag != 0x5A )
                E2PWriteFlag = 0x5A;
        }
/*
        if( (Fee.TOUNo > 0) && (Fee.TOUNo <= MaxFeeNum) && (TouFee == 0) )//费率电量累加
        {
            if( BCD_Check((RamAddr+OffAddr+4*Fee.TOUNo),4) == 0 )//分时电量
            {
                BCDX_INC( (RamAddr+OffAddr+4*Fee.TOUNo), 4 );
                EC_Check = CheckNum( RamAddr, Len );//从起始地址开始算校验和
                memcpy( RamAddr+Len, (unsigned char*)&EC_Check, 2 );//更新校验和
            }
        }
        */
    }
}

/************************************************************************
* 函数名 ：//APulseCnt
* 功能 ：//正、反向有功脉冲计数
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void APulseCnt(void)
{
    if( ( Quadrant[0] < 1 ) || ( Quadrant[0] > 4 ) )
    {
        Quadrant[0] = 1;
    }

    if( ( Quadrant[0] == 1 ) || ( Quadrant[0] == 4 ) )
    {
        PCnt_Blk.PCnt.ActPpCnt[0]++;//正向有功总脉冲累加
    }
    else
    {
        PCnt_Blk.PCnt.ActPnCnt[0]++;//反向有功总脉冲累加
    }
}

/************************************************************************
* 函数名 ：//RPulseCnt
* 功能 ：//无功脉冲累加
* 输入参数 ：//CntAdd: 象限脉冲地址,Channel:通道号 0--总无功 1、2、3--A、B、C相无功
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void RPulseCnt( unsigned short* CntAdd, unsigned char Channel )
{
    if( ( Quadrant[Channel] < 1 ) || ( Quadrant[Channel] > 4 ) )
    {
        Quadrant[Channel] = 1;
    }

    switch( Quadrant[Channel] )
    {
        case 1:
            if( Channel > 0)//分相
                (*CntAdd)++;
            else
            {//合相
                (*CntAdd)++;
                (*(CntAdd+((Tariff.Current&3)+1)))++;
            }
            break;
        case 2:
            if( Channel > 0)
                (*(CntAdd+3))++;
            else
            {
                (*(CntAdd+5))++;
                (*(CntAdd+5+((Tariff.Current&3)+1)))++;
            }
            break;
        case 3:
            if( Channel > 0)
                (*(CntAdd+6))++;
            else
            {
                (*(CntAdd+10))++;
                (*(CntAdd+10+((Tariff.Current&3)+1)))++;
            }
            break;
        case 4:
            if( Channel > 0)
                (*(CntAdd+9))++;
            else
            {
                (*(CntAdd+15))++;
                (*(CntAdd+15+((Tariff.Current&3)+1)))++;
            }
            break;
        default:
            break;
    }
}

/************************************************************************
* 函数名 ：//PulseCount
* 功能 ：//脉冲计数
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 
**************************************************************************/
void PulseCount(void)
{
    unsigned char i;

//    for( i = 0; i < 3; i++ )
//    {
//        while( PowerValue.ActPowerPp[i] >= Factor.Ke )//A、B、C正向有功脉冲累加
//        {
//            PowerValue.ActPowerPp[i] -= Factor.Ke;
//            (*(&PCnt_Blk.PCnt.PhAPpCnt+i))++;//这里一个脉冲代表0.0001KWH
//        }
//        while( PowerValue.ActPowerPn[i] >= Factor.Ke )//A、B、C反向有功脉冲累加
//        {
//            PowerValue.ActPowerPn[i] -= Factor.Ke;
//            (*(&PCnt_Blk.PCnt.PhAPnCnt+i))++;
//        }
//        while( PowerValue.RactPowerPp[i] >= Factor.Ke )//A、B、C 4象限无功脉冲累加
//        {
//            PowerValue.RactPowerPp[i] -= Factor.Ke;
//            RPulseCnt( (unsigned short*)(&PCnt_Blk.PCnt.AQuadr1PCnt+i), (i+1) );
//        }

//        if( i < 2 )
//        {
//            while( PowerValue.VahrPower[i] >= Factor.Ke )//正、反视在脉冲
//            {
//                PowerValue.VahrPower[i] -= Factor.Ke;
//                (*(&PCnt_Blk.PCnt.VAHRCnt[0]+i*5))++;
//                (*(&PCnt_Blk.PCnt.VAHRCnt[((Tariff.Current&3)+1)]+i*5))++;
//            }
//        }
//    }

    while( PowerValue.ActPowerP >= Factor.Ke )
    {
        PowerValue.ActPowerP -= Factor.Ke;
//        PCnt_Blk.PCnt.ActPpCnt[0]++;//正向有功总脉冲累加,脉冲常数小于20000时这里一个脉冲代表0.0001KWH，否则为0.00001KWH
//        PCnt_Blk.PCnt.ActPpCnt[((Tariff.Current&3)+1)]++;//正向有功费率脉冲累加
			  Decimal.Fw[0]++;
        
    }
                                                                                                            
   //反向有功总脉冲
    while( PowerValue.ActPowerN  >= Factor.Ke )
    {
        PowerValue.ActPowerN -= Factor.Ke;
//        PCnt_Blk.PCnt.ActPnCnt[0]++;//反向有功总脉冲累加
//        PCnt_Blk.PCnt.ActPnCnt[((Tariff.Current&3)+1)]++;//反向有功费率脉冲累加 
			  Decimal.Bk[0]++;	
    }

   //4象限无功总脉冲
//    while( PowerValue.RactPower >= Factor.Ke )
//    {
//        PowerValue.RactPower -= Factor.Ke;
//        RPulseCnt( (unsigned short*)&PCnt_Blk.PCnt.Quadr1PCnt, 0 );
//    } 
}

/************************************************************************
* 函数名 ：//ECMeasure
* 功能 ：//当脉冲数累计到一定值时,电能量累加
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//湖南要求正向有功=正向+反向
* 修改日期 :
* ------------------------------------------------------------------------
* 2010/07/06 V1.0 yangxing XXXX
**************************************************************************/
void ECMeasure(void)
{
    unsigned char i;
    unsigned short EC_Check2 = 0;
    unsigned char FeeNo;
	  union B16_B08 PBlk;
	
	
	
	while( Decimal.Fw[0] >= g_PerKwh )
	{
		Decimal.Fw[0] -= g_PerKwh;
		Decimal.Unit_Pluse++;
	}
	
	while( Decimal.Bk[0] >= g_PerKwh )
	{
		Decimal.Bk[0] -= g_PerKwh;
		Decimal.Unit_Bkpluse++;
	}
	
	
	Calc_Pwr_Proc();
//	
//	Decimal.Unit_Pluse++;
//	
//    for( i = 0; i < 2; i++ )//当前正向、反向、组合有功累加
//    {
//        Clear_Wdt();//清看门狗
//			  PBlk.B08[0] =PCnt_Blk.PCntBuff[58+(10*i)];
//			  PBlk.B08[1] =PCnt_Blk.PCntBuff[59+(10*i)];
//			
//        while( PBlk.B16 >= g_PerKwh )
//        {
//            PBlk.B16 -= g_PerKwh;
//					  PCnt_Blk.PCntBuff[58+(10*i)] = PBlk.B08[0] ;
//			      PCnt_Blk.PCntBuff[59+(10*i)] = PBlk.B08[1] ;
//            if( !((MeasureMode.ActMode & (F_IncPp<<(2*i)))&&(MeasureMode.ActMode & (F_DecPp<<(2*i)))) )//当前组合有功电量
//            {
//                if( MeasureMode.ActMode & (F_IncPp<<(2*i)) )
//                {
//// 33                   Energy_checksum(0);
//                    PzEC_INC( (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 0, 20, 0 );//当前组合有功=+正向+反向
//                    
//                }
//                else if( MeasureMode.ActMode & (F_DecPp<<(2*i)) )
//                {
//// 33                   Energy_checksum(0);
//                    PzEC_DEC( (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 0, 20, 0 );//当前组合有功=-正向-反向                   
//                }
//                
//            }           
////33            Energy_checksum(i+1);//正、反向总有功
//            EC_INC( (unsigned char*)(&AEngy_Blk.AEngy.ActPp0[0]+22*i), 0, 20, 0 );
//          
//            PzEC_INC( (unsigned char*)&AEngy_Blk.AEngy.ActUsedPz0[0], 0, 4, 0 );//+正向+反向 采用正反向电量绝对值方式
//            
////33            if((unsigned char*)(&AEngy_Blk.AEngy.ActPp0[0]+22*i)==0) Flag.Eng_Moneystate |=F_EenZD;//电量整数
////33            Amount_deduction(1);      
//             
//        }
//        PBlk.B08[0] =PCnt_Blk.PCntBuff[58+FeeNo*5+(10*i)];
//			  PBlk.B08[1] =PCnt_Blk.PCntBuff[59+FeeNo*5+(10*i)];
//        while( PBlk.B16 >= g_PerKwh )//分费率
//        {
//            FramWriteFlag = 0x5A;
//            PBlk.B16 -= g_PerKwh;
//            PCnt_Blk.PCntBuff[58+FeeNo*5+(10*i)] = PBlk.B08[0];
//			      PCnt_Blk.PCntBuff[59+FeeNo*5+(10*i)] = PBlk.B08[1] ;
//            if( !((MeasureMode.ActMode & (F_IncPp<<(2*i)))&&(MeasureMode.ActMode & (F_DecPp<<(2*i)))) )//组合有功电量
//            {
//                FramGrpWriteFlag = 0x5A;
//                if( MeasureMode.ActMode & (F_IncPp<<(2*i)) )
//                {
////33                    Energy_checksum(0);
//                    if( (FeeNo > 0) && (FeeNo <= MaxFeeNum) )//费率电量累加
//                    {
//                        if( BCD_Check((&AEngy_Blk.AEngy.ActPz0[0]+4*FeeNo),4) == 0 )//分时电量
//                        {
//                            BCD4_INC( &AEngy_Blk.AEngy.ActPz0[0]+4*FeeNo );
//                            EC_Check2 = CheckNum( &AEngy_Blk.AEngy.ActPz0[0], 20 );//从起始地址开始算校验和
//                            memcpy( &AEngy_Blk.AEngy.ActPz0[0]+20, (unsigned char*)&EC_Check2, 2 );//更新校验和
//                        }
//                    }
//                }
//                else if( MeasureMode.ActMode & (F_DecPp<<(2*i)) )
//                {
////33                    Energy_checksum(0);
//                    if( (FeeNo > 0) && (FeeNo <= MaxFeeNum) )//费率电量累加
//                    {
//                        if( BCD_Check((&AEngy_Blk.AEngy.ActPz0[0]+4*FeeNo),4) == 0 )//分时电量
//                        {
//                            BCD4_DEC( &AEngy_Blk.AEngy.ActPz0[0]+4*FeeNo );
//                            EC_Check2 = CheckNum( &AEngy_Blk.AEngy.ActPz0[0], 20 );//从起始地址开始算校验和
//                            memcpy( &AEngy_Blk.AEngy.ActPz0[0]+20, (unsigned char*)&EC_Check2, 2 );//更新校验和
//                        }
//                    }
//                }
//            }

////33            Energy_checksum(i+1);//正、反向有功费率电量
//            if( ( FeeNo > 0) && ( FeeNo <= MaxFeeNum) )//费率电量累加
//            {
//                if( BCD_Check((&AEngy_Blk.AEngy.ActPp0[0]+i*22+4*FeeNo),4) == 0 )//分时电量
//                {
//                    BCD4_INC( &AEngy_Blk.AEngy.ActPp0[0]+i*22+4*FeeNo );
//                    EC_Check2 = CheckNum( &AEngy_Blk.AEngy.ActPp0[0]+i*22, 20 );//从起始地址开始算校验和
//                    memcpy( &AEngy_Blk.AEngy.ActPp0[0]+i*22+20, (unsigned char*)&EC_Check2, 2 );//更新校验和
//                }
////33                Amount_deduction(0);           
//                
//            }
//           
//        }

//        if( FramWriteFlag == 0x5A )//写铁电
//        {
//            FramWriteFlag = 0;
////33            MEM_CRCWrite( FM1_EC_Act_Pp0+22*i, (unsigned char*)(&AEngy_Blk.AEngy.ActPp0[0]+22*i), 20, Mem_FRAM );
//        }
//        if( E2PWriteFlag == 0x5A )//写EEPROM
//        {
//            E2PWriteFlag = 0;
////33            MEM_CRCWrite( E2A_EC_Act_Pp0+22*i, (unsigned char*)(&AEngy_Blk.AEngy.ActPp0[0]+22*i), 20, Mem_E2PROMA );
//        }
//    }
//    
//    if( FramGrpWriteFlag == 0x5A )//写组合有功电量
//    {
//        FramGrpWriteFlag = 0;
//////33        MEM_CRCWrite( FM1_EC_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 20, Mem_FRAM );//往铁电写组合有功电量
//        Y_M_C_CycleEngry( 0,(unsigned char*)&MonthEnery.CurntPz0);//当前周期累计用电量  
//        Y_M_C_CycleEngry( 1,(unsigned char*)&CycEnery.CurntPz0);//当前月度累计用电量
//        Y_M_C_CycleEngry( 2,(unsigned char*)&YearEnery.CurntPz0);//当前年度累计用电量
//    }
//    if( E2PGrpWriteFlag == 0x5A )
//    {
//        E2PGrpWriteFlag = 0;
////33        MEM_CRCWrite( E2A_EC_Act_Pz0, (unsigned char*)&AEngy_Blk.AEngy.ActPz0[0], 20, Mem_E2PROMA );//往E2写组合有功电量
//    }
//    PCnt_Blk.PCnt.ActPzCnt[0] =0;
//    PCnt_Blk.PCnt.ActPzCnt[FeeNo]=0;
//    
//    if( (MeasureMode.ActMode & 0x0F) != 0x00 )//组合有功剩余脉冲累计
//    {
//       
//       if( MeasureMode.ActMode & F_IncPp )
//       {
//         PCnt_Blk.PCnt.ActPzCnt[0] += PCnt_Blk.PCnt.ActPpCnt[0];
//         PCnt_Blk.PCnt.ActPzCnt[FeeNo] += PCnt_Blk.PCnt.ActPpCnt[FeeNo];
//       }

//       if( MeasureMode.ActMode & F_IncPn )
//       {
//         PCnt_Blk.PCnt.ActPzCnt[0] += PCnt_Blk.PCnt.ActPnCnt[0];
//         PCnt_Blk.PCnt.ActPzCnt[FeeNo] += PCnt_Blk.PCnt.ActPnCnt[FeeNo];
//       }
       /*
         if( PCnt_Blk.PCnt.ActPzCnt[0] >= g_PerKwh )
             PCnt_Blk.PCnt.ActPzCnt[0] = (g_PerKwh-1);
         if( PCnt_Blk.PCnt.ActPzCnt[Fee.TOUNo] >= g_PerKwh )
             PCnt_Blk.PCnt.ActPzCnt[Fee.TOUNo] = (g_PerKwh-1);
       */
    
      
//    for( i = 0; i < 4; i++ )//4象限无功、组合无功1、组合无功2电量累加
////    {
////        Clear_Wdt();//清看门狗
//        while( *(&PCnt_Blk.PCnt.Quadr1PCnt[0]+i*5) >= g_PerKwh )
//        {
//            *(&PCnt_Blk.PCnt.Quadr1PCnt[0]+i*5) -= g_PerKwh;

//            if( !((MeasureMode.RactMode1 & (F_IncQuadr1<<(2*i)))&&(MeasureMode.RactMode1 & (F_DecQuadr1<<(2*i)))) )//组合无功1
//            {
//                if( MeasureMode.RactMode1 & (F_IncQuadr1<<(2*i)) )
//                {
////33                    Energy_checksum(3);
//                    PzEC_INC( (unsigned char*)&REngy_Blk.REngy.Grp1Pz0[0], 0, 20, 0 );//组合无功1+象限
//                }
//                else if( MeasureMode.RactMode1 & (F_DecQuadr1<<(2*i)) )
//                {
////33                    Energy_checksum(3);
//                    PzEC_DEC( (unsigned char*)&REngy_Blk.REngy.Grp1Pz0[0], 0, 20, 0 );//组合无功1-象限
//                }
//            }

//            if( !((MeasureMode.RactMode2 & (F_IncQuadr1<<(2*i)))&&(MeasureMode.RactMode2 & (F_DecQuadr1<<(2*i)))) )//组合无功2
//            {
//                if( MeasureMode.RactMode2 & (F_IncQuadr1<<(2*i)) )
//                {
////33                    Energy_checksum(4);
//                    PzEC_INC( (unsigned char*)&REngy_Blk.REngy.Grp2Pz0[0], 0, 20, 0 );//组合无功2+象限
//                }
//                else if( MeasureMode.RactMode2 & (F_DecQuadr1<<(2*i)) )
//                {
////33                    Energy_checksum(4);
//                    PzEC_DEC( (unsigned char*)&REngy_Blk.REngy.Grp2Pz0[0], 0, 20, 0 );//组合无功2+象限
//                }
//            }

////33            Energy_checksum(i+5);//4象限无功电量
//            EC_INC( (unsigned char*)(&REngy_Blk.REngy.Quadr1P0[0]+22*i), 0,20, 0 );
//        }
//        
//        while( *(&PCnt_Blk.PCnt.Quadr1PCnt[FeeNo]+i*5) >= g_PerKwh )//费率电量
//        {
//            FramWriteFlag = 0x5A;
//            FramGrpWriteFlag = 0x5A;
//            *(&PCnt_Blk.PCnt.Quadr1PCnt[FeeNo]+i*5) -= g_PerKwh;

//            if( !((MeasureMode.RactMode1 & (F_IncQuadr1<<(2*i)))&&(MeasureMode.RactMode1 & (F_DecQuadr1<<(2*i)))) )//组合无功1
//            {
//                if( MeasureMode.RactMode1 & (F_IncQuadr1<<(2*i)) )
//                {
////33                    Energy_checksum(3);
//                    //PzEC_INC( (unsigned char*)&REngy_Blk.REngy.Grp1Pz0[0], 0, 20, 0 );
//                    if( (FeeNo > 0) && (FeeNo <= MaxFeeNum) )//费率电量累加
//                    {
//                        if( BCD_Check((&REngy_Blk.REngy.Grp1Pz0[0]+4*FeeNo),4) == 0 )//分时电量
//                        {
//                            BCD4_INC( &REngy_Blk.REngy.Grp1Pz0[0]+4*FeeNo );
//                            EC_Check2 = CheckNum( &REngy_Blk.REngy.Grp1Pz0[0], 20 );//从起始地址开始算校验和
//                            memcpy( &REngy_Blk.REngy.Grp1Pz0[0]+20, (unsigned char*)&EC_Check2, 2 );//更新校验和
//                        }
//                    }
//                }
//                else if( MeasureMode.RactMode1 & (F_DecQuadr1<<(2*i)) )
//                {
//// 33                   Energy_checksum(3);
//                    //PzEC_DEC( (unsigned char*)&REngy_Blk.REngy.Grp1Pz0[0], 0, 20, 0 );
//                    if( (FeeNo > 0) && (FeeNo <= MaxFeeNum) )//费率电量累加
//                    {
//                        if( BCD_Check((&REngy_Blk.REngy.Grp1Pz0[0]+4*FeeNo),4) == 0 )//分时电量
//                        {
//                            BCD4_DEC( &REngy_Blk.REngy.Grp1Pz0[0]+4*FeeNo );
//                            EC_Check2 = CheckNum( &REngy_Blk.REngy.Grp1Pz0[0], 20 );//从起始地址开始算校验和
//                            memcpy( &REngy_Blk.REngy.Grp1Pz0[0]+20, (unsigned char*)&EC_Check2, 2 );//更新校验和
//                        }
//                    }
//                }
//            }

//            if( !((MeasureMode.RactMode2 & (F_IncQuadr1<<(2*i)))&&(MeasureMode.RactMode2 & (F_DecQuadr1<<(2*i)))) )//组合无功2
//            {
//                if( MeasureMode.RactMode2 & (F_IncQuadr1<<(2*i)) )
//                {
////33                    Energy_checksum(4);
//                    //PzEC_INC( (unsigned char*)&REngy_Blk.REngy.Grp2Pz0[0], 0, 20, 0 );
//                    if( (FeeNo > 0) && (FeeNo <= MaxFeeNum) )//费率电量累加
//                    {
//                        if( BCD_Check((&REngy_Blk.REngy.Grp2Pz0[0]+4*FeeNo),4) == 0 )//分时电量
//                        {
//                            BCD4_INC( &REngy_Blk.REngy.Grp2Pz0[0]+4*FeeNo );
//                            EC_Check2 = CheckNum( &REngy_Blk.REngy.Grp2Pz0[0], 20 );//从起始地址开始算校验和
//                            memcpy( &REngy_Blk.REngy.Grp2Pz0[0]+20, (unsigned char*)&EC_Check2, 2 );//更新校验和
//                        }
//                    }
//                }
//                else if( MeasureMode.RactMode2 & (F_DecQuadr1<<(2*i)) )
//                {
//// 33                   Energy_checksum(4);
//                    //PzEC_DEC( (unsigned char*)&REngy_Blk.REngy.Grp2Pz0[0], 0, 20, 0 );
//                    if( (FeeNo > 0) && (FeeNo <= MaxFeeNum) )//费率电量累加
//                    {
//                        if( BCD_Check((&REngy_Blk.REngy.Grp2Pz0[0]+4*FeeNo),4) == 0 )//分时电量
//                        {
//                            BCD4_DEC( &REngy_Blk.REngy.Grp2Pz0[0]+4*FeeNo );
//                            EC_Check2 = CheckNum( &REngy_Blk.REngy.Grp2Pz0[0], 20 );//从起始地址开始算校验和
//                            memcpy( &REngy_Blk.REngy.Grp2Pz0[0]+20, (unsigned char*)&EC_Check2, 2 );//更新校验和
//                        }
//                    }
//                }
//            }

////33            Energy_checksum(i+5);//4象限无功电量
//            //EC_INC( (unsigned char*)(&REngy_Blk.REngy.Quadr1P0[0]+22*i), 0,20, 0 );
//            if( (FeeNo > 0) && (FeeNo <= MaxFeeNum) )//费率电量累加
//            {
//                if( BCD_Check((&REngy_Blk.REngy.Quadr1P0[0]+i*22+4*FeeNo),4) == 0 )//分时电量
//                {
//                    BCD4_INC( &REngy_Blk.REngy.Quadr1P0[0]+i*22+4*FeeNo );
//                    EC_Check2 = CheckNum( &REngy_Blk.REngy.Quadr1P0[0]+i*22, 20 );//从起始地址开始算校验和
//                    memcpy( &REngy_Blk.REngy.Quadr1P0[0]+i*22+20, (unsigned char*)&EC_Check2, 2 );//更新校验和
//                }
//            }
//        }

//        if( FramWriteFlag == 0x5A )
//        {
//            FramWriteFlag = 0;
////33            MEM_CRCWrite( FM1_EC_Quadr1_P0+22*i, (unsigned char*)(&REngy_Blk.REngy.Quadr1P0[0]+22*i), 20, Mem_FRAM );
//        }
//        if( E2PWriteFlag == 0x5A )
//        {
//            E2PWriteFlag = 0;
////33            MEM_CRCWrite( E2A_EC_Quadr1_P0+22*i, (unsigned char*)(&REngy_Blk.REngy.Quadr1P0[0]+22*i), 20, Mem_E2PROMA );
//        }
//    }
//    if( FramGrpWriteFlag == 0x5A )//写组合无功1、2电量
//    {
//        FramGrpWriteFlag = 0;
////33        MEM_CRCWrite( FM1_EC_Group1_Pz0, (unsigned char*)&REngy_Blk.REngy.Grp1Pz0[0], 20, Mem_FRAM );
////33        MEM_CRCWrite( FM1_EC_Group2_Pz0, (unsigned char*)&REngy_Blk.REngy.Grp2Pz0[0], 20, Mem_FRAM );
//    }
//    if( E2PGrpWriteFlag == 0x5A )
//    {
//        E2PGrpWriteFlag = 0;
////33        MEM_CRCWrite( E2A_EC_Group1_Pz0, (unsigned char*)&REngy_Blk.REngy.Grp1Pz0[0], 20, Mem_E2PROMA );
////33        MEM_CRCWrite( E2A_EC_Group2_Pz0, (unsigned char*)&REngy_Blk.REngy.Grp2Pz0[0], 20, Mem_E2PROMA );
//    }

//    PCnt_Blk.PCnt.Grp1PzCnt[0]=0;
//    PCnt_Blk.PCnt.Grp1PzCnt[FeeNo]=0;
//    PCnt_Blk.PCnt.Grp2PzCnt[0]=0;
//    PCnt_Blk.PCnt.Grp2PzCnt[FeeNo]=0;
//组合无功不足0.01度的脉冲数
//    for( i = 0; i < 4; i++ )//组合无功1、组合无功2剩余脉冲以当前所落在的象限为准
//    {
//        if( MeasureMode.RactMode1 & (F_IncQuadr1<<(2*i)) )
//        {
//            PCnt_Blk.PCnt.Grp1PzCnt[0] +=*(&PCnt_Blk.PCnt.Quadr1PCnt[0]+i*5);
//            PCnt_Blk.PCnt.Grp1PzCnt[FeeNo] +=*(&PCnt_Blk.PCnt.Quadr1PCnt[FeeNo]+i*5);
//        }

//        if( MeasureMode.RactMode2 & (F_IncQuadr1<<(2*i)) )
//        {
//            PCnt_Blk.PCnt.Grp2PzCnt[0] +=*(&PCnt_Blk.PCnt.Quadr1PCnt[0]+i*5);
//            PCnt_Blk.PCnt.Grp2PzCnt[FeeNo] +=*(&PCnt_Blk.PCnt.Quadr1PCnt[FeeNo]+i*5);
//        }
//    }
    /*
    if(PCnt_Blk.PCnt.Grp1PzCnt[0] >= g_PerKwh)
        PCnt_Blk.PCnt.Grp1PzCnt[0] = (g_PerKwh-1);
    if(PCnt_Blk.PCnt.Grp2PzCnt[0] >= g_PerKwh)
        PCnt_Blk.PCnt.Grp1PzCnt[0] = (g_PerKwh-1);
    if(PCnt_Blk.PCnt.Grp1PzCnt[Fee.TOUNo] >= g_PerKwh)
        PCnt_Blk.PCnt.Grp1PzCnt[Fee.TOUNo] = (g_PerKwh-1);  
    if(PCnt_Blk.PCnt.Grp2PzCnt[Fee.TOUNo] >= g_PerKwh)
        PCnt_Blk.PCnt.Grp2PzCnt[Fee.TOUNo] = (g_PerKwh-1);
    */
//    for( i = 0; i < 6; i++ )//A、B、c相正向、反向有功
//    {
//        while( *(&PCnt_Blk.PCnt.PhAPpCnt+i) >= g_PerKwh )
//        {
//            *(&PCnt_Blk.PCnt.PhAPpCnt+i) -= g_PerKwh;
////33            Energy_checksum(9);
//            EC_INC( (unsigned char*)&PEngy_Blk.PEngy.PhAPp[0], 4*i, 24, 1 );
//        }
//    }
//    if( FramWriteFlag == 0x5A )
//    {
//        FramWriteFlag = 0;
//// 33       MEM_CRCWrite( FM1_EC_PhA_Pp, (unsigned char*)&PEngy_Blk.PEngy.PhAPp[0], 24, Mem_FRAM );
//    }
//    if( E2PWriteFlag == 0x5A )
//    {
//        E2PWriteFlag = 0;
//// 33       MEM_CRCWrite( E2A_EC_PhA_Pp, (unsigned char*)&PEngy_Blk.PEngy.PhAPp[0], 24, Mem_E2PROMA );
//    }
////不要求分相无功计量
//    for( i = 0; i < 12; i++ )//A、B、c相1、2、3、4无功累计
//    {
//        while( *(&PCnt_Blk.PCnt.AQuadr1PCnt+i) >= g_PerKwh )
//        {
//            *(&PCnt_Blk.PCnt.AQuadr1PCnt+i) -= g_PerKwh;

//            if( !((MeasureMode.RactMode1 & (F_IncQuadr1<<(2*(i/3))))&&(MeasureMode.RactMode1 & (F_DecQuadr1<<(2*(i/3))))) )//分相组合无功1
//            {
//                if( MeasureMode.RactMode1 & (F_IncQuadr1<<(2*(i/3))) )//加4象限
//                {
////33                    Energy_checksum(10);
//                    PzEC_INC( (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 4*(i%3), 24, 1 );
//                }
//                else if( MeasureMode.RactMode1 & (F_DecQuadr1<<(2*(i/3))) )//减4象限
//                {
//// 33                   Energy_checksum(10);
//                    PzEC_DEC( (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 4*(i%3), 24, 1 );
//                }
//            }

//            if( !((MeasureMode.RactMode2 & (F_IncQuadr1<<(2*(i/3))))&&(MeasureMode.RactMode2 & (F_DecQuadr1<<(2*(i/3))))) )//分相组合无功2
//            {
//                if( MeasureMode.RactMode2 & (F_IncQuadr1<<(2*(i/3))) )
//                {
////33                    Energy_checksum(10);
//                    PzEC_INC( (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 4*(i%3)+12, 24, 1 );
//                }
//                else if( MeasureMode.RactMode2 & (F_DecQuadr1<<(2*(i/3))) )
//                {
////33                    Energy_checksum(10);
//                    PzEC_DEC( (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 4*(i%3)+12, 24, 1 );
//                }
//            }

////33            Energy_checksum(11);//分相相4象限无功
//            EC_INC( (unsigned char*)&PEngy_Blk.PEngy.PhAQur1[0], 4*i, 48, 1 );
//        }
//    }

//    if( FramGrpWriteFlag == 0x5A )//分相组合无功
//    {
//        FramGrpWriteFlag = 0;
////33        MEM_CRCWrite( FM1_EC_PhA_Grp1, (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 24, Mem_FRAM );
//    }
//    if( E2PGrpWriteFlag == 0x5A )
//    {
//        E2PGrpWriteFlag = 0;
////33        MEM_CRCWrite( E2A_EC_PhA_Grp1, (unsigned char*)&PEngy_Blk.PEngy.PhAGrp1[0], 24, Mem_E2PROMA );
//    }

//    if( FramWriteFlag == 0x5A )//分相相4象限无功
//    {
//        FramWriteFlag = 0;
////33        MEM_CRCWrite( FM1_EC_PhA_Qudr1, (unsigned char*)&PEngy_Blk.PEngy.PhAQur1[0], 48, Mem_FRAM );
//    }
//    if( E2PWriteFlag == 0x5A )
//    {
//        E2PWriteFlag = 0;
////33        MEM_CRCWrite( E2A_EC_PhA_Qudr1, (unsigned char*)&PEngy_Blk.PEngy.PhAQur1[0], 48, Mem_E2PROMA );
//    }

//    for( i = 0; i < 2; i++ )//正、反向视在电能
//    {
//        while( *(&PCnt_Blk.PCnt.VAHRCnt[0]+i*5) >= g_PerKwh )
//        {
//            *(&PCnt_Blk.PCnt.VAHRCnt[0]+i*5) -= g_PerKwh;
////33            Energy_checksum(i+12);
//            EC_INC( (unsigned char*)(&VEngy_Blk.VEngy.VarPp0[0]+22*i), 0, 20, 0 );
//        }
//        
//        while( *(&PCnt_Blk.PCnt.VAHRCnt[FeeNo]+i*5) >= g_PerKwh )
//        {
//            FramWriteFlag = 0x5A;
//            *(&PCnt_Blk.PCnt.VAHRCnt[FeeNo]+i*5) -= g_PerKwh;
//// 33           Energy_checksum(i+12);
//            //EC_INC( (unsigned char*)(&VEngy_Blk.VEngy.VarPp0[0]+22*i), 0, 20, 0 );
//            if( (FeeNo > 0) && ( FeeNo <= MaxFeeNum) )//费率电量累加
//            {
//                if( BCD_Check((&VEngy_Blk.VEngy.VarPp0[0]+i*22+4*FeeNo),4) == 0 )//分时电量
//                {
//                    BCD4_INC( &VEngy_Blk.VEngy.VarPp0[0]+i*22+4*FeeNo );
//                    EC_Check2 = CheckNum( &VEngy_Blk.VEngy.VarPp0[0]+i*22, 20 );//从起始地址开始算校验和
//                    memcpy( &VEngy_Blk.VEngy.VarPp0[0]+i*22+20, (unsigned char*)&EC_Check2, 2 );//更新校验和
//                }
//            }
//        }
//        if( FramWriteFlag == 0x5A )
//        {
//            FramWriteFlag = 0;
////33            MEM_CRCWrite( FM1_EC_Vahr_Pp0+22*i, (unsigned char*)(&VEngy_Blk.VEngy.VarPp0[0]+22*i), 20, Mem_FRAM );
//        }
//        if( E2PWriteFlag == 0x5A )
//        {
//            E2PWriteFlag = 0;
////33            MEM_CRCWrite( E2A_EC_Vahr_Pp0+22*i, (unsigned char*)(&VEngy_Blk.VEngy.VarPp0[0]+22*i), 20, Mem_E2PROMA );
//        }
//    }
}

/************************************************************************
* 函数名 ：//PowerMeasure
* 功能 ：//电量累加
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 
**************************************************************************/
void PowerMeasure(void)
{
    if( PowerReact1_Flag== 0x5A)
    {
        PowerReact1_Flag= 0x00;
        EC_RactGroup( MeasureMode.RactMode1, 0 );
    }
    if( PowerReact2_Flag== 0x5A)
    {
        PowerReact2_Flag= 0x00;
        EC_RactGroup( MeasureMode.RactMode2, 1 );
    }
    if( PowerRead_Flag == 0x5A )
    {
        PowerRead_Flag = 0;
        InstantPowerMeas();//瞬时值
    }
		
    PulseCount();//脉冲累加
		
		if( FLAG_EA != 0 )
       ECMeasure();//电量累加
   else 
       return;
}
