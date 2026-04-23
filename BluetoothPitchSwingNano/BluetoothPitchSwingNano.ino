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

#include <ProjectConfig.h>
#include <Hc05Nano.h>
#include <ServoTools.h>

ServoSwing pitchSwing;
unsigned long lastReceiveTime = 0;

void setup() {
  setupHc05NanoPins();
  Serial.begin(SERIAL_BAUD_RATE);

  pitchSwing.begin(
    PITCH_SERVO_PIN,
    PITCH_MIN_ANGLE,
    PITCH_MAX_ANGLE,
    PITCH_START_ANGLE,
    PITCH_SWING_STEP_ANGLE,
    SWING_INTERVAL_MS
  );
}

void loop() {
  readBluetooth();

  if (millis() - lastReceiveTime <= RECEIVE_TIMEOUT_MS) {
    pitchSwing.update(millis());
  }
}

void readBluetooth() {
  while (Serial.available() > 0) {
    Serial.read();
    lastReceiveTime = millis();
  }
}
