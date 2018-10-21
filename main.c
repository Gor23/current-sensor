#include "includes.h"

#define LED_HANDLER_POLLTIME 1000

void systemInit(void);
void gpioInit(void);
void sysTickInit(void);
void LED_ledUpdate(void);

volatile uint16_t timeToRunLedHandler = 0;

#pragma vector = TIM4_OVR_UIF_vector
__interrupt void systickHandler(void)
{
  if(button_timeToRunFunction) button_timeToRunFunction--;
  if(timeToRunLedHandler) timeToRunLedHandler--;
  if(engc_TimeToRunFunction) engc_TimeToRunFunction--;
  if(adc_timeToRunFunction) adc_timeToRunFunction--;
  if(adc_timeToRunDelayFunction) adc_timeToRunDelayFunction--;
  TIM4_SR_UIF = false; 
}


#pragma vector = ADC1_EOC_vector
__interrupt void adcHandler(void)
{
  if (adcConversionCounter<ADC_NUMBER_OF_CONVERSIONS_CYCLES)
  {
   adcBuffer[adcConversionCounter] = ADC_GetConversionValue();
   adcConversionCounter++;
   ADC_CR1_ADON = true;
  }
  else
  {
    ADC_Function = &ADC_CalculateResult;
    adc_timeToRunFunction = 0;
  }
  ADC_CSR_EOC = false; 
}


int main( void )
{
  systemInit();
  gpioInit();
  sysTickInit();
  ADC_Init();
  BUTTON_ResetStates();
  ENGC_ResetStates();
  asm("rim"); //Разрешаем прерывания
  
  while(1)
  {
    if(!timeToRunLedHandler) LED_ledUpdate();
    if(!button_timeToRunFunction) BUTTON_Manager();
    if(!engc_TimeToRunFunction) ENGC_StateManager();
    if(!adc_timeToRunFunction) (*ADC_Function)();
    if(!adc_timeToRunDelayFunction)(*ADC_DelayFunction)();
  }
}

void LED_ledUpdate(void)
{
    if(engcData.currentState == ENGC_B_BLOCK)
    {
      LED_2_OFF;
      LED_1_ON;
    }
    else if(engcData.currentState == ENGC_F_BLOCK)
    {
      LED_1_OFF;
      LED_2_ON;
    }
    else
    {
          if(LED_1_STATE) LED_1_OFF;
          else LED_1_ON;
          if(LED_2_STATE) LED_2_OFF;
          else LED_2_ON;
    }  
    timeToRunLedHandler = LED_HANDLER_POLLTIME;
}

void systemInit(void)
{
  CLK_ICKR = 0; // 
  CLK_ICKR_HSIEN = 1; // HSI ON
  CLK_ECKR = 0; // 
  while (CLK_ICKR_HSIRDY == 0); // 
  CLK_CKDIVR = 0; // 
  CLK_PCKENR1 = 0xff; // 
  CLK_PCKENR2 = 0xff; // 
  CLK_CCOR = 0;
  CLK_HSITRIMR = 0;
  CLK_SWIMCCR = 0;
  CLK_SWR = 0xe1;
  CLK_SWCR = 0;
  CLK_SWCR_SWEN = 1; // 
  while (CLK_SWCR_SWBSY != 0);
}

void gpioInit(void)
{
  // inputs//////
  // gpio input////
  PA_DDR_DDR1 = 0;
  PA_DDR_DDR2 = 0;
  PA_DDR_DDR3 = 0;
  PD_DDR_DDR4 = 0;
  // internal pull up /////////
  PA_CR1_C11 = 1;
  PA_CR1_C12 = 1;
  PA_CR1_C13 = 1;
  PD_CR1_C14 = 1;
  // ext interrupt off
  PA_CR2_C21 = 0;
  PA_CR2_C22 = 0;
  PA_CR2_C23 = 0;
  PD_CR2_C24 = 0;
  
  //outputs//////
  //gpio out////
  PD_DDR_DDR3 = 1;
  PC_DDR_DDR3 = 1;
  PC_DDR_DDR5 = 1;
  PC_DDR_DDR6 = 1;
  PC_DDR_DDR7 = 1;
  
   //push-pull////
  PD_CR1_C13 = 1;
  PC_CR1_C13 = 1;
  PC_CR1_C15 = 1;
  PC_CR1_C16 = 1;
  PC_CR1_C17 = 1;
  
  //high speeed///
  PD_CR2_C23 = 1;
  PC_CR2_C23 = 1;
  PC_CR2_C25 = 1;
  PC_CR2_C26 = 1;
  PC_CR2_C27 = 1;
 
}

void sysTickInit(void)
{
  //Настройка Таймера4
    //Interrupts for OVF only, one pulse mod (start timer erytime when ovf)
    TIM4_CR1 |= MASK_TIM4_CR1_URS;
    // Прерывание по обновлению счетного регистра разрешено
    TIM4_IER = MASK_TIM4_IER_UIE;
    TIM4_ARR = SYSTICK_CONST;
    TIM4_PSCR = SYSTICK_PRSC;
    TIM4_CR1 |= MASK_TIM4_CR1_CEN;
}





