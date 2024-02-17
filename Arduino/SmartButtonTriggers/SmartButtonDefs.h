#ifndef SMART_BUTTON_DEFS_H
#define SMART_BUTTON_DEFS_H

#include <Arduino.h>

namespace smartbutton {

constexpr unsigned long DEFAULT_DEBOUNCE_TIMEOUT = 20UL;
constexpr unsigned long DEFAULT_CLICK_TIMEOUT = 500UL;
constexpr unsigned long DEFAULT_HOLD_TIMEOUT = 1000UL;
constexpr unsigned long DEFAULT_LONG_HOLD_TIMEOUT = 2000UL;
constexpr unsigned long DEFAULT_HOLD_REPEAT_PERIOD = 200UL;
constexpr unsigned long DEFAULT_LONG_HOLD_REPEAT_PERIOD = 50UL;
constexpr unsigned long DEFAULT_CHILDS_TIMEOUT = 800UL;

constexpr unsigned long (*getTickValue)() = millis;
#if defined(ARDUINO_NANO_RP2040_CONNECT)
  constexpr PinStatus (*getGpioState)(pin_size_t) = digitalRead;
#else
  constexpr bool (*getGpioState)(int) = digitalRead;
#endif

};

#endif /* SMART_BUTTON_DEFS_H */
