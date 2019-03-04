#ifndef	__DEFINE_H__
#define __DEFINE_H__

//定义常量, 常数
#include "bintohex.h"
#include "V85XX.h"

#define ecode   
#define bdata
#define idata
#define near
#define code    
#define xdata
#define reentrant
#define bit unsigned char
#define huge

//#define EA 0

#define	CARDKEY 	0	//卡座类型：0:常开; 1:常闭
#define LCDTYPE		0	//液晶：0:标准; 3:sld

//从0x811800开始会保存如下信息
#define HXVAR(object, addr)   (*((object ecode *) (addr)))
#define const_tx_flag  		HXVAR( unsigned char, 0x811800 )	//调校标志(0xc3为编程调校)
#define const_rtc_adj_top 	HXVAR( float,         0x811805 )	//顶点误差ppm
#define const_XcapH		HXVAR( unsigned char, 0x81180d )	//调校时使用的电容值
#define const_XcapL		HXVAR( unsigned char, 0x81180e )

#define const_adc_Testq	 	HXVAR( unsigned int , 0x81204e )	//成测时PTAT ADC值(新ADCtrim值下的转换)
#define const_adc_TrimH 	HXVAR( unsigned char, 0x812054 )	//调校时使用的ADCtrim值
#define const_adc_TrimL 	HXVAR( unsigned char, 0x812055 )	//LSB=1.5mV
#define const_adc_OffsetA	HXVAR( unsigned int , 0x812056 )
#define const_adc_TrimBH 	HXVAR( unsigned char, 0x812064 )	//LSB=2.34mV
#define const_adc_TrimBL 	HXVAR( unsigned char, 0x812065 )
#define const_adc_OffsetB 	HXVAR( unsigned int , 0x812066 )
#define const_lvd_40v 		HXVAR( unsigned char, 0x812029 )	//lvd4.0
#define const_lvd_42v 		HXVAR( unsigned char, 0x81202A )	//lvd4.2
#define const_lvd_44v 		HXVAR( unsigned char, 0x81202B )	//lvd4.4
#define const_lvd_46v 		HXVAR( unsigned char, 0x81202C )	//lvd4.6

#define const_temp_TestA 	HXVAR( unsigned char, 0x812090 )	//测试时温度整数位
#define const_temp_TestB 	HXVAR( unsigned char, 0x812091 )	//测试时温度小数位
#define const_adc_Test	 	HXVAR( unsigned int , 0x812092 )	//测试时PTAT ADC值
//#define const_adc_TestH 	HXVAR( unsigned char, 0x812092 )	//测试时PTAT ADC值H
//#define const_adc_TestL 	HXVAR( unsigned char, 0x812093 )	//测试时PTAT ADC值L



#define __NOP                             __nop
//unsigned char GPIO0;

#define RD	0x80
#define WR	0x00
#define	 FM17520				 0x02

//FM17520芯片处理//
#define  PAGE0           0x00
#define  COMMAND         0x01
#define  COMMIEN         0x02
#define  DIVIEN          0x03
#define  COMMIRQ         0x04
#define  DIVIRQ          0x05
#define  REGERROR        0x06
#define  STATUS1         0x07
#define  STATUS2         0x08
#define  FIFODATA        0x09
#define  FIFOLEVEL       0x0A
#define  WATERLEVEL      0x0B
#define  CONTROL         0x0C
#define  BITFRAMING      0x0D
#define  COLL            0x0E
#define  RFU0F           0x0F

#define  PAGE1           0x10
#define  MODE            0x11
#define  TXMODE          0x12
#define  RXMODE          0x13
#define  TXCONTROL       0x14
#define  TXAUTO          0x15
#define  TXSEL           0x16
#define  RXSEL           0x17
#define  RXTRESHOLD      0x18

#define	 MODWIDTH				 0x24
#define  RFCFG           0x26
#define  GSN             0x27
#define  CWGSP					 0x28


#define  IntRegLen  		 13				//初始化的寄存器字节数

//FM11RF08芯片处理//
#define SECTION0  0*4
#define SECTION1  1*4
#define SECTION2  2*4
#define SECTION3  3*4
#define SECTION4  4*4
#define SECTION5  5*4
#define SECTION6  6*4
#define SECTION7  7*4
#define SECTION8  8*4
#define SECTION9  9*4
#define SECTION10  10*4
#define SECTION11  11*4
#define SECTION12  12*4
#define SECTION13  13*4
#define SECTION14  14*4
#define SECTION15  15*4

#define BLOCK0  0
#define BLOCK1  1
#define BLOCK2  2
#define BLOCK3  3


#define ERROR    0
#define SUCCESS  1

#define LCDLEDON 	GPIOB->DAT  |= BIT2
#define LCDLEDOFF 	GPIOB->DAT  &= ~BIT2
#define SHIDUANCTRL_ON	__NOP()//GPIO6 &= B1101_1111
#define SHIDUANCTRL_OFF __NOP()//GPIO6 |= B0010_0000

#define RELAYON1 	GPIOF->DAT  |= BIT1
#define RELAYOFF1 GPIOF->DAT  &= ~BIT1
#define RELAYON2 	GPIOF->DAT  |= BIT0
#define RELAYOFF2 	GPIOF->DAT  &= ~BIT0

#define RELAYON3 	GPIOF->DAT  |= BIT1
#define RELAYOFF3 	GPIOF->DAT  &= ~BIT1

#define PLUSELED_ON	 __NOP()	//预留
#define PLUSELED_OFF __NOP()	//预留

#define EVENOUTOFF __NOP()	//GPIO3 &= B1101_1111		//无事件上报
#define EVENOUTON __NOP()//	GPIO3 |= B0010_0000		//有事件上报

//载波接口定义
#define PLC_RST0 	GPIOE->DAT &= ~BIT9   //GPIO3 &= B1111_0111
#define PLC_RST1 	GPIOE->DAT |= BIT9   //GPIO3 |= B0000_1000

#define BEEPALARMON 	__NOP()//do{if(Beep_Io_Ctrl==0x55)GPIO4 &= B1111_1011;else GPIO5 &= B0111_1111;}while(0)
#define BEEPALARMOFF __NOP()//	do{if(Beep_Io_Ctrl==0x55)GPIO4 |= B0000_0100;else GPIO5 |= B1000_0000;}while(0)

//#define RELAYCTRLON	1//GPIO6 |= B0000_0001
//#define RELAYCTRLOFF	1//GPIO6 &= B1111_1110
#define ESAMPOWEROFF	__NOP()//GPIO7 |= B0000_0001
#define ESAMPOWERON	__NOP()//GPIO7 &= B1111_1110
#define RELAYLEDON	NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn);GPIOB->DAT  &= ~BIT5;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);	//预留//GPIO6 &= B1111_0111
#define RELAYLEDOFF	NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn);GPIOB->DAT |= BIT5;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);	//预留//GPIO6 |= B0000_1000
#define R485CTRLS	__NOP()//GPIO6 &= B1111_1011
#define R485CTRLR	__NOP()//GPIO6 |= B0000_0100

#define RELAYTEST_P	((GPIOE->STS)& BIT3)//(GPIO6&B0000_0010) == B0000_0010
#define RELAYTEST_N	(!((GPIOE->STS)& BIT3))//(GPIO6&B0000_0010) != B0000_0010

#define DISKEY1_P 	((GPIOA->STS)& BIT13)//(GPIO4&B0010_0000) == B0010_0000
#define DISKEY1_N 	(!((GPIOA->STS)& BIT13))//(GPIO4&B0010_0000) != B0010_0000

#define DISKEY_P 	((GPIOA->STS)& BIT12)//(GPIO4&B0010_0000) == B0010_0000
#define DISKEY_N 	(!((GPIOA->STS)& BIT12))//(GPIO4&B0010_0000) != B0010_0000


#define COVKEY_N 	((GPIOA->STS)& BIT7)//(GPIO4&B0100_0000) == B0100_0000
#define COVKEY_P 	(!((GPIOA->STS)& BIT7))//(GPIO4&B0100_0000) != B0100_0000
//#define PROKEY_P	1
//#define PROKEY_N	1
#define RXHW_P		((GPIOB->STS)& BIT0)
#define RXHW_N		(!((GPIOB->STS)& BIT0))

#if(CARDKEY==1)
#define KEY_N		__NOP()
#define KEY_P		__NOP()
#elif(CARDKEY==0)
#define KEY_P		__NOP()
#define KEY_N		__NOP()
#endif

#define RST_ESAM_H	__NOP()
#define RST_ESAM_L	__NOP()
#define RST_CARD_H __NOP()
#define RST_CARD_L __NOP()

#define METER_RST1	__NOP() //NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn); GPIOB->DAT |= BIT6;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);//计量芯片复位脚
#define METER_RST0	__NOP() //NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn); GPIOB->DAT &= ~BIT6;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);

//射频SPI接口定义
#define	RFSPIRST0		NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn);GPIOB->DAT &= ~BIT8;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);
#define	RFSPIRST1		NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn);GPIOB->DAT |= BIT8;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);
#define	RFSPICLK0		GPIOB->DAT &= ~BIT10
#define	RFSPICLK1		GPIOB->DAT |= BIT10
#define	RFSPIMO0		GPIOB->DAT &= ~BIT11
#define	RFSPIMO1		GPIOB->DAT |= BIT11
#define	RFSPICS0		GPIOB->DAT &= ~BIT9
#define	RFSPICS1		GPIOB->DAT |= BIT9
#define	RFSPIMIGPIO		(GPIOB->STS)
//#define	RFSPIMIBIT		GPIO_Pin_2

//IIC接口定义
//#define	P_EeprScl			HF_P6_0
#define	P_EeprScl_Low			{NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn); GPIOB->DAT &= ~BIT13;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);}
#define	P_EeprScl_High		{NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn);GPIOB->DAT |= BIT13;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);}

#define	P_EeprScl_PowLowLow			{GPIOB->DAT &= ~BIT13;}
#define	P_EeprScl_PowLowHigh		{GPIOB->DAT |= BIT13;}

#define	Set_InPut_P_EeprScl		{GPIOB->OEN |=BIT13; GPIOB->IE |=BIT13;}
#define	Set_OutPut_P_EeprScl	{GPIOB->OEN &= ~BIT13;GPIOB->IE &= ~BIT13;}

#define	P_EeprSda			((GPIOB->STS)& BIT14)
#define	P_EeprSda_Low		{NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn);	GPIOB->DAT &= ~BIT14;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);}
#define	P_EeprSda_High	{NVIC_DisableIRQ(PMU_IRQn );NVIC_DisableIRQ(TMR1_IRQn);	GPIOB->DAT |= BIT14;NVIC_EnableIRQ(PMU_IRQn );NVIC_EnableIRQ(TMR1_IRQn);}

#define	P_EeprSda_PowLowLow		{GPIOB->DAT &= ~BIT14;}
#define	P_EeprSda_PowLowHigh	{GPIOB->DAT |= BIT14;}


#define	Set_InPut_P_EeprSda	       { GPIOB->OEN |=BIT14;GPIOB->IE |=BIT14;}
#define	Set_OutPut_P_EeprSda	     { GPIOB->OEN &= ~BIT14;GPIOB->IE &= ~BIT14;}                                  


//-------------------------RN8302-SPI??------------------------//
#define     P_RN8302_CS_PIN         { GPIOC->OEN |=BIT0;GPIOC->IE |=BIT0;} 
#define     P_RN8302_CS_PORT        { GPIOC->OEN &= ~BIT0;GPIOC->IE &= ~BIT0;}  
#define     P_RN8302_CS_H           GPIOC->DAT |= BIT0
#define     P_RN8302_CS_L           GPIOC->DAT &= ~BIT0

#define     P_RN8302_SCL_PIN        { GPIOC->OEN |=BIT1;GPIOC->IE |=BIT1;}
#define     P_RN8302_SCL_PORT      { GPIOC->OEN &= ~BIT1;GPIOC->IE &= ~BIT1;}
#define     P_RN8302_SCL_H          GPIOC->DAT |= BIT1
#define     P_RN8302_SCL_L          GPIOC->DAT &= ~BIT1

#define     P_RN8302_SDI_PIN       { GPIOC->OEN |=BIT2;GPIOC->IE |=BIT2;}
#define     P_RN8302_SDI_PORT       { GPIOC->OEN &= ~BIT2;GPIOC->IE &= ~BIT2;}
#define     P_RN8302_SDI_H         GPIOC->DAT |= BIT2
#define     P_RN8302_SDI_L          GPIOC->DAT &= ~BIT2
#define     P_RN8302_SDI_Read       ((GPIOC->STS)&BIT2)

