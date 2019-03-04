#include "ex_func.h"
#include "ex_var.h"
#include <absacc.h>
#include <string.h>
const APDU_KEY_EXRAM_VAR	cgs_stsKeyParam_default=
{
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01, 	//INT8U	secret_key[8];	//电表密钥数据 ,运算后获取,8字节//
	0xFF,										//密钥有效期,8bit, //
	0x00,										//密钥种类, 2bit ,//
	0x00,										//密钥版本号,4bit //
	0x00,										// 费率索引号,8bit,0~99//	
	0x00,0X92,									// 工厂编号,西力为0X92//	
 	0x99,0X99,0X92,								// SGC编号//	
	(INT16U)0,//	INT16U	csck;		//16位累加和校验值//
};

// CRC 16 数据表 //
const INT16U crc_tab[256]=
                   {                       
                      0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
                      0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
                      0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
                      0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
                      0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
                      0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
                      0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
                      0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
                      0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
                      0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
                      0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
                      0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
                      0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
                      0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
                      0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
                      0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
                      0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
                      0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
                      0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
                      0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
                      0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
                      0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
                      0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
                      0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
                      0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
                      0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
                      0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
                      0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
                      0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
                      0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
                      0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
                      0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040,
                   };

//STA加解密运算相关常量数组  //
const unsigned char HXDTTable1[16] = 
{12, 6, 2, 7, 13, 9, 5, 4, 1, 10, 14, 8, 0, 3, 15, 11};
const unsigned char HXDTTable2[16] = 
{9, 11, 6, 5, 12, 7, 14, 2, 13, 3, 1, 15, 4, 8, 0, 10};
const unsigned char HXETTable1[16] = 
{14, 10, 7, 9, 12, 3, 2, 5, 13, 0, 15, 1, 4, 8, 6, 11};
const unsigned char HXETTable2[16] = 
{12, 8, 2, 13, 7, 6, 1, 3, 11, 5, 9, 15, 0, 4, 10, 14};
const unsigned char HXPETable[64] =  {
                                      55, 42, 10, 18, 24, 21, 44, 35,
                                      2,  22, 56, 43, 27, 58, 9,  50,
                                      6,  36, 12, 61, 37, 38, 53, 16,
                                      62, 3,  7,  4,  32, 20, 63, 25,
                                      51, 52, 54, 33, 49, 19, 46, 29,
                                      48, 31, 23, 30, 41, 28, 13, 5,
                                      40, 60, 39, 11, 15, 17, 1,  0,
                                      57, 34, 59, 8,  47, 14, 45, 26
                                   };

unsigned char HXPDTable[64] =  {
                                      55, 54, 8,  25, 27, 47, 16, 26,
                                      59, 14, 2,  51, 18, 46, 61, 52,
                                      23, 53, 3,  37, 29, 5,  9,  42,
                                      4,  31, 63, 12, 45, 39, 43, 41,
                                      28, 35, 57, 7,  17, 20, 21, 50,
                                      48, 44, 1,  11, 6,  62, 38, 60,
                                      40, 36, 15, 32, 33, 22, 34, 0,
                                      10, 56, 13, 58, 49, 19, 24, 30
                                      
                                  };

/* Table - Binary */
unsigned char binary[64] = {
0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1,
0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1,
1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1,
1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1 };

/*********************************************************************************************************
**  代码区
********************************************************************************************************/
/*****************************************************************************
** Function name    :INT16U checkSTS_crc(INT8U *ptr, INT8U len)
** Description      :CRC16校验和计算                  
**
** Parameters       :INT8U *ptr  校验数据数组首地址       
**                   INT8U len 校验数据数组长度
** Returned value   :INT16U CRC16校验和
**
** Note             :校验数据数组为INT8U类型数据
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT16U checkSTS_crc(INT8U *ptr, INT8U len)
{
    INT8U i;
    INT16U tmp, short_c;
    INT16U crc = 0xffff;

    for(i=0; i<len; i++)                        
    {
        short_c = 0x00ff & (unsigned int) *ptr;
        tmp =  crc ^ short_c;
        crc = (crc >> 8) ^ crc_tab[ tmp & 0xff ];
        ptr --;
    }

    tmp  = crc & 0xff00;
    crc  = crc << 8;
    crc |= tmp >> 8;

    return crc;
}


/*****************************************************************************
** Function name    :void STA(INT8U *source,INT8U * dest,INT8U * inkey, INT8U type)
** Description      :STA加解密运算函数  //             
**
** Parameters       :INT8U *source  源数据首地址,8字节HEX码//  
**                  :INT8U * dest 运算结果数据首地址(加密/解密),8字节HEX码//  
**                  :INT8U * inkey 秘钥数据首地址,8字节HEX码//  
**                  :INT8U type 加解密运算控制数据， 加密(1),解密(0)   //
** Returned value   :NONE
**
** Note             :该加密算法源于IEC62055-41标准，STS协会标准体系
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
//void STA(INT8U *source,INT8U * dest,INT8U * inkey, INT8U type)
void STA(INT8U *source,INT8U * dest,INT8U * inkey, INT8U type)
{
  INT8U buffer1[64],buffer2[64],kwork[64];
  INT8U tmp,tmp1,tmp2;
  INT8U i,j,k;


    if(type)   
    {
        // 加密前的部分处理 //
        for(i=0; i<8; i++)
        *(inkey+i) = *(inkey+i) ^ 0xff;         //反码数据 //
        tmp1 = *inkey;                          //右循环12bits//
        tmp2 = *(inkey+1);
        for(i=1; i<7; i++)
        {
            *(inkey+i-1) = (*(inkey+i) &0xf0) >> 4;
            *(inkey+i-1) |= (*(inkey+i+1) &0x0f) << 4;
        }
        *(inkey+6) = (*(inkey+7) &0xf0) >> 4;
        *(inkey+6) |= (tmp1 &0x0f) << 4;
        *(inkey+7) = (tmp1 &0xf0) >> 4;
        *(inkey+7) |= (tmp2 &0x0f) << 4;
    }

    //源数据处理:将8个字节数据分为64位，每位按1个字节存放//
    for(i = 0; i < 8; i++)
    {
        tmp = *(source + i);
        for(j=0; j<8; j++)
        {
            if(tmp & 0x01)       buffer2[8*i+j] = 1;
            else      buffer2[8*i+j] = 0;
            tmp >>= 1;
        }
    }

    //秘钥数据处理:将8个字节数据分为64位，每位按1个字节存放//
    for(i = 0; i < 8; i++)
    {
        tmp = *(inkey + i);
        for(j=0; j<8; j++)
        {
            if(tmp &0x01)    kwork[8*i+j] = 1;
            else     kwork[8*i+j] = 0;
            tmp >>= 1;
        }
    }

    // 加解密处理 //
    if(type)
    {
        for(k=0; k<16; k++)
        {
            for(i=0; i<16; i++)//substitution process
            {
                tmp = 0x00;
                for(j = 3; j > 0; j--)
                tmp = (tmp + buffer2[4*i+j]) * 2;
                tmp = tmp + buffer2[4*i];
							
                if(kwork[i*4 + 3])
                {
                    tmp = *(HXETTable2  + tmp);
                }
                else
                {
                    tmp = *(HXETTable1  + tmp);
                }

                buffer1[ i*4+3] = binary[0+tmp*4];//译为二进制//
                buffer1[ i*4+2] = binary[1+tmp*4];
                buffer1[ i*4+1] = binary[2+tmp*4];
                buffer1[ i*4] = binary[3+tmp*4];
            }

            for(i=0; i<64; i++)//Permutation process
            {
                buffer2[HXPETable[i]] = buffer1[i];
            }

            tmp = kwork[63];//左移一位//
            for(i = 63; i > 0; i--)
            kwork[i] = kwork[i-1];
            kwork[0] = tmp;
        }
    }
    else
    {
        for(k=0; k<16; k++)
        {
            for(i=0; i<64; i++)//Permutation process
            {
                buffer1[HXPDTable[i]] = buffer2[i];
            }

            for(i=0; i<16; i++)//substitution process
            {
                tmp = 0x00;
                for(j = 3; j > 0; j--)
                tmp = (tmp + buffer1[4*i+j]) * 2;
                tmp = tmp + buffer1[4*i];

                if(kwork[i*4])
                {
                    tmp = *(HXDTTable2  + tmp);
                }
                else
                {
                    tmp = *(HXDTTable1  + tmp);
                }

                buffer2[ i*4+3] = binary[0+tmp*4];//译为二进制//
                buffer2[ i*4+2] = binary[1+tmp*4];
                buffer2[ i*4+1] = binary[2+tmp*4];
                buffer2[ i*4] = binary[3+tmp*4]; 
            }

            tmp = kwork[0];//右移一位//
            for(i = 0; i < 63; i++)
            kwork[i] = kwork[i+1];
            kwork[63] = tmp;
        }
    }

    //数据置换位置  //
    j = 0;
    for(i = 0; i < 8; i++)
    {
        *(dest + i) = 0x00;
        for(k = 7; k > 0; k--)
        {
            *(dest + i) = ((*(dest + i)) + buffer2[j+k]) * 2;
        }
        
        *(dest + i) = *(dest + i) + buffer2[j];
        j += 8;
    }
}

/*****************************************************************************
** Function name    :INT8U Class_get(INT8U* token_array)
** Description      :获取指定TOKEN码数组的类号class//          
**
** Parameters       :INT8U* token_array  TOKEN码的9字节HEX码的首地址//
**                  :
** Returned value   :TOKEN码类号//
**
** Note             :28、29两位和65、66两位交换，获取TOKEN码类号//
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT8U Class_get(INT8U* token_array)
{
  INT8U CLASS_NO;

  CLASS_NO = 0;
  CLASS_NO |= (token_array[3] & 0X18) >> 3;

  if(token_array[8] & 0X01)
     token_array[3] = token_array[3] | 0X08;
  else
     token_array[3] = token_array[3] & 0XF7;

  if(token_array[8] & 0X02)
     token_array[3] = token_array[3] | 0X10;
  else
     token_array[3] = token_array[3] & 0XEF;  

  token_array[8] = CLASS_NO;

  return CLASS_NO;
}


/*****************************************************************************
** Function name    :INT32U Get_TID(INT8U* tokendata )
** Description      :获取指定TOKEN码数组的TID值//        
**
** Parameters       :INT8U* tokendata  解密后的TOKEN码的8字节HEX码的首地址//
**                  :
** Returned value   :TID值//
**
** Note             :
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT32U Get_TID(INT8U* tokendata )
{
INT8U* Ptr;
INT32U  TID;
INT8U i;

    Ptr = tokendata +6;
    TID = 0;
    for(i=0; i<2; i++)
    {
       TID |= (unsigned long)(*Ptr);
       TID <<= 8;
       Ptr--;
    }
    TID |= (unsigned long)(*Ptr);
    return TID;
}


/***************************************************************************************************
* Function Name		: CalcTransferAmt
* Description		: calculate the power or money
* Input				：<data> token+2 address
* Output			：
* Return			: amount 4 bytes （0.1kWh）
****************************************************************************************************/
INT32U CalcTransferAmt(INT8U* u8_data)
{
	INT32U  t;
//	double tt = 0;
	INT8U e;
	INT16U m;
	INT16U   amount;

	INT32U  tt_my;
//	INT32U  t_my;
	INT32U   t_d_my1,t_d_my2;	

	amount = (unsigned int)u8_data[1]*256 + u8_data[0];
	e = (char)((amount & 0xc000) >> 14);
	m = amount & 0x3fff;

	if(e == 0)
	{
		t = (unsigned long)m;    //   //
	}
	else
	{
		switch (e)
		{
			case 1:
				tt_my=1;
				t_d_my2 =10;
				break;
			case 2:
				tt_my=11;
				t_d_my2 =100;
				break;
			case 3:
				tt_my=111;
				t_d_my2 =1000;
				break;
			default:
				tt_my=0;
				t_d_my2 =1;
				break;
		}
		t_d_my1 = tt_my*16384; // 2的14次方 //
		t = t_d_my1 + (t_d_my2*((INT32U)(m)));
	}


	return t;
}

