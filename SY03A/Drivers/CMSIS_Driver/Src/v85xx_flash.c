/**
  ******************************************************************************
  * @file    v85xx_flash.c 
  * @author  VT Application Team
  * @version V1.0.0
  * @date    2017-06-22
  * @brief   FLASH library.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "v85xx_flash.h"  
#include "v85xx_clk.h"

/* FLASH Keys */
#define FLASH_PASS_KEY      0x55AAAA55
#define FLASH_SERASE_KEY    0xAA5555AA
#define FLASH_CERASE_KEY    0xAA5555AA
#define FLASH_DSTB_KEY      0xAA5555AA

#define FLASH_MODE_MASK     0x1F3

/**
  * @brief  FLASH mode initialization.
  * @param  CSMode:
                FLASH_CSMODE_DISABLE 
                FLASH_CSMODE_ALWAYSON
                FLASH_CSMODE_TIM2OF   
                FLASH_CSMODE_RTC      
  * @retval None
  */
void FLASH_Init(uint32_t CSMode)
{
  uint32_t tmp;
  
  /* Check parameters */
  assert_parameters(IS_FLASH_CSMODE(CSMode));  
 
  tmp = FLASH->CTRL;
  tmp &= ~FLASH_MODE_MASK;
  tmp |= CSMode;
  FLASH->CTRL = tmp;
}

/**
  * @brief  Configure FLASH interrupt.
  * @param  IntMask:
                FLASH_INT_CS
            NewState:
                ENABLE
                DISABLE
  * @retval None
  */
void FLASH_INTConfig(uint32_t IntMask, uint32_t NewState)
{
  uint32_t tmp;
  
  /* Check parameters */
  assert_parameters(IS_FLASH_INT(IntMask));
  assert_parameters(IS_FUNCTIONAL_STATE(NewState)); 
  
  tmp = FLASH->CTRL;
  tmp &= ~IntMask;
  if (NewState == ENABLE)
  {
    tmp |= IntMask;
  }
  FLASH->CTRL = tmp;
}

/**
  * @brief  Init FLASH 1USCYCLE.
  * @param  None
  * @retval None
  */
void FLASH_CycleInit(void)
{
  uint32_t hclk;
  
  hclk = CLK_GetHCLKFreq();
  MISC2->FLASHWC = (hclk/1000000 - 1)<<8;
}

/**
  * @brief  Erase FLASH sector.
  * @param  SectorAddr: sector address.
  * @retval None
  */
void FLASH_SectorErase(uint32_t SectorAddr)
{
  /* Check parameters */
  assert_parameters(IS_FLASH_ADDRESS(SectorAddr));
  
  /* Unlock flash */
  FLASH->PASS = FLASH_PASS_KEY;
  
  FLASH->PGADDR = SectorAddr;
  FLASH->SERASE = FLASH_SERASE_KEY;
  while (FLASH->SERASE != 0);
  
  /* Lock flash */
  FLASH->PASS = 0;
}

/**
  * @brief  FLASH word progarm.
  * @param  Addr: program start address
            WordBuffer: word's buffer point to write
            Length: the sum of program operation
  * @retval None  
  */
void FLASH_ProgramWord(uint32_t Addr, uint32_t *WordBuffer, uint32_t Length)
{
  uint32_t i;
  
  /* Check parameters */
  assert_parameters(IS_FLASH_ADRRW(Addr));
  
  /* Unlock flash */
  FLASH->PASS = FLASH_PASS_KEY;
  
  for (i=0; i<Length; i++)
  {
    FLASH->PGADDR = Addr + 4*i;
    FLASH->PGDATA = *(WordBuffer++);
    while (FLASH->STS != 1);
  }
  
  /* Lock flash */
  FLASH->PASS = 0;
}

/**
  * @brief  FLASH half-word progarm.
  * @param  Addr: program start address
            HWordBuffer: half-word's buffer point to write
            Length: the sum of program operation
  * @retval None  
  */
