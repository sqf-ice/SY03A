/**
  ******************************************************************************
  * @file    v85xx_pmu.c 
  * @author  VT Application Team
  * @version V1.1.0
  * @date    2018-05-10
  * @brief   PMU library.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "v85xx_pmu.h"
#include "v85xx_gpio.h"
#include "v85xx_CodeRAM.h"
#include "v85xx_clk.h"
#include "v85xx_cortex.h"

#define DSLEEPPASS_KEY  0xAA5555AA
#define DSLEEPEN_KEY    0x55AAAA55

extern __IO uint32_t ana_reg3_tmp;

/**
  * @brief  Enter Deep sleep mode.
  * @param  None
  * @retval 1: Current mode is debug mode, function failed.
  *         2: Enter deep-sleep mode failed.
  */
uint32_t PMU_EnterDSleepMode(void)
{
  uint32_t hclk;
  
  /* Current MODE is 0, debug mode, return error */
  if (!(PMU->STS & PMU_STS_MODE))
    return 1;
  
  /* Enter deep sleep when WKU event is cleared */
  while (PMU->DSLEEPEN & PMU_DSLEEPEN_WKU)
  {
  }
  
  /* Flash 1USCYCLE configure */
  hclk = CLK_GetHCLKFreq();
  MISC2->FLASHWC = (hclk/1000000 - 1)<<8;  
  
  PMU->DSLEEPPASS = DSLEEPPASS_KEY;
  PMU->DSLEEPEN = DSLEEPEN_KEY;
  
  return 2;
}

/**
  * @brief  Enter idel mode.
  * @note   Any interrupt generate to CPU will break idle mode.
  * @param  None
  * @retval None
  */
void PMU_EnterIdleMode(void)
{
  /* Clear SLEEPDEEP bit of Cortex-M0 System Control Register */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
  
  __WFI();
}

/**
  * @brief  Enter sleep mode.
  * @param  None
  * @retval 1: Current mode is debug mode, function failed.
  *         0: Quit deep-sleep mode ucceeded.
  */
uint32_t PMU_EnterSleepMode(void)
{
  uint32_t hclk;
  uint32_t tmp;

  /* Current MODE is 0, debug mode, return error */
  if (!(PMU->STS & PMU_STS_MODE))
    return 1;
  
  /* Flash 1USCYCLE configure */
  hclk = CLK_GetHCLKFreq();  
  MISC2->FLASHWC = (hclk/1000000 - 1)<<8;
  
  /*VDD3 is on when sleep*/
  tmp = ANA->REGA ;
  tmp |= ANA_REGA_VDD3_OFF;
  ANA->REGA = tmp;

  /* Set SLEEPDEEP bit of Cortex-M0 System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
  __WFI();
  
  return 0;
}

/**
  * @brief  PMU interrupt configuration.
  * @param  INTMask:(between PMU_INT_IOAEN,PMU_INT_32K and PMU_INT_6M, can use the | operator)
                PMU_INT_IOAEN
                PMU_INT_32K
                PMU_INT_6M
            NewState:
                ENABLE
                DISABLE
  * @retval None
  */
void PMU_INTConfig(uint32_t INTMask, uint32_t NewState)
{
  /* Check parameters */
  assert_parameters(IS_PMU_INT(INTMask));
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));   
  
  if (NewState == ENABLE)
  {
    PMU->CONTROL |= INTMask;
  }
  else
  {
    PMU->CONTROL &= ~INTMask;
  }
}

/**
  * @brief  Get interrupt status.
  * @param  INTMask:
                PMU_INTSTS_32K
                PMU_INTSTS_6M
                PMU_INTSTS_EXTRST
                PMU_INTSTS_PORST
                PMU_INTSTS_DPORST
  * @retval 1:status set
            0:status reset
  */