#define     P_RN8302_SDO_PIN         { GPIOC->OEN |=BIT3;GPIOC->IE |=BIT3;}
#define     P_RN8302_SDO_PORT        { GPIOC->OEN &= ~BIT3;GPIOC->IE &= ~BIT3;}
#define     P_RN8302_SDO_H           GPIOC->DAT |= BIT3
#define     P_RN8302_SDO_L           GPIOC->DAT &= ~BIT3

#define     P_RN8302_INTN_PIN        { GPIOA->OEN |=BIT15;GPIOA->IE |=BIT15;}
#define     P_RN8302_INTN_PORT      { GPIOA->OEN &= ~BIT15;GPIOA->IE &= ~BIT15;}
#define     P_RN8302_INTN_H        GPIOA->DAT |= BIT15
#define     P_RN8302_INTN_L         GPIOA->DAT &= ~BIT15
#define     P_RN8302_INTN_Read       ((GPIOA->STS)&BIT15)

//????
#define     P_RN8302_RST_PIN          __NOP()
#define     P_RN8302_RST_PORT        __NOP()
#define     P_RN8302_RST_H         __NOP()
#define     P_RN8302_RST_L          __NOP()


//-------------------------RN8302-SPI??------------------------//



//;============================EEPROM存储定义==========================================
//------------------自动crc校验区、参数区	
#define		ee_remainmoney_page		0x00		//剩余金额数据页地址
	#define		ee_remainmoney_inpage		0
	#define		ee_remainmoney_lenth		16
#define		ee_esampara_page		0x00		//esam里报警金额到身份认证时效参数页地址
	#define		ee_esampara_inpage		ee_remainmoney_inpage+ee_remainmoney_lenth+4
	#define		ee_esampara_lenth		29	
#define		ee_temperature_page		0x00		//温度补偿系数
	#define		ee_temperature_inpage		ee_esampara_inpage+ee_esampara_lenth+2
	#define		ee_temperature_lenth		11	
				
#define		ee_fwdl_page			0x02		//正向电量数据页地址
	#define		ee_fwdl_inpage			0
	#define		ee_fwdl_lenth			24
#define		ee_bkdl_page			0x02		//反向电量数据页地址
	#define		ee_bkdl_inpage			ee_fwdl_inpage+ee_fwdl_lenth+4
	#define		ee_bkdl_lenth			24
#define		ee_commaddr_page		0x02		//通讯地址页地址
	#define		ee_commaddr_inpage		ee_bkdl_inpage+ee_bkdl_lenth+4
	#define		ee_commaddr_lenth		6	

#define		ee_limitmenory_page		0x04		//透支金额囤积金额和合闸允许金额页地址
	#define		ee_limitmenory_inpage		0
	#define		ee_limitmenory_lenth		12
#define		ee_meterconst_page		0x04		//脉冲常数页地址
	#define		ee_meterconst_inpage		ee_limitmenory_inpage+ee_limitmenory_lenth+2
	#define		ee_meterconst_lenth		4
#define		ee_emupara_page			0x04		//计量芯片参数页地址
	#define		ee_emupara_inpage		ee_meterconst_inpage+ee_meterconst_lenth+2
	#define		ee_emupara_lenth		20
#define		ee_password_page		0x04		//03级和04级密码页地址
	#define		ee_password_inpage		ee_emupara_inpage+ee_emupara_lenth+2
	#define		ee_password4_inpage		ee_emupara_inpage+ee_emupara_lenth+8
	#define		ee_password_lenth		4
//------------------
#define		ee_loop_displayitem_page	0x06		//循环显示项目页地址
#define		ee_loop_displayitem_1_page	0x07		//循环显示项目页地址
#define		ee_key_displayitem_page		0x08		//按键显示项目页地址
#define		ee_key_displayitem_1_page	0x09		//按键显示项目页地址
	#define		ee_displayitem_lenth		62		
#define		ee_tariffprice1_page	0x0a		//当前套费率电价页地址
#define		ee_tariffprice2_page	0x0b		//备用套费率电价页地址
	#define		ee_tariffprice_inpage		0
	#define		ee_tariffprice_lenth		16
#define		ee_event_point_page		0x0a		//冻结和事件指针页地址
	#define		ee_eventpoint_inpage		18
#define		ee_validrec_page		0x0c		//冻结和事件有效指针页
	#define		ee_timingf_inpage		0x00	//00h定时冻结
	#define		ee_dayf_inpage			0x01	//01h日冻结
	#define		ee_histroyf_inpage		0x02	//02h结算电量
	#define		ee_hourf_inpage			0x03	//03h整点冻结
	#define		ee_immedf_inpage		0x04	//瞬时冻结
	#define		ee_agreesqf_inpage		0x05	//约定冻结时区表切换
	#define		ee_agreesdf_inpage		0x06	//约定冻结日时段表切换
	#define		ee_agreeflf_inpage		0x07	//约定冻结费率电价切换
	#define		ee_agreejtf_inpage		0x08	//约定冻结阶梯切换
	#define		ee_jtsqswith_inpage		0x09	//阶梯时区切换
	#define		ee_modeswith_inpage		0x0A	//费控模式切换
	#define		ee_clearn_inpage		0x0B	//清零
	#define		ee_cleareventn_inpage		0x0C	//清事件
	#define		ee_powerdownn_inpage		0x0D	//掉电
	#define		ee_setclockn_inpage		0x0E	//校时
	#define		ee_covern_inpage		0x0F	//开表盖
	#define		ee_maxi_inpage			0x10	//过流
	#define		ee_openrelayn_inpage		0x11	//跳闸
	#define		ee_closerelayn_inpage		0x12	//合闸
	#define		ee_programn_inpage		0x13	//编程
	#define		ee_shiqun_inpage		0x14	//时区表编程
	#define		ee_shiduann_inpage		0x15	//时段表编程
	#define		ee_jieshuann_inpage		0x16	//结算日编程
	#define		ee_keysetn_inpage		0x17	//密钥更新编程
	#define		ee_combn_inpage			0x18	//组合方式编程
	#define		ee_relayerrn_inpage		0x19	//负荷开关误动作
	#define		ee_powererrn_inpage		0x1A	//电源异常
	#define		ee_weekdayn_inpage		0x1B	//周休日编程
	#define		ee_errcardn_inpage		0x1C	//异常插卡	
	#define		ee_batterlow_inpage		0x1D	//电池欠压
	#define		ee_buymoneyn_inpage		0x1E	//购电
	#define		ee_dmoneyn_inpage		0x1F	//退费记录
	#define		ee_flpricen_inpage		0x20	//费率电价编程
	#define		ee_jtpricen_inpage		0x21	//阶梯表编程	
	#define		ee_cleardemand_inpage	0x22	//0dh清需  2018-09-28
	#define		ee_histroyDM_inpage		0x23	//结算需量   2018-09-28
	#define		ee_eventpoint_lenth		0x24	//指针数据长度
#define		ee_curyearjs_page		0x0c		//当前第1~6年结算日
	#define		ee_curyearjs_inpage	0x00+ee_eventpoint_lenth+2
	#define		ee_curyearjs_len		18
	
#define		ee_Demand_Period_page		0x0c		//当前第1~6年结算日
	#define		ee_Demand_Period_inpage	ee_curyearjs_inpage+ee_curyearjs_len+2
	#define		ee_Demand_Period_len		2
	
#define		ee_shiqu1_page			0x0e		//第一套时区页地址
#define		ee_shiqu2_page			0x0f		//第二套时区页地址
	#define		ee_shiqu_lenth			42
//------------------
#define		ee_displaypara_page		0x0a		//显示数据页地址
	#define		ee_displaypara_inpage		54		
	#define		ee_displaypara_lenth		8	
#define		ee_weekholiday_page		0x0e		//周休日和周休日时段表号页地址
	#define		ee_weekholiday_inpage		44
	#define		ee_weekholiday_lenth		18
//------------------		
#define		ee_shiduantable1_1_page		0x10		//第一套第1时段表页地址
#define		ee_shiduantable1_2_page		0x11		//第一套第2时段表页地址
#define		ee_shiduantable1_3_page		0x12		//第一套第3时段表页地址
#define		ee_shiduantable1_4_page		0x13		//第一套第4时段表页地址
#define		ee_shiduantable1_5_page		0x14		//第一套第5时段表页地址
#define		ee_shiduantable1_6_page		0x15		//第一套第6时段表页地址
#define		ee_shiduantable1_7_page		0x16		//第一套第7时段表页地址
#define		ee_shiduantable1_8_page		0x17		//第一套第8时段表页地址
#define		ee_shiduantable2_1_page		0x18		//第二套第1时段表页地址
#define		ee_shiduantable2_2_page		0x19		//第二套第2时段表页地址
#define		ee_shiduantable2_3_page		0x1a		//第二套第3时段表页地址
#define		ee_shiduantable2_4_page		0x1b		//第二套第4时段表页地址
#define		ee_shiduantable2_5_page		0x1c		//第二套第5时段表页地址
#define		ee_shiduantable2_6_page		0x1d		//第二套第6时段表页地址
#define		ee_shiduantable2_7_page		0x1e		//第二套第7时段表页地址
#define		ee_shiduantable2_8_page		0x1f		//第二套第8时段表页地址
	#define		ee_shiduantable_lenth		42
//------------------

#define		ee_keysts_page		0x10		//KEY(8)+crc(2) 2018.8.27
  #define		ee_keysts_inpage		44
  #define		ee_keysts_lenth		17	
	
#define		ee_eventthreshold_page		0x12		//事件判断设定范围页地址
	#define		ee_eventthreshold_inpage	44
	#define		ee_eventthreshold_lenth		6			
#define		ee_histroyparameter_page	0x14		//结算时间参数页地址
	#define		ee_histroyparameter_inpage	44
	#define		ee_histroyparameter_lenth	6
#define		ee_feik_number_page		0x14		//时区时段费率阶梯数页地址
	#define		ee_feiknumber_inpage		ee_histroyparameter_inpage+ee_histroyparameter_lenth+2
	#define		ee_feiknumber_lenth		8				
#define		ee_freezeparameter_page		0x16		//冻结时间参数页地址
	#define		ee_freezeparameter_inpage	44
	#define		ee_freezeparameter_lenth	12	
#define		ee_factorymode_page		0x17		//生产模式控制字(西力特殊要求)	
	#define		ee_factorymoder_inpage		44
	#define		ee_factorymode_lenth		1
#define		ee_pre_dayfreeze_page		0x18		//专门用于上电补存的上一次日冻结时间页地址
	#define		ee_predayfreeze_inpage		44
	#define		ee_predayfreeze_lenth		5			
#define		ee_rtcadj_time_page		0x18		//写常温顶点调校值时时间记录页地址(始终不清)
	#define		ee_rtcadjtime_inpage		ee_predayfreeze_inpage+ee_predayfreeze_lenth+2
	#define		ee_rtcadjtime_lenth		10		
#define		ee_usercard_serial_page		0x19		//卡序列号页地址
	#define		ee_usercard_serial_inpage	44
	#define		ee_usercard_serial_lenth	6		
#define		ee_changecontrol_page		0x1a		//时区时段启动控制字页地址
	#define		ee_changecontrol_inpage		44
	#define		ee_changecontrol_lenth		2
#define 	ee_operatorcode_page		0x1a		//操作者代码页地址
	#define		ee_operatorcode_inpage		ee_changecontrol_inpage+ee_changecontrol_lenth+2
	#define		ee_operatorcode_lenth		4	
#define		ee_keystatus_page		0x1b		//密钥状态页地址
	#define		ee_keystatus_inpage		44
	#define		ee_keystatus_lenth		18	
#define		ee_meterstatus1_page		0x1c		//电表状态字1页地址
	#define		ee_meterstatus1_inpage		44
	#define		ee_meterstatus1_lenth		3
#define		ee_keystatus1_page		0x1c		//密钥状态页1地址
	#define		ee_keystatus1_inpage		ee_meterstatus1_inpage+ee_meterstatus1_lenth+2
	#define		ee_keystatus1_lenth		9	
#define		ee_xtlfstop_time_page		0x1d		//晶振停振起始时间页地址
	#define		ee_xtlfstoptime_inpage		44
	#define		ee_xtlfstoptime_lenth		7		
#define		ee_wdt_page			0x1e		//上电复位和看门狗复位次数页地址
	#define		ee_wdt_inpage			44	
	#define		ee_wdt_lenth			8