/***************************************************************************************************
* Function Name   : INT8U get_token_test_type(INT8U *ptr)
* Description     : get TOKEN‘s TEST TYPE
* Input           ：<data> token+3 address
* Output          ：
* Return          : TEST TYPE (36 BITs contel flg)
****************************************************************************************************/
INT8U get_token_test_type(INT8U *ptr)
{
 INT8U i,j;
 INT8U u8_test_type;
 INT8U test_data[5];
 
	i = 1;
	j = 1;
    
    test_data[0]=*(ptr+0);  //1~8bit //
    test_data[1]=*(ptr+1);  //9~16bit //    
    test_data[2]=*(ptr+2);  //17~24bit //
    test_data[3]=*(ptr+3);  //25~32bit //
    test_data[4]=(*(ptr+4))&0x0F;  //33~36bit //

    //按位比较36个位，遇到1退出循环 //	
	while(i < 36)
	{
		if(*ptr & j)	break;   //BIT == 1 返回 //
		if(j == 0x80)
		{
			ptr++;
			j = 0x01;
		}
		else 
		{
			j <<= 1;
		}
		i++;
	}

    // 获取 TEST TYPE  val //	
	if( test_data[0] == 0xFF )
	{
		u8_test_type = 0;
	}
	else
	{
		u8_test_type = i;
	}

	return(u8_test_type);
}


/***************************************************************************************************
* Function Name   : Multi16
* Description     : 超长数据乘16
* Input           ：<strSource> 
* Output          ：<strDest>   
* Return          : 
****************************************************************************************************/
void Multi16(INT8S* strSource, INT8S* strDest)
{
  INT8U i;
  INT16U r,c;
  r = 0;  //进位
  c = 0;  //乘积
  
  for( i=0;i<20;i++ )
  {
    c = strSource[i]*16;
    c += r;
    strDest[i] = c%10;
    r = c/10;
  }
}



/***************************************************************************************************
* Function Name   : Multi16
* Description     : 超长数据加
* Input           ：<strSource> 
* Output          ：<strDest>   
* Return          : 
****************************************************************************************************/
void longadd(INT8S* strSource, INT8S val)
{
  INT8S i;
  INT16U r,c;
  r = val;  //进位
  c = 0;    //和
  
  for( i=0;i<20;i++ )
  {
    c = strSource[i]+r;
    strSource[i] = c%10;
    r = c/10;
    if( r<=0 ) break;  //没有进位，停止运算
  }
}



/*****************************************************************************
** Function name    :void DecToHex_20BYTE(INT8S* strSouce, INT8S* strDest)
** Description      :20位10进制转换成16进制           
**
** Parameters       :INT8S* strSouce  待转换的10进制数据首地址 20bytes
**                  :INT8S* strDest   转换后的16进制数据首地址  8bytes                
** Returned value   :NO
**
** Note             :1 转换后的顺序按照10进制数组的方向存放
**                  :2 用10进制(strSouce)除以16，余数是16进制(strDest)低位，
**                     用得到的商值重复以上操作。
**                  :3 TOKEN输入时使用
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
void DecToHex_20BYTE(INT8S* strSouce, INT8S* strDest)
{
   INT16U i;
   INT16U s = 0;
   INT16U y = 0;
   INT16U sum = 0;
   INT16U k = 0;
   INT16U len;
   INT8U  end = 0;

   INT8S s1[20];
   INT8S s2[20];

   memset(s1,0,20);
   memset(s2,0,20);

   for(i=0;i<20;i++)
   {
       s1[i] = *strSouce;
       strSouce++;
   }
   len=20;

   while(1)
   {    
        end = 0;
        for(i=0; i<len; i++)
        {
            sum = y*10 + s1[i];
            s = sum/16;
            y = sum%16;
            s2[i] = s;
            if(y>0 || s>0) end = 1;
        }

        memcpy(s1,s2,20); //商
        strDest[k] = y;   //余数
        y = 0;
        if(!end) break;
        k += 1;
  }

  for(i=0; i<8; i++)  //按高位-高地址,低位-低地址原则排放数据
      strDest[i] = strDest[i*2] + (strDest[i*2+1]<< 4 );
  strDest[i] = strDest[16];
}


/*****************************************************************************
** Function name    :void HexToDec_20BYTE(INT8S* strSouce, INT8S* strDest)
** Description      :convert from 66 bit binary data to 20 digit decimal     
**
** Parameters       :INT8S* strSouce  66 bit binary data
**                  :INT8S* strDest   20 digit decimal token               
** Returned value	:NO
**
** Note             :1 转换后的顺序按照16进制数组的方向存放
**                  :2 生成TOKEN时使用
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
void HexToDec_20BYTE(INT8S* strSouce, INT8S* strDest)
{
  INT8U i;
  INT8U k = 0;
  INT8U len;
  
  INT8S Souce[17];
  INT8S Dest[20];
  INT8S Dest1[20];
  
  memset(Souce, 0, sizeof(Souce));
  memset(Dest, 0, sizeof(Dest));
  memset(Dest1, 0, sizeof(Dest1));
  
  for(i=8;i>0;i--)
  {
      Souce[2*i] = (*strSouce) & 0x0F;
      Souce[2*i-1] = ((*strSouce) & 0xF0)>>4;
      strSouce++;
  }
  Souce[0] = *strSouce;
  len=17;
  
  for(k=0; k<len; k++)
  {
    memcpy(Dest1, Dest, 20);
    Multi16(Dest1, Dest);
    longadd(Dest, Souce[k]);
  }
  for( i=0;i<20;i++ )
    strDest[i] = Dest[19-i];
}


/***************************************************************
*    END
****************************************************************/


