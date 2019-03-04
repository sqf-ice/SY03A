/**
  ******************************************************************************
  * @file    v85xx_CodeRAM.c 
  * @author  VT Application Team
  * @version V1.0.0
  * @date    2017-06-22
  * @brief   Codes executed in SRAM.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "v85xx_CodeRAM.h"


/**
  * @brief  Flash deep standby, enter idle mode.
  * @note   This function is executed in RAM.
  * @param  None
  * @retval None
  */
__RAM_FUNC void PMU_EnterIdle_FlashDSTB(void)
{
  /* Flash deep standby */
  FLASH->PASS = 0x55AAAA55;
  FLASH->DSTB = 0xAA5555AA;
  /* Enter Idle mode */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
  __WFI();
}

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