uint8_t PMU_GetINTStatus(uint32_t INTMask)
{
  /* Check parameters */
  assert_parameters(IS_PMU_INTFLAGR(INTMask));

  if (PMU->STS&INTMask)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Clear interrupt status.
  * @param  INTMask:specifies the flag to clear.
                This parameter can be any combination of the following values
                PMU_INTSTS_32K
                PMU_INTSTS_6M
                PMU_INTSTS_EXTRST
                PMU_INTSTS_PORST
                PMU_INTSTS_DPORST
  * @retval None
  */
void PMU_ClearINTStatus(uint32_t INTMask)
{
  /* Check parameters */
  assert_parameters(IS_PMU_INTFLAGC(INTMask));

  PMU->STS = INTMask;
}

/**
  * @brief  Get status.
  * @param  Mask:
                PMU_STS_32K 
                PMU_STS_6M
  * @retval 1:status set
            0:status reset
  */
uint8_t PMU_GetStatus(uint32_t Mask)
{
  /* Check parameters */
  assert_parameters(IS_PMU_FLAG(Mask));
  
  if (PMU->STS&Mask)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Get all IOA interrupt status.
  * @param  None
  * @retval IOA's interrupt status
  */
uint16_t PMU_GetIOAAllINTStatus(void)
{
  return (PMU->IOAINT);
}

/**
  * @brief  Get IOA interrupt status.
  * @param  INTMask:
                GPIO_Pin_0 ~ GPIO_Pin_15
  * @retval 1:status set
            0:status reset
  */
uint16_t PMU_GetIOAINTStatus(uint16_t INTMask)
{
  /* Check parameters */
  assert_parameters(IS_GPIO_PINR(INTMask));  
  
  if (PMU->IOAINT&INTMask)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Clear IOA interrupt status.
  * @param  INTMask:
                This parameter can be any combination of the following values
                GPIO_Pin_0 ~ GPIO_Pin_15
  * @retval None
  */
void PMU_ClearIOAINTStatus(uint16_t INTMask)
{
  /* Check parameters */
  assert_parameters(IS_GPIO_PIN(INTMask)); 
  
  PMU->IOAINT = INTMask;
}

/**
  * @brief  Wake-up sources pin configuration.
  * @param  IOAx:   GPIO_Pin_0 ~ GPIO_Pin_15
            Wakeup_Event:
                    IOA_DISABLE
                    IOA_RISING
                    IOA_FALLING
                    IOA_HIGH
                    IOA_LOW
                    IOA_EDGEBOTH
  * @retval None
  */
void PMU_WakeUpPinConfig(uint32_t IOAx, uint32_t Wakeup_Event)
{
  uint32_t tmp;
  uint32_t posision = 0x00U;
  uint32_t iocurrent = 0x00U;
  
  /* Check parameters */
  assert_parameters(IS_GPIO_PINR(IOAx));   
  assert_parameters(IS_PMU_WAKEUP(Wakeup_Event));   
  
  while ((IOAx >> posision) != 0U)
  {
    /* Get current io position */
    iocurrent = IOAx & (0x01U << posision);
    
    if (iocurrent)
    {
      /* Current IO Input configure*/
      GPIOA->OEN |= iocurrent;
      GPIOA->IE  |= iocurrent;
      
      tmp = PMU->IOAWKUEN;
      tmp &= ~(3U << (2 * posision));
      switch (Wakeup_Event)
      {
      /* Disable wake-up function */
      default:
      case IOA_DISABLE:
        break;
       
      /* wake-up function: Rising */
      case IOA_RISING:
        GPIOA->DAT &= ~iocurrent;
        tmp |= 1 << (2 * posision);
        break;
        
      /* wake-up function: falling */
      case IOA_FALLING:
        GPIOA->DAT |= iocurrent;
        tmp |= 1 << (2 * posision);
        break;

      /* wake-up function: high level */
      case IOA_HIGH:
        GPIOA->DAT &= ~iocurrent;
        tmp |= 2 << (2 * posision);
        break;  
        
      /* wake-up function: low level */
      case IOA_LOW:
        GPIOA->DAT |= iocurrent;
        tmp |= 2 << (2 * posision);
        break;
        
      /* wake-up function: boht edge */
      case IOA_EDGEBOTH:
        tmp |= 3 << (2 * posision);
        break;
      }
      PMU->IOAWKUEN = tmp;
    }
    posision++;
  }
}

/**
  * @brief  Control low-power configuration, enter deep-sleep mode.
  *
  * @param  InitStruct : pointer to PMU_LowPWRTypeDef
               COMP1Power:
                       PMU_COMP1PWR_ON
                       PMU_COMP1PWR_OFF
               COMP2Power:
                       PMU_COMP2PWR_ON
                       PMU_COMP2PWR_OFF
               TADCPower:
                       PMU_TADCPWR_ON
                       PMU_TADCPWR_OFF
               BGPPower:
                       PMU_BGPPWR_ON
                       PMU_BGPPWR_OFF
               AVCCPower:
                       PMU_AVCCPWR_ON
                       PMU_AVCCPWR_OFF
               LCDPower:
                       PMU_LCDPWER_ON
                       PMU_LCDPWER_OFF
               VDCINDetector:
                       PMU_VDCINDET_ENABLE
                       PMU_VDCINDET_DISABLE
               APBPeriphralDisable:
                       PMU_APB_ALL
                       PMU_APB_DMA
                       PMU_APB_I2C
                       PMU_APB_SPI1
                       PMU_APB_SPI2
                       PMU_APB_UART0
                       PMU_APB_UART1
                       PMU_APB_UART2
                       PMU_APB_UART3
                       PMU_APB_UART4   
                       PMU_APB_UART5
                       PMU_APB_ISO78160
                       PMU_APB_ISO78161
                       PMU_APB_TIMER
                       PMU_APB_MISC
                       PMU_APB_U32K0
                       PMU_APB_U32K1
                AHBPeriphralDisable:    
                       PMU_AHB_ALL
                       PMU_AHB_DMA
                       PMU_AHB_GPIO
                       PMU_AHB_LCD
                       PMU_AHB_CRYPT   
  
  * @note   This function performs the following:
                Comparator 1 power control            ON or OFF(optional)
                Comparator 2 power control            ON or OFF(optional)
                Tiny ADC power control                ON or OFF(optional)
                Bandgap power control                 ON or OFF(optional)
                AVCC power control                   ON or OFF(optional)
                LCD controller power control          ON or OFF(optional)
                VDCIN detector control                Disable or Enable(optional)
                Disable AHB/APB periphral clock       Modules(optional)
                Disable AVCC output
                Power down ADC, Power down Temp sensor
                Disable resistor/cap division for ADC input signal
  
  * @retval 1: Current MODE is debug mode, enter deep-sleep mode failed.    
            2: VDCIN is not drop before enter deep-sleep mode or enter deep-sleep fail
  */
uint8_t PMU_EnterDSleep_LowPower(PMU_LowPWRTypeDef *InitStruct)
{
  uint32_t tmp;
  uint32_t hclk;

  /* Check parameters */  
  assert_parameters(IS_PMU_COMP1PWR(InitStruct->COMP1Power));
  assert_parameters(IS_PMU_COMP2PWR(InitStruct->COMP2Power));
  assert_parameters(IS_PMU_TADCPWR(InitStruct->TADCPower));
  assert_parameters(IS_PMU_BGPPWR(InitStruct->BGPPower));
  assert_parameters(IS_PMU_AVCCPWR(InitStruct->AVCCPower));
  assert_parameters(IS_PMU_LCDPWER(InitStruct->LCDPower));
  assert_parameters(IS_PMU_VDCINDET(InitStruct->VDCINDetector));
  
  /* Current MODE is 0, debug mode, return error */
  if (!(PMU->STS & PMU_STS_MODE))
    return 1;
  
  /* Disable AVCC output */
  ANA->REGF &= ~ANA_REGF_VD33OFF;
  /* Power down ADC */
  ana_reg3_tmp &= ~ANA_REG3_ADCPDN;
  ANA->REG3 = ana_reg3_tmp;
  /* Power down Temp sensor */
  ANA->ADCCTRL &= ~(ANA_ADCCTRL_MCH | ANA_ADCCTRL_ACH);
  /* Disable resistor/cap division for ADC input signal */
  ANA->REG1 &= ~(ANA_REG1_RESDIV | ANA_REG1_GDE4);
  
  /******** Comparator 1 power control ********/
  ana_reg3_tmp &= ~ANA_REG3_CMP1PDN;
  ana_reg3_tmp |= InitStruct->COMP1Power;
  ANA->REG3 = ana_reg3_tmp;
  
  /******** Comparator 2 power control ********/
  ana_reg3_tmp &= ~ANA_REG3_CMP2PDN;
  ana_reg3_tmp |= InitStruct->COMP2Power;
  ANA->REG3 = ana_reg3_tmp;
  
  /******** Tiny ADC power control ********/
  tmp = ANA->REGF;
  tmp &= ~ANA_REGF_PDNADT;
  tmp |= InitStruct->TADCPower;
  ANA->REGF = tmp;
  
  /******** BGP power control ********/
  ana_reg3_tmp &= ~ANA_REG3_BGPPD;
  ana_reg3_tmp |= InitStruct->BGPPower;
  ANA->REG3 = ana_reg3_tmp; 
  
  /******** AVCC power control ********/
  tmp = ANA->REG8;
  tmp &= ~ANA_REG8_PD_AVCCLDO;
  tmp |= InitStruct->AVCCPower;
  ANA->REG8 = tmp;
  
  /******** LCD controller power control ********/
  tmp = LCD->CTRL;
  tmp &= ~LCD_CTRL_EN;
  tmp |= InitStruct->LCDPower;
  LCD->CTRL = tmp;
  /* LCD power off, disable all SEG */
  if (InitStruct->LCDPower == PMU_LCDPWER_OFF)
  {
    LCD->SEGCTRL0 = 0;
    LCD->SEGCTRL1 = 0;
    LCD->SEGCTRL2 = 0;
  }
  
  /******** VDCIN detector control ********/ 
  tmp = ANA->REGA;
  tmp &= ~ANA_REGA_PD_VDCINDET;
  tmp |= InitStruct->VDCINDetector;
  ANA->REGA = tmp;
  
  /******** AHB Periphral clock disable selection ********/
  tmp = MISC2->HCLKEN;
  tmp &= ~((InitStruct->AHBPeriphralDisable) & PMU_AHB_ALL);
  MISC2->HCLKEN = tmp;
  
  /******** APB Periphral clock disable selection ********/
  tmp = MISC2->PCLKEN;
  tmp &= ~((InitStruct->APBPeriphralDisable) & PMU_APB_ALL);
  MISC2->PCLKEN = tmp;
    
  if ((InitStruct->VDCINDetector) != ANA_REGA_PD_VDCINDET)
  {
    if (!(ANA->COMPOUT & ANA_COMPOUT_VDCINDROP))
    {
      return 2;
    }
  }
  // make sure WKU is 0 before entering deep-sleep mode
  while (PMU->DSLEEPEN & PMU_DSLEEPEN_WKU);
  
  /* Flash 1USCYCLE configure */
  hclk = CLK_GetHCLKFreq();  
  MISC2->FLASHWC = (hclk/1000000 - 1)<<8;
  
  /* Enter deep-sleep mode */
  PMU->DSLEEPPASS = DSLEEPPASS_KEY;
  PMU->DSLEEPEN = DSLEEPEN_KEY;
  
  return 2;
}

/**
  * @brief  Control low-power configuration, enter sleep mode.
  *
  * @param  InitStruct : pointer to PMU_LowPWRTypeDef
               COMP1Power:
                       PMU_COMP1PWR_ON
                       PMU_COMP1PWR_OFF
               COMP2Power:
                       PMU_COMP2PWR_ON
                       PMU_COMP2PWR_OFF
               TADCPower:
                       PMU_TADCPWR_ON
                       PMU_TADCPWR_OFF
               BGPPower:
                       PMU_BGPPWR_ON
                       PMU_BGPPWR_OFF
               AVCCPower:
                       PMU_AVCCPWR_ON
                       PMU_AVCCPWR_OFF
               LCDPower:
                       PMU_LCDPWER_ON
                       PMU_LCDPWER_OFF
               VDCINDetector:
                       PMU_VDCINDET_ENABLE
                       PMU_VDCINDET_DISABLE
               APBPeriphralDisable:
                       PMU_APB_ALL
                       PMU_APB_DMA
                       PMU_APB_I2C
                       PMU_APB_SPI1
                       PMU_APB_SPI2
                       PMU_APB_UART0
                       PMU_APB_UART1
                       PMU_APB_UART2
                       PMU_APB_UART3
                       PMU_APB_UART4   
                       PMU_APB_UART5
                       PMU_APB_ISO78160
                       PMU_APB_ISO78161
                       PMU_APB_TIMER
                       PMU_APB_MISC
                       PMU_APB_U32K0
                       PMU_APB_U32K1
                AHBPeriphralDisable:    
                       PMU_AHB_ALL
                       PMU_AHB_DMA
                       PMU_AHB_GPIO
                       PMU_AHB_LCD
                       PMU_AHB_CRYPT 
  
  * @note   This function performs the following:
                Comparator 1 power control            ON or OFF(optional)
                Comparator 2 power control            ON or OFF(optional)
                Tiny ADC power control                ON or OFF(optional)
                Bandgap power control                 ON or OFF(optional)
                AVCC power control                   ON or OFF(optional)
                LCD controller power control          ON or OFF(optional)
                VDCIN detector control                Disable or Enable(optional)
                Disable AHB/APB periphral clock       Modules(optional)
                Disable AVCC output
                Power down ADC, Power down Temp sensor
                Disable resistor/cap division for ADC input signal
  
  * @retval     2: VDCIN is not drop before enter sleep mode(failed).
                1: Current mode is debug mode, enter sleep mode failed.
                0: Quit from sleep mode success.
*/
uint8_t PMU_EnterSleep_LowPower(PMU_LowPWRTypeDef *InitStruct)
{
  uint32_t tmp;
  uint32_t hclk;

  /* Check parameters */
  assert_parameters(IS_PMU_COMP1PWR(InitStruct->COMP1Power));
  assert_parameters(IS_PMU_COMP2PWR(InitStruct->COMP2Power));
  assert_parameters(IS_PMU_TADCPWR(InitStruct->TADCPower));
  assert_parameters(IS_PMU_BGPPWR(InitStruct->BGPPower));
  assert_parameters(IS_PMU_AVCCPWR(InitStruct->AVCCPower));
  assert_parameters(IS_PMU_LCDPWER(InitStruct->LCDPower));
  assert_parameters(IS_PMU_VDCINDET(InitStruct->VDCINDetector));
  
  /* Current MODE is 0, debug mode, return error */
//  if (!(PMU->STS & PMU_STS_MODE))
//    return 1;
//  
  /* Disable AVCC output */
  ANA->REGF &= ~ANA_REGF_VD33OFF;
  /* Power down ADC */
  ana_reg3_tmp &= ~ANA_REG3_ADCPDN;
  ANA->REG3 = ana_reg3_tmp;
  /* Power down Temp sensor */
  ANA->ADCCTRL &= ~(ANA_ADCCTRL_MCH | ANA_ADCCTRL_ACH);
  /* Disable resistor/cap division for ADC input signal */
  ANA->REG1 &= ~(ANA_REG1_RESDIV | ANA_REG1_GDE4);
  
  /******** Comparator 1 power control ********/
  ana_reg3_tmp &= ~ANA_REG3_CMP1PDN;
  ana_reg3_tmp |= InitStruct->COMP1Power;
  ANA->REG3 = ana_reg3_tmp;
  
  /******** Comparator 2 power control ********/
  ana_reg3_tmp &= ~ANA_REG3_CMP2PDN;
  ana_reg3_tmp |= InitStruct->COMP2Power;
  ANA->REG3 = ana_reg3_tmp;
  
  /******** Tiny ADC power control ********/
  tmp = ANA->REGF;
  tmp &= ~ANA_REGF_PDNADT;
  tmp |= InitStruct->TADCPower;
  ANA->REGF = tmp;
  
  /******** BGP power control ********/
  ana_reg3_tmp &= ~ANA_REG3_BGPPD;
  ana_reg3_tmp |= InitStruct->BGPPower;
  ANA->REG3 = ana_reg3_tmp; 
  
  /******** AVCC power control ********/
  tmp = ANA->REG8;
  tmp &= ~ANA_REG8_PD_AVCCLDO;
  tmp |= InitStruct->AVCCPower;
  ANA->REG8 = tmp;
  
  /******** LCD controller power control ********/
  tmp = LCD->CTRL;
  tmp &= ~LCD_CTRL_EN;
  tmp |= InitStruct->LCDPower;
  LCD->CTRL = tmp;
  /* LCD power off, disable all SEG */
  if (InitStruct->LCDPower == PMU_LCDPWER_OFF)
  {
    LCD->SEGCTRL0 = 0;
    LCD->SEGCTRL1 = 0;
    LCD->SEGCTRL2 = 0;
  }
  
  /******** VDCIN detector control ********/ 
  tmp = ANA->REGA;
  tmp &= ~ANA_REGA_PD_VDCINDET;	
  tmp |= InitStruct->VDCINDetector;
	
  ANA->REGA = tmp;
	ANA->REGA |=BIT1;///CTEST
	
	ANA->REG9|=BIT7;///CTEST
	ANA->REGA|=BIT3;///CTEST
  
  /******** AHB Periphral clock disable selection ********/
  tmp = MISC2->HCLKEN;
  tmp &= ~((InitStruct->AHBPeriphralDisable) & PMU_AHB_ALL);
  MISC2->HCLKEN = tmp;
  
  /******** APB Periphral clock disable selection ********/
  tmp = MISC2->PCLKEN;
  tmp &= ~((InitStruct->APBPeriphralDisable) & PMU_APB_ALL);
  MISC2->PCLKEN = tmp;
  
  if ((InitStruct->VDCINDetector) != ANA_REGA_PD_VDCINDET)
  {
    if (!(ANA->COMPOUT & ANA_COMPOUT_VDCINDROP))
    {
      return 2;
    }
  }
  
  /* Flash 1USCYCLE configure */
  hclk = CLK_GetHCLKFreq();  
  MISC2->FLASHWC = (hclk/1000000 - 1)<<8;
  
  /* Set SLEEPDEEP bit of Cortex-M0 System Control Register */
//  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; 
//  __WFI();
  
  return 0;
}

/**
  * @brief  Flash deep standby, enter idle mode.
  * @param  None
  * @retval None
  */
void PMU_EnterIdle_LowPower(void)
{
  uint32_t hclk;
		
  /* Flash 1USCYCLE configure */
  hclk = CLK_GetHCLKFreq();  
  MISC2->FLASHWC = (hclk/1000000 - 1)<<8;
  
  PMU_EnterIdle_FlashDSTB();
}

/**
  * @brief  IOA wake-up source configure about Sleep mode.
  * @param  IOAx:   ARM_GPIO_Pin_0 ~ ARM_GPIO_Pin_15
            Wakeup_Event:
                    IOA_DISABLE
                    IOA_RISING
                    IOA_FALLING
                    IOA_HIGH
                    IOA_LOW
                    IOA_EDGEBOTH 
           Priority: The preemption priority for the IRQn channel.
                     This parameter can be a value between 0 and 3.
  * @retval 
  */
void PMU_SleepWKUSRC_Config_IOA(uint16_t IOAx, uint32_t Wakeup_Event, uint32_t Priority)
{
  /* Check parameters */
  assert_parameters(IS_GPIO_PINR(IOAx));   
  assert_parameters(IS_PMU_WAKEUP(Wakeup_Event)); 
  
  /* Disable PMU interrupt in NVIC */
  NVIC_DisableIRQ(PMU_IRQn);
  /* Wake-pu pins configuration */
  PMU_WakeUpPinConfig(IOAx, Wakeup_Event);
  /* Clear interrupt flag */
  PMU->IOAINT = IOAx;
  /* Enable PMU interrupt */
  PMU->CONTROL |= PMU_CONTROL_INT_IOA_EN;
  CORTEX_SetPriority_ClearPending_EnableIRQ(PMU_IRQn, Priority);
}

/**
  * @brief  RTC wake-up source configure about Sleep mode.
  * @param  Wakeup_Event:
                This parameter can be any combination of the following values
                PMU_RTCEVT_ACDONE
                PMU_RTCEVT_WKUCNT
                PMU_RTCEVT_MIDNIGHT
                PMU_RTCEVT_WKUHOUR 
                PMU_RTCEVT_WKUMIN 
                PMU_RTCEVT_WKUSEC
                PMU_RTCEVT_TIMEILLE 
           Priority: The preemption priority for the IRQn channel.
                     This parameter can be a value between 0 and 3.
  * @retval 
  */
void PMU_SleepWKUSRC_Config_RTC(uint32_t Wakeup_Event, uint32_t Priority)
{
  /* Check parameters */
  assert_parameters(IS_PMU_RTCEVT(Wakeup_Event));
  
  /* Disable RTC interrupt in NVIC */
  NVIC_DisableIRQ(RTC_IRQn);
  /* Clear interrupt flag */
  RTC->INTSTS = Wakeup_Event;
  /* Enable RTC interrupt */
  RTC->INTEN |= Wakeup_Event & (~0x01UL);
  CORTEX_SetPriority_ClearPending_EnableIRQ(RTC_IRQn, Priority);
}
/**
  * @brief  IOA wake-up source configure about Deep-Sleep mode.
  * @param  IOAx:   ARM_GPIO_Pin_0 ~ ARM_GPIO_Pin_15
            Wakeup_Event:
                    IOA_DISABLE
                    IOA_RISING
                    IOA_FALLING
                    IOA_HIGH
                    IOA_LOW
                    IOA_EDGEBOTH 
  * @retval 
  */
void PMU_DeepSleepWKUSRC_Config_IOA(uint16_t IOAx, uint32_t Wakeup_Event)
{
  /* Check parameters */
  assert_parameters(IS_GPIO_PINR(IOAx));   
  assert_parameters(IS_PMU_WAKEUP(Wakeup_Event)); 
  
  /* Wake-pu pins configuration */
  PMU_WakeUpPinConfig(IOAx, Wakeup_Event);
  /* Clear interrupt flag */
  PMU->IOAINT = IOAx;
}

/**
  * @brief  RTC wake-up source configure about Deep-Sleep mode.
  * @param  Wakeup_Event:
                This parameter can be any combination of the following values
                PMU_RTCEVT_ACDONE
                PMU_RTCEVT_WKUCNT
                PMU_RTCEVT_MIDNIGHT
                PMU_RTCEVT_WKUHOUR 
                PMU_RTCEVT_WKUMIN 
                PMU_RTCEVT_WKUSEC
                PMU_RTCEVT_TIMEILLE 
  * @retval 
  */
void PMU_DeepSleepWKUSRC_Config_RTC(uint32_t Wakeup_Event)
{
  /* Check parameters */
  assert_parameters(IS_PMU_RTCEVT(Wakeup_Event));
  
  /* Clear interrupt flag */
  RTC->INTSTS = Wakeup_Event;
  /* Enable RTC interrupt */
  RTC->INTEN |= Wakeup_Event & (~0x01UL);
}

/**
  * @brief  Set the deep sleep behavior when VDD/VDCIN is not drop.
  * @param  VDCIN_PDNS:
                PMU_VDCINPDNS_0  , can't enter deep-sleep mode when VDCIN is not drop
                                   wake-up when VDCIN change(low to high).
                PMU_VDCINPDNS_1  , can deep-sleep mode when VDCIN is not drop
            VDD_PDNS: 
                PMU_VDDPDNS_0  , can't enter deep-sleep mode when VDD is not drop(>Threshold)
                                  wake-up when VDD change(lower than Threshold to higher than Threshold).
                PMU_VDDPDNS_1  , can deep-sleep mode when VDD is not drop
  * @retval None
  */
void PMU_PDNDSleepConfig(uint32_t VDCIN_PDNS, uint32_t VDD_PDNS)
{
  uint32_t tmp;

  /* Check parameters */  
  assert_parameters(IS_PMU_VDCINPDNS(VDCIN_PDNS));
  assert_parameters(IS_PMU_VDDPDNS(VDD_PDNS));   
    
  tmp = ANA->CTRL;
  tmp &= ~(ANA_CTRL_PDNS | ANA_CTRL_PDNS2);
  tmp |= (VDCIN_PDNS | VDD_PDNS);

  ANA->CTRL = tmp;
}

/**
  * @brief  BGP power control.
  * @param  NewState:
  *             ENABLE
  *             DISABLE
  * @retval None
  */
void PMU_BGP_Cmd(uint32_t NewState)
{
  /* Check parameters */  
  assert_parameters(IS_FUNCTIONAL_STATE(NewState)); 

  if (NewState == ENABLE)
    ana_reg3_tmp &= ~ANA_REG3_BGPPD;
  else
    ana_reg3_tmp |= ANA_REG3_BGPPD;
  ANA->REG3 = ana_reg3_tmp;
}

/**
  * @brief  Configure VDD alarm threshold voltage.
  * @param  PowerThreshold:
  *             PMU_PWTH_4_5
  *             PMU_PWTH_4_2
  *             PMU_PWTH_3_9
  *             PMU_PWTH_3_6
  *             PMU_PWTH_3_2   
  *             PMU_PWTH_2_9   
  *             PMU_PWTH_2_6   
  *             PMU_PWTH_2_3 
  * @retval None
  */
void PMU_VDDAlarmTHConfig(uint32_t PowerThreshold)
{
  uint32_t tmp;

  /* Check parameters */ 
  assert_parameters(IS_PMU_PWTH(PowerThreshold));
  
  tmp = ANA->REG8;
  tmp &= ~ANA_REG8_VDDPVDSEL;
  tmp |= PowerThreshold;
  
  ANA->REG8 = tmp;
}

/**
  * @brief  Get POWALARM status.
  * @param  None
  * @retval POWALARM status
  *           0: Voltage of VDD is higher than threshold.
  *           1: Voltage of VDD is lower than threshold.
  */
uint8_t PMU_GetVDDALARMStatus(void)
{
  if (ANA->COMPOUT & ANA_COMPOUT_VDDALARM)
    return 1;
  else
    return 0;
}

/**
  * @brief  Gets current MODE pin status.
  * @param  None
  * @retval MODE pin status
  *           0: Debug mode.
  *           1: Normal mode.
  */
uint8_t PMU_GetModeStatus(void)
{
  if(PMU->STS & PMU_STS_MODE)
    return 1;
  else
    return 0;
}

/**
  * @brief  Main power configure.
  * @param  PWRState:
  *             PMU_PWRSTA_AUTOVDD_BAT1
  *             PMU_PWRSTA_SWT2BAT1
  *             PMU_PWRSTA_SWT2VDD
  * @retval None
  */
void PMU_MainPowerConfig(uint32_t PWRState)
{
  /* Check parameters */ 
  assert_parameters(IS_PMU_PWRSTA(PWRState));
  
  /* Switching between BAT1 and VDD automatic */
  if (PWRState == PMU_PWRSTA_AUTOVDD_BAT1)
  {
    ANA->REG7 &= ~ANA_REG7_SWT2VDD;
    ANA->REGA &= ~ANA_REGA_SWT2BAT1;
  }
  /* Switching from VDD to BAT1 manually */
  else if (PWRState == PMU_PWRSTA_SWT2BAT1)
  {
    ANA->REG7 &= ~ANA_REG7_SWT2VDD;
    ANA->REGA |= ANA_REGA_SWT2BAT1;
  }
  /* Be forced to switch to VDD, power down switching between VDD and BAT1 */
  else
  {
    ANA->REG7 |= ANA_REG7_SWT2VDD;
    ANA->REGA &= ~ANA_REGA_SWT2BAT1;    
  }
}

/**
  * @brief  power switch level selection.
  * @param  SWTLevel:
  *             PMU_PSWTLEVEL_3_0
  *             PMU_PSWTLEVEL_3_6
  * @retval None
  */
void PMU_PowerSWTLevelConfig(uint32_t SWTLevel)
{
  uint32_t tmp;

  /* Check parameters */ 
  assert_parameters(IS_PMU_PSWTLEVEL(SWTLevel));  
  
  tmp = ANA->REGA;
  tmp &= ~ANA_REGA_PSLSEL;
  tmp |= SWTLevel;
  
  ANA->REGA = tmp;
}

/**
  * @brief  Control AVCC enable.
  * @param  NewState:
  *             ENABLE
  *             DISABLE
  * @retval None
  */
void PMU_AVCC_Cmd(uint32_t NewState)
{
  /* Check parameters */ 
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState == ENABLE)
    ANA->REG8 &= ~ANA_REG8_PD_AVCCLDO;
  else
    ANA->REG8 |= ANA_REG8_PD_AVCCLDO;
}

/**
  * @brief  Control VDD33_O pin power.
  * @param  NewState:
  *             ENABLE
  *             DISABLE
  * @retval None
  */
void PMU_AVCCOutput_Cmd(uint32_t NewState)
{
  /* Check parameters */ 
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState == DISABLE)
    ANA->REGF &= ~ANA_REGF_VD33OFF;
  else
    ANA->REGF |= ANA_REGF_VD33OFF;
}

/**
  * @brief  AVCC Low Voltage detector power control.
  * @param  NewState:
  *             ENABLE
  *             DISABLE
  * @retval None
  */
void PMU_AVCCLVDetector_Cmd(uint32_t NewState)
{
  /* Check parameters */ 
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState == ENABLE)
    ANA->REG5 &= ~ANA_REG5_PD_AVCCDET;
  else
    ANA->REG5 |= ANA_REG5_PD_AVCCDET;
}

