#ifndef SERVO_TOOLS_H
#define SERVO_TOOLS_H

#include <Arduino.h>
#include <Servo.h>
#include "ProjectConfig.h"

class BoundedServo {
public:
  void begin(byte pin, int minimumAngle, int maximumAngle, int startAngle) {
    minAngle = minimumAngle;
    maxAngle = maximumAngle;
    currentAngle = constrain(startAngle, minAngle, maxAngle);
    servo.attach(pin);
    servo.write(currentAngle);
  }

  bool change(int delta) {
    return set(currentAngle + delta);
  }

  bool set(int angle) {
    const int nextAngle = constrain(angle, minAngle, maxAngle);

    if (nextAngle == currentAngle) {
      return false;
    }

    currentAngle = nextAngle;
    servo.write(currentAngle);
    return true;
  }

  int angle() const {
    return currentAngle;
  }

private:
  Servo servo;
  int minAngle = 0;
  int maxAngle = 180;
  int currentAngle = 90;
};

class ServoSwing {
public:
  void begin(
    byte pin,
    int minimumAngle,
    int maximumAngle,
    int startAngle,
    int stepAngle,
    unsigned long intervalMs
  ) {
    minAngle = minimumAngle;
    maxAngle = maximumAngle;
    step = stepAngle;
    interval = intervalMs;
    currentAngle = constrain(startAngle, minAngle, maxAngle);
    direction = 1;
    lastMoveTime = 0;
    servo.attach(pin);
    servo.write(currentAngle);
  }

  void update(unsigned long now) {
    if (now - lastMoveTime < interval) {
      return;
    }

    lastMoveTime = now;
    currentAngle += direction * step;

    if (currentAngle >= maxAngle) {
      currentAngle = maxAngle;
      direction = -1;
    } else if (currentAngle <= minAngle) {
      currentAngle = minAngle;
      direction = 1;
    }

    servo.write(currentAngle);
  }

private:
  Servo servo;
  int minAngle = 0;
  int maxAngle = 180;
  int currentAngle = 90;
  int step = 1;
  int direction = 1;
  unsigned long interval = 20;
  unsigned long lastMoveTime = 0;
};

class RepeatButton {
public:
  void update(bool pressed, bool wasPressed, unsigned long now) {
    if (pressed && !wasPressed) {
      holdStartTime = now;
      lastRepeatTime = now;
    }

    if (!pressed) {
      holdStartTime = 0;
      lastRepeatTime = 0;
    }
  }

  bool shouldStep(bool pressed, bool wasPressed, unsigned long now) const {
    if (!pressed) {
      return false;
    }

    if (!wasPressed) {
      return true;
    }

    if (holdStartTime == 0 || now - holdStartTime < HOLD_REPEAT_DELAY_MS) {
      return false;
    }

    return now - lastRepeatTime >= HOLD_REPEAT_INTERVAL_MS;
  }

  void markStep(unsigned long now) {
    lastRepeatTime = now;
  }

private:
  unsigned long holdStartTime = 0;
  unsigned long lastRepeatTime = 0;
};

#endif