void FLASH_ProgramHWord(uint32_t Addr, uint16_t *HWordBuffer, uint32_t Length)
{
  uint32_t i;
  
  /* Check parameters */
  assert_parameters(IS_FLASH_ADRRHW(Addr));
  
  /* Unlock flash */
  FLASH->PASS = FLASH_PASS_KEY;
  
  for (i=0; i<Length; i++)
  {
    FLASH->PGADDR = Addr + 2*i;
    if (((Addr + 2*i)&0x3) == 0)
      *((__IO uint16_t*)(&FLASH->PGDATA)) = *(HWordBuffer++);
    else
      *((__IO uint16_t*)(&FLASH->PGDATA ) + 1) = *(HWordBuffer++);
    while (FLASH->STS != 1);
  }
  
  /* Lock flash */
  FLASH->PASS = 0;
}

/**
  * @brief  FLASH byte progarm.
  * @param  Addr: program start address
            ByteBuffer: byte's buffer point to write
            Length: the sum of program operation
  * @retval None  
  */
void FLASH_ProgramByte(uint32_t Addr, uint8_t *ByteBuffer, uint32_t Length)
{
  uint32_t i;
  
  /* Check parameters */
  assert_parameters(IS_FLASH_ADDRESS(Addr));
  
  /* Unlock flash */
  FLASH->PASS = FLASH_PASS_KEY;
  
  for (i=0; i<Length; i++)
  {
    FLASH->PGADDR = Addr + i;
    if (((Addr + i)&0x3) == 0)
      *((__IO uint8_t*)(&FLASH->PGDATA)) = *(ByteBuffer++);
    else if (((Addr + i)&0x3) == 1)
      *((__IO uint8_t*)(&FLASH->PGDATA) + 1) = *(ByteBuffer++);
    else if (((Addr + i)&0x3) == 2)
      *((__IO uint8_t*)(&FLASH->PGDATA) + 2) = *(ByteBuffer++);
    else
      *((__IO uint8_t*)(&FLASH->PGDATA) + 3) = *(ByteBuffer++);
    while (FLASH->STS != 1); 
  }
  
  /* Lock flash */
  FLASH->PASS = 0;
}

/**
  * @brief  Get Write status.
  * @param  None.
  * @retval FLASH_WSTA_BUSY  
            FLASH_WSTA_FINISH
  */
uint32_t FLASH_GetWriteStatus(void)
{
  if (FLASH->STS == 1)
  {
    return FLASH_WSTA_FINISH;
  }
  else
  {
    return FLASH_WSTA_BUSY;
  }
}

/**
  * @brief  Set checksum range.
  * @param  AddrStart: check sum start address
          AddrEnd:check sum end address
  * @retval None  
  */
void FLASH_SetCheckSumRange(uint32_t AddrStart, uint32_t AddrEnd)
{
  /* Check parameters */
  assert_parameters(IS_FLASH_CHECKSUMADDR(AddrStart,AddrEnd));
  
  FLASH->CSSADDR = AddrStart;
  FLASH->CSEADDR = AddrEnd;
}

/**
  * @brief  Set checksum compare value.
  * @param  Checksum: checksum compare value
  * @retval None  
  */
void FLASH_SetCheckSumCompValue(uint32_t Checksum)
{
  FLASH->CSCVALUE = Checksum;
}

/**
  * @brief  Get FLASH checksum value.
  * @param  None
  * @retval Checksum  
  */
uint32_t FLASH_GetCheckSum(void)
{
  return FLASH->CSVALUE;
}


/**
  * @brief  Get FLASH interrupt status.
  * @param  IntMask:
                FLASH_INT_CS
  * @retval 1: interrupt status set
            0: interrupt status reset
  */
uint8_t FLASH_GetINTStatus(uint32_t IntMask)
{
  /* Check parameters */
  assert_parameters(IS_FLASH_INT(IntMask));

  if (FLASH->INT&FLASH_INT_CSERR)
  {
    return 1;
  }
  else
  {
    return 0;
  } 
}

/**
  * @brief  Clear FLASH interrupt status.
  * @param  IntMask:
                FLASH_INT_CS
  * @retval None
  */
void FLASH_ClearINTStatus(uint32_t IntMask)
{
  /* Check parameters */
  assert_parameters(IS_FLASH_INT(IntMask));
  
  if (IntMask == FLASH_INT_CS)
  {
    FLASH->INT = FLASH_INT_CSERR;
  }
}

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