#define		ee_newclock_page		0x1e		//新时钟起始时间页地址
	#define		ee_newclock_inpage		ee_wdt_inpage+ee_wdt_lenth+2
	#define		ee_newclock_lenth		7
#define		ee_esam_money_page		0x1f		//esam恢复ee时金额备份页地址
	#define		ee_esam_money_inpage	44
	#define		ee_esam_money_lenth	15	

#define		ee_featureword_page		0x20		//特征模式字页地址
	#define		ee_featureword_inpage		0
	#define		ee_featureword_lenth		30
#define		ee_starttimer_page		0x20		//时区时段费率电价阶和梯电价启动时间页地址
	#define		ee_starttimer_inpage		ee_featureword_inpage+ee_featureword_lenth+2
	#define		ee_starttimer_lenth		30
#define		ee_7051_page			0x22		//计量芯片调校页地址
  #define	ee_7051_lenth				32	
#define		ee_7051_para_page			0x22		//?????????
  #define	ee_7051_para_inpage		ee_7051_lenth+2	
  #define	ee_7051_para_lenth				16	

#define		ee_7051adj_page			0x24		//电压补偿页
#define		ee_reportdata_page		0x26		//停电事件上报数据存储页(状态字1,上报状态字,新增次数和,复位定时器)
	#define		ee_reportdata_lenth		56	
#define		ee_power_down_page		0x27		//掉电页数据页地址
	#define		ee_powerdown_inpage		0
	#define		ee_powerdown_lenth		41	//其中有11字节预留
	#define		ee_loadpoint_lenth		16	
#define		ee_jtshiqu1_page		0x28		//当前套阶梯时区表
#define		ee_jtshiqu2_page		0x29		//备用套阶梯时区表
	#define		ee_jtshiqu_inpage		0
	#define		ee_jtshiqu_len			12	
#define		ee_ptatvalue_page		0x2a		//查表温度ADC页地址
	#define		ee_ptatvalue_inpage		0
	#define		ee_ptatvalue_lenth		42
#define		ee_capvalue_page		0x2b		//查表电容偏置页地址
	#define		ee_capvalue_inpage		0
	#define		ee_capvalue_lenth		42
#define		ee_hearttime_page					0x2a		//心跳间隔时间页地址
	#define		ee_hearttime_inpage			ee_ptatvalue_lenth+2
	#define		ee_hearttime_lenth			1	

#define ee_TOKEN_page	0x2d	//区块首地址  //
  #define		ee_TOKEN_inpage		0
	#define		ee_TOKEN_lenth		50	

#define ee_DayUserINFO_page	0x2e	//区块首地址  //
  #define		ee_UserINFO_inpage		0
	#define		ee_UserINFO_lenth		6	
	
#define ee_MonthUserINFO_page	0x2e	//区块首地址  //
  #define		ee_MonthUserINFO_inpage		8
	#define		ee_MonthUserINFO_lenth		6			
	
#define ee_TIDCNT_page	0x2e	//区块首地址  //
  #define		ee_TIDCNT_inpage		16 
	#define		ee_TIDCNT_lenth		1  //1条数	
	
//------------------非自动crc校验区	
#define		ee_holiday_page			0x30		//节假日时段表页地址,支持60个公共假日(共4页)
#define		ee_holiday_lenth		60									
//------------------用电量计算区
#define		ee_pre1_histroyusedl_page	0x36		//上1-2结算历史用电量页地址	
#define		ee_pre1_mhistroyusedl_page	0x37		//上1-2月历史用电量页地址
	#define		ee_histroyusedl_lenth		16	//数据长度16字节
#define		ee_pre1_yhistroyusedl_page	0x38		//上1-7年历史用电量页地址	
	#define		ee_yhistroyusedl_lenth		56	//数据长度56字节	
//------------------规格区
#define		ee_protocolver_page		0x34		//协议版本号页地址
	#define		ee_protocolver_inpage		48
	#define		ee_protocolver_lenth		16
#define		ee_softver_page			0x35		//软件版本号页地址
	#define		ee_softver_inpage		0
	#define		ee_softver_lenth		32
#define		ee_hardver_page			0x35		//硬件版本号页地址
	#define		ee_hardver_inpage		32
	#define		ee_hardver_lenth		32	
#define		ee_factorynum_page		0x36		//厂家编码页地址
	#define		ee_factorynum_inpage		24
	#define		ee_factorynum_lenth		32
#define		ee_softbk_page			0x36		//软件备案号页地址
	#define		ee_softbk_inpage		56
	#define		ee_softbk_lenth			8
#define		ee_ratedcurr_page		0x37		//额定电流页地址
	#define		ee_ratedcurr_inpage		24
	#define		ee_ratedcurr_lenth		6
#define		ee_maxcurr_page			0x37		//最大电流页地址
	#define		ee_maxcurr_inpage		30
	#define		ee_maxcurr_lenth		6
#define		ee_activeclass_page		0x37		//电表精度等级页地址 
	#define		ee_activeclass_inpage		36
	#define		ee_activeclass_lenth		4	
#define		ee_matertype_page		0x37		//电表型号页地址
	#define		ee_matertype_inpage		40
	#define		ee_matertype_lenth		10						
#define		ee_meterdate_page		0x37		//生产日期
	#define		ee_meterdate_inpage		50
	#define		ee_meterdate_lenth		10			
//------------------本地费控编程记录区	
#define		ee_recordnumber_page		0x39		//事件记录次数页
	#define		ee_recordnumber_lenth		63	//数据长度63字节
#define         ee_maxi_note_page             	0x3a		//电流过载记录页地址
	#define		ee_maxinote_inpage		0
	#define		ee_maxinote_lenth		38	//数据长度38字节
#define		ee_jtpricerecord_page		0x3a		//阶梯表编程记录(发生时间6和操作者代码4)
	#define		ee_jtpricerecord_inpage		ee_maxinote_inpage+ee_maxinote_lenth	
	#define		ee_jtpricerecord_len		10
#define		ee_keysetrecord_page		0x3a		//密钥更新记录页
	#define		ee_keysetrecord_inpage		ee_jtpricerecord_inpage+ee_jtpricerecord_len
	#define		ee_keysetrecord_len		15
#define		ee_batterlow_note_page		0x43		//电池欠压记录页，只有一条记录
	#define		ee_batterlownote_inpage		ee_keysetrecord_inpage
	#define		ee_batterlownote_len		14
#define		ee_powerdown_note_page		0x44		//掉电事件记录页地址
	#define		ee_powerdownnote_inpage		0
	#define		ee_powerdownnote_lenth		28
#define		ee_dmoney_note_page		0x44		//退费事件记录页地址
	#define		ee_dmoneynote_inpage		ee_powerdownnote_inpage+ee_powerdownnote_lenth
	#define		ee_dmoneynote_lenth		19
	
#define		ee_clear_demand_page		0x4e
  #define		ee_clear_demand_inpage		0
	#define		ee_clear_demand_lenth		26

#define		ee_errcardrecord_page		0x58		//异常插卡记录
	#define		ee_errcardrecord_inpage		0
	#define		ee_errcardrecord_len		36	
#define		ee_buydl_note_page		0x58		//购电事件记录页地址
	#define		ee_buydlnote_inpage		ee_errcardrecord_inpage+ee_errcardrecord_len	
	#define		ee_buydlnote_lenth		23	//数据长度23字节	
	
//------------------事件记录区
#define		ee_cleardl_note_page		0x62		//清电量事件页地址
	#define		ee_cleardlnote_inpage		0
	#define		ee_cleardlnote_lenth		18
#define		ee_clearevent_note_page		0x62		//清事件记录页地址
	#define		ee_cleareventknote_inpage	ee_cleardlnote_inpage+ee_cleardlnote_lenth
	#define		ee_cleareventnote_lenth		14
#define		ee_week_note_page			0x62	//周休日编程记录
	#define		ee_week_note_inpage		ee_cleareventknote_inpage+ee_cleareventnote_lenth
	#define		ee_week_note_len		11
#define		ee_powererr_note_page		0x62		//电源异常记录页地址
	#define		ee_powererrnote_inpage		ee_week_note_inpage+ee_week_note_len
	#define		ee_powererrnote_lenth		20	
	
#define		ee_closerelay_note_page		0x6c		//合闸事件记录页地址
	#define		ee_closerelaynote_inpage	0
	#define		ee_closerelaynote_lenth		18
#define		ee_openrelay_note_page		0x6c		//断闸事件记录页地址
	#define		ee_openrelaynote_inpage		ee_closerelaynote_inpage+ee_closerelaynote_lenth		
	#define		ee_openrelaynote_lenth		18	
#define		ee_opencover_note_page		0x6c		//开盖记录页地址
	#define		ee_opencovernote_inpage		ee_openrelaynote_inpage+ee_openrelaynote_lenth
	#define		ee_opencovernote_lenth		28

#define		ee_jieshuanrecord_page		0x76		//结算日编程记录页地址
	#define		ee_jieshuanrecord_inpage	0
	#define		ee_jieshuanrecord_lenth		16	
#define		ee_setclock_note_page		0x76		//设置时钟记录页地址
	#define		ee_setclocknote_inpage		ee_jieshuanrecord_inpage+ee_jieshuanrecord_lenth
	#define		ee_setclocknote_lenth		16
#define		ee_relayerr_note_page		0x76		//负荷开关异常记录页地址
	#define		ee_relayerrnote_inpage		ee_setclocknote_inpage+ee_setclocknote_lenth
	#define		ee_relayerrnote_lenth		29
	
#define		ee_program_note_page		0x80		//编程事件页地址
	#define		ee_programnote_inpage		0
	#define		ee_programnote_lenth		50
	#define		max_eventrecord_p		10	//事件指针最大值(10次)
#define		ee_combstatusrecord_page	0x80		//有功组合状态字记录页地址
	#define		ee_combstatusrecord_inpage	ee_programnote_inpage+ee_programnote_lenth
	#define		ee_combstatusrecord_lenth	11
	
#define		ee_shiqurecord_page		0x8a		//时区表编程记录页地址
	#define		ee_shiqurecord_lenth		57
#define		ee_shiduanrecord_page		0x94		//时段表编程记录页地址
	#define		ee_shiduanrecord_lenth		57
	
//------------------整点冻结记录区
#define         ee_hourfreezedl_page	        0x9e		//整点冻结电量页地址
	#define		ee_hourfreezedl_lenth		13	//数据长度13字节
	#define		max_hourfreeze_p		254	//整点冻结电量指针最大值(254整点)
#define         ee_dayfreezemoney_page	        0x9e		//日冻结金额页地址
	#define		ee_dayfreezemoney_inpage	52
	#define		ee_dayfreezemoney_lenth		10	//数据长度10字节
	
//------------------规格区
#define		ee_meterpos_page		0xdc		//电能表位置信息
	#define		ee_meterpos_inpage		52
	#define		ee_meterpos_lenth		11	
#define		ee_zichannum_page		0xdd		//资产管理编码页地址
	#define		ee_zichannum_inpage		26
	#define		ee_zichannum_lenth		32	
	
//------------------其他冻结记录区
#define		ee_timingfreeze_dl_page		0xde		//定时冻结数据页地址
	#define		max_timingfreeze_p		60	//约定冻结电量指针最大值(60次)
#define		ee_dayfreezedl_page		0x156		//日冻结数据页地址
	#define		ee_dayfreezedl_len		55
	#define		max_dayfreeze_p			62	//日冻结电量指针最大值(62天)
#define		ee_realtimefreeze1_page		0x1d2		//瞬时冻结冻结页地址
#define		ee_appointfreeze1_page		0x1d8		//时区转换约定冻结页地址
#define		ee_appointfreeze2_page		0x1dc		//时段转换约定冻结页地址
#define		ee_appointfreeze3_page		0x1e0		//费率转换约定冻结页地址
#define		ee_appointfreeze4_page		0x1e4		//阶梯转换约定冻结页地址
	#define		ee_freezedl_lenth		48	//数据长度24字节(共48字节存放2页)
#define         ee_histroy_dl_page              0x1e8		//结算日电量页地址
	#define		ee_histroydl_lenth		48	//数据长度24字节(共48字节存放2页)
	#define		max_histroy_p			12	//结算电量指针最大值(12)
	
//******************************************************************
 //BLOCK51	 预付费_ STS 秘钥参数数据索引号//
 #define F01_BLOCK51_ADR_START	0							//区块首地址  //
 #define LEN_BLOCK51_STSKEY_E2P	1					//区块有效数据内容长度  //
 #define OFFSET_BLOCK51_CSCK	1		//区块16bits校验和数据偏移量  //
 #define ADR_BLOCK51_CSCK_E2P	1 //区块16bits校验和物理存储地址  //
 
