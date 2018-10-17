#include "iostm8s003f3.h"
#include "stdint.h"
#include "intrinsics.h"
#include "main.h"

#define LED_HANDLER_POLLTIME 1000

#define CURRENT_CHANEL  2
#define VOLTAGE_CHANEL  3

void systemInit(void);
void gpioInit(void);
void sysTickInit(void);
void LED_ledUpdate(void);

void ADC_Init(void);
void ADC_StartConversion(void);
void ADC_StartConversionCycle(void);
void ADC_EmptyFunction(void);
uint16_t ADC_GetConversionValue(void);
void ADC_CalculateResult(void);

void buttonHandler(void);

void (*ADC_Function)(void);

volatile uint16_t timeToRunButtonHandler = 0;
volatile uint16_t timeToRunLedHandler = 0;
volatile uint16_t timeToRunADCHandler = 0;
volatile uint8_t stateMachiene;

volatile uint16_t adcBuffer[ADC_NUMBER_OF_CONVERSIONS_CYCLES];
volatile uint8_t adcConversionCounter = 0;
volatile uint8_t adcDataReady = false;
volatile uint16_t adcData = 0;

#pragma vector = TIM4_OVR_UIF_vector
__interrupt void systickHandler(void)
{
  if(timeToRunButtonHandler) timeToRunButtonHandler--;
  if(timeToRunLedHandler) timeToRunLedHandler--;
  if(timeToRunADCHandler) timeToRunADCHandler--;
  TIM4_SR_UIF = false; 
}

#pragma vector = ADC1_EOC_vector
__interrupt void adcHandler(void)
{
  if (adcConversionCounter<ADC_NUMBER_OF_CONVERSIONS_CYCLES)
  {
   adcBuffer[adcConversionCounter] = ADC_GetConversionValue();
   adcConversionCounter++;
  }
  else
  {
    ADC_Function = &ADC_CalculateResult;
    timeToRunADCHandler = 0;
  }
  ADC_CSR_EOC = false; 
}

int main( void )
{
  systemInit();
  gpioInit();
  sysTickInit();
  ADC_Init();
  asm("rim"); //Разрешаем прерывания
  
  while(1)
  {
    if(!timeToRunLedHandler) 
    {
      LED_ledUpdate();
    }
    
    if(!timeToRunButtonHandler)
    {
      buttonHandler();
    }
    if(!timeToRunADCHandler)
    {
      (*ADC_Function)();
    }
  }
}

void LED_ledUpdate(void)
{
    if(LED_1_STATE) LED_1_OFF;
    else LED_1_ON;
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
  PC_DDR_DDR5 = 1;
  PC_DDR_DDR6 = 1;
  PC_DDR_DDR7 = 1;
  
   //push-pull////
  PD_CR1_C13 = 1;
  PC_CR1_C15 = 1;
  PC_CR1_C16 = 1;
  PC_CR1_C17 = 1;
  
  //high speeed///
  PD_CR2_C23 = 1;
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

void ADC_Init(void)
{
  //interrupt enable
  ADC_CSR_EOCIE = false;
  //chanel 2
  ADC_CSR_CH = VOLTAGE_CHANEL;
  //prescaler 18
  ADC_CR1_SPSEL = 7;
  
  
  ADC_TDRL &= (uint8_t)(~(uint8_t)((uint8_t)0x01 << (uint8_t)CURRENT_CHANEL));
  ADC_TDRL &= (uint8_t)(~(uint8_t)((uint8_t)0x01 << (uint8_t)VOLTAGE_CHANEL));
  timeToRunADCHandler = 5000;
  ADC_Function = &ADC_EmptyFunction;
  
  ADC_StartConversionCycle();
  
}

void ADC_EmptyFunction(void)
{
  timeToRunADCHandler = 5000;
}

void ADC_StartConversion(void)
{
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
  timeToRunADCHandler = 5000;
  ADC_Function = &ADC_EmptyFunction;
  adcDataReady = true;
}

void ADC_StartConversionCycle(void)
{
  adcConversionCounter = 0;
  adcDataReady = false;
  ADC_StartConversion();
}

 void buttonHandler(void)
 {
    if  (IN_1_STATE)  stateMachiene = GO_FORWARD;
    else if (IN_2_STATE) stateMachiene = GO_BACKWARD;
    else stateMachiene = STOP;
    timeToRunButtonHandler = BUTTON_HANDLER_POLLTIME;
 }


