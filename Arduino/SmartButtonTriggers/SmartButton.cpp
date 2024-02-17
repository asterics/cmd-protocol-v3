#include "SmartButton.h"

extern uint8_t debugOutput;

namespace smartbutton {

SmartButton *_smartButtons = NULL;

#define MAX_BLOCKLIST 15
uint32_t removeList[MAX_BLOCKLIST] = {0};
uint8_t blocksToRemove = 0;

SmartButton::SmartButton(
    SmartButton::IsPressedHandler isPressedHandler,
    bool *isPressedFlag,
    unsigned long debounceTimeout,
    unsigned long clickTimeout,
    unsigned long holdTimeout,
    unsigned long longHoldTimeout,
    unsigned long holdRepeatPeriod,
    unsigned long longHoldRepeatPeriod):
        isPressedHandler(isPressedHandler),
        isPressedFlag(isPressedFlag),
        debounceTimeout(debounceTimeout),
        clickTimeout(clickTimeout),
        holdTimeout(holdTimeout),
        longHoldTimeout(longHoldTimeout),
        holdRepeatPeriod(holdRepeatPeriod),
        longHoldRepeatPeriod(longHoldRepeatPeriod),
        eventCallback(NULL)
{

}


SmartButton::SmartButton(SmartButton::IsPressedHandler isPressed): SmartButton(isPressed, NULL)
{
    
}

void SmartButton::begin(void *context)
{
    this->context = context;
    this->debounceTick = 0;
    this->pressTick = 0;
    this->clickCounter = 0;
    this->pressedFlag = false;
    this->state = SmartButton::State::RELEASED;

    this->child = NULL;
    this->blockedBy = 0;
    this->blockToRemove = 0;
    this->waitForChildsTimeout=0;

    if (context == NULL) {
      this->next = _smartButtons;
     _smartButtons = this;
    } else {
      this->next=((SmartButton *) context)->child;
      ((SmartButton *) context)->child=this;
    }
}

void SmartButton::begin(SmartButton::EventCallback eventCallback, void *context)
{
    this->eventCallback = eventCallback;
    this->begin(context);
}

void SmartButton::end()
{
    SmartButton *button = _smartButtons;
    SmartButton *prev = NULL;

    while (button != NULL) {
        if (button == this) {
            if (prev == NULL) {
                _smartButtons = button->next;
            } else {
                prev->next = button->next;
            }
            break;
        }
        prev = button;
        button = button->next;
    }
}

SmartButton::State SmartButton::getState()
{
    return this->state;
}

bool SmartButton::isPressedDebounced()
{
    return this->pressedFlag;
}

void* SmartButton::getContext()
{
    return this->context;
}

bool SmartButton::getInputState()
{
    return this->isPressedHandler(this);
}

void SmartButton::callEvent(SmartButton::Event event)
{
    if (this->eventCallback != NULL) {
        this->eventCallback(this, event, this->clickCounter);
    }
}

void SmartButton::debounce()
{
    if (this->getInputState() == this->pressedFlag) {
        this->debounceTick = getTickValue();
    } else if (getTickValue() - this->debounceTick >= this->debounceTimeout) {
        this->debounceTick = getTickValue();
        this->pressedFlag = !this->pressedFlag;
        this->callEvent((this->pressedFlag != false) ? SmartButton::Event::PRESSED : SmartButton::Event::RELEASED);
    }
}

void SmartButton::process()
{
    this->debounce();

    switch (this->state) {
    case SmartButton::State::RELEASED:
        if (this->pressedFlag == false) {
            if ((this->clickCounter != 0) && (getTickValue() - this->pressTick > this->clickTimeout)) {
                this->callEvent(SmartButton::Event::CLICK);
                this->clickCounter = 0;
            }
            break;
        }
        this->state = SmartButton::State::PRESSED;
        this->pressTick = getTickValue();
        break;
    case SmartButton::State::PRESSED:
        if (this->pressedFlag == false) {
            this->clickCounter++;
            this->state = SmartButton::State::RELEASED;
            break;
        }
        if (getTickValue() - this->pressTick < this->holdTimeout)
            break;
        this->callEvent(SmartButton::Event::HOLD);
        this->pressTick = getTickValue();
        this->repeatTick = getTickValue();
        this->state = SmartButton::State::HOLD;
        break;
    case SmartButton::State::HOLD:
        if (this->pressedFlag == false) {
            this->state = SmartButton::State::RELEASED;
            this->clickCounter = 0;
            break;
        }
        if (getTickValue() - this->pressTick < this->longHoldTimeout) {
            if (getTickValue() - this->repeatTick < this->holdRepeatPeriod)
                break;
            this->callEvent(SmartButton::Event::HOLD_REPEAT);
            this->repeatTick = getTickValue();
            break;
        }
        this->callEvent(SmartButton::Event::LONG_HOLD);
        this->pressTick = getTickValue();
        this->repeatTick = getTickValue();
        this->state = SmartButton::State::LONG_HOLD;
        break;
    case SmartButton::State::LONG_HOLD:
        if (this->pressedFlag == false) {
            this->state = SmartButton::State::RELEASED;
            this->clickCounter = 0;
        }
        if (getTickValue() - this->repeatTick < this->longHoldRepeatPeriod)
            break;
        this->callEvent(SmartButton::Event::LONG_HOLD_REPEAT);
        this->repeatTick = getTickValue();
        break;
    }
}

void SmartButton::service(SmartButton *button)
{
  uint8_t firstLayer=0;
  if (button == NULL) {
    button = _smartButtons;
    firstLayer=1;
  }

  while (button != NULL) {
    button->process();
    if (button->waitForChildsTimeout) {
      if (getTickValue() > button->waitForChildsTimeout) {
        if (debugOutput) {Serial.print("delayed action triggered: ");}
        Serial.println(button->action);
        addToRemoveList(button->triggerID);
      }       
      else if (button->child!=NULL) service(button->child);
    }     
    button = button->next;
  }
  if (firstLayer==1) clearRemovedBlocks();
}

void SmartButton::removeBlock(uint32_t triggerID)
{
  SmartButton *button = _smartButtons;
  
  while (button != NULL) {
    if (button->blockedBy==triggerID) {
      if (debugOutput) {Serial.print("removed block from button: "); Serial.println(button->triggerID);}
      button->blockedBy=0;
    }
    if (button->triggerID==triggerID) {
      if (debugOutput) {Serial.println("  removed delayed action timeout");}
      button->waitForChildsTimeout=0;
    }
    button = button->next;
  }
}

SmartButton * SmartButton::findTrigger(SmartButton *button, uint32_t triggerID)
{    
  while (button != NULL) {
    if (button->triggerID == triggerID) return(button);
    button = button->next;
  }
  return(NULL);
}

void SmartButton::addToRemoveList(uint32_t triggerID)  {
  if (blocksToRemove<MAX_BLOCKLIST) {
    removeList[blocksToRemove++]=triggerID;
    if (debugOutput) {Serial.print("added to removeBlock list: "); Serial.println(triggerID); }
  }
}

void SmartButton::clearRemovedBlocks()  {
  if (!blocksToRemove) return;
  for (int b=0;b<blocksToRemove;b++) {
    uint32_t triggerID=removeList[b];
    SmartButton::removeBlock(triggerID);
    memmove(&removeList[b],&removeList[b+1],sizeof(uint32_t) * (blocksToRemove-b));
    blocksToRemove--;
  }
}

void SmartButton::blockChilds(SmartButton * actButton)
{  
  SmartButton *button;
  uint32_t triggerToBlock = actButton->triggerID;
  if (debugOutput) {Serial.print("trigger to block: "); Serial.println(triggerToBlock);}
  
  SmartButton *actChild=actButton->child;
  while (actChild != NULL) {
    button = _smartButtons;
    while (button != NULL) {
      if (button->triggerID == actChild->triggerID) {
        button->blockedBy = triggerToBlock;
        actChild->blockToRemove= triggerToBlock;
        if (debugOutput) {Serial.print("blocked trigger: "); Serial.println(button->triggerID);}
      }
      button=button->next;
    }
    actChild=actChild->next;
  }
}

void SmartButton::freeAll(SmartButton *button)
{
  if (button==NULL) button = _smartButtons;
  while (button != NULL) {
    if (button->child != NULL) freeAll(button->child);
    button->end();
    delete (button);
    button = button->next;
  }
  _smartButtons=NULL;
}
};