#define ADR_STSKEY_DATA8	1//电表密钥数据 ,运算后获取,8字节//
#define ADR_STSKEY_KEN		1	//密钥有效期,8bit, //
#define ADR_STSKEY_KT		1//密钥种类, 2bit ,//
#define ADR_STSKEY_KRN	1	//密钥版本号,4bit //
#define ADR_STSKEY_TI		1// 费率索引号,8bit,0~99//	
#define ADR_STSKEY_FACID	1	// sts 注册工厂编号西力0092//	
#define ADR_STSKEY_SGC		1	// sts 注册SGC编号//	


 //BLOCK52	 预付费_最近20天TID记录数据索引号//
 #define F01_BLOCK52_ADR_START	1	//区块首地址  //
 #define LEN_BLOCK52_TIDREC_E2P	1	//区块有效数据内容长度  //
 #define OFFSET_BLOCK52_CSCK	1//区块16bits校验和数据偏移量  //
 #define ADR_BLOCK52_CSCK_E2P	1		//区块16bits校验和物理存储地址  //
 
#define ADR_TIDREC_01 1//最近1次TID记录//


//------------------负荷记录区
#define		ee_ploadrec_page		0xde
	#define		max_ploadrec_p			289
	#define		ee_ploadrec1_inpage		24	//时间(5)+电压(2)+电流(3)+频率(2)
		#define		ee_ploadrec1_len	12
	#define		ee_ploadrec2_inpage     	36	//时间(5)+有功功率(3)
		#define		ee_ploadrec2_len	8
	#define		ee_ploadrec3_inpage		44	//时间(5)+功率因数(2)
		#define		ee_ploadrec3_len	7
	#define		ee_ploadrec4_inpage		51	//时间(5)+正反向总电量(8)
		#define		ee_ploadrec4_len	13

#define		ee_illegalcardnum_page		0x1ff		//非法插卡次数页
	#define		ee_illegalcardnum_inpage	24
	#define		ee_illegalcardnum_len		20

//------------------密钥区
#define		ee_keydata_1_page			0x240		//编号0 的32字节密钥+编号1 的32字节密钥，依次向下每页2组
#define		ee_jtsqswith_note_page		0x250		//阶梯时区切换冻结
	#define		ee_jtsqswith_note_inpage	0
#define		ee_modeswith_note_page		0x254		//费控模式切换冻结
	#define		ee_modeswith_note_inpage	0
#define		ee_flpricerecord_page		0x258	//费率表编程记录
	#define		ee_flpricerecord_inpage		0	
	#define		ee_flpricerecord_len		42			
#define		ee_jtpricerecord2_page		0x300		//阶梯表编程记录
	#define		ee_jtpricerecord2_inpage	0	
	#define		ee_jtpricerecord2_len		70

#define		ee_ladder1_page			0x350		//当前套阶梯表电量/电价，4张表，每页一张表
#define		ee_ladder2_page			0x354		//备用套阶梯表电量/电价，4张表，每页一张表
	#define		ee_ladder_lenth			70	
	
//BLOCK52	 预付费_最近20天TID记录数据索引号//
#define ee_TID_page	            0x360	//区块首地址  //
  #define		ee_TID_inpage		    0
	#define		ee_TID_lenth		    60  //1条数
	
		
#define ee_Pre12monthMon_page	  0x362	//区块首地址  //
  #define		ee_Pre12monthMon_inpage		0
	#define		ee_Pre12monthMon_lenth		72	
	
#define ee_Pre7DayMon_page	0x363	//区块首地址  //
  #define		ee_Pre7DayMon_inpage	0
	#define		ee_Pre7DayMon_lenth		42	


#define		ee_p_demand_page		0x364		//正向需量数据页地址
  #define		ee_p_demand_inpage		0
	#define		ee_pdemand_lenth		40
#define		ee_n_demand_page		0x364		//反向电量数据页地址
  #define		ee_n_demand_inpage		42
	#define		ee_ndemand_lenth		40
	
#define		ee_histroyp_demand_page		0x366		//结算日需量数据页地址,一次两页
  #define		ee_histroyp_demand_inpage		0
	#define		ee_histroydemand_lenth		40	
#define		ee_histroypn_demand_page		0x366		//结算日需量数据页地址,一次两页
  #define		ee_histroypn_demand_inpage		42
	#define		ee_histroydemand_lenth		40
	
	
#define		ee_Chk_RN8302_page		0x380		//	校表参数校验和
  #define		ee_Chk_RN8302_inpage		100		//	校表参数校验和
  #define		ee_Chk_RN8302_lenth		4
	
#define		ee_para1_RN8302_page		0x380		//	校表参数
  #define		ee_para1_RN8302_inpage		0		//	校表参数
  #define		ee_para1_RN8302_lenth		30	
	
#define		ee_para2_RN8302_page		0x380		//	校表参数
  #define		ee_para2_RN8302_inpage		32		//	校表参数
  #define		ee_para2_RN8302_lenth		30	
	
	#define		ee_para3_RN8302_page		0x380		//	校表参数
  #define		ee_para3_RN8302_inpage		64		//	校表参数
  #define		ee_para3_RN8302_lenth		30	
	

	
	
	
	
	
	
	
//清需总次数(3)+发生时刻(6)+操作者代码(4)+清需时正向有功总最大需量和发生时间(8)+清需时反向有功总最大需量和发生时间(8)

#define		max_histroydm_p			12	//结算需量指针最大值(12)	
#define	  max_cleardemand_p   10

//#define		ee_event_demand_page		0x362		//冻结和事件指针页地址
//	#define		ee_event_demand_inpage		74
//	#define		ee_event_demand_lenth		1
//	
//#define		ee_validrecdemand_page		0x362		//冻结和事件有效指针页	
//  #define		ee_validrecdemand_inpage		77
//	#define		ee_validrecdemand_lenth		1


//#define		ee_clear_demand_page		0x372		//清需量记录页地址
//	#define		ee_cleardemand_lenth		29
//;=============================相关常数定义＝==========================================
#define		maxtimes			6		//最大脉冲倍数,96A时,7个脉冲/s
#define		max_maxholiday			60		//最大公共假日数
#define		default_updatemoneytimer	120		//默认钱包更新时间(min),范围必须在15min~240min之间
#define		ms_timer_delay			100	//460ms时标定时,刷新显示
#define		comm_led_timer_delay		5		//5s通讯指示灯
#define		shiduanctrl_timer_delay		20		//80ms时段改变输出定时器延时
#define		max_shiduancount		14		//最大时段14字节时段表
#define		default_loopdisplaytimer	5		//默认的循显时间5s
#define		default_keydisplaytimer		60		//默认按键显示时间为60s
#define		lcdled_timer_delay		60		//60s背光亮
#define	 	disp_num			33		//显示列表中的项目数

#define 	ESAM				0		//esam
#define 	CARD				1		//IC card
#define 	u7816_buf_size			220		//

//;=============================相关常数定义======================
#define   STS_MAIN_CLASS_PURCHASE   0X00     //购电类TOKEN
//  main_class= 0  sub_classs define 
#define   STS0_SUB_CLASS_PURCHASE_KWH       0X00     //purchase  by KWH
#define   STS0_SUB_CLASS_PURCHASE_MONEY 0X04     //purchase  by MONEY

////////////////////////////////////////////////////////
//main_class= 1 相关 测试类
////////////////////////////////////////////////////////
#define   STS_MAIN_CLASS_TEST   0X01     //测试TOKEN
#define   STS1_SUB_CLASS_TEST  0X00     //  main_class= 1  sub_classs define 

//  main_class= 1  sub_classs=0     STS TEST  CODE define
#define   STS_TEST_ALL                  0   //test all the contents
#define   STS_TEST_RELAY                1   ///test relay 
#define   STS_TEST_LCD                  2   //test LCD display
#define   STS_TEST_TOTAL_ENERGY         3   //Display total energy
#define   STS_TEST_KRN       			4   //Display KRN
#define   STS_TEST_TI         			5   //display TI
#define   STS_TEST_REV_6        		6   //保留
#define   STS_TEST_MPL        			7   //display maximum power limit
#define   STS_TEST_TAMPER_STATUS		8   //display tamper status
#define   STS_TEST_CURRENT_POWER		9   //display power consumption
#define   STS_TEST_METER_VERSION		10  //display meter version number

////////////////////////////////////////////////////////
//main_class= 2 相关 设置类
////////////////////////////////////////////////////////
#define   STS_MAIN_CLASS_SET        0X02     //各类设置TOKEN
//  main_class= 2  sub_classs define 
#define   STS2_SUB_CLASS_SET_MAXPOWER           0    //set up Max Power Limit Mode Token
#define   STS2_SUB_CLASS_CLR_BALANCE            1     //To clear balance(清除余额)
#define   STS2_SUB_CLASS_SET_SINGLE_TARRIF      2     //To set up single tarrif
#define   STS2_SUB_CLASS_SET_SECRET_KEY_H       3     //key change Token -1
#define   STS2_SUB_CLASS_SET_SECRET_KEY_L       4     //key change Token -2
#define   STS2_SUB_CLASS_CLR_TAMPER_EVEN        5     //To clear event

////////////////////////////////////////////////////////
//TID记录参数
////////////////////////////////////////////////////////
#define TID_DATA_LEN  	3		//TID记录数据长度3字节  //
#define TID_REC_MAXNUM  20		//TID记录数据50条//

// 2串密钥的间隔时间定义，单位秒 //
#define MAX_SECONDS_OF_MULTIPLE_TOKEN_INTERVAL		90    // 90秒 //   

//gs_APDU_analyze_var.err_flg 解析错误字定义//
#define	STS_TOKEN_OK					0	//代表token解析结果OK//
#define	STS_TOKEN_REJRCT_1				31	//解密失败，可能是秘钥错误//				
#define	STS_TOKEN_REJRCT_2				32	//不支持的TOKEN类//	
#define	STS_TOKEN_REJRCT_3				33	//DDTK秘钥，不支持充值TOKEN//	
#define	STS_TOKEN_REJRCT_4				34	//解密成功，但是TOKEN内的数据超出范围//	
#define	STS_TOKEN_REJRCT_5				35	//秘钥替换权限不足，其他秘钥禁止修改DITK秘钥//	
#define	STS_TOKEN_REJRCT_6				36	//TID超出秘钥有效期//	
#define	STS_TOKEN_REJRCT_USED			37	//TOKEN已经被使用过//	
#define	STS_TOKEN_REJRCT_OLD			38	//TOKEN已过期（当前TOKEN中TID<记录内最近1次TID）//	
#define	STS_TOKEN_REJRCT_OVER_CRD		39	//充值解析正确但是超出囤积上限//	
#define	STS_TOKEN_CHG_KEY_PLS_2nd		40	//秘钥更改第1串解析成功，等待输入第2串//	
#define	STS_TOKEN_CHG_KEY_PLS_1st		41	//秘钥更改第2串解析成功，等待输入第1串//	

//sts_kay_change_data.flg define 
#define KeyChangeHO BIT0
#define KeyChangeLO BIT1

//LineV 选择相电压测量频率
#define	    F_LineA	  0x01//以A相电压测量为基准标志
#define	    F_LineB	  0x02//以B相电压测量为基准标志
#define	    F_LineC	  0x04//以C相电压测量为基准标志
#define	    F_LineV	  0x07

//ECDir
#define	    F_ActRevZ	  0x01//有功总功率反向
#define	    F_ActRevA	  0x02//A相有功反向
#define	    F_ActRevB	  0x04//B相有功反向
#define	    F_ActRevC	  0x08//C相有功反向
#define	    F_RActRevZ	  0x10//无功总功率反向
#define	    F_RActRevA	  0x20//A相无功反向
#define	    F_RActRevB	  0x40//B相有功反向
#define	    F_RActRevC	  0x80//C相有功反向


//Run
#define	    F_Disp        0x01//????????
#define	    F_Fist	  0x02
#define	    F_Time	  0x04//?????
#define	    F_RevZP0	  0x08
#define	    F_RevZP	  0x10
#define	    F_ZBRest	  0x20//??????

//ECDir1
#define	    F_RevDmdStar  0x01//功率状态改变标志1
#define	    F_RevDmdStar1 0x02//功率状态改变标志2
#define	    F_RevDmdStar2 0x04//功率状态改变标志3
#define	    F_RevDmdStar3 0x08//功率状态改变标志4
#define	    F_RevDmdStar4 0x10//功率状态改变标志5
#define	    F_RevDmdStar5 0x20//功率状态改变标志6
#define	    F_RevDmdStar_Z 0x40//功率状态改变标志7

