/*
  Nano pitch servo self-test.

  This sketch does not use Bluetooth. It continuously swings the pitch servo
  after boot so you can verify that the Nano, D4 signal pin, servo power, and
  servo wiring are working.

  Wiring from README:
  - Nano D4 -> S_PITCH
*/

#include <ProjectConfig.h>
#include <ServoTools.h>

ServoSwing pitchSwing;

void setup() {
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
  pitchSwing.update(millis());
}
