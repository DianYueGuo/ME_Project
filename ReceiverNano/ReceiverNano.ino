/*
  HC-05 reliable control receiver for Arduino Nano.

  Receives framed packets from SenderUNO:
    0xAA 0x55 sequence buttons crc8

  Only C and D are used for now:
  - C increases the picker servo angle.
  - D decreases the picker servo angle.
  - The picker servo angle is clamped by hard code limits.
*/

#include <Servo.h>

const unsigned long SERIAL_BAUD_RATE = 9600;
const unsigned long FAILSAFE_TIMEOUT_MS = 300;
const unsigned long HOLD_REPEAT_DELAY_MS = 400;
const unsigned long HOLD_REPEAT_INTERVAL_MS = 150;

const byte PICKER_SERVO_PIN = 2;

const int PICKER_MIN_ANGLE = 10;
const int PICKER_MAX_ANGLE = 180;
const int PICKER_START_ANGLE = 90;
const int PICKER_STEP_ANGLE = 10;

const byte PACKET_START_1 = 0xAA;
const byte PACKET_START_2 = 0x55;
const byte PACKET_LENGTH = 5;
const byte BUTTON_C_MASK = 0x01;
const byte BUTTON_D_MASK = 0x02;

Servo pickerServo;

byte packetBuffer[PACKET_LENGTH];
byte packetIndex = 0;
byte lastSequenceNumber = 0;
bool hasReceivedPacket = false;

byte latestButtons = 0;
byte previousButtons = 0;
int pickerAngle = PICKER_START_ANGLE;

unsigned long lastValidPacketTime = 0;
unsigned long cHoldStartTime = 0;
unsigned long dHoldStartTime = 0;
unsigned long lastCRepeatTime = 0;
unsigned long lastDRepeatTime = 0;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  pickerServo.attach(PICKER_SERVO_PIN);
  pickerAngle = constrain(PICKER_START_ANGLE, PICKER_MIN_ANGLE, PICKER_MAX_ANGLE);
  pickerServo.write(pickerAngle);
}

void loop() {
  readBluetoothPackets();
  applyPickerControl();
}

void readBluetoothPackets() {
  while (Serial.available() > 0) {
    const byte incoming = Serial.read();
    parseByte(incoming);
  }
}

void parseByte(byte incoming) {
  if (packetIndex == 0) {
    if (incoming == PACKET_START_1) {
      packetBuffer[packetIndex++] = incoming;
    }
    return;
  }

  if (packetIndex == 1) {
    if (incoming == PACKET_START_2) {
      packetBuffer[packetIndex++] = incoming;
    } else if (incoming == PACKET_START_1) {
      packetBuffer[0] = incoming;
      packetIndex = 1;
    } else {
      packetIndex = 0;
    }
    return;
  }

  packetBuffer[packetIndex++] = incoming;

  if (packetIndex >= PACKET_LENGTH) {
    handleCandidatePacket();
    packetIndex = 0;
  }
}

void handleCandidatePacket() {
  const byte expectedCrc = crc8(packetBuffer, PACKET_LENGTH - 1);

  if (packetBuffer[PACKET_LENGTH - 1] != expectedCrc) {
    return;
  }

  const byte sequenceNumber = packetBuffer[2];

  if (hasReceivedPacket && sequenceNumber == lastSequenceNumber) {
    return;
  }

  lastSequenceNumber = sequenceNumber;
  hasReceivedPacket = true;
  latestButtons = packetBuffer[3] & (BUTTON_C_MASK | BUTTON_D_MASK);
  lastValidPacketTime = millis();
}

void applyPickerControl() {
  const unsigned long now = millis();

  if (!hasReceivedPacket || now - lastValidPacketTime > FAILSAFE_TIMEOUT_MS) {
    latestButtons = 0;
  }

  const bool cPressed = latestButtons & BUTTON_C_MASK;
  const bool dPressed = latestButtons & BUTTON_D_MASK;
  const bool cWasPressed = previousButtons & BUTTON_C_MASK;
  const bool dWasPressed = previousButtons & BUTTON_D_MASK;

  updateHoldTiming(cPressed, cWasPressed, cHoldStartTime, lastCRepeatTime, now);
  updateHoldTiming(dPressed, dWasPressed, dHoldStartTime, lastDRepeatTime, now);

  if (cPressed && !dPressed && shouldStep(cPressed, cWasPressed, cHoldStartTime, lastCRepeatTime, now)) {
    changePickerAngle(PICKER_STEP_ANGLE);
    lastCRepeatTime = now;
  }

  if (dPressed && !cPressed && shouldStep(dPressed, dWasPressed, dHoldStartTime, lastDRepeatTime, now)) {
    changePickerAngle(-PICKER_STEP_ANGLE);
    lastDRepeatTime = now;
  }

  previousButtons = latestButtons;
}

void updateHoldTiming(
  bool pressed,
  bool wasPressed,
  unsigned long &holdStartTime,
  unsigned long &lastRepeatTime,
  unsigned long now
) {
  if (pressed && !wasPressed) {
    holdStartTime = now;
    lastRepeatTime = now;
  }

  if (!pressed) {
    holdStartTime = 0;
    lastRepeatTime = 0;
  }
}

bool shouldStep(
  bool pressed,
  bool wasPressed,
  unsigned long holdStartTime,
  unsigned long lastRepeatTime,
  unsigned long now
) {
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

void changePickerAngle(int delta) {
  const int nextAngle = constrain(
    pickerAngle + delta,
    PICKER_MIN_ANGLE,
    PICKER_MAX_ANGLE
  );

  if (nextAngle != pickerAngle) {
    pickerAngle = nextAngle;
    pickerServo.write(pickerAngle);
  }
}

byte crc8(const byte *data, byte length) {
  byte crc = 0x00;

  for (byte i = 0; i < length; i++) {
    crc ^= data[i];

    for (byte bit = 0; bit < 8; bit++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc <<= 1;
      }
    }
  }

  return crc;
}