/**
  * @brief  Get AVCC low power status.
  * @param  None
  * @retval low power status of AVCC
  *           0: status not set, AVCC is higher than 2.5V.
  *           1: status set, AVCC is lower than 2.5V.
  */
uint8_t PMU_GetAVCCLVStatus(void)
{
  if (ANA->COMPOUT & ANA_COMPOUT_AVCCLV)
    return 1;
  else
    return 0;
}

/**
  * @brief  Control VDCIN decector enable.
  * @param  NewState:
  *             ENABLE
  *             DISABLE
  * @retval None
  */
void PMU_VDCINDetector_Cmd(uint32_t NewState)
{
  /* Check parameters */ 
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState == ENABLE)
    ANA->REGA &= ~ANA_REGA_PD_VDCINDET;
  else
    ANA->REGA |= ANA_REGA_PD_VDCINDET;
}

/**
  * @brief  Get VDCIN drop status.
  * @param  None
  * @retval drop status of VDCIN
  *           0: status not set, VDCIN is not drop.
  *           1: status set, VDCIN is drop.
  */
uint8_t PMU_GetVDCINDropStatus(void)
{
  if (ANA->COMPOUT & ANA_COMPOUT_VDCINDROP)
    return 1;
  else
    return 0;
}

/**
  * @brief  Discharge the BAT battery.
  * @param  BATDisc:
  *             PMU_BAT1_DISC
  *             PMU_RTCBAT_DISC
  *         NewState:
  *             ENABLE
  *             DISABLE
  * @retval None
  */
