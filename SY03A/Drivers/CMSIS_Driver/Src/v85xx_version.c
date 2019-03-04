/**
*************************(C) COPYRIGHT 2017 Vangotech***************************
* @file    v85xx_version.c
* @author  zhouyf
* @version V1.0
* @date    2017/09/19
* @brief   Version library.
*******************************************************************************/
#include "v85xx_version.h"

#define V85XX_DriveVersion  DRIVER_VERSION(4, 0)

/**
  * @brief  Get V85XX driver's current version.
  * @param  None
  * @retval Version value
  *           Bit[15:8] : Major version
  *           Bit[7:0]  : Minor version 
  */
uint16_t V85XX_GetDriveVersion(void)
{
  return (V85XX_DriveVersion);
}

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
