/************************************************************************
* 版权所有 (C)2016, 杭州西力智能科技股份有限公司。
*
* 文件名称： // RN8302Drive.c
* 文件标识： // 
* 内容摘要： //RN8302计量驱动程序
* 其它说明： // 
* 当前版本： // V01.00
* 作 者：   
* 完成日期： // 
*
* 修改记录1：//
* 修改日期： //
* 版 本 号： // V01
* 修 改 人： //
* 修改内容： //
************************************************************************/
//#define  RN8302Drive_H
//#include "RN8302Drive.H"
#include "ex_func.h"
#include "ex_var.h"
/************************************************************************
* 函数名 ：//RN8302_SPI_Init
* 功能 ：//RN8302管脚初始化
* 输入参数 ：//无
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 
**************************************************************************/
void RN8302_SPI_Init(void)
{
    P_RN8302_SCL_H;
	 
    P_RN8302_SDO_H;
    P_RN8302_SCL_PORT;
    P_RN8302_SDO_PORT;
    P_RN8302_SDI_PIN;
}

/************************************************************************
* 函数名 ：//RN8302_Delay
* 功能 ：//延时子程序
* 输入参数 ：//n：延时时间
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//无
* 修改日期 :
* ------------------------------------------------------------------------
* 
**************************************************************************/
void RN8302_Delay( unsigned long n )
{
    unsigned long i;
 
    for( i = 0; i < n; i++ )
    {
       __NOP(); //__NOP(); __NOP(); __NOP(); __NOP();//Clear_Wdt();//清看门狗
    }
}

/************************************************************************
* 函数名 ：//RN8302_SendByte
* 功能 ：//发送一个字节数据或地址
* 输入参数 ：//TxData:待发送的信息
* 输出参数 ：//无
* 返回值说明 ：//无
* 其他说明 ：//主机在CLK高电平时发送SD0信号；从机在低电平时接收数据
* 修改日期 :
* ------------------------------------------------------------------------
* 
**************************************************************************/
void RN8302_SendByte(unsigned char TxData)
{
    unsigned char i;

    Clear_Wdt();//清看门狗
    RN8302_SPI_Init();
    for( i = 0; i < 8; i++ )
    {
        P_RN8302_SCL_H;// CLK=1
        RN8302_Delay(1);
        if( (TxData & 0x80) == 0x80 )
            P_RN8302_SDO_H;// SD0=1
        else
        {
            P_RN8302_SDO_L;// SD0 = 0
        }
        RN8302_Delay(3);
        P_RN8302_SCL_L;// CLK = 0
        RN8302_Delay(3);
        TxData <<= 1;
    }
}

/************************************************************************
* 函数名 ：//RN8302_ReceiveByte
* 功能 ：//接收一个字节数据
* 输入参数 ：//无
* 输出参数 ：//RxData 读取数据
* 返回值说明 ：//无
* 其他说明 ：//从机在CLK高电平时发送SDI信号；主机在低电平时接收数据
* 修改日期 :
* ------------------------------------------------------------------------
*
**************************************************************************/
unsigned char RN8302_ReceiveByte(void)
{
    unsigned char i,RxData=0;
    
    Clear_Wdt();//清看门狗
    RN8302_SPI_Init();
    for( i = 0; i < 8; i++ )
    {
        P_RN8302_SCL_H;// CLK=1
        RN8302_Delay(3);
        P_RN8302_SCL_L;// CLK=0
        RxData <<= 1;
        RN8302_Delay(3);
        if( P_RN8302_SDI_Read )
            RxData |= 0x01;
        RN8302_Delay(3);
    }
    return(RxData);
}

