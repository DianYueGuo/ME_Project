/*
  Nano HC-05 packet receive test for SenderUNO.

  This sketch swings the pitch servo only while valid SenderUNO packets are
  received. It ignores button values. Use it to distinguish:
  - Bluetooth bytes are arriving, but packets are invalid.
  - Valid SenderUNO packets are arriving, but button/servo logic is the issue.

  Expected packet:
    0xAA 0x55 sequence buttons crc8

  Wiring from README:
  - Nano D4 -> S_PITCH
  - Nano D0/RX <- HC-05 TX
  - Nano D1/TX -> HC-05 RX through voltage divider
*/

#include <Servo.h>

const unsigned long SERIAL_BAUD_RATE = 38400;
const unsigned long RECEIVE_TIMEOUT_MS = 300;
const unsigned long SWING_INTERVAL_MS = 20;

const byte HC05_STATE_PIN = 13;
const byte HC05_ENABLE_PIN = A0;
const byte HC05_ENABLE_NORMAL_LEVEL = LOW;

const byte PITCH_SERVO_PIN = 4;

const int PITCH_MIN_ANGLE = 40;
const int PITCH_MAX_ANGLE = 133;
const int PITCH_START_ANGLE = 90;
const int PITCH_STEP_ANGLE = 1;

const byte PACKET_START_1 = 0xAA;
const byte PACKET_START_2 = 0x55;
const byte PACKET_LENGTH = 5;

Servo pitchServo;

byte packetBuffer[PACKET_LENGTH];
byte packetIndex = 0;
int pitchAngle = PITCH_START_ANGLE;
int swingDirection = 1;
unsigned long lastValidPacketTime = 0;
unsigned long lastSwingTime = 0;

void setup() {
  setupBluetoothPins();
  Serial.begin(SERIAL_BAUD_RATE);

  pitchServo.attach(PITCH_SERVO_PIN);
  pitchAngle = constrain(PITCH_START_ANGLE, PITCH_MIN_ANGLE, PITCH_MAX_ANGLE);
  pitchServo.write(pitchAngle);
}

void setupBluetoothPins() {
  pinMode(HC05_STATE_PIN, INPUT);
  pinMode(HC05_ENABLE_PIN, OUTPUT);
  digitalWrite(HC05_ENABLE_PIN, HC05_ENABLE_NORMAL_LEVEL);
}

void loop() {
  readBluetoothPackets();

  if (millis() - lastValidPacketTime <= RECEIVE_TIMEOUT_MS) {
    swingPitchServo();
  }
}

void readBluetoothPackets() {
  while (Serial.available() > 0) {
    parseByte(Serial.read());
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
    if (packetBuffer[PACKET_LENGTH - 1] == crc8(packetBuffer, PACKET_LENGTH - 1)) {
      lastValidPacketTime = millis();
    }
    packetIndex = 0;
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
