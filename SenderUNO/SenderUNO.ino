/*
  HC-05 reliable control sender for Arduino UNO.

  Sends the current A/B/C/D/E/F button state as a framed packet:
    0xAA 0x55 sequence buttons crc8

  Button wiring from README:
  - A: D2, pressed LOW
  - B: D3, pressed LOW
  - C: D4, pressed LOW
  - D: D5, pressed LOW
  - E: D6, pressed LOW
  - F: D7, pressed LOW
*/

#include <ProjectConfig.h>
#include <ControlProtocol.h>

byte sequenceNumber = 0;
unsigned long lastSendTime = 0;

void setup() {
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  pinMode(BUTTON_C_PIN, INPUT_PULLUP);
  pinMode(BUTTON_D_PIN, INPUT_PULLUP);
  pinMode(BUTTON_E_PIN, INPUT_PULLUP);
  pinMode(BUTTON_F_PIN, INPUT_PULLUP);

  Serial.begin(SERIAL_BAUD_RATE);
}

void loop() {
  const unsigned long now = millis();

  if (now - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = now;
    writeControlPacket(Serial, sequenceNumber, readButtons());
    sequenceNumber++;
  }
}

byte readButtons() {
  byte buttons = 0;

  if (digitalRead(BUTTON_A_PIN) == LOW) {
    buttons |= BUTTON_A_MASK;
  }

  if (digitalRead(BUTTON_B_PIN) == LOW) {
    buttons |= BUTTON_B_MASK;
  }

  if (digitalRead(BUTTON_C_PIN) == LOW) {
    buttons |= BUTTON_C_MASK;
  }

  if (digitalRead(BUTTON_D_PIN) == LOW) {
    buttons |= BUTTON_D_MASK;
  }

  if (digitalRead(BUTTON_E_PIN) == LOW) {
    buttons |= BUTTON_E_MASK;
  }

  if (digitalRead(BUTTON_F_PIN) == LOW) {
    buttons |= BUTTON_F_MASK;
  }

  return buttons;
}
