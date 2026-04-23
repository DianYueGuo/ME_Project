/*
  Nano pitch servo self-test.

  This sketch does not use Bluetooth. It continuously swings the pitch servo
  after boot so you can verify that the Nano, D4 signal pin, servo power, and
  servo wiring are working.

  Wiring from README:
  - Nano D4 -> S_PITCH
*/

#include <Servo.h>

const unsigned long SWING_INTERVAL_MS = 20;

const byte PITCH_SERVO_PIN = 4;

const int PITCH_MIN_ANGLE = 40;
const int PITCH_MAX_ANGLE = 133;
const int PITCH_START_ANGLE = 90;
const int PITCH_STEP_ANGLE = 1;

Servo pitchServo;

int pitchAngle = PITCH_START_ANGLE;
int swingDirection = 1;
unsigned long lastSwingTime = 0;

void setup() {
  pitchServo.attach(PITCH_SERVO_PIN);
  pitchAngle = constrain(PITCH_START_ANGLE, PITCH_MIN_ANGLE, PITCH_MAX_ANGLE);
  pitchServo.write(pitchAngle);
}

void loop() {
  const unsigned long now = millis();

  if (now - lastSwingTime < SWING_INTERVAL_MS) {
    return;
  }

  lastSwingTime = now;
  pitchAngle += swingDirection * PITCH_STEP_ANGLE;

  if (pitchAngle >= PITCH_MAX_ANGLE) {
    pitchAngle = PITCH_MAX_ANGLE;
    swingDirection = -1;
  } else if (pitchAngle <= PITCH_MIN_ANGLE) {
    pitchAngle = PITCH_MIN_ANGLE;
    swingDirection = 1;
  }

  pitchServo.write(pitchAngle);
}
