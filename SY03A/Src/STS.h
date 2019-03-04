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
#define TID_REC_MAXNUM  50		//TID记录数据50条//

// 2串密钥的间隔时间定义，单位秒 //
#define MAX_SECONDS_OF_MULTIPLE_TOKEN_INTERVAL		90    // 90秒 //   

//******************************************************************
//             4     函数声明                                      
//******************************************************************
extern void api_Reset_APDU_dataOfDefault(void);
extern void api_initialize_APDU_when_reset(void);
extern void api_save_APDU_key_var(void);
extern void api_updata_APDU_key_var(void);
extern void api_set_APDU_TID_Record(INT32U u32_tid_tmp,INT8U mode);
extern INT8U api_Handle_key_sts_code(INT8U *ptr_kay_data);
extern void api_judge_keyChg_TOKEN_interval_pre_second(void);
extern ErrorStatus Lnk_OBIS_D7X_STS_deal(void);

//******************************************************************
//             5     其他内容，例如伪指令等             
//******************************************************************
//gs_APDU_analyze_var.err_flg 解析错误字定义//
#define	STS_TOKEN_OK					0	//代表token解析结果OK//
#define	STS_TOKEN_REJRCT_1				1	//解密失败，可能是秘钥错误//				
#define	STS_TOKEN_REJRCT_2				2	//不支持的TOKEN类//	
#define	STS_TOKEN_REJRCT_3				3	//DDTK秘钥，不支持充值TOKEN//	
#define	STS_TOKEN_REJRCT_4				4	//解密成功，但是TOKEN内的数据超出范围//	
#define	STS_TOKEN_REJRCT_5				5	//秘钥替换权限不足，其他秘钥禁止修改DITK秘钥//	
#define	STS_TOKEN_REJRCT_6				6	//TID超出秘钥有效期//	
#define	STS_TOKEN_REJRCT_USED			7	//TOKEN已经被使用过//	
#define	STS_TOKEN_REJRCT_OLD			8	//TOKEN已过期（当前TOKEN中TID<记录内最近1次TID）//	
#define	STS_TOKEN_REJRCT_OVER_CRD		9	//充值解析正确但是超出囤积上限//	
#define	STS_TOKEN_CHG_KEY_PLS_2nd		10	//秘钥更改第1串解析成功，等待输入第2串//	
#define	STS_TOKEN_CHG_KEY_PLS_1st		11	//秘钥更改第2串解析成功，等待输入第1串//	

//sts_kay_change_data.flg define 
#define KeyChangeHO BIT0
#define KeyChangeLO BIT1



//******************************************************************
//             6     数据存储声明        
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
 #define F01_BLOCK52_ADR_START	ADR_BLOCK52_TIDREC_E2P	//区块首地址  //
 #define LEN_BLOCK52_TIDREC_E2P	(sizeof(APDU_TID_REC))	//区块有效数据内容长度  //
 #define OFFSET_BLOCK52_CSCK	(ST_MB_OFFSET(APDU_TID_REC,u16_csck))	//区块16bits校验和数据偏移量  //
 #define ADR_BLOCK52_CSCK_E2P	(F01_BLOCK52_ADR_START+OFFSET_BLOCK52_CSCK)		//区块16bits校验和物理存储地址  //
 
#define ADR_TIDREC_01	(F01_BLOCK52_ADR_START +ST_MB_OFFSET(APDU_TID_REC,hex24_tid[0][0]))//最近1次TID记录//


/***************************************************************
*    END
****************************************************************/

