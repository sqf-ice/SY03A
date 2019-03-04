/**
* @file    v_stdio.h
* @author  weaktea
* @version V1.0
* @date    2016/07/05
* @brief   standard printf.
******************************************************************************/

#include "v_stdio.h"
#include "v85xx.h"
#include <stdio.h>

/**
  * @brief  printf init.
  * @param  None
  * @retval None
  */
void Stdio_Init(void)
{
  UART5->BAUDDIV = CLK_GetPCLKFreq()/115200;
  UART5->CTRL = UART_CTRL_TXEN;
}

/**
  * @brief   fputc.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
  //while (UART5->STATE&UART_STATE_TXFULL);
  UART5->DATA = ch;
  while (!(UART5->STATE&UART_STATE_TXDONE)); 
  UART5->STATE = UART_STATE_TXDONE;
  return ch;
}

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
