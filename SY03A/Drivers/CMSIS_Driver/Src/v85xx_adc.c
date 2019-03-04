/**
  ******************************************************************************
  * @file    v85xx_adc.c 
  * @author  VT Application Team
  * @version V1.0.0
  * @date    2017-06-22
  * @brief   ADC library.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "v85xx_adc.h"

extern __IO uint32_t ana_reg3_tmp;
#define ANA_REG1_RSTValue     (0U)
#define ANA_ADCCTRL_RSTValue  (0U)

/**
  * @brief  Deinitializes ADC peripheral registers to their default reset values.
  * @note   1. Disable ADC
            2. Disable ADC overall bias current trim
            3. Disable resistor/cap division.
            4. Disable ADC auto/manual done interrupt
            5. ANA_ADCCTRL(register) write default value.
  * @param  None
  * @retval None
  */
void ADC_DeInit(void)
{
  /* Power down ADC */
  ana_reg3_tmp &= ~ANA_REG3_ADCPDN;  
  ANA->REG3 = ana_reg3_tmp;
  /* Disable resistor/cap division. */
  ANA->REG1 = ANA_REG1_RSTValue;
  /* Disable interrupt, Clear interrupt flag */
  ANA->INTEN &= ~(ANA_INTEN_INTEN0 | ANA_INTEN_INTEN1);
  ANA->INTSTS = (ANA_INTSTS_INTSTS0 | ANA_INTSTS_INTSTS1);
  ANA->ADCCTRL = ANA_ADCCTRL_RSTValue;
}

/**
  * @brief  Fills each ADC_InitStruct member with its default value.
  * @param  ADC_InitStruct: pointer to an ADCInitType structure which will be initialized.
  * @retval None
  */
void ADC_StructInit(ADCInitType* ADC_InitStruct)
{
  /*--------------- Reset ADC init structure parameters values ---------------*/
  /* Initialize the ClockSource member */
  ADC_InitStruct->ClockSource = ADC_CLKSRC_RCH;
  /* Initialize the Channel member */
  ADC_InitStruct->Channel = ADC_CHANNEL0;
  /* Initialize the ClockDivider member */
  ADC_InitStruct->ClockDivider = ADC_CLKDIV_1;
  /* Initialize the ConvMode member */
  ADC_InitStruct->ConvMode = ADC_CONVMODE_SINGLECHANNEL;
  /* Initialize the TrigMode member */
  ADC_InitStruct->TrigMode = ADC_TRIGMODE_MANUAL;
}

/**
  * @brief  ADC initialization.
  * @param  ADC_InitStruct:
                TrigMode:
                    ADC_TRIGMODE_AUTO
                    ADC_TRIGMODE_MANUAL
                ConvMode:
                    ADC_CONVMODE_SINGLECHANNEL
                    ADC_CONVMODE_MULTICHANNEL
                ClockSource:
                    ADC_CLKSRC_RCH
                    ADC_CLKSRC_PLLL
                ClockDivider:
                    ADC_CLKDIV_1
                    ADC_CLKDIV_2
                    ADC_CLKDIV_3
                    ADC_CLKDIV_4
                    ADC_CLKDIV_5
                    ADC_CLKDIV_6
                    ADC_CLKDIV_7
                    ADC_CLKDIV_8
                    ADC_CLKDIV_9
                    ADC_CLKDIV_10
                    ADC_CLKDIV_11
                    ADC_CLKDIV_12
                    ADC_CLKDIV_13
                    ADC_CLKDIV_14
                    ADC_CLKDIV_15
                    ADC_CLKDIV_16
                Channel:(be valid when ConvMode is ADC_CONVMODE_SINGLECHANNEL)
                    ADC_CHANNEL0
                    ADC_CHANNEL1
                    ADC_CHANNEL2
                    ADC_CHANNEL3
                    ADC_CHANNEL4
                    ADC_CHANNEL5
                    ADC_CHANNEL6
                    ADC_CHANNEL7
                    ADC_CHANNEL8
                    ADC_CHANNEL9
                    ADC_CHANNEL10
                    ADC_CHANNEL11
                    
  * @retval None
  */
