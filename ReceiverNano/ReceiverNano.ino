/*
  HC-05 reliable control receiver for Arduino Nano.

  Receives framed packets from SenderUNO:
    0xAA 0x55 sequence buttons crc8

  Button controls:
  - A increases the pitch servo angle.
  - B decreases the pitch servo angle.
  - C increases the picker servo angle.
  - D decreases the picker servo angle.
  - K moves the stricker servo from neutral to its strike angle while pressed.
  - E moves the gate servo to one fixed angle.
  - F moves the gate servo to another fixed angle.
  - Servo angles are clamped by hard code limits.
*/

#include <ProjectConfig.h>
#include <Hc05Nano.h>
#include <ControlProtocol.h>
#include <ServoTools.h>

ControlPacketParser packetParser;
BoundedServo pickerServo;
BoundedServo strickerServo;
BoundedServo pitchServo;
BoundedServo gateServo;
RepeatButton aButton;
RepeatButton bButton;
RepeatButton cButton;
RepeatButton dButton;

byte lastSequenceNumber = 0;
bool hasReceivedPacket = false;
byte latestButtons = 0;
byte previousButtons = 0;
unsigned long lastValidPacketTime = 0;

void setup() {
  setupHc05NanoPins();
  Serial.begin(SERIAL_BAUD_RATE);

  pickerServo.begin(PICKER_SERVO_PIN, PICKER_MIN_ANGLE, PICKER_MAX_ANGLE, PICKER_START_ANGLE);
  strickerServo.begin(STRICKER_SERVO_PIN, 0, 180, STRICKER_START_ANGLE);
  pitchServo.begin(PITCH_SERVO_PIN, PITCH_MIN_ANGLE, PITCH_MAX_ANGLE, PITCH_START_ANGLE);
  gateServo.begin(GATE_SERVO_PIN, 0, 180, GATE_START_ANGLE);
}

void loop() {
  readBluetoothPackets();
  applyServoControl();
}

void readBluetoothPackets() {
  while (Serial.available() > 0) {
    ControlPacket packet;

    if (packetParser.feed(Serial.read(), packet)) {
      handleControlPacket(packet);
    }
  }
}

void handleControlPacket(const ControlPacket &packet) {
  if (hasReceivedPacket && packet.sequence == lastSequenceNumber) {
    return;
  }

  lastSequenceNumber = packet.sequence;
  hasReceivedPacket = true;
  latestButtons = packet.buttons;
  lastValidPacketTime = millis();
}

void applyServoControl() {
  const unsigned long now = millis();

  if (!isControlLinkAllowed() || !hasReceivedPacket || now - lastValidPacketTime > FAILSAFE_TIMEOUT_MS) {
    latestButtons = 0;
  }

  const bool aPressed = latestButtons & BUTTON_A_MASK;
  const bool bPressed = latestButtons & BUTTON_B_MASK;
  const bool cPressed = latestButtons & BUTTON_C_MASK;
  const bool dPressed = latestButtons & BUTTON_D_MASK;
  const bool ePressed = latestButtons & BUTTON_E_MASK;
  const bool fPressed = latestButtons & BUTTON_F_MASK;
  const bool kPressed = latestButtons & BUTTON_K_MASK;
  const bool aWasPressed = previousButtons & BUTTON_A_MASK;
  const bool bWasPressed = previousButtons & BUTTON_B_MASK;
  const bool cWasPressed = previousButtons & BUTTON_C_MASK;
  const bool dWasPressed = previousButtons & BUTTON_D_MASK;
  const bool eWasPressed = previousButtons & BUTTON_E_MASK;
  const bool fWasPressed = previousButtons & BUTTON_F_MASK;
  const bool kWasPressed = previousButtons & BUTTON_K_MASK;

  aButton.update(aPressed, aWasPressed, now);
  bButton.update(bPressed, bWasPressed, now);
  cButton.update(cPressed, cWasPressed, now);
  dButton.update(dPressed, dWasPressed, now);

  if (aPressed && !bPressed && aButton.shouldStep(aPressed, aWasPressed, now)) {
    pitchServo.change(PITCH_STEP_ANGLE);
    aButton.markStep(now);
  }

  if (bPressed && !aPressed && bButton.shouldStep(bPressed, bWasPressed, now)) {
    pitchServo.change(-PITCH_STEP_ANGLE);
    bButton.markStep(now);
  }

  if (cPressed && !dPressed && cButton.shouldStep(cPressed, cWasPressed, now)) {
    pickerServo.change(PICKER_STEP_ANGLE);
    cButton.markStep(now);
  }

  if (dPressed && !cPressed && dButton.shouldStep(dPressed, dWasPressed, now)) {
    pickerServo.change(-PICKER_STEP_ANGLE);
    dButton.markStep(now);
  }

  if (ePressed && !fPressed && !eWasPressed) {
    gateServo.set(GATE_E_ANGLE);
  }

  if (fPressed && !ePressed && !fWasPressed) {
    gateServo.set(GATE_F_ANGLE);
  }

  if (kPressed && !kWasPressed) {
    strickerServo.set(STRICKER_ACTIVE_ANGLE);
  }

  if (!kPressed && kWasPressed) {
    strickerServo.set(STRICKER_NEUTRAL_ANGLE);
  }

  previousButtons = latestButtons;
}
