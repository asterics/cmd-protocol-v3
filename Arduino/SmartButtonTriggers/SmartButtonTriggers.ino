#include <Arduino.h>
#include <SmartButton.h>
#include "parser.h"
#include "trigger.h"

#define NUM_PHYSICAL_BUTTONS 3

#if defined(ARDUINO_AVR_MICRO)
  int mapButtonToPin[] = {10, 16, 19}; // GPIO input pins for Fabi B1,B2,B3
#else
  int mapButtonToPin[] = {17, 20, 28}; // GPIO input pins for FlipMouse B1,B2,B3
#endif

using namespace smartbutton;

bool isPressedHandler(SmartButton *button)
{
  int value = 0;
  int triggerID = ((t_buttoncontext *) button->getContext())->triggerID;

  // Note: currently only 3 GPIO input triggers implemented!
  if ((triggerID >= 1) && (triggerID <= 3))
    value = !digitalRead(mapButtonToPin[triggerID - 1]);

  return value;
}

void processCommand (String cmd) {
  if (cmd == "ti")
    createTrigger(getParameter("id"),getParameter("condition"),getParameter("action"));

  if (cmd == "free") {
    Serial.print("freeHeap="); Serial.println(getFreeHeap());
  }

  if (cmd == "clear") {
    freeTriggers();
    Serial.print("buttons cleared, freeHeap="); Serial.println(getFreeHeap());
  }
}

void setup()
{
  for (int i = 0; i < NUM_PHYSICAL_BUTTONS; i++)
    pinMode(mapButtonToPin[i], INPUT_PULLUP);  // Digital input with pull-up resistors (normal high)

  Serial.begin(115200);
  while (!Serial);
  delay(1000);

  Serial.println("Welcome to AT-command based SmartButton testing!");
  Serial.println("examples:");
  Serial.println("at ti id=Button1 hold,action=B1held!");
  Serial.println("at ti id=Button2 click1,action=B2clicked!");
  Serial.println("at ti id=Button3 pressed,action=B3pressed!");
  Serial.println("at ti id=Button3 released,action=B3released!");
  Serial.println("at ti id=Button2 click2,action=B2doubleClicked!");
  Serial.println("at ti id=Button1 click1,action=B2dbl->B1clicked!,condition=Button2 click2");
  Serial.println("at free");
  Serial.println("at clear\n");
}

void loop()
{
  if (Serial.available())
    parseByte(Serial.read());      // parse serial for AT commands, calls back processCommand()
  SmartButton::service();          // Asynchronous service routine, should be called periodically
}

#if defined(ARDUINO_AVR_MICRO)
uint32_t getFreeHeap(void) {
  extern char *__brkval;
  char top;
  return &top - __brkval;
  // return __brkval ? &top - __brkval : &top - __malloc_heap_start;
}
#else
#include <malloc.h>
uint32_t getTotalHeap(void) {
  extern char __StackLimit, __bss_end__;
  return &__StackLimit  - &__bss_end__;
}
uint32_t getFreeHeap(void) {
  struct mallinfo m = mallinfo();
  return getTotalHeap() - m.uordblks;
}
#endif