void ADC_Init(ADCInitType* ADC_InitStruct)
{
  uint32_t tmp = 0;
  
  /* Check parameters */
  assert_parameters(IS_ADC_TRIGMODE(ADC_InitStruct->TrigMode));
  assert_parameters(IS_ADC_CONVMODE(ADC_InitStruct->ConvMode));
  assert_parameters(IS_ADC_CLKDIV(ADC_InitStruct->ClockDivider));
  assert_parameters(IS_ADC_CLKSRC(ADC_InitStruct->ClockSource));
  
  tmp = ANA->ADCCTRL;
  tmp &= ~(ANA_ADCCTRL_AMODE \
          |ANA_ADCCTRL_MMODE \
          |ANA_ADCCTRL_CLKSEL \
          |ANA_ADCCTRL_CLKDIV \
          |ANA_ADCCTRL_AEN \
          |ANA_ADCCTRL_MCH \
          |ANA_ADCCTRL_ACH);
  tmp |= (ADC_InitStruct->ClockDivider | ADC_InitStruct->ClockSource);
  
  if(ADC_InitStruct->TrigMode == ADC_TRIGMODE_AUTO)  //Auto mode
  {
    if(ADC_InitStruct->ConvMode == ADC_CONVMODE_SINGLECHANNEL)  //signal channel
    {
      assert_parameters(IS_ADC_CHANNEL(ADC_InitStruct->Channel));
      tmp &= (~ANA_ADCCTRL_ACH);
      tmp |= (ADC_InitStruct->Channel << ANA_ADCCTRL_ACH_Pos);
    }
    else   //multi channels
    {
      tmp |= ANA_ADCCTRL_AMODE;
    }
  }
  else  // Manual mode
  {
    if(ADC_InitStruct->ConvMode == ADC_CONVMODE_SINGLECHANNEL)  //signal channel
    {
      assert_parameters(IS_ADC_CHANNEL(ADC_InitStruct->Channel));
      tmp &= (~ANA_ADCCTRL_MCH);
      tmp |= (ADC_InitStruct->Channel << ANA_ADCCTRL_MCH_Pos);
    }
    else   //multi channels
    {
      tmp |= ANA_ADCCTRL_MMODE;
    }     
  }
  
  ANA->ADCCTRL = tmp;
}

/**
  * @brief  Get BAT1 Voltage(costs 170us).
  * @note   This function costs about 170us when SystemClock is 26M.
  *         ADC data refresh time is 40~50us.
  * @note   This function will release ADC resource(write ADC registers with their
  *         default reset values).
  * @note   ADC configurarion:
  *             - Trigger mode:       manual mode
  *             - Conversion mode:    single channel mode(BAT1 channel 1)
  *             - ADC clock:          3.2M
  *             - Skip samples:       No skip any samples
  *             - Down sampling rate: 1/64
  * @param  [in]Division
                ADC_BAT_CAPDIV  (Cap division 1/4)
                ADC_BAT_RESDIV  (Resistance division 1/4)
            [out]CalResults.BAT1Voltage     The value of BAT1 Voltage
            [out]CalResults.BATRTCVoltage   is ignored
            [out]CalResults.Temperature     is ignored
  * @retval 1  NVR BAT offset information checksum error.
            0  Function successed.
  */
uint32_t ADC_GetBAT1Voltage_Fast(uint32_t Division, ADC_CalResType *CalResults)
{
  float Vbatcap;
  float Vbatres;
  NVR_BATMEARES BAT_OffsetInfo;

  assert_parameters(IS_ADC_BATDIV(Division));
  
  /* Get NVR BAT offset information */
  if (NVR_GetBATOffset(&BAT_OffsetInfo))
  {
    return (1);
  }
  else  
  {
    Vbatcap = BAT_OffsetInfo.BATCAPResult;
    Vbatres = BAT_OffsetInfo.BATRESResult;
  }
  
  /* ADC initialization */
  ADC_DeInit();
  ANA->ADCCTRL = 0x04C00101;
  
  /* Enable division */
  ANA->REG1 |= Division;
  /* Enable ADC */
  ana_reg3_tmp |= ANA_REG3_ADCPDN;  
  ANA->REG3 = ana_reg3_tmp;
  
  /* Start a manual ADC conversion */
  ADC_StartManual();
  /* Waiting last operation done */
  ADC_WaitForManual();
 
  /* Calculate the voltage of  BAT1 */
  if (Division & ADC_BAT_CAPDIV)
  {
    CalResults->BAT1Voltage = (((int16_t)ANA->ADCDATA1 - 49.588) / 7088.91575) + Vbatcap;
  }
  else
  {
    CalResults->BAT1Voltage = (0.00015392*(int16_t)ANA->ADCDATA1 + 0.06667986) + Vbatres;
  }
  
  /* ADC resource release */
  ADC_DeInit();
  
  return (0);
}

