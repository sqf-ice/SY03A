/**
  ******************************************************************************
  * @file    v85xx_lcd.h 
  * @author  VT Application Team
  * @version V1.0.0
  * @date    2017-06-22
  * @brief   LCD library.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#ifndef __V85XX_LCD_H
#define __V85XX_LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "v85xx.h"
   
/* LCD IO typedef */
typedef struct 
{
  __IO uint32_t    *GPIO;
       uint16_t     Pin;
}LCD_SEGIO;

typedef struct 
{
  uint32_t Type;
  uint32_t Drv;
  uint32_t FRQ;
  uint32_t SWPR;
  uint32_t FBMODE;
  uint32_t BKFILL;
} LCD_InitType;
//Type
#define LCD_TYPE_4COM   LCD_CTRL_TYPE_4COM
#define LCD_TYPE_6COM   LCD_CTRL_TYPE_6COM
#define LCD_TYPE_8COM   LCD_CTRL_TYPE_8COM
//DrivingRes
#define LCD_DRV_300     LCD_CTRL_DRV_300KOHM
#define LCD_DRV_600     LCD_CTRL_DRV_600KOHM
#define LCD_DRV_150     LCD_CTRL_DRV_150KOHM
#define LCD_DRV_200     LCD_CTRL_DRV_200KOHM
//ScanFRQ
#define LCD_FRQ_64H     LCD_CTRL_FRQ_64HZ
#define LCD_FRQ_128H    LCD_CTRL_FRQ_128HZ
#define LCD_FRQ_256H    LCD_CTRL_FRQ_256HZ
#define LCD_FRQ_512H    LCD_CTRL_FRQ_512HZ
//SwitchMode
#define LCD_FBMODE_BUFA         LCD_CTRL2_FBMODE_BUFA
#define LCD_FBMODE_BUFAB        LCD_CTRL2_FBMODE_BUFAANDBUFB
#define LCD_FBMODE_BUFABLANK    LCD_CTRL2_FBMODE_BUFAANDBLANK
//BlankFill
#define LCD_BKFILL_1       LCD_CTRL2_BKFILL
#define LCD_BKFILL_0       0 

//BiasSelection
#define LCD_BMODE_DIV3                     0
#define LCD_BMODE_DIV4       ANA_REG6_LCD_BMODE

//VLCDSelection
#define LCD_VLCD_0           0
#define LCD_VLCD_INC60MV     1
#define LCD_VLCD_INC120MV    2
#define LCD_VLCD_INC180MV    3
#define LCD_VLCD_INC240MV    4
#define LCD_VLCD_INC300MV    5
#define LCD_VLCD_DEC60MV     6
#define LCD_VLCD_DEC120MV    7
#define LCD_VLCD_DEC180MV    8
#define LCD_VLCD_DEC240MV    9
#define LCD_VLCD_DEC300MV    10
#define LCD_VLCD_DEC360MV    11
#define LCD_VLCD_DEC420MV    12
#define LCD_VLCD_DEC480MV    13
#define LCD_VLCD_DEC540MV    14
#define LCD_VLCD_DEC600MV    15

/* Private macros ------------------------------------------------------------*/
#define IS_LCD_TYPE(__TYPE__)  (((__TYPE__) == LCD_TYPE_4COM) ||\
                                ((__TYPE__) == LCD_TYPE_6COM) ||\
                                ((__TYPE__) == LCD_TYPE_8COM))

#define IS_LCD_DRV(__DRV__)  (((__DRV__) == LCD_DRV_300) ||\
                              ((__DRV__) == LCD_DRV_600) ||\
                              ((__DRV__) == LCD_DRV_150) ||\
                              ((__DRV__) == LCD_DRV_200)) 

#define IS_LCD_FRQ(__FRQ__)  (((__FRQ__) == LCD_FRQ_64H)  ||\
                              ((__FRQ__) == LCD_FRQ_128H) ||\
                              ((__FRQ__) == LCD_FRQ_256H) ||\
                              ((__FRQ__) == LCD_FRQ_512H))

#define IS_LCD_SWPR(__SWPR__)  ((__SWPR__) <= 0xFFUL)   

#define IS_LCD_FBMODE(__FBMODE__)  (((__FBMODE__) == LCD_FBMODE_BUFA)  ||\
                                    ((__FBMODE__) == LCD_FBMODE_BUFAB) ||\
                                    ((__FBMODE__) == LCD_FBMODE_BUFABLANK))                                   

#define IS_LCD_BKFILL(__BKFILL__)  (((__BKFILL__) == LCD_BKFILL_1) || ((__BKFILL__) == LCD_BKFILL_0))

#define IS_LCD_BMODE(__BMODE__)  (((__BMODE__) == LCD_BMODE_DIV3) ||\
                                  ((__BMODE__) == LCD_BMODE_DIV4))

#define IS_LCD_VLCD(__VLCD__)  (((__VLCD__) == LCD_VLCD_0)        ||\
                                ((__VLCD__) == LCD_VLCD_INC60MV)  ||\
                                ((__VLCD__) == LCD_VLCD_INC120MV)  ||\
                                ((__VLCD__) == LCD_VLCD_INC180MV) ||\
                                ((__VLCD__) == LCD_VLCD_INC240MV) ||\
                                ((__VLCD__) == LCD_VLCD_INC300MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC60MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC120MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC180MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC240MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC300MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC360MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC420MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC480MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC540MV) ||\
                                ((__VLCD__) == LCD_VLCD_DEC600MV))

/* Exported Functions ------------------------------------------------------- */
/* LCD Exported Functions Group1: 
                                  (De)Initialization -------------------------*/
void LCD_DeInit(void);
void LCD_StructInit(LCD_InitType *LCD_InitStruct);
void LCD_Init(LCD_InitType *InitStruct);
/* LCD Exported Functions Group1: 
                                  MISC Configuration -------------------------*/
void LCD_Cmd(uint32_t NewState);
void LCD_SetSEG(uint32_t SegCtrl0, uint32_t SegCtrl1, uint16_t SegCtrl2);
void LCD_BiasModeConfig(uint32_t BiasSelection);
uint32_t LCD_VoltageConfig(uint32_t VLCDSelection);

#ifdef __cplusplus
}
#endif
     
#endif /* __V85XX_LCD_H */

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/

