#include "includes.h"



StateMachieneData engcData;
volatile uint16_t engc_TimeToRunFunction; // time to run current function (1=1ms)    

void ENGC_SetPrevState(void);
void ENGC_MoveForvard(void);
void ENGC_MoveBack(void);
void ENGC_OverCurrentStop(void);
uint8_t ENGC_ChangeState (uint8_t newState);
void ENGC_ChangeStateWithTimeout(uint8_t nextState, uint16_t timeout);
void ENGC_StateManager(void);

void ENGC_ResetStates(void)
{
  engcData.currentState = ENGC_STOP;
  engcData.nextState = ENGC_IDLE;
  ENGC_SetPrevState();
}

void ENGC_OverCurrentStop(void)
{
  //TODO mosfet off
  if (engcData.currentState == ENGC_B_MOVE)
  {
    engcData.currentState = ENGC_B_BLOCK;
  }
  else if (engcData.currentState == ENGC_F_MOVE)
  {
    engcData.currentState = ENGC_F_BLOCK;
  }
  else
  {
    //engcData.currentState = ENGC_ERROR;
  }
}

void ENGC_ChangeStateWithTimeout(uint8_t nextState, uint16_t timeout)
{
  engc_TimeToRunFunction = timeout;
  engcData.nextState = nextState;
}


uint8_t ENGC_ChangeState (uint8_t newState)
{
  switch (newState)
  {
  case ENGC_IDLE:
    engcData.currentState = ENGC_IDLE;
    break;
    
  case ENGC_F_BLOCK:
    if(engcData.currentState == ENGC_F_START_MOVE ||
          engcData.currentState == ENGC_F_MOVE ||
            engcData.currentState == ENGC_F_END_MOVE)
    {
      engcData.currentState = ENGC_F_BLOCK;
    }
    else
    {
      return true;
    }
    break;
    
  case ENGC_B_BLOCK:
    if(engcData.currentState == ENGC_B_START_MOVE ||
      engcData.currentState == ENGC_B_MOVE ||
        engcData.currentState == ENGC_B_END_MOVE)
    {
      engcData.currentState = ENGC_B_BLOCK;
    }
    else
    {
      return true;
    }
    break;
    
  case ENGC_F_START_MOVE:
    if(engcData.currentState == ENGC_STOP||
       engcData.currentState == ENGC_B_BLOCK)
    {
      engc_TimeToRunFunction = 0;
      engcData.currentState = ENGC_F_START_MOVE;
    }
    else
    {
      return true;
    }
    break;
    
  case ENGC_B_START_MOVE:
    if(engcData.currentState == ENGC_STOP||
       engcData.currentState == ENGC_F_BLOCK)
    {
      engc_TimeToRunFunction = 0;
      engcData.currentState = ENGC_B_START_MOVE;
    }
    else
    {
      return true;
    }
    break;
    
  case ENGC_F_END_MOVE:
    if(engcData.currentState == ENGC_F_MOVE ||
       engcData.currentState == ENGC_F_START_MOVE ||
       engcData.currentState == ENGC_F_END_MOVE)
    {
      engc_TimeToRunFunction = 0;
      engcData.currentState = ENGC_F_END_MOVE;
    }
    else
    {
      return true;
    }
    break;
    
  case ENGC_B_END_MOVE:
    if(engcData.currentState == ENGC_B_MOVE ||
       engcData.currentState == ENGC_B_START_MOVE ||
       engcData.currentState == ENGC_B_END_MOVE)
    {
      engc_TimeToRunFunction = 0;
      engcData.currentState = ENGC_B_END_MOVE;
    }
    else
    {
      return true;
    }
    break;
    
  case ENGC_F_FAULT_END_MOVE:
    if(engcData.currentState == ENGC_F_MOVE ||
       engcData.currentState == ENGC_F_START_MOVE ||
       engcData.currentState == ENGC_F_END_MOVE ||
       engcData.currentState == ENGC_F_FAULT_END_MOVE)
    {
      engc_TimeToRunFunction = 0;
      engcData.currentState = ENGC_F_END_MOVE;
    }
    else
    {
      return true;
    }
    break;
    
  case ENGC_B_FAULT_END_MOVE:
    if(engcData.currentState == ENGC_B_MOVE ||
       engcData.currentState == ENGC_B_START_MOVE ||
       engcData.currentState == ENGC_B_END_MOVE||
       engcData.currentState == ENGC_B_FAULT_END_MOVE)
    {
      engc_TimeToRunFunction = 0;
      engcData.currentState = ENGC_B_END_MOVE;
    }
    else
    {
      return true;
    }
    break;
    
    
  case ENGC_B_MOVE:
    if(engcData.currentState == ENGC_B_START_MOVE)
    {
      engcData.currentState = ENGC_B_MOVE;
    }
    else
    {
      return true;
    }
    break;
    
  case ENGC_F_MOVE:
     if(engcData.currentState == ENGC_F_START_MOVE)
    {
      engcData.currentState = ENGC_F_MOVE;
    }
    else
    {
      return true;
    }
    break;
    
  case ENGC_STOP:
    if(engcData.currentState == ENGC_B_END_MOVE||
       engcData.currentState == ENGC_F_END_MOVE||
         engcData.currentState == ENGC_F_BLOCK||
          engcData.currentState == ENGC_B_BLOCK)
    {
      engcData.currentState = ENGC_STOP;
    }
    else
    {
      return true;
    }  
    
    break;
    
  case ENGC_ERROR:
    
    break;
  default: break;
  }
  engcData.nextState = ENGC_IDLE;
  return false;
}

