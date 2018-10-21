#ifndef MAIN_H
#define MAIN_H

#define IN_1_STATE    PA_IDR_IDR3
#define IN_2_STATE    PA_IDR_IDR2
#define IN_3_STATE    PA_IDR_IDR1
#define BUTTON_STATE  PD_IDR_IDR1

#define MOSFET_ON     PC_ODR_ODR3=1
#define MOSFET_OFF    PC_ODR_ODR3=1

#define REL_1_ON      PC_ODR_ODR5=1
#define REL_1_OFF     PC_ODR_ODR5=0

#define REL_2_ON      PC_ODR_ODR6=1
#define REL_2_OFF     PC_ODR_ODR6=0

#define LED_2_ON      PC_ODR_ODR7=1
#define LED_2_OFF     PC_ODR_ODR7=0
#define LED_2_STATE   PC_IDR_IDR7

#define LED_1_ON      PD_ODR_ODR3=1
#define LED_1_OFF     PD_ODR_ODR3=0
#define LED_1_STATE   PD_IDR_IDR3

#define F_CPU                   16000000UL
#define SYSTICK_PRSC            7               //128 prescaller
#define SYSTICK_CONST           123

#define true    1
#define false   0

typedef struct 
{
  uint8_t currentState;
  uint8_t prevState;
  uint8_t nextState;
} StateMachieneData;

#endif