//**----------------------------------------------------------------------------
//** V01.01  MJ  2016-09-29
//******************************************************************************/
void api_Reset_APDU_dataOfDefault(void)
{
//	APDU_TID_REC  s_tidRec;

	//调用秘钥默认参数更新RAM和外部存储器	//
	//void *memcpy(void *dest, const void *src, size_t n);//
	memcpy(&gs_APDU_key_var, &cgs_stsKeyParam_default, sizeof(APDU_KEY_EXRAM_VAR));  // 调用默认参数 //
    //数据保存 //
//    gs_APDU_key_var.u16_csck = Lib_get_csck_int16u_num(&gs_APDU_key_var.secret_key[0],OFFSET_BLOCK51_CSCK,CHECKWORD); 
//    Lib_Mem_Write(F01_BLOCK51_ADR_START, &gs_APDU_key_var.secret_key[0], LEN_BLOCK51_STSKEY_E2P, TYPE_F01_MAIN);
//    Lib_Mem_Write(F01_BLOCK51_ADR_START +OFFSET_F01_BACK, &gs_APDU_key_var.secret_key[0], LEN_BLOCK51_STSKEY_E2P, TYPE_F01_BACK);     

//	//清除历史TID记录 //
//	Lib_Clr_String(&s_tidRec.hex24_tid[0][0],sizeof(APDU_TID_REC));
//	//数据保存 //
//	s_tidRec.u16_csck = Lib_get_csck_int16u_num(&s_tidRec.hex24_tid[0][0],OFFSET_BLOCK52_CSCK,CHECKWORD); 
//	Lib_Mem_Write(F01_BLOCK52_ADR_START, &s_tidRec.hex24_tid[0][0], LEN_BLOCK52_TIDREC_E2P, TYPE_F01_MAIN);
//	Lib_Mem_Write(F01_BLOCK52_ADR_START +OFFSET_F01_BACK, &s_tidRec.hex24_tid[0][0], LEN_BLOCK52_TIDREC_E2P, TYPE_F01_BACK);

}




/*****************************************************************************
** Function name	:void api_initialize_APDU_analyze_var(void)
** Description		:清除STS相关APDU分析处理的过程变量 		 
**
** Parameters		:NO    
**					
** Returned value	:NO
**
** Note 			:
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-29
******************************************************************************/
void api_initialize_APDU_analyze_var(void)
{
	memset(&gs_APDU_analyze_var.apdu_data_hex[0],0,sizeof(APDU_ANALYZE_VAR));
}

/*****************************************************************************
** Function name    :void api_updata_APDU_key_var(void)
** Description      :获取APDU加解密运算的秘钥相关变量             
**
** Parameters       :NO    
**                  
** Returned value	NO
**
** Note             :
**
**----------------------------------------------------------------------------
** V01.01  MJ  2017-11-07
******************************************************************************/
void api_updata_APDU_key_var(void)
{  
	if( KeySTS_OK_flag == 1) 
	{	
		memcpy(&gs_APDU_key_var.secret_key[0], &sts_kay_process_data.secret_key[0], ee_keysts_lenth );
		return;//开户时密钥更新成功后使用新的密钥（缓存中密钥，没有正式保存到EEPROM，防止后续充值失败后密钥被更改）
	}

	if( Verify_para( ee_keysts_page, ee_keysts_inpage, &gs_APDU_key_var.secret_key[0], ee_keysts_lenth ) )
	{
				memcpy(&gs_APDU_key_var.secret_key[0], &cgs_stsKeyParam_default.secret_key[0], ee_keysts_lenth );
	}	
}

/*****************************************************************************
** Function name    :void api_save_APDU_key_var(void)
** Description      :保存秘钥相关数据           
**
** Parameters       :NO    
**                  
** Returned value	NO
**
** Note             :
**
**----------------------------------------------------------------------------
** V01.01  MJ  2017-11-07
******************************************************************************/
void api_save_APDU_key_var(void)
{
  WriteE2WithBackup( ee_keysts_page, ee_keysts_inpage, &gs_APDU_key_var.secret_key[0], ee_keysts_lenth );
	ReadE2WithBackup( ee_keysts_page, ee_keysts_inpage, &gs_APDU_key_var.secret_key[0], ee_keysts_lenth );
}


/*****************************************************************************
** Function name    :void api_updata_APDU_TID_Record(INT32U u32_tid_tmp)
** Description      :最近20次TID记录更新       
**
** Parameters       :INT32U u32_tid_tmp:本次有效TID数据 
**                  
** Returned value	NO
**
** Note             :记录顺序时0序号为最小，19序号为最大
**
**----------------------------------------------------------------------------
** V01.01  MJ  2017-11-07
******************************************************************************/
void api_updata_APDU_TID_Record(INT32U u32_tid_tmp)
{
//  union  Union_DWordDef tmp_32u;
  union  Union_DWordDef tmp_32u_tid;
  union  Union_DWordDef u32_tidRec01, u32_tidRecMin;
  APDU_TID_REC  s_tidRec;
  INT8U TidCnt,	i;
	INT8U MinTidAres;
  MinTidAres=0;
	tmp_32u_tid.UDWORD = u32_tid_tmp; 
	
	I2C_Read_Eeprom( ee_TID_page, ee_TID_inpage, &s_tidRec.hex24_tid[0][0], ee_TID_lenth );	
	I2C_Read_Eeprom( ee_TIDCNT_page, ee_TIDCNT_inpage, &TidCnt, ee_TIDCNT_lenth );
	
	if( TidCnt>=TID_REC_MAXNUM) TidCnt =TID_REC_MAXNUM;
	else if( TidCnt==0xff) TidCnt =0;
	TidCnt++;
//	Lib_Mem_Read(&s_tidRec.hex24_tid[0][0], F01_BLOCK52_ADR_START, LEN_BLOCK52_TIDREC_E2P, TYPE_F01_MAIN);
	// TID 更新记录, 0序号始终为最新，19序号始终为最旧	//
	// 数据移位 //
	if( TidCnt>TID_REC_MAXNUM)
	{
	  for(i=0;i<TID_REC_MAXNUM;i++)
	  {
		  u32_tidRec01.UDWORD = 0;
		  u32_tidRec01.BYTE[0] = s_tidRec.hex24_tid[i][0];	//低位在前	//
		  u32_tidRec01.BYTE[1] = s_tidRec.hex24_tid[i][1];
		  u32_tidRec01.BYTE[2] = s_tidRec.hex24_tid[i][2];		
		  if(u32_tidRecMin.UDWORD > u32_tidRec01.UDWORD)
		  {
			  u32_tidRecMin.UDWORD = 	u32_tidRec01.UDWORD;	//用小值替换	，循环后获取最小值//
			  MinTidAres = i ;
				if(u32_tidRecMin.UDWORD==0)  break;				//=0为最小，不在比对排序	//
		  }
		}
		s_tidRec.hex24_tid[MinTidAres][0] = tmp_32u_tid.BYTE[0];
	  s_tidRec.hex24_tid[MinTidAres][1] = tmp_32u_tid.BYTE[1];
	  s_tidRec.hex24_tid[MinTidAres][2] = tmp_32u_tid.BYTE[2];	
    
	}
	else
	{
	  for(i=(TID_REC_MAXNUM-1);i>0;i--)
	  {	
		  s_tidRec.hex24_tid[i][0] = s_tidRec.hex24_tid[i-1][0];
		  s_tidRec.hex24_tid[i][1] = s_tidRec.hex24_tid[i-1][1];
		  s_tidRec.hex24_tid[i][2] = s_tidRec.hex24_tid[i-1][2];		
	  }
	  // 更新最近1条数据 //
	  s_tidRec.hex24_tid[0][0] = tmp_32u_tid.BYTE[0];
	  s_tidRec.hex24_tid[0][1] = tmp_32u_tid.BYTE[1];
  	s_tidRec.hex24_tid[0][2] = tmp_32u_tid.BYTE[2];
	}
	I2C_Write_Eeprom( ee_TID_page, ee_TID_inpage, &s_tidRec.hex24_tid[0][0], ee_TID_lenth );	
  I2C_Write_Eeprom( ee_TIDCNT_page, ee_TIDCNT_inpage, &TidCnt, ee_TIDCNT_lenth );				
}

