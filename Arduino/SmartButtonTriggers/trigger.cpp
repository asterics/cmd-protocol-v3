/*
    trigger.cpp
*/

#include <SmartButton.h>
#include "trigger.h"

using namespace smartbutton;

extern bool isPressedHandler(SmartButton *button);   // implements all isPressedHandlers for different trigger types!

SmartButton * buttons[MAX_SMARTBUTTONS] = {0};
int numButtons = 0;

uint32_t getTriggerID(String id) {
  uint16_t tid = 0;
  uint16_t eid = 0;
  
  if (id.startsWith("Button"))
    tid = id.substring(6).toInt();
  
  if (id.indexOf("pressed") > -1) eid = (uint16_t) SmartButton::Event::PRESSED;
  else if (id.indexOf("released") > -1) eid = (uint16_t)SmartButton::Event::RELEASED;
  else if (id.indexOf("hold_repeat") > -1) eid = (uint16_t)SmartButton::Event::HOLD_REPEAT;
  else if (id.indexOf("hold") > -1) eid = (uint16_t)SmartButton::Event::HOLD;
  else if (id.indexOf("long_hold_repeat") > -1) eid = (uint16_t)SmartButton::Event::LONG_HOLD_REPEAT;
  else if (id.indexOf("long_hold") > -1) eid = (uint16_t)SmartButton::Event::LONG_HOLD;
  else if (id.indexOf("click") > -1) {
    eid = (uint16_t)SmartButton::Event::CLICK;
    eid |= (id.substring(id.indexOf("click") + 5).toInt()) << 8;  // store click number in higher 8 bit!
  }

  return ((uint32_t)tid | ((uint32_t)eid <<16));
}


static uint32_t conditionTimeout = 0;
static char * delayAction=NULL;
static uint32_t lastCondition = 0;


void removeBlocks()  {
  for (int i=0;i<numButtons;i++) {
    t_buttoncontext * ac = (t_buttoncontext *) buttons[i]->getContext();
    ac->blocked=0;
  }  
}

void updateTimeouts() {
  if (millis() - conditionTimeout > CONDITION_TIMEOUT)  // reset last condition in case of timeout
  {
    if (delayAction!=NULL) {
       Serial.print("action: ");Serial.println(delayAction);
       delayAction=NULL;
    }
    if (lastCondition) {
      lastCondition= 0;
      removeBlocks();
      // Serial.println("removed blocks");
    }
  }
}

int blockDependents(uint32_t triggerID) {  
  int dependents=0;
  for (int i=0;i<numButtons;i++) {
    t_buttoncontext * ac = (t_buttoncontext *) buttons[i]->getContext();
    if (ac->condition == triggerID) {
      dependents++;
      uint32_t findCondition = ac->triggerID;
      for (int j=0;j<numButtons;j++) {
        t_buttoncontext * ac2 = (t_buttoncontext *) buttons[j]->getContext();
        if ((ac2->triggerID == findCondition) && (i!=j)) {
           ac2->blocked = 1;
           Serial.print ("  blocking trigger "); Serial.println((ac->triggerID)&255);
        }
      }
    }
  }
  return(dependents);
}



void eventCallback(SmartButton *button, SmartButton::Event event, int clickCounter)
{
  t_buttoncontext * context = (t_buttoncontext *) button->getContext();
  uint16_t thisEventID = (event==SmartButton::Event::CLICK) ? (uint16_t) event | (clickCounter << 8) : (uint16_t) event;
  
  if (thisEventID == (context->triggerID) >> 16) {

    if (context->blocked) {
      Serial.print("  trigger");Serial.print((context->triggerID)&255); Serial.println(" blocked!");
      return;
    }
    
    if (context->condition == 0) {
      if (blockDependents(context->triggerID) == 0) {
        Serial.print("action: ");
        Serial.println(context->action);        
      }
      else {
        Serial.println("  delay action because there are dependents! ");
        delayAction=context->action;
      }
    }            
    else if (context->condition == lastCondition) {      
      Serial.print("action: ");
      Serial.println(context->action);
      delayAction=NULL;
    }
   // else Serial.println("  condition not met");

    lastCondition = context->triggerID; // update condition to current triggerID
    conditionTimeout = millis();
  }
}


void freeTriggers() {
  for (int i = 0; i < numButtons; i++) {
    free (buttons[i]->getContext());
    buttons[i]->end();
    delete (buttons[i]);
  }
  numButtons = 0;
}

void createTrigger(String id, String condition, String action) {

  if (numButtons < MAX_SMARTBUTTONS) {
    Serial.print("creating new trigger for "); Serial.print(id);
    t_buttoncontext * actContext =  (t_buttoncontext *) malloc (sizeof(t_buttoncontext));
    if (!actContext) {
      Serial.println("  MALLOC ERROR!");
      return;
    }
    SmartButton * button = new SmartButton(isPressedHandler);
    actContext->triggerID = getTriggerID(id);
    actContext->blocked = 0;
    action.toCharArray(actContext->action, MAX_ACTIONSTRING_LEN);

    if (condition.length() > 0) {
      Serial.print("  with pretrigger condition "); Serial.print(condition);
      actContext->condition = getTriggerID(condition);
    } else actContext->condition = 0;
    Serial.println();

    buttons[numButtons] = button;
    numButtons++;
    button->begin(eventCallback, actContext);
  }
}
