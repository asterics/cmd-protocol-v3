/*
    trigger.cpp
*/

#include <SmartButton.h>
#include "trigger.h"

using namespace smartbutton;

extern bool isPressedHandler(SmartButton *button);   // implements all isPressedHandlers for different trigger types!

SmartButton * buttons[MAX_SMARTBUTTONS] = {0};
int numButtons = 0;

uint16_t getTriggerID(String source) {
  int tid = 0;
  if (source.startsWith("Button"))
    tid = source.substring(6).toInt();
  return (tid);
}

uint16_t getEventID(String source) {
  uint16_t eid = 0;
  if (source.indexOf("pressed") > -1) eid = (uint16_t) SmartButton::Event::PRESSED;
  else if (source.indexOf("released") > -1) eid = (uint16_t)SmartButton::Event::RELEASED;
  else if (source.indexOf("hold_repeat") > -1) eid = (uint16_t)SmartButton::Event::HOLD_REPEAT;
  else if (source.indexOf("hold") > -1) eid = (uint16_t)SmartButton::Event::HOLD;
  else if (source.indexOf("long_hold_repeat") > -1) eid = (uint16_t)SmartButton::Event::LONG_HOLD_REPEAT;
  else if (source.indexOf("long_hold") > -1) eid = (uint16_t)SmartButton::Event::LONG_HOLD;
  else if (source.indexOf("click") > -1) {
    eid = (uint16_t)SmartButton::Event::CLICK;
    eid |= (source.substring(source.indexOf("click") + 5).toInt()) << 8;  // store click number in higher 8 bit!
  }
  return (eid);
}

void eventCallback(SmartButton *button, SmartButton::Event event, int clickCounter)
{
  static uint32_t lastCondition = 0;
  uint16_t thisEventID = (uint16_t) event | (clickCounter << 8);
  t_buttoncontext * context = (t_buttoncontext *) button->getContext();
  
  if (thisEventID == context->eventID) {
      if ((context->condition == 0) || (context->condition == lastCondition)) {
        Serial.print("  triggered action: ");
        Serial.println(context->action);
      }
      else Serial.println("  triggered, but pre-condition not met");
      lastCondition = context->triggerID | (context->eventID<<16);  // update condition ID to current one
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

void createTrigger(String source, String condition, String action) {

  uint16_t triggerID = getTriggerID(source);
  uint16_t eventID = getEventID(source);
  
  if (numButtons < MAX_SMARTBUTTONS) {
    Serial.print("creating new trigger for "); Serial.print(source);
    t_buttoncontext * actContext =  (t_buttoncontext *) malloc (sizeof(t_buttoncontext));
    if (!actContext) {
      Serial.println("  MALLOC ERROR!");
      return;
    }
    SmartButton * button = new SmartButton(isPressedHandler);
    actContext->triggerID = triggerID;
    actContext->eventID = eventID;
    action.toCharArray(actContext->action, MAX_ACTIONSTRING_LEN);

    if (condition.length() > 0) {
      Serial.print("  with pretrigger condition "); Serial.print(condition);
      actContext->condition = getTriggerID(condition) | (getEventID(condition)<<16);
    } else actContext->condition = 0;
    Serial.println();

    buttons[numButtons] = button;
    numButtons++;
    button->begin(eventCallback, actContext);
  }
}
