/**
  ******************************************************************************
  * @file    v85xx_lib.c 
  * @author  VT Application Team
  * @version V1.0.0
  * @date    2017-06-22
  * @brief   Dirver configuration.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef __V85XX_LIB_H
#define __V85XX_LIB_H

/* ########################## Assert Selection ############################## */

#define ASSERT_NDEBUG    1

/* Includes ------------------------------------------------------------------*/
/**
  * @brief Include module's header file 
  */
#include "v85xx_ana.h"
#include "v85xx_adc.h"
#include "v85xx_adc_tiny.h"
#include "v85xx_clk.h"
#include "v85xx_comp.h"
#include "v85xx_crypt.h"
#include "v85xx_dma.h"
#include "v85xx_flash.h"
#include "v85xx_gpio.h"
#include "v85xx_i2c.h"
#include "v85xx_iso7816.h"
#include "v85xx_lcd.h"
#include "v85xx_misc.h"
#include "v85xx_pmu.h"
#include "v85xx_pwm.h"
#include "v85xx_rtc.h"
#include "v85xx_spi.h"
#include "v85xx_tmr.h"
#include "v85xx_u32k.h"
#include "v85xx_uart.h"
#include "v85xx_version.h"
#include "v85xx_wdt.h"
#include "v85xx_LoadNVR.h"
#include "v85xx_CodeRAM.h"
#include "v85xx_cortex.h"

/* Exported macro ------------------------------------------------------------*/
#ifndef  ASSERT_NDEBUG
  #define assert_parameters(expr) ((expr) ? (void)0U : assert_errhandler((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_errhandler(uint8_t* file, uint32_t line);
#else
  #define assert_parameters(expr) ((void)0U)
#endif /* ASSERT_NDEBUG */ 

#endif

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