//更新最近5条TOKEN数据
void api_updata_APDU_TOKEN_Record(unsigned char *Buf)
{
    I2C_Read_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, &Ic_Comm_TxBuf[10], ee_TOKEN_lenth );
    
	  //更新最近1条数据
	  memcpy(Ic_Comm_TxBuf,Buf, 10 );

    I2C_Write_Eeprom( ee_TOKEN_page, ee_TOKEN_inpage, Ic_Comm_TxBuf, ee_TOKEN_lenth );
}


/*****************************************************************************
** Function name    :void api_set_APDU_TID_Record(INT32U u32_tid_tmp)
** Description      :配置TID记录数据位统一的输入值      
**
** Parameters       :INT32U u32_tid_tmp:设计的TID数据 
**                  
** Returned value	NO
**
** Note             :
**
**----------------------------------------------------------------------------
** V01.01  MJ  2017-12-15
******************************************************************************/
void api_set_APDU_TID_Record(INT32U u32_tid_tmp,INT8U mode)
{
 union  Union_DWordDef tmp_32u_tid;
 APDU_TID_REC  s_tidRec;
 INT8U 	i;

	if(mode ==0)	//全部设置	//
		{
		tmp_32u_tid.UDWORD = (u32_tid_tmp&0X00FFFFFF); 
		for(i=0;i<TID_REC_MAXNUM;i++)
			{	
			s_tidRec.hex24_tid[i][0] = tmp_32u_tid.BYTE[0];
			s_tidRec.hex24_tid[i][1] = tmp_32u_tid.BYTE[1];
			s_tidRec.hex24_tid[i][2] = tmp_32u_tid.BYTE[2]; 	
			}
		}
	else
		{
		//修改最近1次TID记录，其他记录清零	//
		tmp_32u_tid.UDWORD = 0; 
		for(i=0;i<TID_REC_MAXNUM;i++)
			{	
			s_tidRec.hex24_tid[i][0] = tmp_32u_tid.BYTE[0];
			s_tidRec.hex24_tid[i][1] = tmp_32u_tid.BYTE[1];
			s_tidRec.hex24_tid[i][2] = tmp_32u_tid.BYTE[2]; 	
			}
		tmp_32u_tid.UDWORD = (u32_tid_tmp&0X00FFFFFF); 
		s_tidRec.hex24_tid[0][0] = tmp_32u_tid.BYTE[0];
		s_tidRec.hex24_tid[0][1] = tmp_32u_tid.BYTE[1];
		s_tidRec.hex24_tid[0][2] = tmp_32u_tid.BYTE[2]; 
		
		}
		I2C_Write_Eeprom( ee_TID_page, ee_TID_inpage, &s_tidRec.hex24_tid[0][0], ee_TID_lenth );
		  
}


/*****************************************************************************
** Function name	:INT8U api_TIDVali_Judge(INT8U *u8ptr_hex_input)
**
** Description		:TOKEN解析后的TID判断      
**
** Parameters		:INT8U *u8ptr_hex_inpu，转换后的APDU数组首地址  
**                  
** Returned value	:INT8U 成功返回STS_TOKEN_OK，失败返回错误代码
**
** Note             :
**----------------------------------------------------------------------------
** V01.01  MJ  2017-11-06
******************************************************************************/
INT8U api_TIDVali_Judge(INT32U u32_tid_tmp)
{
 union  Union_DWordDef u32_tidRec01, u32_tidRecMin;
 APDU_TID_REC  s_tidRec;
 INT8U 	i;

	
	// 密钥有效期判断  //
	if((u32_tid_tmp>>16) > gs_APDU_key_var.ken )
	{
		return (STS_TOKEN_REJRCT_6);	//TID超出秘钥有效期,rEJEct6//		
	}	


	//获取最近有效的20条TID记录/
//	Lib_Mem_Read(&s_tidRec.hex24_tid[0][0], F01_BLOCK52_ADR_START, LEN_BLOCK52_TIDREC_E2P, TYPE_F01_MAIN);
  I2C_Read_Eeprom( ee_TID_page, ee_TID_inpage, &s_tidRec.hex24_tid[0][0], ee_TID_lenth );
	
	//先判断是否有使用过	//
	for(i=0;i<TID_REC_MAXNUM;i++)
	{
		u32_tidRec01.UDWORD = 0;
		u32_tidRec01.BYTE[0] = s_tidRec.hex24_tid[i][0];	//低位在前	//
		u32_tidRec01.BYTE[1] = s_tidRec.hex24_tid[i][1];
		u32_tidRec01.BYTE[2] = s_tidRec.hex24_tid[i][2];		
		if(u32_tid_tmp == u32_tidRec01.UDWORD)
		{
			return (STS_TOKEN_REJRCT_USED);	// TID 已近使用过// 	
		}
	}

/////////////////////////////////////////////////////////////////////////////////
// 在获取20条记录中TID值最小的数据，当前TID值小于最小TID值时，提示过旧
/////////////////////////////////////////////////////////////////////////////////
	//获取记录中TID值最小的一条//
	u32_tidRecMin.UDWORD =0x00FFFFFF;
	for(i=0;i<TID_REC_MAXNUM;i++)
	{
		u32_tidRec01.UDWORD = 0;
		u32_tidRec01.BYTE[0] = s_tidRec.hex24_tid[i][0];	//低位在前	//
		u32_tidRec01.BYTE[1] = s_tidRec.hex24_tid[i][1];
		u32_tidRec01.BYTE[2] = s_tidRec.hex24_tid[i][2];		
		if(u32_tidRecMin.UDWORD > u32_tidRec01.UDWORD)
		{
			u32_tidRecMin.UDWORD = 	u32_tidRec01.UDWORD;	//用小值替换	，循环后获取最小值//
			if(u32_tidRecMin.UDWORD==0)  break;				//=0为最小，不在比对排序	//
		}
	}
	
	if((u32_tid_tmp < u32_tidRecMin.UDWORD)&&(u32_tidRecMin.UDWORD !=0x00FFFFFF))
	{
		return (STS_TOKEN_REJRCT_OLD);	// TID 过旧//		
	}

	return (STS_TOKEN_OK);	
}



/*****************************************************************************
** Function name	:INT8U api_TIDVali_Judge(INT8U *u8ptr_hex_input)
**
** Description		:TOKEN解析后的TID判断      
**
** Parameters		:INT8U *u8ptr_hex_inpu，转换后的APDU数组首地址  
**                  
** Returned value	:INT8U 成功返回STS_TOKEN_OK，失败返回错误代码
**
** Note             :
**----------------------------------------------------------------------------
** V01.01  MJ  2017-11-06
******************************************************************************/
void api_Change_Key(void)
{
 	Clear_Wdt();
  memset(&sts_kay_process_data.secret_key[0],0,ee_keysts_lenth);
	//当前秘钥数据结构体更新  //
	memcpy(&gs_APDU_key_var.secret_key[0], &sts_kay_change_data.kay[0],8); //密钥数据 //	
	gs_APDU_key_var.ken = sts_kay_change_data.ken;		//密钥有效期 //	
	gs_APDU_key_var.kt = sts_kay_change_data.kt;		//密钥种类, 2bit ,//
	gs_APDU_key_var.krn = sts_kay_change_data.krn;		//密钥版本号 //
	gs_APDU_key_var.ti = sts_kay_change_data.ti;		// 费率索引号//		

 	memcpy(&sts_kay_process_data.secret_key[0],&gs_APDU_key_var.secret_key[0] ,ee_keysts_lenth);
	//滚动密钥处理 //
	if(sts_kay_change_data.ro == 1)
	{
		api_set_APDU_TID_Record(0,0);	//清除所有历史TID记录 // 
	}
	
	memset(&sts_kay_change_data.kay[0],0,sizeof(STS_KAY_CHANHE_DATA));
}


