/*
  HC-05 reliable control sender for Arduino UNO.

  Sends the current C/D button state as a framed packet:
    0xAA 0x55 sequence buttons crc8

  Button wiring from README:
  - C: D4, pressed LOW
  - D: D5, pressed LOW
*/

const unsigned long SERIAL_BAUD_RATE = 9600;
const unsigned long SEND_INTERVAL_MS = 25;

const byte BUTTON_C_PIN = 4;
const byte BUTTON_D_PIN = 5;

const byte PACKET_START_1 = 0xAA;
const byte PACKET_START_2 = 0x55;
const byte BUTTON_C_MASK = 0x01;
const byte BUTTON_D_MASK = 0x02;

byte sequenceNumber = 0;
unsigned long lastSendTime = 0;

void setup() {
  pinMode(BUTTON_C_PIN, INPUT_PULLUP);
  pinMode(BUTTON_D_PIN, INPUT_PULLUP);

  Serial.begin(SERIAL_BAUD_RATE);
}

void loop() {
  const unsigned long now = millis();

  if (now - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = now;
    sendControlPacket();
  }
}

void sendControlPacket() {
  const byte buttons = readButtons();
  const byte packet[] = {
    PACKET_START_1,
    PACKET_START_2,
    sequenceNumber,
    buttons
  };
  const byte crc = crc8(packet, sizeof(packet));

  Serial.write(packet, sizeof(packet));
  Serial.write(crc);

  sequenceNumber++;
}

byte readButtons() {
  byte buttons = 0;

  if (digitalRead(BUTTON_C_PIN) == LOW) {
    buttons |= BUTTON_C_MASK;
  }

  if (digitalRead(BUTTON_D_PIN) == LOW) {
    buttons |= BUTTON_D_MASK;
  }

  return buttons;
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
