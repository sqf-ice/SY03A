/**
  ******************************************************************************
  * @file    v85xx_CodeRAM.h 
  * @author  VT Application Team
  * @version V1.0.0
  * @date    2017-06-22
  * @brief   Codes executed in SRAM.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef __V85XX_CODERAM_H
#define __V85XX_CODERAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "v85xx.h"   


#ifdef __V85XX_EWARM_USED  /* EWARM */
  #define __RAM_FUNC 	__ramfunc
#else                      /* MDK-ARM */
  #define __RAM_FUNC	__attribute__((used))
#endif

/* Exported Functions ------------------------------------------------------- */

__RAM_FUNC void PMU_EnterIdle_FlashDSTB(void);

#ifdef __cplusplus
}
#endif

#endif /* __V85XX_CODERAM_H */

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