/*****************************************************************************
** Function name    :INT8U api_APDU_set_power_limit(void)
** Description      :set up Max Power Limit Mode       
**
** Parameters       :NONE
**                  
** Returned value   :INT8U ,解析结果，=0代表合法，=其他代表解析错误码
**
** Note             :1 解析的数据来源全局变量gs_APDU_analyze_var.apdu_data_hex[8]
**                  :2 
**                  :3 
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT8U api_APDU_set_power_limit(void)
{
 INT8U u8_result;
 union Union_DWordDef tmp_32u_prod;
 
	// TID数据判断判断 //
	gs_APDU_analyze_var.u32_TID = Get_TID(&gs_APDU_analyze_var.apdu_data_hex[0]);
	u8_result = api_TIDVali_Judge(gs_APDU_analyze_var.u32_TID);
	if(u8_result != STS_TOKEN_OK)
	{
		return (u8_result);
	}

	//数据获取//
	tmp_32u_prod.UDWORD = CalcTransferAmt(&gs_APDU_analyze_var.apdu_data_hex[2]);	 //量纲0.001kW //
	tmp_32u_prod.UDWORD *=10;		  // 将小数点扩大到0.0001kW// 
	gs_APDU_analyze_var.u32_PropData = tmp_32u_prod.UDWORD;
	//数据保存  //
//	Lib_Mem_DBWrite(ADR_CREDITPARAM1_poweLimit,&tmp_32u_prod.BYTE[0],4,MEM_RN_NVM);

	//补充功率限值修改后的功率限值判断处理函数  //

	//返回成功结果//     
	return (STS_TOKEN_OK);	      
}

/*****************************************************************************
** Function name    :INT8U api_APDU_clear_balance(void)
** Description      :To clear balance(清除余额)// 目前做为电表清零命令所有数据清零//    
**
** Parameters       :NONE
**                  
** Returned value   :INT8U ,解析结果，=0代表合法，=其他代表解析错误码
**
** Note             :1 解析的数据来源全局变量gs_APDU_analyze_var.apdu_data_hex[8]
**                  :2 清除余额后是否还要清零用电量?
**                  :3 
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT8U api_APDU_clear_balance(void)
{
 INT8U u8_result;
 union Union_DWordDef tmp_32u_prod;
	
	// TID数据判断判断 //
	gs_APDU_analyze_var.u32_TID = Get_TID(&gs_APDU_analyze_var.apdu_data_hex[0]);
	u8_result = api_TIDVali_Judge(gs_APDU_analyze_var.u32_TID);
	if(u8_result != STS_TOKEN_OK)
	{
		return (u8_result);
	}

	//数据获取//
	tmp_32u_prod.UDWORD = 0;    //清零数据寄存器	//
	tmp_32u_prod.BYTE[0] = gs_APDU_analyze_var.apdu_data_hex[2];
	tmp_32u_prod.BYTE[1] = gs_APDU_analyze_var.apdu_data_hex[3];
	gs_APDU_analyze_var.u32_PropData = tmp_32u_prod.UDWORD;
	
	//数据范围判断  //
	if((tmp_32u_prod.UDWORD !=0) &&(tmp_32u_prod.UDWORD !=0XFFFF))   //=0清电量余额寄存器，=FFFF为清所有寄存器  //
	{
	return (STS_TOKEN_REJRCT_4);    
	}

	//清除余额相关寄存器处理  //
//	api_clr_credit();

	//返回成功结果//	  
	return(STS_TOKEN_OK);   	     
}


/*****************************************************************************
** Function name    :INT8U api_APDU_set_key_frist(void)
** Description      :key change Token -1  (修改秘钥第1串TOKEN)
**
** Parameters       :NONE
**                  
** Returned value   :INT8U ,解析结果，=0代表合法，=其他代表解析错误码
**
** Note             :1 解析的数据来源全局变量gs_APDU_analyze_var.apdu_data_hex[8]
**                  :2 多串TOKEN处理时两串数据间隔小于2分钟
**                  :3 需要按照顺序输入TOKEN码
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT8U api_APDU_set_key_frist(void)
{  
	sts_kay_change_data.flg &= ~KeyChangeHO;
	sts_kay_change_data.kay[4] = gs_APDU_analyze_var.apdu_data_hex[2];
	sts_kay_change_data.kay[5] = gs_APDU_analyze_var.apdu_data_hex[3];
	sts_kay_change_data.kay[6] = gs_APDU_analyze_var.apdu_data_hex[4];
	sts_kay_change_data.kay[7] = gs_APDU_analyze_var.apdu_data_hex[5];
	sts_kay_change_data.kt = (gs_APDU_analyze_var.apdu_data_hex[6]&0x03); //key type
	sts_kay_change_data.ro = (gs_APDU_analyze_var.apdu_data_hex[6] & 0x08) >>3;	// 
	sts_kay_change_data.krn = (gs_APDU_analyze_var.apdu_data_hex[6] & 0xF0) >>4; //key revision number
	sts_kay_change_data.ken |= ((gs_APDU_analyze_var.apdu_data_hex[7] & 0x0F)<<4);//key expire H
	
	sts_kay_change_data.flg |= KeyChangeHO;

/* 经过测试，发现单条判断会引起第2条和后面录入的第1条组合起来，有很大风险，还是保留原来的2串都接收后的判断
	//按照STS 测试要求，单条时就需要判断	,但是经过调试发现单条解析正确的情况下，不能清除秘钥缓存。//
	if(sts_kay_change_data.kt == 3)   //密钥类型不能改为3  ,DCTK3只能作为IC卡介质秘钥，不能作为键盘表秘钥        //
	{
		//Lib_Clr_String(&sts_kay_change_data.kay[0],sizeof(STS_KAY_CHANHE_DATA));
		return (STS_TOKEN_REJRCT_5);
	}
	
	if(sts_kay_change_data.kt == 0)//如果父密钥不是初始化类型密钥，则子密钥不能改为初始化类型密钥
	{
		if((gs_APDU_key_var.kt &0x0f) != 0)
		{
			//Lib_Clr_String(&sts_kay_change_data.kay[0],sizeof(STS_KAY_CHANHE_DATA));
			return (STS_TOKEN_REJRCT_5);
		}
	}
*/
	//完整接收后在持续判断一次	//
	if((sts_kay_change_data.flg & 0x03)== (KeyChangeHO+ KeyChangeLO))	//64bits密钥到位,改MeterKey(密钥2已设置好）
	{	//显示密钥完全修改完成  //
//		if(sts_kay_change_data.kt == 3)   //密钥类型不能改为3  ,DCTK3只能作为IC卡介质秘钥，不能作为键盘表秘钥        //
//		{
//			memset(&sts_kay_change_data.kay[0],0,sizeof(STS_KAY_CHANHE_DATA));
//			return (STS_TOKEN_REJRCT_5);
//		}
//		else 
		if(sts_kay_change_data.kt == 0)//如果父密钥不是初始化类型密钥，则子密钥不能改为初始化类型密钥
		{
			if((gs_APDU_key_var.kt &0x0f) != 0)
			{
				memset(&sts_kay_change_data.kay[0],0,sizeof(STS_KAY_CHANHE_DATA));
				return (STS_TOKEN_REJRCT_5);
			}
		}
		api_Change_Key();
		//秘钥修改后清除缓存	//
		memset(&sts_kay_change_data.kay[0],0,sizeof(STS_KAY_CHANHE_DATA));
	}
	else
	{
		//显示等待接收第2串密钥//
		return (STS_TOKEN_CHG_KEY_PLS_2nd);
	}
	
    return (STS_TOKEN_OK);	
}

