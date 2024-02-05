/*
    trigger.cpp
*/

#include <SmartButton.h>
#include "trigger.h"

using namespace smartbutton;

extern bool isPressedHandler(SmartButton *button);   // implements all isPressedHandlers for different trigger types!

SmartButton * buttons[MAX_SMARTBUTTONS] = {0};
int numButtons = 0;

uint16_t getTriggerID(String id) {
  int tid = 0;
  if (id.startsWith("Button"))
    tid = id.substring(6).toInt();
  return (tid);
}

uint16_t getEventID(String id) {
  uint16_t eid = 0;
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
  return (eid);
}

void eventCallback(SmartButton *button, SmartButton::Event event, int clickCounter)
{
  static uint32_t lastCondition = 0;
  static uint32_t conditionTimeout = 0;
  uint16_t thisEventID = (uint16_t) event | (clickCounter << 8);
  t_buttoncontext * context = (t_buttoncontext *) button->getContext();

  if (millis() - conditionTimeout > CONDITION_TIMEOUT)  // reset last condition in case of timeout
    lastCondition = 0;

  if (thisEventID == context->eventID) {
    if ((context->condition == 0) || (context->condition == lastCondition)) {
      Serial.print("  triggered action: ");
      Serial.println(context->action);
    }
    else Serial.println("  triggered, but pre-condition not met");
    lastCondition = context->triggerID | (context->eventID << 16); // update condition ID to current one
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

  uint16_t triggerID = getTriggerID(id);
  uint16_t eventID = getEventID(id);

  if (numButtons < MAX_SMARTBUTTONS) {
    Serial.print("creating new trigger for "); Serial.print(id);
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
      actContext->condition = getTriggerID(condition) | (getEventID(condition) << 16);
    } else actContext->condition = 0;
    Serial.println();

    buttons[numButtons] = button;
    numButtons++;
    button->begin(eventCallback, actContext);
  }
}
