/*
    trigger.h
*/
#include "SmartButton.h"

using namespace smartbutton;

#define ACTIONSTRING_BUFLEN 100

uint32_t getTriggerID(String id, int* actpos);
SmartButton * createTrigger(SmartButton * parent, String id, String action);
void freeTriggers();
void toggleDebugOutput();
