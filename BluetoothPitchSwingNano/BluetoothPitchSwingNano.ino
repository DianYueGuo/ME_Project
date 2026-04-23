/*
  Nano HC-05 pitch servo receive test.

  The pitch servo swings while any bytes are received from Bluetooth.
  If no bytes arrive for RECEIVE_TIMEOUT_MS, the servo stops at its current
  angle.

  Wiring from README:
  - Nano D4 -> S_PITCH
  - Nano D0/RX <- HC-05 TX
  - Nano D1/TX -> HC-05 RX through voltage divider
*/

#include <Servo.h>

const unsigned long SERIAL_BAUD_RATE = 38400;
const unsigned long RECEIVE_TIMEOUT_MS = 300;
const unsigned long SWING_INTERVAL_MS = 20;

const byte PITCH_SERVO_PIN = 4;

const int PITCH_MIN_ANGLE = 40;
const int PITCH_MAX_ANGLE = 133;
const int PITCH_START_ANGLE = 90;
const int PITCH_STEP_ANGLE = 1;

Servo pitchServo;

int pitchAngle = PITCH_START_ANGLE;
int swingDirection = 1;
unsigned long lastReceiveTime = 0;
unsigned long lastSwingTime = 0;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  pitchServo.attach(PITCH_SERVO_PIN);
  pitchAngle = constrain(PITCH_START_ANGLE, PITCH_MIN_ANGLE, PITCH_MAX_ANGLE);
  pitchServo.write(pitchAngle);
}

void loop() {
  readBluetooth();

  if (millis() - lastReceiveTime <= RECEIVE_TIMEOUT_MS) {
    swingPitchServo();
  }
}

void readBluetooth() {
  while (Serial.available() > 0) {
    Serial.read();
    lastReceiveTime = millis();
  }
}

void swingPitchServo() {
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