//ActMode组合有功模式字
#define	    F_IncPp  	  0x01//加正向有功
#define	    F_DecPp       0x02//减正向有功
#define	    F_IncPn       0x04//加反向有功
#define	    F_DecPn       0x08//减反向有功

//RactMode组合无功模式字
#define	    F_IncQuadr1   0x01//加1象限无功
#define	    F_DecQuadr1   0x02//减1象限无功
#define	    F_IncQuadr2   0x04//加2象限无功
#define	    F_DecQuadr2   0x08//减2象限无功
#define	    F_IncQuadr3   0x10//加3象限无功
#define	    F_DecQuadr3   0x20//减3象限无功
#define	    F_IncQuadr4   0x40//加4象限无功
#define	    F_DecQuadr4   0x80//减4象限无功


//;==============================变量类型定义＝============================================
typedef unsigned char	uint08; 		//unsigned 8 bit data
typedef unsigned short	uint16;
typedef	unsigned long int uint32;


typedef signed char	int08; 			//unsigned 8 bit data
typedef signed short	int16;
typedef	signed long int	int32;

typedef unsigned char INT8U;
typedef unsigned short	INT16U;
typedef	unsigned long int INT32U;


typedef	signed	short	INT16S;
typedef signed	long	INT32S;

typedef	unsigned char  BOOLEAN;
typedef	unsigned char  BOOL;
typedef	unsigned char  ErrorStatus; 

typedef	unsigned char  uchar;
typedef	unsigned char  uint8;
typedef	unsigned char  u8;
typedef	signed char	INT8S;
typedef	signed char	schar;



typedef	unsigned 	short	uint;
typedef	unsigned 	short	uint16;
typedef	unsigned 	short	u16;
typedef	signed	short	INT16S;
typedef	signed	short	sint;



#pragma pack(push,1)
typedef union
{
  unsigned char B08;
  struct
  {
    unsigned char bit0:1;
    unsigned char bit1:1;
    unsigned char bit2:1;
    unsigned char bit3:1;
    unsigned char bit4:1;
    unsigned char bit5:1;
    unsigned char bit6:1;
    unsigned char bit7:1;
  }Bit;
}B08_Bit;



union Union_WordDef
{
	unsigned short WORD;
	unsigned char BYTE[2];
};
union Union_DWordDef
{
	signed long   DWORD;
	unsigned long  UDWORD;
	unsigned short WORD[2];
	unsigned char BYTE[4];
};

union B16_B08
{
	unsigned char B08[2];
	uint16  B16;
};

union S16_B08
{
	unsigned char B08[2];
	int16  S16;
};

union B32_B08
{
	unsigned char     B08[4];
	unsigned long int B32;
};
union B32_B08_2						//小端存储(51为大端)
{
	unsigned long int B32;
	unsigned char     B08[4];
};

union B16_B08_2						//小端存储(51为大端)
{
	unsigned short	B16;
	unsigned char     B08[2];
};

struct B32ADDB08
{
	union B32_B08     E32;
	uint08		  B08;
};

struct MONEY_FRM
{
	uint08		Remain[6];			//剩余金额
	uint08		Tick[6];			//赊欠金额
	union B32_B08	Buy_Times;			//购电次数
	union B16_B08	Crc16;				//crc16
};

struct USERINFOR_FRM
{
	uint08	Day_Moneyused[6];			//上一天使用金额 BCD		
  union B16_B08	DayCrc16;	 
	uint08	Month_Moneyused[6];			//上一天使用金额 BCD		
	union B16_B08	MonthCrc16;				//crc16
	union B32_B08		Day_Moneyviable;			//剩余金额可用天数BCD
};

struct FWDL_FRM
{
	union B32_B08	Fwdl_T;				//正向总电量
	union B32_B08	Fwdl[4];			//费率1~4正向电量
	union B32_B08	Usedl;				//本月正向用电量
	union B16_B08	Crc16;				//crc
};

struct BKDL_FRM
{
	union B32_B08	Bkdl_T;				//反向总电量
	union B32_B08	Bkdl[4];			//费率1~4反向电量
	union B32_B08	Usedl;				//本月反向用电量
	union B16_B08	Crc16;				//crc	
};


struct LOADREC_FRM
{
	union B16_B08	Validpoint[4];			//有效指针
	union B16_B08	Point[4];			//查找指针
	union B16_B08 	Crc16;				//CRC校验码
};


struct METERCONST_FRM
{
	union B32_B08	Const;				//脉冲常数bcd码,4字节,高字节在前
	union B16_B08 	Crc16;				//CRC校验码
	uint08		Const_H;			//0.01kwh的脉冲数
};

struct PARA_ESAM_FRM
{
	union B32_B08	Alarm_Limit1;			//报警金额1
	union B32_B08	Alarm_Limit2;			//报警金额2
	uint08		Current_Ratio[3];		//电流变比
	uint08		Voltage_Ratio[3];		//电压变比
	uint08		Meter_Id[6];			//表号
	uint08		User_Id[6];			//用户号
	uint08		Card_Type;			//卡类型
	union B16_B08	Identity_Delay;			//身份认证时效
	union B16_B08 	Crc16;				//CRC校验码
};

struct PARA_LIMITMENORY_FRM
{
	union B32_B08	Tick;				//透支金额
	union B32_B08	Regrate;			//囤积金额
	union B32_B08	Closerelay;			//闭合继电器允许金额
	union B16_B08 	Crc16;				//CRC校验码
};

struct PARA_DISPLAY_FRM
{
	uint08		S_Number;			//显示屏数
	uint08		Loop_Time;			//循显时间
	uint08		Dl_Dot;				//电量小数位数
	uint08		P_Dot;				//功率小数位数
	uint08		K_Number;			//按键显示屏数
	uint08		Vthreshold;			//电池欠压阀值
	uint08		MeterCtrl;			//费控模式字
	uint08		A_Time;				//上电全显时间
	union B16_B08 	Crc16;				//CRC校验码
};

struct PARA_DEMAND_FRM
{	
	uint08		Max_Demand_Period;		//最大需量周期
	uint08		Sliding_Time;			    //滑差时间	
	union B16_B08 	Crc16;				  //CRC校验码
};

struct COMM_ADDRESS_FRM
{
	uint08		Comm_Id[6];			      //通讯地址
	union B16_B08 	Crc16;				  //CRC校验码
};

struct RTC_TRIMA_FRM
{
	union B16_B08	PTAT_Value[21];		//温度ADC
	union B16_B08 	Crc16;				  //CRC校验码
};

struct RTC_TRIMC_FRM
{
	union S16_B08	CAP_Value[21];		//电容偏置
	union B16_B08 	Crc16;				  //CRC校验码
};

struct STARTTIME_FRM
{
	uint08		Shiqu[5];			//新时区启动时间
	uint08		Shiduan[5];			//新时段表启动时间
	uint08		Rate[5];			//新费率电价启动时间
	uint08		Ladder[5];			//新阶梯电价启动时间
	uint08		LadSQ[5];			//阶梯时区切换时间
	uint08		FKmode[5];			//费控模式切换时间(预留)
	union B16_B08 	Crc16;				//CRC校验码
};

struct EMU_FRM
{
	union B32_B08	rate[5];
//	union B32_B08	Uxxx;				//电压系数
//	union B32_B08	I1xxx;				//电流1系数
//	union B32_B08	I2xxx;				//电流2系数
//	union B32_B08   P1xxx;				//功率系数
//	union B32_B08 	Fxxx;				//BL频率系数
	union B16_B08 	Crc16;				//CRC校验码
};


struct WEEK_FRM
{
	uint08		Status;				//周休日
	uint08		Shiduan_N;			//时段表号(范围0~7)	
	uint08		Lstarttimer[5];			//负荷记录起始时间,内部扩展年,共5字节(年在前)
	uint16		Lspacetimer[4];			//第1~4类负荷记录间隔时间,共8字节(高字节在前)
	union B16_B08	Max_I;				//过流事件电流触发下限    高位在前 符合判断
	uint08		Max_I_Period;			//过流事件电流延时时间							
	union B16_B08 	Crc16;				//CRC校验码	
};


struct PARA_FEATUREWORD_FRM
{
	uint08		Bps[1];				//通信口1
	uint08		Comp_B;				//有功组合字
	uint08		Freeze[6];			//负荷/定时/瞬时/约定/整点/日冻结模式字		
	uint08		Feature1;			//特征字1
	uint08		Report[8];			//上报模式字
	uint08		Limit_I[3];			//继电器拉闸控制电流门限值 
	uint08		Exp_Timer[6];			//扩展分钟定时bcd码,分别为拉闸延时2,大电流拉闸延时2,红外认证时间1,上报自动复位时间1
	uint08		Wz_Clocerelay;			//继电器合闸允许方式(0x55内置表示自动合闸,0x55外置表示手动合闸)
	uint08		Plc;				//扩展模式字,0x0A或者0xAA每天20:00复位载波一次(主要针对瑞斯康载波),0xA0或者0xAA反向点亮背光
	uint08		Other;				//预留
	uint08		Zbps[1];			//载波通信口
	union B16_B08 	Crc16;				//CRC校验码
};


struct TEMPERATURE_FRM
{
	uint08		Offset[8];			//温度调整系数8字节
	uint08		RtcTMode;			//RTC补偿工作模式，0x55:测试模式，0xAA常温查表插值模式，其他：普通模式
	union B16_B08	XLcap;				//晶体匹配电容
	union B16_B08 	Crc16;				//CRC校验码
};


struct PRICE_FRM
{
	union B32_B08	Ladder_Dl[6];			//当前阶梯电量
	union B32_B08	Ladder[7];			//当前阶梯电价
	uint08		Year_Ladder[18];		//当前年阶梯结算日1~6
	union B16_B08 	Crc16a;				//CRC校验码
	union B32_B08	Tariff[4];			//当前费率电价表
	union B16_B08 	Crc16c;				//CRC校验码
	union B32_B08   Current_L;			//当前阶梯电价
	union B32_B08   Current_F;			//当前费率电价
	union B32_B08   Current;			//当前电价
	union B32_B08   Current_X;			//当前电价乘以变比后电价
	uint08		Ladder_Mode;		//结算模式字,0x00为月结算,0x55为年结算
};

struct TARIFF_FRM
{
	uint08		Table[42];			//当前费率时段表,3*14＝42字节
	union B16_B08 	Crc16;				//CRC校验码
	uint08		Current;			//当前费率(范围0~3)
	uint08		Current_Ladder;			//当前阶梯(范围0~4)
	uint08		Current_N;			//当前时段表号(范围0~7)
	uint08		Period_N;			//当前时段号(范围0~13)
	uint08		CurLad_N;			//当前阶梯表号(范围0~3)
	uint08		Status;				//bit3时区/bit2时段表/bit1费率/bit0阶梯(0:第一套；1:第二套)
		
};


struct ITEM_FRM
{
	uint08		 Display[62];			//显示项目1*62存放在1页,共4页
	union B16_B08 	 Crc16;
};

struct SYS_CLOCK_FRM
{
	uint08   	Clockmain[7];			//系统时钟,[0]秒[1]分[2]时[3]周[4]日[5]月[6]年
	uint08   	Clockbackup[7];			//系统时钟,[0]秒[1]分[2]时[3]日[4]月[5]年[6]周
//	uint08   	Status;				//时钟状态字
};

struct UIP_FRM
{
	union B32_B08   U;				//电压xxxxxxx.x
	union B32_B08   I1;				//电流xxxxx.xxx
	union B32_B08   P1;				//有功功率xxxx.xxxx
	union B32_B08   I2;				//零线电流xxxxx.xxx
  union B32_B08   P2;				//视在功率xxxx.xxxx	
	union B32_B08   Cosa;				//功率因数xxxxx.xxx	
	union B32_B08   Freq;				//电网频率
	
	union B32_B08   Pav;				//一分钟平均功率xxxx.xxxx
};

struct FUNCTION_FRM
{
	union B16_B08   Temper_M;			//当前电表温度xxx.x
	union B16_B08   Voltage_B;			//当前电池电压xx.xx
	union B32_B08   Voltage_B_T;			//电池电压工作时间xxxxxxxx(hex保存)
	union B16_B08 	Crc16;				//
};

struct TIMER_FRM
{
	uint08		Timer;				//定时器
	uint08		Status;				//状态
};

