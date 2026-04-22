/*
  Simplest remote test.

  Board: Arduino UNO/Nano-compatible remote controller
  Bluetooth: HC-05 on hardware serial D0/D1, 9600 baud

  Button A on D2:
  - pressed  -> sends '1'
  - released -> sends '0'
*/

const unsigned long BLUETOOTH_BAUD_RATE = 9600;
const unsigned long SEND_INTERVAL_MS = 100;

const byte BUTTON_A_PIN = 2;

unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(BLUETOOTH_BAUD_RATE);
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
}

void loop() {
  const unsigned long now = millis();

  if (now - lastSendTime < SEND_INTERVAL_MS) {
    return;
  }

  lastSendTime = now;

  if (digitalRead(BUTTON_A_PIN) == LOW) {
    Serial.println('1');
  } else {
    Serial.println('0');
  }
}