void PMU_BATDischargeConfig(uint32_t BATDisc, uint32_t NewState)
{
  /* Check parameters */ 
  assert_parameters(IS_PMU_BAT1DISC(BATDisc));
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState == ENABLE)
    ANA->REG6 |= BATDisc;
  else
    ANA->REG6 &= ~BATDisc;
}

/**
  * @brief  Get RTC power status.
  * @param  None
  * @retval power status of RTC
  *           0: RTC power is off.
  *           1: RTC power is on.
  */
uint8_t PMU_GetRTCPWRStatus(void)
{
  if (ANA->COMPOUT & ANA_COMPOUT_VDCINDROP)
    return 1;
  else
    return 0;
}

/**
  * @brief  Get status of power source.
  * @param  None
  * @retval status of MAINPRSTS
  *           0: VDD power is larger than the threshold set by ¡®PSLSE¡¯, 
  *              and main power use VDD as power source.
  *           1: VDD power is lower than the threshold set by ¡®PSLSE¡¯, 
  *              and main power use BAT1 as power source.
  */
uint8_t PMU_GetPWRSRCStatus(void)
{
  if (ANA->COMPOUT & ANA_COMPOUT_MAINPRSTS)
    return 1;
  else
    return 0;
}

/**
  * @brief  Power drop de-bounce control.
  * @param  Debounce:
  *             PMU_PWRDROP_DEB_0
  *             PMU_PWRDROP_DEB_1
  *             PMU_PWRDROP_DEB_2
  *             PMU_PWRDROP_DEB_3
  * @retval None
  */
