/**
  ******************************************************************************
  * @file    v85xx_adc.h 
  * @author  VT Application Team
  * @version V1.0.0
  * @date    2017-06-22
  * @brief   ADC library.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#ifndef __V85XX_ADC_H
#define __V85XX_ADC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "v85xx.h"

typedef struct
{
  uint32_t TrigMode; 
  uint32_t ConvMode;  
  uint32_t ClockSource;  
  uint32_t ClockDivider;
  uint32_t Channel;
} ADCInitType;

//TrigMode
#define ADC_TRIGMODE_AUTO   0
#define ADC_TRIGMODE_MANUAL ANA_ADCCTRL_MTRIG
#define IS_ADC_TRIGMODE(__TRIGMODE__)  (((__TRIGMODE__) == ADC_TRIGMODE_AUTO) ||\
                                        ((__TRIGMODE__) == ADC_TRIGMODE_MANUAL))

//ConvMode
#define ADC_CONVMODE_SINGLECHANNEL 0
#define ADC_CONVMODE_MULTICHANNEL  1
#define IS_ADC_CONVMODE(__CONVMODE__)  (((__CONVMODE__) == ADC_CONVMODE_SINGLECHANNEL) ||\
                                        ((__CONVMODE__) == ADC_CONVMODE_MULTICHANNEL))

//ClockSource
#define ADC_CLKSRC_RCH  0
#define ADC_CLKSRC_PLLL ANA_ADCCTRL_CLKSEL
#define IS_ADC_CLKSRC(__CLKSRC__)  (((__CLKSRC__) == ADC_CLKSRC_RCH) ||\
                                    ((__CLKSRC__) == ADC_CLKSRC_PLLL))

//TrigSource
#define ADC_TRIGSOURCE_OFF      ANA_ADCCTRL_AEN_OFF
#define ADC_TRIGSOURCE_TIM0     ANA_ADCCTRL_AEN_TMR0 
#define ADC_TRIGSOURCE_TIM1     ANA_ADCCTRL_AEN_TMR1 
#define ADC_TRIGSOURCE_TIM2     ANA_ADCCTRL_AEN_TMR2 
#define ADC_TRIGSOURCE_TIM3     ANA_ADCCTRL_AEN_TMR3 
#define IS_ADC_TRIGSOURCE(__TRIGSOURCE__)  (((__TRIGSOURCE__) == ADC_TRIGSOURCE_OFF) ||\
                                            ((__TRIGSOURCE__) == ADC_TRIGSOURCE_TIM0) ||\
                                            ((__TRIGSOURCE__) == ADC_TRIGSOURCE_TIM1) ||\
                                            ((__TRIGSOURCE__) == ADC_TRIGSOURCE_TIM2) ||\
                                            ((__TRIGSOURCE__) == ADC_TRIGSOURCE_TIM3))
//CurrentTrim
#define ADC_CURRIT_1       ANA_REG0_CURRIT_1
#define ADC_CURRIT_DEC33   ANA_REG0_CURRIT_0
#define ADC_CURRIT_DEC66   ANA_REG0_CURRIT_3
#define ADC_CURRIT_DEC75   ANA_REG0_CURRIT_2
#define IS_ADC_CURRIT(__CURRIT__)  (((__CURRIT__) ==ADC_CURRIT_1 )     ||\
                                    ((__CURRIT__) ==ADC_CURRIT_DEC33 ) ||\
                                    ((__CURRIT__) ==ADC_CURRIT_DEC66 ) ||\
                                    ((__CURRIT__) ==ADC_CURRIT_DEC75 ))

//ClockDivider
#define ADC_CLKDIV_1    ANA_ADCCTRL_CLKDIV_1
#define ADC_CLKDIV_2    ANA_ADCCTRL_CLKDIV_2
#define ADC_CLKDIV_3    ANA_ADCCTRL_CLKDIV_3
#define ADC_CLKDIV_4    ANA_ADCCTRL_CLKDIV_4
#define ADC_CLKDIV_5    ANA_ADCCTRL_CLKDIV_5
#define ADC_CLKDIV_6    ANA_ADCCTRL_CLKDIV_6
#define ADC_CLKDIV_7    ANA_ADCCTRL_CLKDIV_7
#define ADC_CLKDIV_8    ANA_ADCCTRL_CLKDIV_8
#define ADC_CLKDIV_9    ANA_ADCCTRL_CLKDIV_9
#define ADC_CLKDIV_10   ANA_ADCCTRL_CLKDIV_10
#define ADC_CLKDIV_11   ANA_ADCCTRL_CLKDIV_11
#define ADC_CLKDIV_12   ANA_ADCCTRL_CLKDIV_12
#define ADC_CLKDIV_13   ANA_ADCCTRL_CLKDIV_13
#define ADC_CLKDIV_14   ANA_ADCCTRL_CLKDIV_14
#define ADC_CLKDIV_15   ANA_ADCCTRL_CLKDIV_15
#define ADC_CLKDIV_16   ANA_ADCCTRL_CLKDIV_16
#define IS_ADC_CLKDIV(__CLKDIV__)  (((__CLKDIV__) == ADC_CLKDIV_1)  ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_2)  ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_3)  ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_4)  ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_5)  ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_6)  ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_7)  ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_8)  ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_9)  ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_10) ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_11) ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_12) ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_13) ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_14) ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_15) ||\
                                    ((__CLKDIV__) == ADC_CLKDIV_16))

//Channel
#define ADC_CHANNEL0       0   
#define ADC_CHANNEL1       1   
#define ADC_CHANNEL2       2   
#define ADC_CHANNEL3       3   
#define ADC_CHANNEL4       4   
#define ADC_CHANNEL5       5   
#define ADC_CHANNEL6       6   
#define ADC_CHANNEL7       7   
#define ADC_CHANNEL8       8   
#define ADC_CHANNEL9       9   
#define ADC_CHANNEL10      10  
#define ADC_CHANNEL11      11

#define IS_ADC_CHANNEL(__CHANNEL__)  (((__CHANNEL__) == ADC_CHANNEL0)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL1)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL2)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL3)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL4)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL5)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL6)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL7)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL8)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL9)  ||\
                                      ((__CHANNEL__) == ADC_CHANNEL10) ||\
                                      ((__CHANNEL__) == ADC_CHANNEL11))
                                        
//INTMask
#define ADC_INT_AUTODONE    ANA_INTEN_INTEN1
#define ADC_INT_MANUALDONE  ANA_INTEN_INTEN0
#define ADC_INT_Msk        (ADC_INT_AUTODONE | ADC_INT_MANUALDONE)
#define IS_ADC_INT(__INT__)  ((((__INT__) & ADC_INT_Msk) != 0U) &&\
                              (((__INT__) & ~ADC_INT_Msk) == 0U))

//ScaleDown
#define ADC_SCA_NONE    0
#define ADC_SCA_DIV2    ANA_ADCCTRL_CICSCA
#define IS_ADC_SCA(__SCA__)  (((__SCA__) == ADC_SCA_NONE) || ((__SCA__) == ADC_SCA_DIV2))

//Skip
#define ADC_SKIP_4  ANA_ADCCTRL_CICSKIP_4
#define ADC_SKIP_5  ANA_ADCCTRL_CICSKIP_5
#define ADC_SKIP_6  ANA_ADCCTRL_CICSKIP_6
#define ADC_SKIP_7  ANA_ADCCTRL_CICSKIP_7
#define ADC_SKIP_0  ANA_ADCCTRL_CICSKIP_0
#define ADC_SKIP_1  ANA_ADCCTRL_CICSKIP_1
#define ADC_SKIP_2  ANA_ADCCTRL_CICSKIP_2
#define ADC_SKIP_3  ANA_ADCCTRL_CICSKIP_3
#define IS_ADC_SKIP(__SKIP__)  (((__SKIP__) == ADC_SKIP_4) ||\
                                ((__SKIP__) == ADC_SKIP_5) ||\
                                ((__SKIP__) == ADC_SKIP_6) ||\
                                ((__SKIP__) == ADC_SKIP_7) ||\
                                ((__SKIP__) == ADC_SKIP_0) ||\
                                ((__SKIP__) == ADC_SKIP_1) ||\
                                ((__SKIP__) == ADC_SKIP_2) ||\
                                ((__SKIP__) == ADC_SKIP_3))

//DSRSelection
#define ADC_SDRSEL_DIV512  ANA_ADCCTRL_DSRSEL_512
#define ADC_SDRSEL_DIV256  ANA_ADCCTRL_DSRSEL_256
#define ADC_SDRSEL_DIV128  ANA_ADCCTRL_DSRSEL_128
#define ADC_SDRSEL_DIV64   ANA_ADCCTRL_DSRSEL_64
#define IS_ADC_SDR(__SDR__)  (((__SDR__) == ADC_SDRSEL_DIV512) ||\
                              ((__SDR__) == ADC_SDRSEL_DIV256) ||\
                              ((__SDR__) == ADC_SDRSEL_DIV128) ||\
                              ((__SDR__) == ADC_SDRSEL_DIV64))

typedef struct
{
  float BAT1Voltage;
  float BATRTCVoltage;
  float Temperature;
} ADC_CalResType;
//Division
#define ADC_BAT_CAPDIV  (ANA_REG1_GDE4 | ANA_REG1_PRES_EN)
#define ADC_BAT_RESDIV  (ANA_REG1_RESDIV)
                                  
#define IS_ADC_BATDIV(__BATDIV__)  (((__BATDIV__) == ADC_BAT_CAPDIV) ||\
                                    ((__BATDIV__) == ADC_BAT_RESDIV))

/* Exported Functions ------------------------------------------------------- */
/* ADC Exported Functions Group1: 
                                  (De)Initialization -------------------------*/