/**
  * @brief  Get BAT1 Voltage(costs 3.3ms).
  * @note   This function costs about 3.3ms when SystemClock is 26M.
  *         ADC data refresh time is about 3.2ms.
  * @note   This function will release ADC resource(write ADC registers with their
  *         default reset values).
  * @note   ADC configurarion:
  *             - Trigger mode:       manual mode
  *             - Conversion mode:    single channel mode(BAT1 channel 1)
  *             - ADC clock:          1.6M
  *             - Skip samples:       Skip first 4 samples
  *             - Down sampling rate: 1/512
  * @param  [in]Division
                ADC_BAT_CAPDIV  (Cap division 1/4)
                ADC_BAT_RESDIV  (Resistance division 1/4)
            [out]CalResults.BAT1Voltage     The value of BAT1 Voltage
            [out]CalResults.BATRTCVoltage   is ignored
            [out]CalResults.Temperature     is ignored
  * @retval 1  NVR BAT offset information checksum error.
            0  Function successed.
  */
uint32_t ADC_GetBAT1Voltage_Normal(uint32_t Division, ADC_CalResType *CalResults)
{
  float Vbatcap;
  float Vbatres;
  NVR_BATMEARES BAT_OffsetInfo;
  ADCInitType ADC_InitStruct;
  
  assert_parameters(IS_ADC_BATDIV(Division));
  
  /* Get NVR BAT offset information */
  if (NVR_GetBATOffset(&BAT_OffsetInfo))
  {
    return (1);
  }
  else  
  {
    Vbatcap = BAT_OffsetInfo.BATCAPResult;
    Vbatres = BAT_OffsetInfo.BATRESResult;
  }
  
  /* ADC initialization */
  ADC_DeInit();
  ADC_InitStruct.TrigMode = ADC_TRIGMODE_MANUAL;
  ADC_InitStruct.ConvMode = ADC_CONVMODE_SINGLECHANNEL;
  ADC_InitStruct.Channel = ADC_CHANNEL1;
  ADC_InitStruct.ClockDivider = ADC_CLKDIV_4;
  ADC_InitStruct.ClockSource = ADC_CLKSRC_RCH;
  ADC_Init(&ADC_InitStruct);

  /* Enable division */
  ANA->REG1 |= Division;
  /* Enable ADC */
  ana_reg3_tmp |= ANA_REG3_ADCPDN;  
  ANA->REG3 = ana_reg3_tmp;
  
  /* Start a manual ADC conversion */
  ADC_StartManual();
  /* Waiting last operation done */
  ADC_WaitForManual();
 
  /* Calculate the voltage of  BAT1 */
  if (Division & ADC_BAT_CAPDIV)
  {
    CalResults->BAT1Voltage = (((int16_t)ANA->ADCDATA1 - 49.588) / 7088.91575) + Vbatcap;
  }
  else
  {
    CalResults->BAT1Voltage = (0.00015392*(int16_t)ANA->ADCDATA1 + 0.06667986) + Vbatres;
  }
  /* ADC resource release */
  ADC_DeInit();
  
  return (0);  
}

/**
  * @brief  Get BATRTC Voltage(costs 170us).
  * @note   This function costs about 170us when SystemClock is 26M.
  *         ADC data refresh time is 40~50us.
  * @note   This function will release ADC resource(write ADC registers with their
  *         default reset values).
  * @note   ADC configurarion:
  *             - Trigger mode:       manual mode
  *             - Conversion mode:    single channel mode(BATRTC channel 2)
  *             - ADC clock:          3.2M
  *             - Skip samples:       No skip any samples
  *             - Down sampling rate: 1/64
  * @param  [in]Division
                ADC_BAT_CAPDIV  (Cap division 1/4)
                ADC_BAT_RESDIV  (Resistance division 1/4)
            [out]CalResults.BAT1Voltage     is ignored
            [out]CalResults.BATRTCVoltage   The value of BATRTC Voltage
            [out]CalResults.Temperature     is ignored
  * @retval 1  NVR BAT offset information checksum error.
            0  Function successed.
  */