/************************************************************************
* 函数名 ：//RN8302_Read
* 功能 ：//从RN8302里读数据
* 输入参数 ：//RAM_Addr:数据地址,RN8302_Addr:寄存器地址,Lenth:数据长度
* 输出参数 ：//先高后低
* 返回值说明 ：//
* 其他说明 ：//在CS低电平，发送命令
* 修改日期 :
* ------------------------------------------------------------------------
* 
**************************************************************************/
void RN8302_Read( unsigned char *RAM_Addr, unsigned int RN8302_Addr, unsigned char Lenth )
{
    unsigned char i,Temp=0;
    unsigned char chksum=0;
    
    RN8302Status = SUCCESS;
    P_RN8302_CS_L;// CS = 0
    P_RN8302_CS_PORT;
    RN8302_Delay(30);
    Temp = (unsigned char)(RN8302_Addr & 0x00ff);
    chksum = Temp;
    RN8302_SendByte( Temp );//寄存器低地址
    Temp = (((unsigned char)(RN8302_Addr >> 4)) & 0xf0) + 0x00;
    chksum += Temp;
	  RN8302_Delay(30);
    RN8302_SendByte( Temp );//寄存器高地址+读操作
    RN8302_Delay(30);
    for( i = Lenth; i > 0; i-- )
    {
        RAM_Addr[i-1] = RN8302_ReceiveByte(); //先高后低
        chksum += RAM_Addr[i-1];
    }
    chksum = ~chksum;
    Temp = RN8302_ReceiveByte();   
    if( Temp != chksum )//校验码错误，认为是SPI总线错，则继续读SPI校验寄存器 
    {
        RN8302_SendByte( 0x8c );//前次SPI读出数据寄存器低地址
        chksum = 0x8c;
        Temp = (((unsigned char)(0x018C >> 4)) & 0xf0) + 0x00;
        chksum += Temp;
        RN8302_SendByte( Temp );//寄存器高地址+读操作
        for(i = 4 ; i > 0 ; i--)//RData寄存器为4字节
        {
            Temp = RN8302_ReceiveByte();
            if(Lenth >= i)
            {					
                if(Temp != RAM_Addr[i-1]) 
                {
                    RAM_Addr[i-1] = Temp;//替换上次读出数据
                }	
            }
            chksum += Temp;
        }
        chksum = ~chksum;
        Temp = RN8302_ReceiveByte();   
        if( Temp != chksum )//SPI校验寄存器的数据校验码也错，则返回错误，要求复位芯片
        {
            //for( i = 0; i < Lenth; i++ )
            for(i = Lenth ; i > 0 ; i--)
            {
                RAM_Addr[i-1] = 0;//若两者都错，则清缓存器
            }
            RN8302Status = ERROR;
        }
    }
    P_RN8302_CS_H;  //CS = 1
    if( (RN8302Status == ERROR)&&(RN8302Errcum<10) )
        RN8302Errcum++;
}

/************************************************************************
* 函数名 ：//RN8302_Write
* 功能 ：//往RN8302里写数据
* 输入参数 ：//RN8302_Addr:寄存器地址,RAM_Addr:数据地址,Lenth:数据长度
* 输出参数 ：//无
* 返回值说明 ：//0正确
* 其他说明 ：//在CLK上升沿时，发送SD0信号
* 修改日期 :
* ------------------------------------------------------------------------
* 
**************************************************************************/
void RN8302_Write( unsigned int RN8302_Addr, unsigned char *RAM_Addr, unsigned char Lenth )
{
    unsigned char i,Temp,Repeat;
    unsigned char chksum=0;
    
    for( Repeat =10; Repeat != 0 ; Repeat--)		
    {
        RN8302Status = SUCCESS;
        P_RN8302_CS_L;// CS = 0
        P_RN8302_CS_PORT;
        RN8302_Delay(30);
        Temp = (unsigned char)(RN8302_Addr & 0x00ff);
        chksum = Temp;
        RN8302_SendByte( Temp );//寄存器低地址
        Temp = (((unsigned char)(RN8302_Addr >> 4)) & 0xf0) + 0x80;
        chksum += Temp;
        RN8302_SendByte( Temp );//寄存器高地址+写操作
        RN8302_Delay(30);
        for(i = Lenth; i > 0;i-- )
        {
            RN8302_SendByte(RAM_Addr[i-1]);//先高后低传输数据
            chksum += RAM_Addr[i-1];
        }
        chksum = ~chksum;
        RN8302_SendByte( chksum );//校验码
//-------------------读WData寄存器检查-----------------------
        RN8302_SendByte( 0x8D );//前次SPI写入数据寄存器低地址
        chksum = 0x8D;
        Temp = (((unsigned char)(0x018D >> 4)) & 0xf0) + 0x00;
        chksum += Temp;
        RN8302_SendByte( Temp );//寄存器高地址+读操作
        RN8302_Delay(30);
        for(i = 3 ; i > 0 ; i--)//WData为3个字节
        {
            Temp = RN8302_ReceiveByte(); //先高后低
            if(Lenth >= i)
            {					
                if(Temp != RAM_Addr[i-1]) 
                {
                    RN8302Status = ERROR;
                    break;	
                }	
            }
            chksum += Temp;
        }
        if(RN8302Status == SUCCESS) 
        {
            chksum = ~chksum;
            Temp = RN8302_ReceiveByte();   
            if(Temp != chksum)  RN8302Status = ERROR; 
        }
        P_RN8302_CS_H;  //CS = 1
        if( RN8302Status == SUCCESS )
            break; 
				
			RN8302_Delay(100);	
    }
    if( (RN8302Status == ERROR)&&(RN8302Errcum<10) )//如果写入数据与RAM中不一样，则继续写，最多10次
        RN8302Errcum++;
}

