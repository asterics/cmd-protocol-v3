/*
    trigger.cpp
*/

#include "trigger.h"

using namespace smartbutton;

extern bool isPressedHandler(SmartButton *button);   // implements all isPressedHandlers for different trigger types!
uint8_t debugOutput=0;
char stringBuf[ACTIONSTRING_BUFLEN]; 
int stringBufIndex=0;

uint32_t getTriggerID(String id, int * actTriggerIndex) {
  uint16_t tid = 0;
  uint16_t eid = 0;
  String actid = id.substring(*actTriggerIndex);
  int nextTriggerIndex = actid.indexOf("->");
  if (nextTriggerIndex >0) *actTriggerIndex=nextTriggerIndex+2;
  else *actTriggerIndex=-1;
  
  if (actid.startsWith("Button"))
    tid = actid.substring(6).toInt();
  
  if (actid.indexOf("pressed") > -1) eid = (uint16_t) SmartButton::Event::PRESSED;
  else if (actid.indexOf("released") > -1) eid = (uint16_t)SmartButton::Event::RELEASED;
  else if (actid.indexOf("hold_repeat") > -1) eid = (uint16_t)SmartButton::Event::HOLD_REPEAT;
  else if (actid.indexOf("hold") > -1) eid = (uint16_t)SmartButton::Event::HOLD;
  else if (actid.indexOf("long_hold_repeat") > -1) eid = (uint16_t)SmartButton::Event::LONG_HOLD_REPEAT;
  else if (actid.indexOf("long_hold") > -1) eid = (uint16_t)SmartButton::Event::LONG_HOLD;
  else if (actid.indexOf("click") > -1) {
    eid = (uint16_t)SmartButton::Event::CLICK;
    eid |= (actid.substring(actid.indexOf("click") + 5).toInt()) << 8;  // store click number in higher 8 bit!
  }

  return ((uint32_t)tid | ((uint32_t)eid <<16));
}


void eventCallback(SmartButton *button, SmartButton::Event event, int clickCounter)
{
  uint16_t thisEventID = (event==SmartButton::Event::CLICK) ? (uint16_t) event | (clickCounter << 8) : (uint16_t) event;
  uint16_t triggerEventID = (button->triggerID) >> 16;    // extract eventID from triggerID
  if (thisEventID == triggerEventID)  {

    if (button->blockedBy != 0) {
      if (debugOutput) Serial.println("action blocked"); 
      return;
    }
    
    if (button->child != NULL) {
       SmartButton::blockChilds(button);
       button->waitForChildsTimeout=millis()+DEFAULT_CHILDS_TIMEOUT;
    }
    if ((debugOutput) && (button->waitForChildsTimeout)) {
      if (debugOutput) Serial.println("action delayed");
    }
    if (!button->waitForChildsTimeout) {
      if (debugOutput) Serial.print("action triggered: "); 
      Serial.println(button->action);
      if (button->blockToRemove) {
        SmartButton::addToRemoveList(button->blockToRemove);
        button->blockToRemove=0;     
      }
    }
  }
}

void freeTriggers() {
  SmartButton::freeAll(NULL);
  stringBufIndex=0;
}

SmartButton * createTrigger(SmartButton * parent, String id, String action) {
  SmartButton * button = NULL;
  SmartButton * root = NULL;
  uint32_t triggerID;
  int nextTriggerIndex=0;
  triggerID = getTriggerID(id,&nextTriggerIndex);

  // Serial.print("trigger ID: "); Serial.println(triggerID);
  if (triggerID<=0) return (NULL);
  
  if (parent == NULL)  {
    if (debugOutput) Serial.print("create trigger: ");
    root=_smartButtons;
  } else {
    if (debugOutput) Serial.print ("  create child trigger: ");
    root=parent->child;
  }
  if (debugOutput) Serial.println(triggerID);

  button = SmartButton::findTrigger(root,triggerID);
  if (!button) {
    button = new SmartButton(isPressedHandler);
    if (button) {
      button->triggerID=triggerID;
      action.toCharArray(&stringBuf[stringBufIndex], 20);  // TBD: check length!
      button->action=&stringBuf[stringBufIndex];
      stringBufIndex+=(action.length()+1);
      button->begin(eventCallback, parent);
    } else Serial.println("trigger memory full!");
  } else {
    if (debugOutput) Serial.println("  found existing trigger");
    if (nextTriggerIndex <= 0) {
      if (debugOutput) Serial.println("  replacing action");
      action.toCharArray(&stringBuf[stringBufIndex], 20);  // TBD: check length!
      button->action=&stringBuf[stringBufIndex];
      stringBufIndex+=(action.length()+1);
    }
  }

  if (nextTriggerIndex > 0) button->child = createTrigger (button, id.substring(nextTriggerIndex), action);   
  return (button);
}

void toggleDebugOutput() {
  debugOutput=!debugOutput;
  Serial.print("Debug Output is now ");
  debugOutput ? Serial.println("on") : Serial.println("off");
}