uint32_t ADC_GetBATRTCVoltage_Fast(uint32_t Division, ADC_CalResType *CalResults)
{
  float Vbatcap;
  float Vbatres;
  
  NVR_BATMEARES BAT_OffsetInfo;
  
  assert_parameters(IS_ADC_BATDIV(Division));
  
  /* Get NVR BAT offset information */
  if (NVR_GetBATOffset(&BAT_OffsetInfo))
  {
    return (1);
  }
  else  
  {
    Vbatcap = BAT_OffsetInfo.BATCAPResult;
    Vbatres = BAT_OffsetInfo.BATRESResult;
  }
  
  /* ADC initialization */
  ADC_DeInit();
  ANA->ADCCTRL = 0x04C00102;
  /* Enable division */
  ANA->REG1 |= Division;
  /* Enable ADC */
  ana_reg3_tmp |= ANA_REG3_ADCPDN;  
  ANA->REG3 = ana_reg3_tmp;
  
  /* Start a manual ADC conversion */
  ADC_StartManual();
  /* Waiting last operation done */
  ADC_WaitForManual();
 
  /* Calculate the voltage of  BAT1 */
  if (Division & ADC_BAT_CAPDIV)
  {
    CalResults->BATRTCVoltage = (((int16_t)ANA->ADCDATA2 - 49.588) / 7088.91575) + Vbatcap;
  }
  else
  {
    CalResults->BATRTCVoltage = (0.00015392*(int16_t)ANA->ADCDATA2 + 0.06667986) + Vbatres;
  }
  /* ADC resource release */
  ADC_DeInit();
  
  return (0);  
}

/**
  * @brief  Get BATRTC Voltage(costs 3.3ms).
  * @note   This function costs about 3.3ms when SystemClock is 26M.
  *         ADC data refresh time is about 3.2ms.
  * @note   This function will release ADC resource(write ADC registers with their
  *         default reset values).
  * @note   ADC configurarion:
  *             - Trigger mode:       manual mode
  *             - Conversion mode:    single channel mode(BATRTC channel 2)
  *             - ADC clock:          1.6M
  *             - Skip samples:       Skip first 4 samples
  *             - Down sampling rate: 1/512
  * @param  [in]Division
                ADC_BAT_CAPDIV  (Cap division 1/4)
                ADC_BAT_RESDIV  (Resistance division 1/4)
            [out]CalResults.BAT1Voltage     is ignored
            [out]CalResults.BATRTCVoltage   The value of BATRTC Voltage
            [out]CalResults.Temperature     is ignored
  * @retval 1  NVR BAT offset information checksum error.
            0  Function successed.
  */
uint32_t ADC_GetBATRTCVoltage_Normal(uint32_t Division, ADC_CalResType *CalResults)
{
  float Vbatcap;
  float Vbatres;
  NVR_BATMEARES BAT_OffsetInfo;
  ADCInitType ADC_InitStruct;
  
  assert_parameters(IS_ADC_BATDIV(Division));
  
  /* Get NVR BAT offset information */
  if (NVR_GetBATOffset(&BAT_OffsetInfo))
  {
    return (1);
  }
  else  
  {
    Vbatcap = BAT_OffsetInfo.BATCAPResult; 
    Vbatres = BAT_OffsetInfo.BATRESResult;
  }
  
  /* ADC initialization */
  ADC_DeInit();
  ADC_InitStruct.TrigMode = ADC_TRIGMODE_MANUAL;
  ADC_InitStruct.ConvMode = ADC_CONVMODE_SINGLECHANNEL;
  ADC_InitStruct.Channel = ADC_CHANNEL2;
  ADC_InitStruct.ClockDivider = ADC_CLKDIV_4;
  ADC_InitStruct.ClockSource = ADC_CLKSRC_RCH;
  ADC_Init(&ADC_InitStruct);

  /* Enable division */
  ANA->REG1 |= Division;
  /* Enable ADC */
  ana_reg3_tmp |= ANA_REG3_ADCPDN;  
  ANA->REG3 = ana_reg3_tmp;
  
  /* Start a manual ADC conversion */
  ADC_StartManual();
  /* Waiting last operation done */
  ADC_WaitForManual();
 
  /* Calculate the voltage of  BAT1 */
  if (Division & ADC_BAT_CAPDIV)
  {
    CalResults->BATRTCVoltage = (((int16_t)ANA->ADCDATA2 - 49.588) / 7088.91575) + Vbatcap;
  }
  else
  {
    CalResults->BATRTCVoltage = (0.00015392*(int16_t)ANA->ADCDATA2 + 0.06667986) + Vbatres;
  }
  /* ADC resource release */
  ADC_DeInit();
  
  return (0);   
}