void PMU_PWRDropDEBConfig(uint32_t Debounce)
{
  uint32_t tmp;

  /* Check parameters */ 
  assert_parameters(IS_PMU_PWRDROP_DEB(Debounce)); 
  
  tmp = ANA->CTRL;
  tmp &= ~ANA_CTRL_PWRDROPDEB;
  tmp |= Debounce;
  
  ANA->CTRL = tmp;
}

/**
  * @brief  Get last reset source.
  * @param  RSTSource:
              PMU_RSTSRC_EXTRST
              PMU_RSTSRC_PORST
              PMU_RSTSRC_DPORST
  * @retval 1:status set
            0:status reset
  */
uint8_t PMU_GetRSTSource(uint32_t RSTSource)
{
  /* Check parameters */ 
  assert_parameters(IS_PMU_RSTSRC(RSTSource)); 
  
  if (PMU->STS & RSTSource)
  {
    PMU->STS = RSTSource; //Clear flag
    return (1);
  }
  else
  {
    return (0);
  }
}

/**
  * @brief  Get power status.
  * @param  StatusMask:
                PMU_PWRSTS_AVCCLV
                PMU_PWRSTS_VDCINDROP
                PMU_PWRSTS_VDDALARM
  * @retval power status
  *           1  status set
  *           0  status not set
  */
uint8_t PMU_GetPowerStatus(uint32_t StatusMask)
{
  if (ANA->COMPOUT & StatusMask)
    return 1;
  else
    return 0;
}

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
