/**
* @file    main.c
* @author  MCD Application Team
* @version V1.0.0
* @date    2017/02/05
* @brief   Main program body.
******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Error_Handle.
  * @param  None
  * @retval None
  */
void Error_Handle(void)
{
  while (1)
  {
    ;
  }
}

/**
  * @brief  Clock_Init:
              - PLLL input clock    : External 32K crystal
              - PLLL frequency      : 26M
              - AHB Clock source    : PLLL
              - AHB Clock frequency : 26M (PLLL divided by 1)
              - APB Clock frequency : 13M (AHB Clock divided by 2)
  * @param  None
  * @retval None
  */
void Clock_Init(void)
{
  CLK_InitTypeDef CLK_Struct;

  CLK_Struct.ClockType = CLK_TYPE_AHBSRC \
                        |CLK_TYPE_PLLL   \
                        |CLK_TYPE_HCLK   \
                        |CLK_TYPE_PCLK;
  CLK_Struct.AHBSource      = CLK_AHBSEL_LSPLL;
  CLK_Struct.PLLL.Frequency = CLK_PLLL_26_2144MHz;
  CLK_Struct.PLLL.Source    = CLK_PLLLSRC_XTALL;
  CLK_Struct.PLLL.State     = CLK_PLLL_ON;
  CLK_Struct.HCLK.Divider   = 1;
  CLK_Struct.PCLK.Divider   = 2;
  CLK_ClockConfig(&CLK_Struct);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  __IO uint32_t start_tick;
  
  /* Clock init, system clock 26M */
  Clock_Init();
  /* SystemTick init */
  System_InitTick();
  
  /* IOB1 Output */
  Driver_GPIOB.Control(ARM_GPIO_MODE_OUTPUT_CMOS, ARM_GPIO_Pin_7);
  
  /* IOA15 Output */
  Driver_GPIOA.Control(ARM_GPIO_MODE_OUTPUT_CMOS, ARM_GPIO_Pin_15);
  while (1)
  {
    /* Delay 200ms, Toggle IO */
    start_tick = System_GetTick();
    while ((System_GetTick() - start_tick) < 200);
    Driver_GPIOA.TogglePin(ARM_GPIO_Pin_15);
    Driver_GPIOB.TogglePin(ARM_GPIO_Pin_7);
  }
}

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
