#include "includes.h"

uint8_t adcConversionCounter = 0;
uint16_t adcBuffer[ADC_NUMBER_OF_CONVERSIONS_CYCLES];
uint16_t adcData = 0;
uint16_t adc_timeToRunFunction = 0;
uint8_t adcDataReady = false;

void (*ADC_Function)(void);

void ADC_Init(void);
uint16_t ADC_GetConversionValue(void);
void ADC_CalculateResult(void);
void ADC_StartConversionCycle(void);
void ADC_EmptyFunction(void);

void ADC_Init(void)
{
  //interrupt enable
  ADC_CSR_EOCIE = true;
  //chanel 2
  ADC_CSR_CH = CURRENT_CHANEL;
  //prescaler 18
  ADC_CR1_SPSEL = 7;
  
  
  ADC_TDRL &= (uint8_t)(~(uint8_t)((uint8_t)0x01 << (uint8_t)CURRENT_CHANEL));
  ADC_TDRL &= (uint8_t)(~(uint8_t)((uint8_t)0x01 << (uint8_t)VOLTAGE_CHANEL));
  
  ADC_Function = &ADC_EmptyFunction;
}

void ADC_StartConversion(void)
{
  ADC_CR1_ADON = true;
  ADC_CR1_ADON = true;
}

uint16_t ADC_GetConversionValue(void)
{
  uint16_t temph = 0;
  uint8_t templ = 0;

  if (ADC_CR2_ALIGN) /* Right alignment */
  {
    /* Read LSB first */
    templ = ADC_DRL;
    /* Then read MSB */
    temph = ADC_DRH;

    temph = (uint16_t)(templ | (uint16_t)(temph << (uint8_t)8));
  }
  else /* Left alignment */
  {
    /* Read MSB first*/
    temph = ADC_DRH;
    /* Then read LSB */
    templ = ADC_DRL;

    temph = (uint16_t)((uint16_t)((uint16_t)templ << 6) | (uint16_t)((uint16_t)temph << 8));
  }

  return ((uint16_t)temph);
}

void ADC_CalculateResult(void)
{
  uint8_t i = 0;
  uint32_t tempAdcData = 0;
  for(i=0; i<ADC_NUMBER_OF_CONVERSIONS_CYCLES; i++)
  {
    tempAdcData += adcBuffer[i];
  }
  adcData = (uint16_t)(tempAdcData/ADC_NUMBER_OF_CONVERSIONS_CYCLES);
  adc_timeToRunFunction = 5000;
  ADC_Function = &ADC_EmptyFunction;
  if (adcData > ADC_ENGINE_OVERCURRENT)
  {
    if (engcData.currentState == ENGC_F_MOVE||
        engcData.currentState == ENGC_F_END_MOVE||
        engcData.currentState == ENGC_F_FAULT_END_MOVE||
        engcData.currentState == ENGC_F_START_MOVE)
    {
      ENGC_ChangeState(ENGC_F_FAULT_END_MOVE);
    }
    else if(engcData.currentState == ENGC_B_MOVE||
        engcData.currentState == ENGC_B_END_MOVE||
        engcData.currentState == ENGC_B_FAULT_END_MOVE||
        engcData.currentState == ENGC_B_START_MOVE)
    {
      ENGC_ChangeState(ENGC_B_FAULT_END_MOVE);
    }
  }
  adcDataReady = true;
}

void ADC_EmptyFunction(void)
{
  adc_timeToRunFunction = 5000;
}

void ADC_StartConversionCycle(void)
{
  adcConversionCounter = 0;
  adcDataReady = false;
  ADC_StartConversion();
}

void ADC_StopConversionCycle(void)
{
  ADC_CR1_ADON = false;
}