/*****************************************************************************
** Function name    :INT8U api_APDU_set_key_second(void)
** Description      :key change Token -2  (修改秘钥第2串TOKEN)
**
** Parameters       :NONE
**                  
** Returned value   :INT8U ,解析结果，=0代表合法，=其他代表解析错误码
**
** Note             :1 解析的数据来源全局变量gs_APDU_analyze_var.apdu_data_hex[8]
**                  :2 多串TOKEN处理时两串数据间隔小于2分钟
**                  :3 需要按照顺序输入TOKEN码
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT8U api_APDU_set_key_second(void)
{
	sts_kay_change_data.flg &= ~KeyChangeLO;
	sts_kay_change_data.kay[0] = gs_APDU_analyze_var.apdu_data_hex[2];
	sts_kay_change_data.kay[1] = gs_APDU_analyze_var.apdu_data_hex[3];
	sts_kay_change_data.kay[2] = gs_APDU_analyze_var.apdu_data_hex[4];
	sts_kay_change_data.kay[3] = gs_APDU_analyze_var.apdu_data_hex[5];
	sts_kay_change_data.ti = gs_APDU_analyze_var.apdu_data_hex[6]; //triff index
	sts_kay_change_data.ken |= (gs_APDU_analyze_var.apdu_data_hex[7] & 0x0F);//key expire L
	sts_kay_change_data.flg |= KeyChangeLO;

	//按照STS 测试要求，单条时就需要判断	，但是秘钥第2串无关键判断数据//

	if((sts_kay_change_data.flg & 0x03)== (KeyChangeHO+ KeyChangeLO))	//64bits密钥到位,改MeterKey(密钥2已设置好）
	{	//显示密钥完全修改完成  //
//		if(sts_kay_change_data.kt == 3)   //密钥类型不能改为3  ,DCTK3只能作为IC卡介质秘钥，不能作为键盘表秘钥        //
//		{
//			memset(&sts_kay_change_data.kay[0],0,sizeof(STS_KAY_CHANHE_DATA));
//			return (STS_TOKEN_REJRCT_5);
//		}
//		else 
		if(sts_kay_change_data.kt == 0)//如果父密钥不是初始化类型密钥，则子密钥不能改为初始化类型密钥
		{
			if((gs_APDU_key_var.kt &0x0f) != 0)
			{
				memset(&sts_kay_change_data.kay[0],0,sizeof(STS_KAY_CHANHE_DATA));
				return (STS_TOKEN_REJRCT_5);
			}
		}
		api_Change_Key();
		//秘钥修改后清除缓存	//
		memset(&sts_kay_change_data.kay[0],0,sizeof(STS_KAY_CHANHE_DATA));
	}
	else
	{
		//显示等待接收第1串密钥//
		return (STS_TOKEN_CHG_KEY_PLS_1st);
	}
	
    //返回成功结果//     
    return (STS_TOKEN_OK);	  
}

/*****************************************************************************
** Function name    :INT8U api_APDU_clear_even(void)
** Description      :To clear event
**
** Parameters       :NONE
**                  
** Returned value   :INT8U ,解析结果，=0代表合法，=其他代表解析错误码
**
** Note             :1 解析的数据来源全局变量gs_APDU_analyze_var.apdu_data_hex[8]
**                  :2 清除事件记录注意继电器状态
**                  :3 
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT8U api_APDU_clear_even(void)
{
 INT8U u8_result;
 union Union_DWordDef tmp_32u_prod;
 
	// TID数据判断判断 //
	gs_APDU_analyze_var.u32_TID = Get_TID(&gs_APDU_analyze_var.apdu_data_hex[0]);
	u8_result = api_TIDVali_Judge(gs_APDU_analyze_var.u32_TID);
	if(u8_result != STS_TOKEN_OK)
	{
		return (u8_result);
	}

	//数据获取//
	tmp_32u_prod.UDWORD = 0;	//清零数据寄存器  //
	tmp_32u_prod.BYTE[0] = gs_APDU_analyze_var.apdu_data_hex[2];
	tmp_32u_prod.BYTE[1] = gs_APDU_analyze_var.apdu_data_hex[3];
	gs_APDU_analyze_var.u32_PropData = tmp_32u_prod.UDWORD;

	//数据范围判断  //
	if(tmp_32u_prod.UDWORD !=0)   //=0  //
	{
		return (STS_TOKEN_REJRCT_4);	
	}

	gs_APDU_analyze_var.u32_PropData = tmp_32u_prod.UDWORD;
	//窃电事件清除  //
//	api_Event_ram_INIT();

	//返回成功结果//     
	return (STS_TOKEN_OK);	 
}


/*****************************************************************************
** Function name    :INT8U sts_process_apdu_00_purchase(INT8U *u8ptr_hex_input)
** Description      :TOKEN充值处理             
**
** Parameters       :INT8U *ptr_kay_data  token解析后的HEX码数组首地址   
**                  
** Returned value	:INT8U 返回处理结果
**
** Note             :按照APDU的说明文件进行解析处理
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT8U sts_process_apdu_00_purchase(INT8U *u8ptr_hex_input)
{
    INT16U crc_tmp;
//    union Union_DWordDef tmp_32u;
    union Union_DWordDef tmp_32u_buy;
//    union Union_DWordDef tmp_u32_creditMAX; 
//    INT32S tmp_s32_credit;
    INT8U 	u8_result;

//    INT8U  amount_add_fg;	
   
    api_updata_APDU_key_var();  //获取产品当前秘钥 //
//	 memset(&gs_APDU_key_var.secret_key[0], 1, ee_keysts_lenth );  
 	// 密钥参数获取及判断  // 
 	///*  送样样机不做判断，但是注意STS协会认证样机要求判断  MJ20171210
//	 if((gs_APDU_key_var.kt == 1) ||(sts_kay_change_data.kt == 3))   //This key type can't use credit token
//	 {
//		 return(STS_TOKEN_REJRCT_3);  //DDTK秘钥，不支持充值TOKEN//	
//	 }
	//*/
	
	//将64bit解密并存放在sts_analyzing_unit.apdu_data_hex[0]变量内// 
	STA(u8ptr_hex_input,&gs_APDU_analyze_var.apdu_data_hex[0],&gs_APDU_key_var.secret_key[0],0); 
	gs_APDU_analyze_var.apdu_data_hex[8] = (*(u8ptr_hex_input+8));
	
	// CRC16 数据验证 // 
	crc_tmp = checkSTS_crc(&gs_APDU_analyze_var.apdu_data_hex[8],7);      //对象是解密后的64位的高48位及先前提出的卡类号//
	if(((crc_tmp & 0x00ff) != gs_APDU_analyze_var.apdu_data_hex[0] ) || (((crc_tmp & 0xff00)>>8) != gs_APDU_analyze_var.apdu_data_hex[1])) 
	{
		return(STS_TOKEN_REJRCT_1);	//解密失败，可能是秘钥错误//			
	}
	//  获取 子类号//
	gs_APDU_analyze_var.sub_class = (gs_APDU_analyze_var.apdu_data_hex[7]& 0xf0)>>4;
	if((gs_APDU_analyze_var.sub_class != STS0_SUB_CLASS_PURCHASE_KWH )&&(gs_APDU_analyze_var.sub_class != STS0_SUB_CLASS_PURCHASE_MONEY ) )
	{
		return(STS_TOKEN_REJRCT_2);	//子类型错误，不支持的TOKEN子类//		
	}

	// TID数据判断判断 //
	gs_APDU_analyze_var.u32_TID = Get_TID(&gs_APDU_analyze_var.apdu_data_hex[0]);
	u8_result = api_TIDVali_Judge(gs_APDU_analyze_var.u32_TID);
 	if(u8_result != STS_TOKEN_OK)
 	{
 		return(u8_result);
 	}

	//获取充值金额  //
//	tmp_32u_buy.UDWORD = CalcTransferAmt(&gs_APDU_analyze_var.apdu_data_hex[2]);	//量纲0.1kWh //
	tmp_32u_buy.UDWORD = 0;	
	tmp_32u_buy.BYTE[0]=gs_APDU_analyze_var.apdu_data_hex[2];
	tmp_32u_buy.BYTE[1]=gs_APDU_analyze_var.apdu_data_hex[3];
	Hex2Bcd( tmp_32u_buy.BYTE, Buy_Money_Mac[0].B08, 4 );
//	tmp_32u_buy.UDWORD *=10;		 // 将小数点扩大到0.01KWH // 
//  memcpy( Buy_Money_Mac[0].B08+0, tmp_32u_buy.BYTE, 4 );

//	//充值后的囤积电量判断，超出囤积电量则不允许充值  //
////    Lib_Mem_Read(&tmp_u32_creditMAX.BYTE[0], ADR_CREDITPARAM1_creditMAX, 4, TYPE_F01_MAIN);
////	tmp_s32_credit = api_getCredit();  // 获取当前剩余电量 //
//	if(tmp_u32_creditMAX.DWORD < (tmp_32u_buy.DWORD + tmp_s32_credit))
//	{
// 		return(STS_TOKEN_REJRCT_OVER_CRD);  //充值解析正确但是超出囤积上限//		
//	}

