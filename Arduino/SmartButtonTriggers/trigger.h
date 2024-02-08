/*
    trigger.h
*/

#define MAX_SMARTBUTTONS 8
#define MAX_ACTIONSTRING_LEN 20
#define MAX_BLOCKLIST 5
#define CONDITION_TIMEOUT 800

typedef struct s_buttoncontext
{
  uint32_t triggerID; // identifies the trigger source (e.g. Button1, xAxisUp, PressureStrongSip etc.)  and event
                      // event (e.g. pressed, released, hold, click) sotred in bit 16-23, click count is stored in bit 24-31
  uint32_t condition;   // pre-trigger (condition) ID  
  uint32_t blockedBy;
  uint32_t delayActionTimeout;
  char action[MAX_ACTIONSTRING_LEN];  // the action to be triggered, just a placeholder string
} t_buttoncontext;


uint32_t getTriggerID(String id);
void createTrigger(String id, String condition, String action);
void removeBlock(uint32_t triggerID);
void updateBlocks();
void freeTriggers();
void toggleDebugOutput();
