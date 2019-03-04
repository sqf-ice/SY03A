/**
  ******************************************************************************
  * @file    v85xx_wdt.h 
  * @author  VT Application Team
  * @version V1.0.0
  * @date    2017-06-22
  * @brief   WDT library.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#ifndef __V85XX_WDT_H
#define __V85XX_WDT_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "v85xx.h"
 
#define  WDT_2_SECS         PMU_WDTEN_WDTSEL_0
#define  WDT_1_SECS         PMU_WDTEN_WDTSEL_1
#define  WDT_0_5_SECS       PMU_WDTEN_WDTSEL_2
#define  WDT_0_25_SECS      PMU_WDTEN_WDTSEL_3

/* Private macros ------------------------------------------------------------*/
#define IS_WDT_PERIOD(__PERIOD__)  (((__PERIOD__) == WDT_2_SECS)   ||\
                                    ((__PERIOD__) == WDT_1_SECS)   ||\
                                    ((__PERIOD__) == WDT_0_5_SECS) ||\
                                    ((__PERIOD__) == WDT_0_25_SECS))

/* Exported Functions ------------------------------------------------------- */
void WDT_Enable(void);
void WDT_Disable(void);
void WDT_Clear(void);
void WDT_SetPeriod(uint32_t period);
uint16_t WDT_GetCounterValue(void);
   
#ifdef __cplusplus
}
#endif
     
#endif  /* __V85XX_WDT_H */

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
