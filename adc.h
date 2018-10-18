#ifndef ADC_H_
#define ADC_H_

#define ADC_NUMBER_OF_CONVERSIONS_CYCLES 20
#define ADC_ENGINE_OVERCURRENT           50

extern uint8_t adcConversionCounter;
extern uint16_t adcBuffer[ADC_NUMBER_OF_CONVERSIONS_CYCLES];
extern uint16_t adcData;
extern uint16_t adc_timeToRunFunction;
extern uint16_t adc_timeToRunDelayFunction;
extern uint8_t adcDataRead;
extern void (*ADC_Function)(void);
extern void (*ADC_DelayFunction)(void);

extern void ADC_Init(void);
extern uint16_t ADC_GetConversionValue(void);
extern void ADC_CalculateResult(void);
extern void ADC_StartConversionCycle(void);
extern void ADC_StopConversionCycle(void);
extern void ADC_RunAdcWithDelay(uint16_t delay);
extern void ADC_DelayHandler(void);

#define CURRENT_CHANEL  2
#define VOLTAGE_CHANEL  3

#endif 