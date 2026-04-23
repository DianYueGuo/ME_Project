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

#include <ProjectConfig.h>
#include <Hc05Nano.h>
#include <ControlProtocol.h>
#include <ServoTools.h>

ControlPacketParser packetParser;
ServoSwing pitchSwing;
unsigned long lastValidPacketTime = 0;

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
  readBluetoothPackets();

  if (millis() - lastValidPacketTime <= RECEIVE_TIMEOUT_MS) {
    pitchSwing.update(millis());
  }
}

void readBluetoothPackets() {
  while (Serial.available() > 0) {
    ControlPacket packet;

    if (packetParser.feed(Serial.read(), packet)) {
      lastValidPacketTime = millis();
    }
  }
}