void ADC_DeInit(void);
void ADC_StructInit(ADCInitType* ADC_InitStruct);
void ADC_Init(ADCInitType* ADC_InitStruct);
/* ADC Exported Functions Group2: 
                                  Get NVR Info, Calculate datas --------------*/
uint32_t ADC_GetBAT1Voltage_Fast(uint32_t Division, ADC_CalResType *CalResults);
uint32_t ADC_GetBAT1Voltage_Normal(uint32_t Division, ADC_CalResType *CalResults);
uint32_t ADC_GetBATRTCVoltage_Fast(uint32_t Division, ADC_CalResType *CalResults);
uint32_t ADC_GetBATRTCVoltage_Normal(uint32_t Division, ADC_CalResType *CalResults);
uint32_t ADC_GetTemperature(ADC_CalResType *CalResults);
/* ADC Exported Functions Group3: 
                                  Interrupt (flag) ---------------------------*/
uint32_t ADC_GetADCConversionValue(uint32_t Channel);
void ADC_INTConfig(uint32_t INTMask, uint32_t NewState);
uint8_t ADC_GetAutoDoneFlag(void);
uint8_t ADC_GetManualDoneFlag(void);
void ADC_ClearAutoDoneFlag(void);
void ADC_ClearManualDoneFlag(void);
/* ADC Exported Functions Group4: 
                                  MISC Configuration -------------------------*/
void ADC_Cmd(uint32_t NewState);
void ADC_StartManual(void);
void ADC_WaitForManual(void);
void ADC_StopConv(uint32_t NewState);
void ADC_TrigSourceConfig(uint32_t TrigSource);
void ADC_BiasConfig(uint32_t CurrentTrim);
void ADC_RESDivisionCmd(uint32_t NewState);
void ADC_CAPDivisionCmd(uint32_t NewState);
void ADC_PullDownRESCmd(uint32_t NewState);
//CIC Control
void ADC_CICAlwaysOnCmd(uint32_t NewState);
void ADC_CICINVCmd(uint32_t NewState);
void ADC_CICScaleDownConfig(uint32_t ScaleDown);
void ADC_CICSkipConfig(uint32_t Skip);
void ADC_CICDownSamRateConfig(uint32_t DSRSelection);

#ifdef __cplusplus
}
#endif
     
#endif  /* __V85XX_ADC_H */

/******************* (C) COPYRIGHT Vango Technologies, Inc *****END OF FILE****/