void ENGC_SetPrevState(void)
{
   engcData.prevState = engcData.currentState;
} 

void ENGC_StateManager(void)
{
   
  if (engcData.nextState != ENGC_IDLE)
  {
    engcData.currentState = engcData.nextState;
    engcData.nextState = ENGC_IDLE;
  }
  if(engcData.currentState == engcData.prevState)
  {
    engc_TimeToRunFunction = ENGC_IDLE_POLLTIME;
    return;
  }
  
  switch(engcData.currentState)
  {
  case ENGC_B_START_MOVE:
    REL_1_ON;
    REL_2_OFF;
    ENGC_SetPrevState();
    ENGC_ChangeState(ENGC_B_MOVE);
    engc_TimeToRunFunction = ENGC_FROM_START_TO_MOVE_POLLTIME;
    break;
    
   case ENGC_F_START_MOVE:
    REL_2_ON;
    REL_1_OFF;
    ENGC_SetPrevState();
    ENGC_ChangeState(ENGC_F_MOVE);
    engc_TimeToRunFunction = ENGC_FROM_START_TO_MOVE_POLLTIME;
    break;
    
   case ENGC_F_MOVE:
    MOSFET_ON;
    ADC_StartConversionCycle();
    ENGC_SetPrevState();
    ENGC_ChangeState(ENGC_F_FAULT_END_MOVE);
    engc_TimeToRunFunction = ENGC_MOVE_POLLTIME;
    break;
    
   case ENGC_B_MOVE:
    MOSFET_ON;
    ADC_StartConversionCycle();
    ENGC_SetPrevState();
    ENGC_ChangeState(ENGC_B_FAULT_END_MOVE);
    engc_TimeToRunFunction = ENGC_MOVE_POLLTIME;
    break;
    
   case ENGC_B_FAULT_END_MOVE:
    MOSFET_OFF;
    ENGC_SetPrevState();
    ENGC_ChangeState(ENGC_B_BLOCK);
    engc_TimeToRunFunction = ENGC_FROM_MOVE_TO_END_POLLTIME;
    break;
    
   case ENGC_F_FAULT_END_MOVE:
    MOSFET_OFF;
    ENGC_SetPrevState();
    ENGC_ChangeState(ENGC_F_BLOCK);
    engc_TimeToRunFunction = ENGC_FROM_MOVE_TO_END_POLLTIME;
    break;
    
   case ENGC_B_END_MOVE:
    MOSFET_OFF;
    ENGC_SetPrevState();
    ENGC_ChangeState(ENGC_STOP);
    engc_TimeToRunFunction = ENGC_FROM_MOVE_TO_END_POLLTIME;
    break;
    
  case ENGC_F_END_MOVE:
    MOSFET_OFF;
    ENGC_SetPrevState();
    ENGC_ChangeState(ENGC_STOP);
    engc_TimeToRunFunction = ENGC_FROM_MOVE_TO_END_POLLTIME;
    break;
    
   case ENGC_F_BLOCK:
    REL_2_OFF;
    REL_1_OFF;
    ENGC_SetPrevState();
    ENGC_ChangeStateWithTimeout(ENGC_STOP, ENGC_BLOCK_POLLTIME);
    break;
    
   case ENGC_B_BLOCK:
    REL_2_OFF;
    REL_1_OFF;
    ENGC_SetPrevState();
    ENGC_ChangeStateWithTimeout(ENGC_STOP, ENGC_BLOCK_POLLTIME);
    break;
    
   case ENGC_STOP:
    REL_2_OFF;
    REL_1_OFF;
    ENGC_ResetStates();
    //ENGC_SetPrevState();
    //ENGC_ChangeState(ENGC_STOP);
    engc_TimeToRunFunction = ENGC_IDLE_POLLTIME;
    break;
    
  default: break;
  }
}