/**
  * @brief  Get Temperature(costs 6.5ms).
  * @note   This function costs about 6.5ms when SystemClock is 26M.
  *         ADC data refresh time is about 3.2ms.
  * @note   This function will release ADC resource(write ADC registers with their
  *         default reset values).
  * @note   ADC configurarion:
  *             - Trigger mode:       manual mode
  *             - Conversion mode:    single channel mode(Temperature channel 10)
  *             - ADC clock:          1.6M
  *             - Skip samples:       Skip first 4 samples
  *             - Down sampling rate: 1/512
  * @param  [out]CalResults.BAT1Voltage     is ignored
            [out]CalResults.BATRTCVoltage   is ignored
            [out]CalResults.Temperature     The value of Temperature
  * @retval 1  Temperature delta information checksum error.
            0  Function successed.
  */
uint32_t ADC_GetTemperature(ADC_CalResType *CalResults)
{
  int32_t P2;
  int16_t P1, P0;
  int16_t adc_data;
  uint32_t retval;
  NVR_RTCINFO RTC_DataStruct;
  ADCInitType ADC_InitStruct;
  
  /* Get RTC Px parameters */
  retval = NVR_GetInfo_LoadRTCData(&RTC_DataStruct);
  if (retval & 0x1U)
  {
    return (1);
  }
  else
  {
    P0 = RTC_DataStruct.RTCTempP0;
    P1 = RTC_DataStruct.RTCTempP1;
    P2 = RTC_DataStruct.RTCTempP2;
  }  
  
  /* ADC initialization */
  ADC_DeInit();
  ADC_InitStruct.TrigMode = ADC_TRIGMODE_MANUAL;
  ADC_InitStruct.ConvMode = ADC_CONVMODE_SINGLECHANNEL;
  ADC_InitStruct.Channel = ADC_CHANNEL10;
  ADC_InitStruct.ClockDivider = ADC_CLKDIV_4;
  ADC_InitStruct.ClockSource = ADC_CLKSRC_RCH;
  ADC_Init(&ADC_InitStruct);
  
  /* Configure 1/512 down-sampling rate */
  ADC_CICDownSamRateConfig(ADC_SDRSEL_DIV512);
  /* Enable ADC */
  ADC_Cmd(ENABLE);
  
  /*---------- Get ADC data1 ----------*/
  ANA->REG7 &= ~ANA_REG7_TMPCKOFF;
  /* Starts a manual ADC conversion */
  ADC_StartManual();
  /* Waiting Manual ADC conversion done */
  ADC_WaitForManual();
  adc_data = (int16_t)ADC_GetADCConversionValue(ADC_CHANNEL10);

  /* ADC resource release */
  ADC_DeInit();
    
  /* Calculate temperature */
  CalResults->Temperature = (float)((((P0 * ((adc_data*adc_data)>>16)) + P1*adc_data + P2) >> 8) / 256.0);
  
  return (0);
}

/**
  * @brief  ADC power control.
  * @param  NewState
                ENABLE
                DISABLE
  * @retval None
  */
void ADC_Cmd(uint32_t NewState)
{
  /* Check parameters */
  assert_parameters(IS_FUNCTIONAL_STATE(NewState)); 
  
  if (NewState == DISABLE)
    ana_reg3_tmp &= ~ANA_REG3_ADCPDN;
  else
    ana_reg3_tmp |= ANA_REG3_ADCPDN;
  
  ANA->REG3 = ana_reg3_tmp;
}

/**
  * @brief  Manual ADC trigger
  * @param  None
  * @retval None
  */
void ADC_StartManual(void)
{
  ANA->ADCCTRL |= ANA_ADCCTRL_MTRIG;
}