struct DECIMAL_FRM
{
	uint08		Fw[1];	//正向脉冲尾数
	uint08		Bk[1];	//反向脉冲尾数
	uint08		Unit_Pluse;		//单位正向电量累计(0.01kwh的倍数)
	uint08		Unit_Bkpluse;	//单位反向电量累计(0.01kwh的倍数)
	uint08		Status;			//bit0整度更新;bit1金额变化;bit2钱包更新,其他预留
};

struct DISPALY_FRM
{
	uint08		Number;				//显示号
	uint08		Timer;				//显示时间
	uint08		Status;				//状态
};

struct EEPROM_FRM
{
	uint08		Buffer[64];			//数据ee处理缓冲/lcd显示缓冲复用
	uint08		Status;				//状态字节
};

struct HisEvent_STR
{
	uint16 	        DI21;	
	uint16  	page;
	unsigned char  	offset;
	unsigned char  	len;
	unsigned char  	offset2;
};

struct ADCDATA_FRM
{
	uint16		Buffer[16];			//ADC结果
	uint08		ReadCount;			//计数
};

struct FEIK_FRM
{
	unsigned char Num[8];
	union B16_B08 Crc16;
};

struct TIMER2_FRM
{
	//0非法插卡次数(4)
	//1过流持续时间(4)
	//2电池欠压总累计时间(4)
	//3掉电总累计时间(4)
	//4电池工作时间(4) 
	union B32_B08	Times[5];
};

//定义通信超时时间常数
#define txtimeout_max	125				//500ms，串口字符间最大延时
#define rxtimeout_max	125  				//500ms 
#define framedelay_mx	15 				//60ms
//通信缓冲区最大长度定义
#define	utx_buf_size	230				//通信缓冲区最大长度＝220
#define	urx_buf_size	230				//通信缓冲区最大长度＝220

struct COMMUNICATION_FRM
{
	uint08 Buf[ urx_buf_size ];			//接收缓冲/发送缓冲
	uint08 Len;					//接收长度字节/发生长度
	uint08 Total_Len;				//接收总长度/发送总长度
	uint08 Step;  					//接收步骤
	uint08 Flag;  					//通讯标志
	uint08 Response_Timer;				//通讯响应定时器
	uint08 Overtime_Timer;				//通讯接收或者发生超时定时器
};

typedef struct 
{
    INT8U   apdu_data_bcd[10]; 	//原始20位TOKEN数据//
    INT8U   apdu_data_hex[9]; 	//转换后的9字节数据 //
    INT8U   main_class;			//主类 //
    INT8U   sub_class;			//子类//        
	  INT32U  u32_TID;			//TID//     
	  INT32U  u32_PropData;		//传递数据//   	
	  INT8U	u8_result;			//解析结果  //
}APDU_ANALYZE_VAR;

typedef struct 
{
    INT8U   secret_key[8];  //电表密钥数据 ,运算后获取,8字节//
    INT8U   ken;			//密钥有效期,8bit, //
    INT8U   kt;				//密钥种类, 2bit ,//
    INT8U   krn;			//密钥版本号,4bit //
	  INT8U	ti;				// 费率索引号,8bit,0~99//	
	  INT8U	id_factory[2];	// 工厂编号//		
	  INT8U	u24_SGC[3];		// SGC编号//		
    INT16U  u16_csck;               //16位累加和校验值//	
}APDU_KEY_EXRAM_VAR;

typedef struct  
{
    INT8U   kay[8];                	//电表密钥数据  //
    INT8U   ken;					//密钥有效期 //
    INT8U   kt;                    	//密钥种类 //
    INT8U   krn;					//密钥版本号 //
    INT8U   ro;						//滚动密钥标识 //
	  INT8U	ti;						// 费率索引号//
	  INT8U	flg;					//第一串密钥有效标识//
	  INT16U   tmr_second;             // 第一串密钥有效的秒时间计数器 //
}STS_KAY_CHANHE_DATA;

typedef struct 
{
    INT8U   hex24_tid[TID_REC_MAXNUM][TID_DATA_LEN];  //TID记录，最近20条//
    INT16U  u16_csck;			//16位累加和校验值//	
}APDU_TID_REC;

//电表计量参数 增加结构体成员时需要修改数据长度
typedef struct//添加成员时注意结构体对齐
{
    unsigned short  Un150;
    unsigned short  Un90;
    unsigned short  Un75;
    unsigned short  Un68;
    unsigned short  Un60;
    unsigned short  Un40;
    unsigned long   StartP;//启动功率
    unsigned long   StartI;//启动电流
    unsigned long   StartP2;//启动功率2
    unsigned long   StartI2;//启动电流
    unsigned long   Ib05;//0.05Ib
    unsigned long   Ib30;//0.3Ib
    unsigned long   Ib350;//3.5Ib
    unsigned long   Imax120;//1.2Imax
    unsigned long   DmdCp;
} MEASPARA;

#define  MeasParaLen  sizeof(MEASPARA)
//瞬时量定义
typedef struct
{
    unsigned short U[3];    //XXX.X A、B、C相电压 BCD
    unsigned short PF[4];   //X.XXX 总、A、B、C相功率因素 BCD
    unsigned long  I[3];    //XXX.XXX A、B、C相电流 BCD
    unsigned long  PKva[4]; //XX.XXXX 总、A、B、C相视在功率 BCD
    unsigned long  PKw[4];  //XX.XXXX 总、A、B、C相有功功率 BCD
    unsigned long  PKvar[4];//XX.XXXX 总、A、B、C相无功功率 BCD
    unsigned short Freq;    //电网频率
    unsigned short PhaseAngle[3];//A、B、C相角
    unsigned short ComBatU;//停抄电池电压
    unsigned short RTCBatU;//时钟电池电压
    unsigned short Temp;//表内温度
} REAL;


//校表瞬时量结构体定义
typedef struct
{
    unsigned long  UAdj[3]; //XXXX.XXXX A、B、C相电压 BCD
    unsigned long  IAdj[3]; //XX.XXXX A、B、C相电流 BCD
    unsigned long  PAdj[4]; //XX.XXXXXX 总、A、B、C相有功功率 BCD
    unsigned long  RAdj[4]; //XX.XXXXXX 总、A、B、C相无功功率 BCD
} REALADJ;


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




//标识 增加结构体成员时需要修改数据长度
typedef struct
{
    unsigned char  Error;
    unsigned char  MeterErrAlarm;
    unsigned char  EvenErrAlarm;
    unsigned char  Key;
    unsigned char  Card;
    unsigned char  DispKey;
    unsigned short Power;
    unsigned char  ResetReport;//复位主动上报标志
    unsigned char  CardErr;
    unsigned char  RemoteCmd;
    unsigned char  Run;
    unsigned char  Disp;
    unsigned char  Clock;
    unsigned short Comm;
    unsigned short CommDisp;
    unsigned char  LineV;//频率测量相电压选择
    unsigned char  ECDir;
    unsigned char  ECDir1;//视在功率标志
    unsigned short Relay;
    unsigned char  RelayChk;//继电器状态检测
    unsigned char  MeterState;//电表状态
    unsigned char  MeterState1;//电表状态1
    unsigned char  Alarm;
    unsigned short MeterRun1;
    unsigned short MeterRun2;
    unsigned short MeterRun3;
    unsigned short MeterRun4;
    unsigned short MeterRun5;
    unsigned short MeterRun6;
    unsigned short MeterRun7;
    unsigned char  Fee;
    unsigned char  MulPortMode;
    unsigned char  LoadMode;//负荷记录模式字标志
    unsigned char  LoadTime;//通道时间间隔标志
    unsigned char  FrzType;//冻结标志
    unsigned char  FrzType1;//冻结标志
    unsigned char  BalanRequest;//结算标志
    unsigned char  DmdSave;//最大需量存储
    unsigned char  EvenLostV;//失压标志
    unsigned char  EvenLowV;//欠压标志
    unsigned char  EvenOverV;//过压标志
    unsigned char  EvenLossP;//断相标志
    unsigned char  EvenUnbla;//电压、电流逆向序、不平衡标志
    unsigned char  EvenLostI;//失流标志
    unsigned char  EvenOverI;//过流标志
    unsigned char  EvenDuanI;//断流标志
    unsigned char  EvenRevP;//功率反向标志
    unsigned char  EvenOverP;//过载标志
    unsigned char  EvenOverDmdA;//有功需量超限标志
    unsigned char  EvenOverDmdR;//无功需量超限标志
    unsigned char  EletPar;//开表盖、开端钮盖标志
    unsigned char  EleIYZBla;//电流严重不平衡标志
    unsigned char  EvenLosPower;//辅助电源失电标志
    unsigned char  AlllostV;
    unsigned char  AllLostVEecF;
    unsigned char  RecordDelF;
    unsigned char  RecordDelTim;
    unsigned char  LowPower;
    unsigned char  DataSave;
    unsigned char  MeterPowerState;//电表掉电前的状态：时钟电池、停抄电池、开表盖、开端盖、合闸、拉闸
    unsigned char  RevAlarm;
    unsigned char  SymbolGprs;//无线通信信号强度
    unsigned short PhOfset0;//相位修正
    unsigned short PhOfset1;//相位修正
    unsigned long  LedAlarmEn;//报警光使能开关
    unsigned long  BellAlarmEn;//报警蜂鸣器使能开关
    unsigned long  ExternAlarmEn;//外置声报警使能开关
    unsigned long  AlarmEn0;
    unsigned long  AlarmEn1;
    unsigned long  AlarmEn2;
    unsigned long  AlarmEn3;
    unsigned long  AlarmEn00;
    unsigned long  AlarmEn11;
    unsigned long  AlarmEn22;
    unsigned long  AlarmEn33;
    unsigned short  MinFrzType; //冻结标志最多16通道

    unsigned char  BalanEngNum;//电量补冻结总数
    unsigned char  BalanDmdNum;//需量补冻结总数
    unsigned char  BalanDmdClr;//结算后清需量标志
    unsigned char  MonitorNum;//补电压合格率数
    unsigned char  BalanceEng1_Num;//过结算日1的冻结总数
    unsigned char  EsamState;//当前密钥状态
    unsigned char  EsamChg;//密钥改变
    unsigned char  TimeChgBill;
    unsigned char  TimeChgFrz;
    unsigned char  BalanceEng1_MDNum;//过结算日1的冻结总数需量日冻结用
    unsigned char  BalanceEng1_MDNum2;//过结算日1的冻结总数需量结算用
    unsigned char  NewEvents;//磁场干扰、电源异常、负荷开关误动作标志
    unsigned char  RunCharact;//电表运行特征字
    unsigned char  EvenOverPF;//功率因数超限
    unsigned long  Key_State;//密钥状态字
    unsigned long  Key_LastState;//上次密钥状态字
    unsigned short CardState;//IC卡操作状态标志
    unsigned char  EsamStateErr;
    unsigned char  AAAAstate;
    unsigned char  LoadState;//直接补负荷记录标志
    unsigned char  Addr99;//表号地址
    unsigned short ProgramState;//编程状态标志  
    unsigned char  BalanBDong;//结算时掉电上电补结算
    unsigned char  RemoteRelayAllow;//运行合闸命令
    unsigned long  CpuCard;//CPU卡标志
    unsigned char  Eng_Moneystate; //能量金额变化标志    2016-11-23
    unsigned char  EvenBatLow;//电池欠压标志

    unsigned short Comm698;
    unsigned char ProtectState;//保电状态标志
    unsigned char RelayOutState;//继电器输出状态：bit0～bit7分别按顺序对位表示1～8号继电器遥控跳闸输出状态，置“1”：跳闸状态；置“0”：合闸状态。
    unsigned char RelayAlarmState;//告警状态：bit0～bit7分别按顺序对位表示1～8号继电器遥控告警输出状态，置“1”：处于告警状态；置“0”：未处于告警状态。
    unsigned char RelayCmdState;//继电器命令状态：bit0～bit7分别按顺序对位表示1～8号继电器遥控跳闸命令输出状态，置“1”：跳闸命令；置“0”：合闸命令。
    unsigned char AlarmOutState;

} FLAG;

#define  FlagLen  sizeof(FLAG)


