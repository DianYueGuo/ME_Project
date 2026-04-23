#ifndef HC05_NANO_H
#define HC05_NANO_H

#include <Arduino.h>
#include "ProjectConfig.h"

inline void setupHc05NanoPins() {
  pinMode(HC05_STATE_PIN, INPUT);

  if (HC05_DRIVE_ENABLE_PIN) {
    pinMode(HC05_ENABLE_PIN, OUTPUT);
    digitalWrite(HC05_ENABLE_PIN, HC05_ENABLE_NORMAL_LEVEL);
  } else {
    pinMode(HC05_ENABLE_PIN, INPUT);
  }
}

inline bool isHc05Connected() {
  return digitalRead(HC05_STATE_PIN) == HIGH;
}

inline bool isControlLinkAllowed() {
  return !REQUIRE_HC05_STATE_CONNECTED || isHc05Connected();
}

#endif
