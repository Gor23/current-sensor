#ifndef ENGCONTROL_H_
#define ENGCONTROL_H_

#include "main.h"


#define ENGC_FROM_START_TO_MOVE_POLLTIME        20
#define ENGC_MOVE_POLLTIME                      (uint16_t)4500
#define ENGC_FROM_MOVE_TO_END_POLLTIME          20
#define ENGC_BLOCK_POLLTIME                     (uint16_t)5000
#define ENGC_IDLE_POLLTIME                      (uint16_t)100
#define ENGC_ADC_START_DELAY                    150

enum ENGC_STATES
{
  ENGC_IDLE,
  ENGC_F_BLOCK,
  ENGC_B_BLOCK,
  ENGC_F_START_MOVE,
  ENGC_B_START_MOVE,
  ENGC_F_END_MOVE,
  ENGC_B_END_MOVE,
  ENGC_F_FAULT_END_MOVE,
  ENGC_B_FAULT_END_MOVE,
  ENGC_F_MOVE,
  ENGC_B_MOVE,
  ENGC_STOP,
  ENGC_ERROR,
};

extern StateMachieneData engcData;
extern volatile uint16_t engc_TimeToRunFunction;


extern void ENGC_MoveForvard(void);
extern void ENGC_MoveBack(void);
extern void ENGC_OverCurrentStop(void);
extern uint8_t ENGC_ChangeState (uint8_t newState);
extern void ENGC_StateManager(void);
extern void ENGC_ResetStates(void);

#endif 