//计数器 增加结构体成员时需要修改数据长度
typedef struct
{
    unsigned long  Authen_Tmr;//身份认证有效时间
    //unsigned long  TerminalAuthen_Tmr;//交互终端认证有效时间    //2016-11-21交互终端增加
    unsigned long  InfAuthen_Tmr;//红外身份认证有效时间 
    unsigned long   Monitor_Tmr;//本月总电压合格时间
    unsigned short  RemoteRelayOn_DelayTmr;//限电时间
    unsigned long   RemoteRelayOff_DelayTmr;//跳闸告警时间
    unsigned long   Bat_Useing_Tmr;//电池使用时间
    unsigned short  LoadInter_Tmr[6];//负荷记录间隔时间计数器
    unsigned short   Bell_Tmr;//蜂鸣器延时
    unsigned char   CommMACErrCnt;//数据帧MAC校验计数器
    unsigned char   Pass02or03Err_Cnt;//通讯03级密码错误计数器
    unsigned char   Pass04Err_Cnt;//通讯04级密码错误计数器    
    unsigned char   PowerRead_Tmr;//分相电能读取计数器
    unsigned char   ChkAde_Cnt;
    unsigned char   KeyAble_Tmr;//掉电按键唤醒延时
    unsigned char   Wake_Tmr;//掉电唤醒允许天数
    unsigned char   IrWake_Cnt;//每天红外唤醒次数
    unsigned char   KeyUp_Tmr;//上翻按键
    unsigned char   Keydown_Tmr;//下翻按键
    unsigned char  CloseESAM_Tmr;//ESAM延时关电源
    unsigned char   PLCInit_Tmr;//上电载波表RESET时间
    unsigned char   ChkEsam_Cnt;//ESAM错误操作次数    
    unsigned short  ResetAute_Tmr;//复位主动上报延迟时间
    unsigned short  Relayon_Tmr;//继电器手动合闸延迟时间
    unsigned long   UpProtRelay_Tmr;//跳闸大电流保护延时时间
    unsigned char   Disp_Tmr;//液晶刷新延时
    unsigned char   CheckRelay_Tmr2;//继电器反馈自检延时2
    unsigned char   CommReadCredit_Tmr;
    unsigned char   MulPortCnt;//多功能口脉宽控制时间
    unsigned char   Rev_Cnt[5];
    unsigned char   MulPort_Cnt;//多功能脉冲宽度计数器
    unsigned char   KeyUpWake_Cnt;//上翻按键唤醒计数器
    unsigned char   KeyDownWake_Cnt;//下翻按键唤醒计数器
    unsigned char   IRWakeUp_Cnt;//红外唤醒计数器
    unsigned char   IRCommWait_Cnt;//通讯等待计数器计数器
    unsigned char   AllLostDel_Cnt;//全失压延时计数器
    unsigned char   EngSaveCnt;//电量转存计数器
    unsigned char   DmdSaveCnt;//需量转存计数器
    unsigned char   InvOrderV_Tmr;//电压逆相序开始延时计数器
    unsigned char   InvOrderV_TmrEnd;//电压逆相序结束延时计数器
    unsigned char   InvOrderI_Tmr;//电流逆相序开始延时计数器
    unsigned char   InvOrderI_TmrEnd;//电流逆相序结束延时计数器
    unsigned char   OpenCov_Tmr;//开表盖延时计数器
    unsigned char   OpenCov_TmrEnd;//开表盖延时计数器
    unsigned char   OpenTer_Tmr;//开端钮盖延时计数器
    unsigned char   OpenTer_TmrEnd;//开端钮盖延时计数器
    unsigned char   OverMagn_Tmr;//磁场干扰延时计数器
    unsigned char   OverMagn_TmrEnd;//磁场干扰延时计数器
    unsigned char   ErrPower_Tmr;//电源异常延时计数器
    unsigned char   ErrPower_TmrEnd;//电源异常延时计数器
    unsigned char   ErrRelay_Tmr;//负荷记录误动作时计数器
    unsigned char   ErrRelay_TmrEnd;//负荷记录误动作延时计数器   
    unsigned char   ErrClock_Tmr;//时钟故障计数器
    unsigned char   ErrClock_TmrEnd;//时钟故障延时计数器
    unsigned char   ErrEMU_Tmr;//计量芯片故障计数器
    unsigned char   ErrEMU_TmrEnd;//计量芯片故障延时计数器
    unsigned char   OpenRelyDel_Tmr;//上电操作继电器延时
    unsigned char   VoltChkCnt;//上电报警电压阀值检测延时
    unsigned short   Led_Tmr;//上电Led点亮时间
    unsigned char   SaveTim_Cnt;//定时保存计数器
    unsigned char   IntTim_Cnt;
    unsigned char   IntErr_Cnt;
    unsigned char   IntTim_Tmr;
    unsigned short  InfTim_Cnt;
    unsigned char   LossPower_Tmr;//辅助电源掉电延时计数器
    unsigned char   LossPower_TmrEnd;//辅助电源掉电延时计数器
    unsigned short  UpIProtect_Tmr;
    unsigned char   IRAuthen_Tmr;
    unsigned char   ResetReportState_Tmr;
    unsigned char   IntLoad_Cnt;
    unsigned char   PowerUpTim_Tmr;//上电延时计数器
    unsigned char   LowRTCBat_Tmr;//时钟电池欠压延时计数器
    unsigned char   LowRTCBat_TmrEnd;//时钟电池欠压延时计数器
    unsigned char   LowComBat_Tmr;//停抄电池欠压延时计数器
    unsigned char   LowComBat_TmrEnd;//停抄电池欠压延时计数器
    unsigned char   TestRelay_Tmr;//继电器检测
    unsigned char   MeterClr_Tmr;//清零延时判电压
    unsigned char   RelayOn_Tmr;//继电器合闸延时判断
    unsigned short InstFrzDy;
//    unsigned short MinFrzDy;
//    unsigned short HourFrzDy;
//    unsigned short DayFrzDy;
//    //unsigned short BalanceFrzDy;
//    unsigned short MonthFrzDy;
//    unsigned short SqFrzDy;
//    unsigned short SdFrzDy;
    //unsigned long  DmdPeriodSec;   //期间需量周期计数器
    unsigned char Delay0p5s;
} TIMER;

#define  TimerLen  sizeof(TIMER)

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


//校表参数结构体定义
typedef struct
{
    unsigned short  Igain; //电流增益
    unsigned short  Ugain; //电压增益
    unsigned short  Pgain; //有功功率增益
    unsigned short  Qgain; //无功功率增益
    unsigned short  Iofst; //电流偏移
    unsigned short  Uofst; //电压偏移
    unsigned short  Pofst; //功率偏移
    unsigned char  UTofst; //采样通道U相位偏移
    unsigned char  ITofst[3]; //采样通道I相位偏移
    unsigned short PTofst[3]; //有功中、低、高功率相位偏移
    unsigned short QTofst[3]; //无功中、低、高功率相位偏移
    unsigned short AdjChk;
} ADJ;
#define  LenAdjBuff  30
typedef union
{
    unsigned char   AdjBuff[LenAdjBuff];
    ADJ 	    Adj;
} ADJ_BLOCK;

typedef struct
{
    unsigned short  Iofst2; 
} ADJ2;
#define  LenAdjBuff2  2
typedef union
{
    unsigned char   AdjBuff2[LenAdjBuff2];
    ADJ2 	    Adj2;
} ADJ_BLOCK2;


typedef struct
{
    unsigned char CT;
    unsigned char Meter_Type;
    unsigned char EXTERN_LSE;
    float  MeterUrms;
    float  MeterIrms;   
    float  RN8302Urms;
    unsigned long  RN8302Irms;
    unsigned long  RN8302Prms;
    unsigned short MeterHFConst1;
    unsigned short MeterHFConst2;
    unsigned short MeterUn;
    unsigned short MeterIb;
    unsigned short MeterImax;
    unsigned long MeterCP;
    unsigned long MeterCP2;
}METERPARAMETER;
//K系数结构体定义
typedef struct
{    
    float  Ku;//电压有效值系数
    float  Ki;//电流有效值系数
    float  Kp;//功率有效值系数
    unsigned long    Ke;//1个脉冲能量值
    unsigned long    EnergyUp;//能量寄存器读取的上限值
    unsigned short  IStart_P;//视在、有功启动电流门限值寄存器
    unsigned long  ImaxUp;//电流显示上限
    unsigned long DispUnUp;//电压显示上限
    unsigned long DispUnDown;//电压显示下限
    unsigned long MConst;//表计常数
} FACTOR;

typedef struct  
{
    unsigned char  b0:1;
    unsigned char  b1:1;
    unsigned char  b2:1;
    unsigned char  b3:1;
    unsigned char  b4:1;
    unsigned char  b5:1;
    unsigned char  b6:1;
    unsigned char  b7:1;
}BITDEFINE;

//字节定义
typedef union
{
    BITDEFINE      BitDefine;
    unsigned char  ByteDefine;
}BYTEDEFINE;

//4字节定义
typedef union
{
    unsigned char  Byte[4];
    unsigned long  LongDefine;
}LONGDEFINE;

typedef struct
{
    unsigned char  CurntPz0[4];//当前结算周期组合有功总电量
    unsigned char  LastPz0[4];//上1结算周期组合有功总电量
}CYCENERY;

typedef struct
{
    unsigned char  CurntPz0[4];//当前月度组合有功总累计用电量
    unsigned char  LastPz0[4];//上1月度组合有功总累计用电量
}MONTHENERY;

typedef struct
{
    unsigned char  BalanceTime[4];//当前时间
    unsigned char  CurntPz0[4];//当前年度组合有功总累计用电量
    unsigned char  LastPz0[4];//上1年度组合有功总累计用电量
}YEARENERY;

//有功电能量 增加结构体成员时需要修改数据长度
typedef struct
{
    unsigned char  ActPz0[4];//组合有功
    unsigned char  ActPz1[4];
    unsigned char  ActPz2[4];
    unsigned char  ActPz3[4];
    unsigned char  ActPz4[4];
    unsigned char  ActPzChk[2];//校验
    unsigned char  ActPp0[4];//正向有功
    unsigned char  ActPp1[4];
    unsigned char  ActPp2[4];
    unsigned char  ActPp3[4];
    unsigned char  ActPp4[4];
    unsigned char  ActPpChk[2];//校验
    unsigned char  ActPn0[4];//反向有功
    unsigned char  ActPn1[4];
    unsigned char  ActPn2[4];
    unsigned char  ActPn3[4];
    unsigned char  ActPn4[4];
    unsigned char  ActPnChk[2];//校验
    unsigned char  ActUsedPz0[4];//阶梯电价判断使用用电量
    unsigned char  ActUsedPzChk[2];//校验
} ACTENERY;
#define  LenAct  72    ///       66
typedef union
{
    unsigned char   AEngyBuff[LenAct];
    ACTENERY 	    AEngy;
} ACTENERGY_BLOCK;


//无功电能量 增加结构体成员时需要修改数据长度
typedef struct
{
    unsigned char  Grp1Pz0[4];//组合无功1
    unsigned char  Grp1Pz1[4];
    unsigned char  Grp1Pz2[4];
    unsigned char  Grp1Pz3[4];
    unsigned char  Grp1Pz4[4];
    unsigned char  Grp1PzChk[2];//校验
    unsigned char  Grp2Pz0[4];//组合无功2
    unsigned char  Grp2Pz1[4];
    unsigned char  Grp2Pz2[4];
    unsigned char  Grp2Pz3[4];
    unsigned char  Grp2Pz4[4];
    unsigned char  Grp2PzChk[2];//校验
    unsigned char  Quadr1P0[4];//第一象限无功
    unsigned char  Quadr1P1[4];
    unsigned char  Quadr1P2[4];
    unsigned char  Quadr1P3[4];
    unsigned char  Quadr1P4[4];
    unsigned char  Quadr1Chk[2];//校验
    unsigned char  Quadr2P0[4];//第二象限无功
    unsigned char  Quadr2P1[4];
    unsigned char  Quadr2P2[4];
    unsigned char  Quadr2P3[4];
    unsigned char  Quadr2P4[4];
    unsigned char  Quadr2Chk[2];//校验
    unsigned char  Quadr3P0[4];//第三象限无功
    unsigned char  Quadr3P1[4];
    unsigned char  Quadr3P2[4];
    unsigned char  Quadr3P3[4];
    unsigned char  Quadr3P4[4];
    unsigned char  Quadr3Chk[2];//校验
    unsigned char  Quadr4P0[4];//第四象限无功
    unsigned char  Quadr4P1[4];
    unsigned char  Quadr4P2[4];
    unsigned char  Quadr4P3[4];
    unsigned char  Quadr4P4[4];
    unsigned char  Quadr4Chk[2];//校验
} RACTENERY;
#define  LenRact  132
typedef union
{
    unsigned char   REngyBuff[LenRact];
    RACTENERY       REngy;
} RACTENERGY_BLOCK;

