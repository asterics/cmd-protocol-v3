/*
    trigger.h
*/

#define MAX_SMARTBUTTONS 8
#define MAX_ACTIONSTRING_LEN 20
#define CONDITION_TIMEOUT 1000

typedef struct s_buttoncontext
{
  uint16_t triggerID; // identifies the trigger source (e.g. Button1, xAxisUp, PressureStrongSip etc.)
  uint16_t eventID;   // identifies the event (e.g. pressed, released, hold, click3) - click count is stored in higher 8 bit!
  uint32_t condition;                 // pre-trigger-condition ID  (triggerID) | (eventID<<16))
  char action[MAX_ACTIONSTRING_LEN];  // the action to be triggerd, just a placeholder string
} t_buttoncontext;

uint16_t getTriggerID(String id);
uint16_t getEventID(String id);
void createTrigger(String id, String condition, String action);
void freeTriggers();
