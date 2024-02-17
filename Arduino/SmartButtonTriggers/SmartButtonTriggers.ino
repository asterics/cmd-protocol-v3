#include <Arduino.h>
#include "SmartButton.h"
#include "parser.h"
#include "trigger.h"
#include <malloc.h>

#define NUM_PHYSICAL_BUTTONS 3
int mapButtonToPin[] = {17, 20, 28}; // GPIO input pins for FlipMouse B1,B2,B3

using namespace smartbutton;

bool isPressedHandler(SmartButton *button)
{
  int value = 0;
  uint8_t buttonNumber = button->triggerID & 255;

  // Note: currently only 3 GPIO input triggers implemented!
  if ((buttonNumber >= 1) && (buttonNumber <= 3))
    value = !digitalRead(mapButtonToPin[buttonNumber - 1]);

  return value;
}

void processCommand (String cmd) {
  cmd.toLowerCase();
  if (cmd == "ti")
    createTrigger(NULL,getParameter("id"),getParameter("action"));

  if (cmd == "free") {
    Serial.print("freeHeap="); Serial.println(getFreeHeap());
  }

  if (cmd == "clear") {
    freeTriggers();
    Serial.print("buttons cleared, freeHeap="); Serial.println(getFreeHeap());
  }

  if (cmd == "debug") {
    toggleDebugOutput();
  }
}

void setup()
{
  for (int i = 0; i < NUM_PHYSICAL_BUTTONS; i++)
    pinMode(mapButtonToPin[i], INPUT_PULLUP);  // Digital input with pull-up resistors (normal high)

  Serial.begin(115200);
  while (!Serial);
  delay(1000);

  createTrigger(NULL,"Button1 pressed","B1pressed");
  createTrigger(NULL,"Button2 pressed","B2pressed");
  createTrigger(NULL,"Button3 pressed","B3pressed");
  createTrigger(NULL,"Button2 pressed->Button1 pressed","B2p->B1p");
  createTrigger(NULL,"Button3 released","B3released");
  createTrigger(NULL,"Button3 click1","B3click");
  createTrigger(NULL,"Button3 click2","B3doubleClick");

  Serial.println("Welcome to AT-command based SmartButton testing! Supported AT commands:");
  Serial.println("at ti id=<trigger-ID-string> [-> <next trigger-ID-string>],action=<actionString>");
  Serial.println("at free");
  Serial.println("at clear");
  Serial.println("at debug");

  Serial.println("This example was created:");
  Serial.println("  at ti id=Button1 pressed,action=B1pressed");
  Serial.println("  at ti id=Button2 pressed,action=B2pressed");
  Serial.println("  at ti id=Button3 pressed,action=B3pressed");
  Serial.println("  at ti id=Button2 pressed->Button1 pressed,action=B2p->B1p");
  Serial.println("  at ti id=Button3 released,action=B3released");
  Serial.println("  at ti id=Button3 click1,action=B3click");
  Serial.println("  at ti id=Button3 click2,action=B3doubleClick");
}

void loop()
{
  if (Serial.available())
    parseByte(Serial.read());      // parse serial for AT commands, calls back processCommand()
  SmartButton::service(NULL);      // Asynchronous service routine, should be called periodically
}



uint32_t getTotalHeap(void) {
  extern char __StackLimit, __bss_end__;
  return &__StackLimit  - &__bss_end__;
}

uint32_t getFreeHeap(void) {
  struct mallinfo m = mallinfo();
  return getTotalHeap() - m.uordblks;
}