/**
  * @brief  Wait for the last Manual ADC conversion done.
  * @param  None
  * @retval None
  */
void ADC_WaitForManual(void)
{
  while (ANA->ADCCTRL & ANA_ADCCTRL_MTRIG)
  {
  }
}

/**
  * @brief  Stop current ADC conversion
  * @param  NewState
                ENABLE
                DISABLE
  * @retval None
  */
void ADC_StopConv(uint32_t NewState)
{
  /* Check parameters */
  assert_parameters(IS_FUNCTIONAL_STATE(NewState)); 

  if (NewState == ENABLE)
  {
    ANA->ADCCTRL |= ANA_ADCCTRL_STOP;
  }  
  else
  {
    ANA->ADCCTRL &= ~ANA_ADCCTRL_STOP;
  }
}

/**
  * @brief  ADC auto mode trigger source configure.
  * @param  TrigSource:
                ADC_TRIGSOURCE_OFF  
                ADC_TRIGSOURCE_TIM0  
                ADC_TRIGSOURCE_TIM1  
                ADC_TRIGSOURCE_TIM2  
                ADC_TRIGSOURCE_TIM3  
  * @retval None
  */
void ADC_TrigSourceConfig(uint32_t TrigSource)
{
  uint32_t tmp;
  
  /* Check parameters */
  assert_parameters(IS_ADC_TRIGSOURCE(TrigSource));
  
  tmp = ANA->ADCCTRL;
  tmp &= ~ANA_ADCCTRL_AEN;
  tmp |= TrigSource;
  ANA->ADCCTRL = tmp;  
}

/**
  * @brief  Resistor division enable control.
  * @param  NewState
                ENABLE   (x1/4)
                DISABLE  (x1)
  * @retval None
  */
void ADC_RESDivisionCmd(uint32_t NewState)
{
  /* Check parameters */
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState == ENABLE)
    ANA->REG1 |= ANA_REG1_RESDIV;
  else
    ANA->REG1 &= ~ANA_REG1_RESDIV;
}

/**
  * @brief  Cap division enable control.
  * @param  NewState
                ENABLE   (x1/4)
                DISABLE  (x1)
  * @retval None
  */
void ADC_CAPDivisionCmd(uint32_t NewState)
{
  /* Check parameters */
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState == ENABLE)
    ANA->REG1 |= ANA_REG1_GDE4;
  else
    ANA->REG1 &= ~ANA_REG1_GDE4;
}

/**
  * @brief  Pull down resistor(1M) for input signal.
  * @param  NewState
                ENABLE
                DISABLE
  * @retval None
  */
void ADC_PullDownRESCmd(uint32_t NewState)
{
  /* Check parameters */
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));

  if (NewState == ENABLE)  
    ANA->REG1 |= ANA_REG1_PRES_EN;
  else
    ANA->REG1 &= ~ANA_REG1_PRES_EN;
}

/**
  * @brief  CIC filter always on control.
  * @param  NewState
                ENABLE   (CIC filter always on)
                DISABLE  (CIC filter will be disabled when no ADC sample process is ongoing.)
  * @retval None
  */
void ADC_CICAlwaysOnCmd(uint32_t NewState)
{
  /* Check parameters */
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState == ENABLE)
    ANA->ADCCTRL |= ANA_ADCCTRL_CICAON;
  else
    ANA->ADCCTRL &= ~ANA_ADCCTRL_CICAON;
}

/**
  * @brief  CIC filter input inversion control.
  * @param  NewState
                ENABLE
                DISABLE
  * @retval None
  */
void ADC_CICINVCmd(uint32_t NewState)
{
  /* Check parameters */
  assert_parameters(IS_FUNCTIONAL_STATE(NewState));

  if (NewState == ENABLE)  
    ANA->ADCCTRL |= ANA_ADCCTRL_CICINV;
  else
    ANA->ADCCTRL &= ~ANA_ADCCTRL_CICINV;
}

/**
  * @brief  CIC output scale-down configure.
  * @param  ScaleDown:
                ADC_SCA_NONE
                ADC_SCA_DIV2
  * @retval None
  */