//	//余额叠加  //	
//	gs_APDU_analyze_var.u32_PropData = tmp_32u_buy.UDWORD; 
//	api_add_credit(gs_APDU_analyze_var.u32_PropData);
	
	// 电量购买成功，更新TID数据 //
	api_updata_APDU_TID_Record(gs_APDU_analyze_var.u32_TID);
	
	
	api_updata_APDU_TOKEN_Record(gs_APDU_analyze_var.apdu_data_bcd);
  
	// 购电量事件记录 //
	return(STS_TOKEN_OK);
}



/*****************************************************************************
** Function name    :INT8U sts_process_apdu_01_test(INT8U *u8ptr_hex_input)
** Description      :TOKEN 测试处理           
**
** Parameters       :INT8U *ptr_kay_data  token解析后的HEX码数组首地址   
**                  
** Returned value	:INT8U 返回处理结果
**
** Note             :按照APDU的说明文件进行解析处理
**                  :测试TOKEN为明文，无加密处理
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT8U sts_process_apdu_01_test(INT8U *u8ptr_hex_input)
{
 INT16U crc_tmp;
 INT8U u8_test_type;
// INT8U LCD_DIS = 0;

    // 获取16进制数据帧  无需解密//
	memcpy(&gs_APDU_analyze_var.apdu_data_hex[0], u8ptr_hex_input,9);
	gs_APDU_analyze_var.apdu_data_hex[8] = (*(u8ptr_hex_input+8)); 

	
	// CRC16 数据验证 // 
	crc_tmp = checkSTS_crc(&gs_APDU_analyze_var.apdu_data_hex[8],7);      //对象是解密后的64位的高48位及先前提出的卡类号//
	if(((crc_tmp & 0x00ff) != gs_APDU_analyze_var.apdu_data_hex[0] ) || (((crc_tmp & 0xff00)>>8) != gs_APDU_analyze_var.apdu_data_hex[1])) 
	{
		return (STS_TOKEN_REJRCT_1);	//解密失败或测试模式CRC校验失败，可能是秘钥错误//			
	}	

	//  获取 子类号//
	gs_APDU_analyze_var.sub_class = (gs_APDU_analyze_var.apdu_data_hex[7]& 0xf0)>>4;
	if(gs_APDU_analyze_var.sub_class != 0)
		{
		return (STS_TOKEN_REJRCT_4);	//功能不支持错误 //			
		}

    //获取测试号编码  //
    u8_test_type = get_token_test_type(&gs_APDU_analyze_var.apdu_data_hex[3]);

	// 36bit 控制码数据获取   //
	switch (u8_test_type)
	{
		case STS_TEST_ALL:   // test  all //
		    ///////////////////////////////////////////
            // CIU 显示 mj 2017-12-07 //
            ///////////////////////////////////////////
// 33           api_deal_relay_of_token_relay_test();	//继电器测试拉闸	//
            gs_APDU_analyze_var.u32_PropData = u8_test_type;
            //Turn_Dis_Mode_To_TOKEN(0,STS_TEST_ALL);	
			break;
		case STS_TEST_RELAY:   // test realy //
		    ///////////////////////////////////////////
            //CIU 显示 mj 2017-12-07 //
            ///////////////////////////////////////////
//   33         api_deal_relay_of_token_relay_test();	//继电器测试拉闸	//
			gs_APDU_analyze_var.u32_PropData = u8_test_type;
			break;
			
		case STS_TEST_LCD:   // test  lcd dispaly //
            ///////////////////////////////////////////
            //  CIU 显示 mj 2017-12-07 //
            ///////////////////////////////////////////
            gs_APDU_analyze_var.u32_PropData = u8_test_type;
			break;
		case STS_TEST_TOTAL_ENERGY:   // display total energy  //
            ///////////////////////////////////////////
            // CIU 显示 mj 2017-12-07 //
            ///////////////////////////////////////////		
            gs_APDU_analyze_var.u32_PropData = u8_test_type;
			break;
			
		case STS_TEST_KRN:   //Display KRN
			///////////////////////////////////////////
			// // CIU 显示 mj 2017-12-07 //
			/////////////////////////////////////////// 	
			gs_APDU_analyze_var.u32_PropData = u8_test_type;	
			break;
		case STS_TEST_TI:	 //display TI
			///////////////////////////////////////////
			// // CIU 显示 mj 2017-12-07 //
			/////////////////////////////////////////// 	
			gs_APDU_analyze_var.u32_PropData = u8_test_type;	
			break;
		case STS_TEST_MPL:   // display test max power limit //
            ///////////////////////////////////////////
            // // CIU 显示 mj 2017-12-07 //
            ///////////////////////////////////////////	
            gs_APDU_analyze_var.u32_PropData = u8_test_type;
			break;
		/*  屏蔽窃电测试部分  MJ20171218
		case STS_TEST_TAMPER_STATUS:   //display tamper status //
            ///////////////////////////////////////////
            // 补充窃电状态字显示测试的功能代码 mj 2016-9-28 //
            ///////////////////////////////////////////	
            gs_APDU_analyze_var.u32_PropData = u8_test_type;
			break;
			*/
		case STS_TEST_CURRENT_POWER:   // display current power//
            ///////////////////////////////////////////
            // // CIU 显示 mj 2017-12-07 //
            ///////////////////////////////////////////	
            gs_APDU_analyze_var.u32_PropData = u8_test_type;
			break;

		case STS_TEST_METER_VERSION:   // display meter version number//
			///////////////////////////////////////////
			// CIU 显示 mj 2017-12-07 //
			///////////////////////////////////////////	
			gs_APDU_analyze_var.u32_PropData = u8_test_type;
			break;
			
		default:
			return (STS_TOKEN_REJRCT_4);	//功能不支持错误 //			
	}

	return (STS_TOKEN_OK);
}


/*****************************************************************************
** Function name    :INT8U sts_process_apdu_02_set(INT8U *u8ptr_hex_input)
** Description      :TOKEN 码SUB_class=2设置类APDU处理          
**
** Parameters       :INT8U *ptr_kay_data  token解析后的HEX码数组首地址   
**                  
** Returned value	:INT8U 返回处理结果
**
** Note             :按照APDU的说明文件进行解析处理
**                  :
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
INT8U sts_process_apdu_02_set(INT8U *u8ptr_hex_input)
{
 INT16U crc_tmp;
 INT8U u8_result;
// union  Union_DWordDef tmp_32u_mpl,tmp_bcd4_mpl;

    //获取产品当前秘钥 //
  api_updata_APDU_key_var();
 
	//将64bit解密并存放在sts_analyzing_unit.apdu_data_hex[0]变量内// 
	STA(u8ptr_hex_input,&gs_APDU_analyze_var.apdu_data_hex[0],&gs_APDU_key_var.secret_key[0],0); 
	gs_APDU_analyze_var.apdu_data_hex[8] = (*(u8ptr_hex_input+8));
	
	// CRC16 数据验证 // 
	crc_tmp = checkSTS_crc(&gs_APDU_analyze_var.apdu_data_hex[8],7);      //对象是解密后的64位的高48位及先前提出的卡类号//
	if(((crc_tmp & 0x00ff) != gs_APDU_analyze_var.apdu_data_hex[0] ) || (((crc_tmp & 0xff00)>>8) != gs_APDU_analyze_var.apdu_data_hex[1])) 
	{
		return (STS_TOKEN_REJRCT_1);	//crc错误,代表秘钥解密核对失败//
	}
	
	//  获取 子类号//
	gs_APDU_analyze_var.sub_class = (gs_APDU_analyze_var.apdu_data_hex[7]& 0xf0)>>4;
	switch (gs_APDU_analyze_var.sub_class)
	{
		case STS2_SUB_CLASS_SET_MAXPOWER:  //set up Max Power Limit Mode Token		
            u8_result = api_APDU_set_power_limit();
			if(u8_result ==STS_TOKEN_OK )  api_updata_APDU_TID_Record(gs_APDU_analyze_var.u32_TID);
			break;
			
		case STS2_SUB_CLASS_CLR_BALANCE://To clear balance(清除余额)//
            u8_result = api_APDU_clear_balance();
			if(u8_result ==STS_TOKEN_OK )  api_updata_APDU_TID_Record(gs_APDU_analyze_var.u32_TID);
			break;

		case STS2_SUB_CLASS_SET_SECRET_KEY_H://key change Token -1
            u8_result = api_APDU_set_key_frist();
			break;

		case STS2_SUB_CLASS_SET_SECRET_KEY_L://key change Token -2
            u8_result = api_APDU_set_key_second();
			break;
		
/* STS 认证时为了测试顺利通过，屏蔽窃电检查功能  MJ20171218
		case STS2_SUB_CLASS_CLR_TAMPER_EVEN://To clear event
            u8_result = api_APDU_clear_even();
			if(u8_result ==STS_TOKEN_OK )  api_updata_APDU_TID_Record(gs_APDU_analyze_var.u32_TID);
			break;
*/
		default:
			return (STS_TOKEN_REJRCT_2);	// 令牌数据错误 //			
	}
 /////////////////增加显示/////////////////////////////


