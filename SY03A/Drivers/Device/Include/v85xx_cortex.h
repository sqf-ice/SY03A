/**
  ******************************************************************************
  * @file    v85xx_Cortex.h 
  * @author  VT Application Team
  * @version V1.0.0
  * @date    2017-06-22
  * @brief   Cortex module driver.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef __V85XX_CORTEX_H
#define __V85XX_CORTEX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "v85xx.h"   


#define IS_CORTEX_NVIC_DEVICE_IRQ(IRQ)  ((IRQ) >= 0x00)
   
#define IS_CORTEX_NVIC_PREEMPTION_PRIORITY(PRIORITY)  ((PRIORITY) < 0x4)
   
/* Exported Functions ------------------------------------------------------- */
void     CORTEX_SetPriority_ClearPending_EnableIRQ(IRQn_Type IRQn, uint32_t Priority);
   
void     CORTEX_NVIC_EnableIRQ(IRQn_Type IRQn);
void     CORTEX_NVIC_DisableIRQ(IRQn_Type IRQn);
uint32_t CORTEX_NVIC_GetPendingIRQ(IRQn_Type IRQn);
void     CORTEX_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void     CORTEX_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
uint32_t CORTEX_NVIC_GetPriority(IRQn_Type IRQn);
void     CORTEX_NVIC_SetPriority(IRQn_Type IRQn, uint32_t Priority);
void     CORTEX_NVIC_SystemReset(void);
uint32_t CORTEX_SystemTick_Config(uint32_t TicksNum);

#ifdef __cplusplus
}
#endif

#endif /* __V85XX_CORTEX_H */

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
