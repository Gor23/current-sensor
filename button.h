#ifndef BUTTON_H_
#define BUTTON_H_

#define   BUTTON_SCAN_POLLTIME        100

enum BUTTONS_STATES
{
  NO_BUTTON_PRESSED,
  F_BUTTON_PRESSED,
  B_BUTTON_PRESSED,
  F_AND_B_BUTTON_PRESSED,  
};

extern volatile uint16_t button_timeToRunFunction;
extern void BUTTON_Manager(void);
extern void BUTTON_ResetStates(void);

#endif 