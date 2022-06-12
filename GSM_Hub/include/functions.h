#ifndef _MAIN_FUNCTIONS_H_
#define _MAIN_FUNCTIONS_H_

#include "main.h"

//-------------------------------------------------------------------------------




//-------------------------------------------------------------------------------
bool WiFiConnectionState(void);
void wifi_STA_init(void);
void wifi_AP_init(void);
void GSM_RESET();
bool sendATCommand(const char* cmd, bool waiting, bool process = true);
// bool sendATCommand(char* cmd, bool waiting);
bool GSM_responseProcess(void);





#endif //_MAIN_FUNCTIONS_H_