typedef struct
{    
    float  Ki[3];//电流有效值系数
    float  B_Ki;//电流有效值系数
    float  C_Ki;//电流有效值系数
} NVM1KI;


//分相电能量 增加结构体成员时需要修改数据长度
typedef struct
{
    unsigned char  PhAPp[4];//A相正向有功总
    unsigned char  PhBPp[4];//B相正向有功总
    unsigned char  PhCPp[4];//C相正向有功总
    unsigned char  PhAPn[4];//A相反向有功总
    unsigned char  PhBPn[4];//B相反向有功总
    unsigned char  PhCPn[4];//C相反向有功总
    unsigned char  PhChk[2];//校验
    unsigned char  PhAGrp1[4];//A相组合无功1
    unsigned char  PhBGrp1[4];//B相组合无功1
    unsigned char  PhCGrp1[4];//C相组合无功1
    unsigned char  PhAGrp2[4];//A相组合无功2
    unsigned char  PhBGrp2[4];//B相组合无功2
    unsigned char  PhCGrp2[4];//C相组合无功2
    unsigned char  PhGrpChk[2];//校验
    unsigned char  PhAQur1[4];//A相第1象限无功
    unsigned char  PhBQur1[4];//B相第1象限无功
    unsigned char  PhCQur1[4];//C相第1象限无功
    unsigned char  PhAQur2[4];//A相第2象限无功
    unsigned char  PhBQur2[4];//B相第2象限无功
    unsigned char  PhCQur2[4];//C相第2象限无功
    unsigned char  PhAQur3[4];//A相第3象限无功
    unsigned char  PhBQur3[4];//B相第3象限无功
    unsigned char  PhCQur3[4];//C相第3象限无功
    unsigned char  PhAQur4[4];//A相第4象限无功
    unsigned char  PhBQur4[4];//B相第4象限无功
    unsigned char  PhCQur4[4];//C相第4象限无功
    unsigned char  PhQurChk[2];//校验
} PHASEENERY;
#define  LenPhase  102
typedef union
{
    unsigned char   PEngyBuff[LenPhase];
    PHASEENERY 	    PEngy;
} PHASEENERGY_BLOCK;


//视在电能量 增加结构体成员时需要修改数据长度
typedef struct
{
    unsigned char  VarPp0[4];//正向
    unsigned char  VarPp1[4];
    unsigned char  VarPp2[4];
    unsigned char  VarPp3[4];
    unsigned char  VarPp4[4];
    unsigned char  VarPpChk[2];//校验
    unsigned char  VarPn0[4];//反向
    unsigned char  VarPn1[4];
    unsigned char  VarPn2[4];
    unsigned char  VarPn3[4];
    unsigned char  VarPn4[4];
    unsigned char  VarPnChk[2];//校验
} VARENERY;
#define  LenVar  44
typedef union
{
    unsigned char   VEngyBuff[LenVar];
    VARENERY 	    VEngy;
} VARENERY_BLOCK;


//脉冲数 增加结构体成员时需要修改数据长度
typedef struct
{
    unsigned short   PhAPpCnt;//A相正向有功脉冲数
    unsigned short   PhBPpCnt;//B相正向有功脉冲数
    unsigned short   PhCPpCnt;//C相正向有功脉冲数
    unsigned short   PhAPnCnt;//A相反向有功脉冲数
    unsigned short   PhBPnCnt;//B相反向有功脉冲数
    unsigned short   PhCPnCnt;//C相反向有功脉冲数
    unsigned short   AQuadr1PCnt;//A相第一象限无功脉冲数
    unsigned short   BQuadr1PCnt;//B相第一象限无功脉冲数
    unsigned short   CQuadr1PCnt;//C相第一象限无功脉冲数
    unsigned short   AQuadr2PCnt;//A相第二象限无功脉冲数
    unsigned short   BQuadr2PCnt;//B相第二象限无功脉冲数
    unsigned short   CQuadr2PCnt;//C相第二象限无功脉冲数
    unsigned short   AQuadr3PCnt;//A相第三象限无功脉冲数
    unsigned short   BQuadr3PCnt;//B相第三象限无功脉冲数
    unsigned short   CQuadr3PCnt;//C相第三象限无功脉冲数
    unsigned short   AQuadr4PCnt;//A相第四象限无功脉冲数
    unsigned short   BQuadr4PCnt;//B相第四象限无功脉冲数
    unsigned short   CQuadr4PCnt;//C相第四象限无功脉冲数
    signed   short   AGrp1Cnt;//A相组合无功1脉冲数
    signed   short   BGrp1Cnt;//B相组合无功1脉冲数
    signed   short   CGrp1Cnt;//C相组合无功1脉冲数
    signed   short   AGrp2Cnt;//A相组合无功2脉冲数
    signed   short   BGrp2Cnt;//B相组合无功2脉冲数
    signed   short   CGrp2Cnt;//C相组合无功2脉冲数
    signed   short   ActPzCnt[5];//组合有功脉冲数
    unsigned short   ActPpCnt[5];//正向有功脉冲数
    unsigned short   ActPnCnt[5];//反向有功脉冲数
    signed   short   Grp1PzCnt[5];//组合无功1脉冲数
    signed   short   Grp2PzCnt[5];//组合无功2脉冲数
    unsigned short   Quadr1PCnt[5];//第一象限无功脉冲数
    unsigned short   Quadr2PCnt[5];//第二象限无功脉冲数
    unsigned short   Quadr3PCnt[5];//第三象限无功脉冲数
    unsigned short   Quadr4PCnt[5];//第四象限无功脉冲数
    unsigned short   VAHRCnt[5];//正向视在电能脉冲数
    unsigned short   VAHFCnt[5];//反向视在电能脉冲数

}PULSECOUNT;
#define  LenCnt  158
#define  LenCnt1  48
#define  LenCnt2  110
typedef union
{
    unsigned char   PCntBuff[LenCnt];
    PULSECOUNT      PCnt;
} PULSECOUNT_BLOCK;


//当前需量结构体定义
typedef struct
{
    unsigned char  ActDemand[3];//当前有功需量
    unsigned char  RactDemand[3];//当前无功需量
    unsigned char  VarDemand[3];//当前视在需量
    unsigned char  Act_F_Demand[3];//当前正向有功需量
    unsigned char  Act_R_Demand[3];//当前反向有功需量
    unsigned char  Ract_Q1_Demand[3];//当前无功1象限需量   
    unsigned char  Ract_Q2_Demand[3];//当前无功2象限需量 
    unsigned char  Ract_Q3_Demand[3];//当前无功3象限需量 
    unsigned char  Ract_Q4_Demand[3];//当前无功4象限需量 
    
}CURRENTDEMAND;


//最大需量结构体定义
typedef struct
{
    unsigned char  TotalDmd[3];//总最大需量
    unsigned char  TotalDmdTime[5];//总最大需量发生时间
    unsigned char  Tou1Dmd[3];//尖最大需量
    unsigned char  Tou1DmdTime[5];//尖最大需量发生时间
    unsigned char  Tou2Dmd[3];//峰最大需量
    unsigned char  Tou2DmdTime[5];//峰最大需量发生时间
    unsigned char  Tou3Dmd[3];//平最大需量
    unsigned char  Tou3DmdTime[5];//平最大需量发生时间
    unsigned char  Tou4Dmd[3];//谷最大需量
    unsigned char  Tou4DmdTime[5];//谷最大需量发生时间
    unsigned char  DmdChk[2];
}MAXDEMAND;
#define  LenDmd  42
typedef union
{
    unsigned char   MaxDmdBuff[LenDmd];
    MAXDEMAND       MaxDmd;
} MAXDMD_BLOCK;



//组合计量模式
typedef struct
{
    unsigned char  ActMode;
    unsigned char  RactMode1;
    unsigned char  RactMode2;
} MEASUREMODE;




struct Att7051Eep 
{

  union B32_B08 HFConstD;	
	union B32_B08 PStartD;	
	union B32_B08 GPQAD;	
	union B32_B08 PhsAD;	
	union B32_B08 APOSAD;	
	union B32_B08 IARMSOSD;	
	union B32_B08 IBRMSOSD;
  union B32_B08 EMUCON2D ;
	
	union B16_B08 Crc;	
			//2?????
};
			
struct Att7051 
{
	union B32_B08 HFConst;
  union B32_B08 PStart;		//03H PStart ?????????
	union B32_B08 GPQA;		//0AH L?????????
	union B32_B08 PhsA;		//0EH L??????????
	union B32_B08 APOSA;		//0FH N??????????
	union B32_B08 IARMSOS;		//02H ???????
	union B32_B08 IBRMSOS;		//00H ???????
	union B32_B08 EMUCON2 ;
        
	union B32_B08 DStart;		//01H ???????
	union B32_B08  GPQB;	
	union B32_B08 PhsB;	
	union B32_B08 QPhsCal;	
	union B32_B08 APOSB;	
	union B32_B08 RPOSA;	
	union B32_B08 RPOSB;	
	union B32_B08 IBGain;
	union B32_B08 D2FPL;		//05H L???????
	union B32_B08 D2FPH;		//07H L?????
	union B32_B08 DCIAH;	
	union B32_B08 DCIBH;	
	union B32_B08 DCUH;	
	union B32_B08 DCL;	
		
	
  union B32_B08 SYSCON;	
	union B32_B08 EMUCON;
	
	union B32_B08 CKSUM;	
			//2?????
};

 
union Att7051Para
{
	struct Att7051 A25;
	unsigned long int  E32[25];
};




//需量
typedef struct
{
   
	
	
	
	unsigned char TimBef ;//计算用需量周期 
    unsigned char TimRec ;//实时需量周期 程序使用和E2存储为16进制数
    unsigned char SlidTimB;//计算用滑差时间 
    unsigned char SlidTimR;//实时滑差时间 程序使用和E2存储为16进制数

    unsigned char TotalTimCnt;//总需量周期计数器
    unsigned char TouTimCnt;//分时需量周期计数器

    unsigned char Grp1TimCnt;//组合无功1需量周期计数器
    unsigned char Grp1TouTimCnt;//组合无功1分时需量周期计数器
    
    unsigned char Grp2TimCnt;//组合无功2需量周期计数器
    unsigned char Grp2TouTimCnt;//组合无功2分时需量周期计数器

    unsigned char SlidTimCnt;//总需量滑差计数器
    unsigned char Grp1SlidTimCnt;//组合无功1需量滑差计数器
    unsigned char Grp2SlidTimCnt;//组合无功2需量滑差计数器

    unsigned char SlidTimTouCnt;//分时需量滑差计数器
    unsigned char Grp1SlidTimTouCnt;//组合无功1分时需量滑差计数器
    unsigned char Grp2SlidTimTouCnt;//组合无功2分时需量滑差计数器

    unsigned char AddFCnt;//1分钟正向有功功率累加计数器
    unsigned char AddRCnt;//1分钟反向有功功率累加计数器
    unsigned char AddFTouCnt;//1分钟正向有功谷功率累加计数器
    unsigned char AddRTouCnt;//1分钟反向有功谷功率累加计数器
    unsigned char AddZCnt[3];//1分钟当前总有、无功、视在功率累加计数器
    unsigned char QuadrAddCnt[4];//4象限总无功功率累加计数器
    unsigned char QuadrAddTouCnt[4];//1分钟4象限谷无功功率累加计数器
    unsigned char GrpAddCnt[2];//组合无功1、2功率累加计数器
    unsigned char GrpAddTouCnt[2];//组合无功1、2谷功率计数器
    
    unsigned char DmdSec;//需量秒标志
    unsigned char DmdMinCnt;//需量分计数器
    unsigned char TouDmdMinCnt;//分时需量分计数器
    unsigned char Grp1DmdMinCnt;//组合无功1需量分计数器
    unsigned char TouGrp1DmdMinCnt;//组合无功1分时需量分计数器
    unsigned char Grp2DmdMinCnt;//组合无功2需量分计数器
    unsigned char TouGrp2DmdMinCnt;//组合无功2分时需量分计数器
    unsigned char DmdTouNo;//分时需量费率号
    unsigned char DmdTouWait;
    
    unsigned char RMode1;//组合无功1需量模式
    unsigned char RMode2;//组合无功2需量模式
    
    unsigned char DmdoutCnt;//需量投切计数器
}DMDCOMM;

/*********************电压、电流采样值，由硬件决定***********************/
typedef struct
{
    unsigned char Addendum_File7;//补遗文件7
    unsigned char Relay_Mode;//继电器控制
    unsigned char AdjCheck_Mode;
}METERSPECIAL;







#pragma pack(pop)














#endif

