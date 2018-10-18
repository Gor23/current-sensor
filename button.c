#include "includes.h"

StateMachieneData buttonData;

volatile uint16_t button_timeToRunFunction = 0;
void BUTTON_SetPrevState(void);
void BUTTON_Manager(void);
void BUTTON_ResetStates(void);

void BUTTON_SetPrevState(void)
{
  buttonData.prevState = buttonData.currentState;
}

void BUTTON_ResetStates(void)
{
  buttonData.currentState = NO_BUTTON_PRESSED;
  BUTTON_SetPrevState();
}
void BUTTON_Manager(void)
{
  button_timeToRunFunction = BUTTON_SCAN_POLLTIME;
  if (!IN_1_STATE)
  {
    buttonData.currentState = F_BUTTON_PRESSED;
  } 
  if (!IN_2_STATE)
  {
    buttonData.currentState = B_BUTTON_PRESSED;
  }
  if (IN_2_STATE && IN_1_STATE)
  {
    buttonData.currentState = NO_BUTTON_PRESSED;
  }
  
  if (buttonData.prevState == buttonData.currentState)
  {
    return;
  }
  
  switch(buttonData.currentState)
  {
  /*case NO_BUTTON_PRESSED:
    if (buttonData.prevState == B_BUTTON_PRESSED)
    {
      ENGC_ChangeState(ENGC_B_END_MOVE);
    }
    else if (buttonData.prevState == F_BUTTON_PRESSED)
    {
      ENGC_ChangeState(ENGC_F_END_MOVE);
    }
    BUTTON_SetPrevState();
    break;*/
    
  case F_BUTTON_PRESSED:
    ENGC_ChangeState(ENGC_F_START_MOVE);
    break;
    
  case B_BUTTON_PRESSED:
    ENGC_ChangeState(ENGC_B_START_MOVE);;
    break;

  default: break;
  }
  BUTTON_SetPrevState();
}