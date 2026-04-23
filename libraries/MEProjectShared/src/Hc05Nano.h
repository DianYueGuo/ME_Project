#ifndef HC05_NANO_H
#define HC05_NANO_H

#include <Arduino.h>
#include "ProjectConfig.h"

inline void setupHc05NanoPins() {
  pinMode(HC05_STATE_PIN, INPUT);
  pinMode(HC05_ENABLE_PIN, OUTPUT);
  digitalWrite(HC05_ENABLE_PIN, HC05_ENABLE_NORMAL_LEVEL);
}

inline bool isHc05Connected() {
  return digitalRead(HC05_STATE_PIN) == HIGH;
}

#endif