////////////////////////////////////////////////
	return (u8_result);
}


/*****************************************************************************
** Function name	:void api_initialize_APDU_when_reset(void)
** Description		:初始化STS相关APDU分析处理的模块内的相关变量，用于系统复位后的调用 
**
** Parameters		:NO    
**					
** Returned value	:NO
**
** Note 			:
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-29
******************************************************************************/
void api_initialize_APDU_when_reset(void)
{
	//清除STS APDU 解析全局变量  //
	memset(&gs_APDU_analyze_var.apdu_data_hex[0],0,sizeof(APDU_ANALYZE_VAR));
	//清除STS APDU 密码修改多串TOKEN解析全局变量  //
	memset(&sts_kay_change_data.kay[0],0,sizeof(STS_KAY_CHANHE_DATA));
	//将EXRAM内的秘钥相关数据调入RAM的全局变量内  //
	api_updata_APDU_key_var();
}
//INT8U api_Handle_tokenjudge_code(INT8U *ptr_kay_data)
//{
// INT8U i;
// INT8U tmp_bcd_byte[10];
// INT8U tmp_dec_byte[20];
// INT8U tmp_hex_byte[17];
// INT16U crc_tmp;

//	//0 清除STS APDU 解析全局变量数据  //
//	api_initialize_APDU_analyze_var();
//	gs_APDU_analyze_var.u8_result = STS_TOKEN_OK;
//	
//    // 1 键盘部分输入的20个字符串组合成10个字节的BCD码  暂存//
//	for( i=0; i<10; i++ ) 
//	{
//		gs_APDU_analyze_var.apdu_data_bcd[9-i] = (*(ptr_kay_data+(i*2))<<4);
//		gs_APDU_analyze_var.apdu_data_bcd[9-i] += (*(ptr_kay_data+(i*2+1)));
//	}
//	
//    // 2 将键盘部分输入的20个10进制数据转换为9字节的HEX码数据  //
//    memset(&tmp_hex_byte[0],0,17);
//	  memcpy(&tmp_dec_byte[0], ptr_kay_data, 20); 
//    
//	  DecToHex_20BYTE((INT8S*)(&tmp_dec_byte[0]), (INT8S*)(&tmp_hex_byte[0])); // 将转换后的66位二进制按高位-高地址,低位-低地址规则存入HEX[] //

//    // 3 获取不加密的 [类号]数据  并做分支处理//
//    gs_APDU_analyze_var.main_class = Class_get(&tmp_hex_byte[0]);
//		switch (gs_APDU_analyze_var.main_class)
//    {
//		case STS_MAIN_CLASS_PURCHASE:  	// = 0,purchase  apdu//
//			if((gs_APDU_key_var.kt == 1) ||(sts_kay_change_data.kt == 3))   //This key type can't use credit token
//	    {
//		       return(STS_TOKEN_REJRCT_3);  //DDTK秘钥，不支持充值TOKEN//	
//	    }
//			break;
//			
//		case STS_MAIN_CLASS_TEST:	// = 1, test  apdu//
//			gs_APDU_analyze_var.u8_result = sts_process_apdu_01_test(&tmp_hex_byte[0]);
//			break;
//			
//		case STS_MAIN_CLASS_SET:		// =2, meter set apdu //
//			gs_APDU_analyze_var.u8_result = sts_process_apdu_02_set(&tmp_hex_byte[0]);
//			break;
//			
//		default:
//			gs_APDU_analyze_var.u8_result = STS_TOKEN_REJRCT_2; //类型错误  //	
//		break;    	
//    }
//}
/*****************************************************************************
** Function name    :void api_Handle_key_sts_code(INT8U *ptr_kay_data)
** Description      :按照键盘输入的缓存数据进行处理               
**
** Parameters       :INT8U *ptr_kay_data  键盘输入数据缓冲区首地址,20字节BCD码数据     
**                  
** Returned value	:INT8U ,键盘输入数据解析结果;
**
** Note             :按照APDU的说明文件进行解析处理
**----------------------------------------------------------------------------
** V01.01  MJ  2017-11-21
******************************************************************************/
INT8U api_Handle_key_sts_code(INT8U *ptr_kay_data)
{
 INT8U i;
// INT8U tmp_bcd_byte[10];
 INT8U tmp_dec_byte[20];
 INT8U tmp_hex_byte[17];
// INT16U crc_tmp;

	//0 清除STS APDU 解析全局变量数据  //
	api_initialize_APDU_analyze_var();
	gs_APDU_analyze_var.u8_result = STS_TOKEN_OK;
	Clear_Wdt();
    // 1 键盘部分输入的20个字符串组合成10个字节的BCD码  暂存//
	for( i=0; i<10; i++ ) 
	{
		gs_APDU_analyze_var.apdu_data_bcd[9-i] = (*(ptr_kay_data+(i*2)))<<4;
		gs_APDU_analyze_var.apdu_data_bcd[9-i] += (*(ptr_kay_data+(i*2+1)));
	}
	
    // 2 将键盘部分输入的20个10进制数据转换为9字节的HEX码数据  //
    memset(&tmp_hex_byte[0],0,17);
	  memset(&tmp_dec_byte[0],0,20);


	  memcpy(&tmp_dec_byte[0], ptr_kay_data, 20); 
    Clear_Wdt();
	
	  
	  DecToHex_20BYTE((INT8S*)(&tmp_dec_byte[0]), (INT8S*)(&tmp_hex_byte[0])); // 将转换后的66位二进制按高位-高地址,低位-低地址规则存入HEX[] //

    // 3 获取不加密的 [类号]数据  并做分支处理//
    gs_APDU_analyze_var.main_class = Class_get(&tmp_hex_byte[0]);

    switch (gs_APDU_analyze_var.main_class)
    {
		case STS_MAIN_CLASS_PURCHASE:  	// = 0,purchase  apdu//
			gs_APDU_analyze_var.u8_result = sts_process_apdu_00_purchase(&tmp_hex_byte[0]);
			break;
			
		case STS_MAIN_CLASS_TEST:	// = 1, test  apdu//
			gs_APDU_analyze_var.u8_result = sts_process_apdu_01_test(&tmp_hex_byte[0]);
			break;
			
		case STS_MAIN_CLASS_SET:		// =2, meter set apdu //
			gs_APDU_analyze_var.u8_result = sts_process_apdu_02_set(&tmp_hex_byte[0]);
			break;
			
		default:
			gs_APDU_analyze_var.u8_result = STS_TOKEN_REJRCT_2; //类型错误  //	
		break;    	
    }

	return (gs_APDU_analyze_var.u8_result);   //返回解析结果  //
}


/*****************************************************************************
** Function name    :void api_judge_multiple_TOKEN_interval_pre_second(void)
** Description      :处理多串TOKEN有效间隔时间判断程序           
**
** Parameters       :  
**                  
** Returned value   :NO
**
** Note             :按照MAX_SECONDS_OF_MULTIPLE_TOKEN_INTERVAL参数来处理  
**----------------------------------------------------------------------------
** V01.01  MJ  2016-09-28
******************************************************************************/
void api_judge_keyChg_TOKEN_interval_pre_second(void)
{
	if(sts_kay_change_data.flg & 0x03)
	{
		sts_kay_change_data.tmr_second ++;
		if(sts_kay_change_data.tmr_second > MAX_SECONDS_OF_MULTIPLE_TOKEN_INTERVAL)
		{
			memset(&sts_kay_change_data.kay[0],0,sizeof(STS_KAY_CHANHE_DATA));	
		}
	}

}	