void ADC_CICScaleDownConfig(uint32_t ScaleDown)
{
  uint32_t tmp;
  
  /* Check parameters */
  assert_parameters(IS_ADC_SCA(ScaleDown));  
  
  tmp = ANA->ADCCTRL;
  tmp &= ~ANA_ADCCTRL_CICSCA;
  tmp |= ScaleDown;
  ANA->ADCCTRL = tmp;
}

/**
  * @brief  CIC output skip control.
  * @param  Skip:
                ADC_SKIP_4
                ADC_SKIP_5
                ADC_SKIP_6
                ADC_SKIP_7
                ADC_SKIP_0
                ADC_SKIP_1
                ADC_SKIP_2
                ADC_SKIP_3
  * @retval None
  */
void ADC_CICSkipConfig(uint32_t Skip)
{
  uint32_t tmp;
  
  /* Check parameters */
  assert_parameters(IS_ADC_SKIP(Skip));  
  
  tmp = ANA->ADCCTRL; 
  tmp &= ~ANA_ADCCTRL_CICSKIP;
  tmp |= Skip;
  ANA->ADCCTRL = tmp;
}

/**
  * @brief  CIC down sampling rate control.
  * @param  DSRSelection:
                ADC_SDRSEL_DIV512
                ADC_SDRSEL_DIV256
                ADC_SDRSEL_DIV128
                ADC_SDRSEL_DIV64
  * @retval None
  */
void ADC_CICDownSamRateConfig(uint32_t DSRSelection)
{
  uint32_t tmp;
  
  /* Check parameters */
  assert_parameters(IS_ADC_SDR(DSRSelection));  
  tmp = ANA->ADCCTRL;
  tmp &= ~ANA_ADCCTRL_DSRSEL;
  tmp |= DSRSelection;
  ANA->ADCCTRL = tmp;
}

/**
  * @brief  Get ADC vonversion value.
  * @param  Channel:
                 ADC_CHANNEL0 
                 ADC_CHANNEL1  
                 ADC_CHANNEL2  
                 ADC_CHANNEL3  
                 ADC_CHANNEL4  
                 ADC_CHANNEL5  
                 ADC_CHANNEL6  
                 ADC_CHANNEL7  
                 ADC_CHANNEL8  
                 ADC_CHANNEL9  
                 ADC_CHANNEL10 
                 ADC_CHANNEL11 
  * @retval ADC vonversion value.
  */
uint32_t ADC_GetADCConversionValue(uint32_t Channel)
{
  __IO uint32_t *addr;
  
  /* Check parameters */
  assert_parameters(IS_ADC_CHANNEL(Channel));
 
  addr = &ANA->ADCDATA0 + Channel;

  return *addr;
}

/**
  * @brief  ADC interrupt control.
  * @param  INTMask:
                ADC_INT_AUTODONE
                ADC_INT_MANUALDONE
            NewState
                ENABLE
                DISABLE
  * @retval None
  */
void ADC_INTConfig(uint32_t INTMask, uint32_t NewState)
{
  /* Check parameters */
  assert_parameters(IS_FUNCTIONAL_STATE(NewState)); 
  assert_parameters(IS_ADC_INT(INTMask));
  
  if (NewState == ENABLE)
    ANA->INTEN |= INTMask;
  else
    ANA->INTEN &= ~INTMask;
}

/**
  * @brief  Get auto done flag
  * @param  None
  * @retval 1   flag set
  *         0   flag reset.
  */
uint8_t ADC_GetAutoDoneFlag(void)
{
  if(ANA->INTSTS & ANA_INTSTS_INTSTS1)
    return 1;
  else
    return 0;
}

/**
  * @brief  Get manual done flag
  * @param  None
  * @retval 1   flag set
  *         0   flag reset.
  */
uint8_t ADC_GetManualDoneFlag(void)
{
  if(ANA->INTSTS & ANA_INTSTS_INTSTS0)
    return 1;
  else
    return 0;
}

/**
  * @brief  Clear auto done flag
  * @param  None
  * @retval None
  */
void ADC_ClearAutoDoneFlag(void)
{
  ANA->INTSTS = ANA_INTSTS_INTSTS1;
}

/**
  * @brief  Clear manual done flag
  * @param  None
  * @retval None
  */
void ADC_ClearManualDoneFlag(void)
{
  ANA->INTSTS = ANA_INTSTS_INTSTS0;
}

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
