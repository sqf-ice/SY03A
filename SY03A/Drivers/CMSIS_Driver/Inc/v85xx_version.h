/**
*************************(C) COPYRIGHT 2017 Vangotech***************************
* @file    v85xx_version.h
* @author  zhouyf
* @version V1.0
* @date    
* @brief   Version library.
*******************************************************************************/

#ifndef __V85XX_VERSION_H
#define __V85XX_VERSION_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "v85xx.h"
   
#define DRIVER_VERSION(major,minor) (((major) << 8) | (minor))

/* Exported Functions ------------------------------------------------------- */
   
/**
  * @brief  Read receive data register.
  * @param  None
  * @retval Version value
  */
uint16_t V85XX_GetDriveVersion(void);

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
