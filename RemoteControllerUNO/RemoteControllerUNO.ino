/*
  Remote controller for the RC car.

  Board: Arduino UNO
  Bluetooth: HC-05 paired with the car's HC-05, 9600 baud

  HC-05 wiring used by this sketch:
  - HC-05 TX  -> UNO D0 RX
  - HC-05 RX  -> UNO D1 TX through a resistor divider
  - HC-05 GND -> UNO GND
  - HC-05 VCC -> suitable 5 V module supply

  Controller wiring from README.md:
  - Buttons A, B, C, D, E, F, K -> D2, D3, D4, D5, D6, D7, D8
  - Stick X -> A0
  - Stick Y -> A1

  Packet format sent to the car:
  <x,y,buttons>

  x and y are signed values from -255 to 255.
  buttons is a bit mask where bit 0 is Button A, bit 1 is Button B, etc.
*/

const unsigned long BLUETOOTH_BAUD_RATE = 9600;
const unsigned long SEND_INTERVAL_MS = 30;

const byte STICK_X_PIN = A0;
const byte STICK_Y_PIN = A1;

const byte BUTTON_PINS[] = {2, 3, 4, 5, 6, 7, 8};
const byte BUTTON_COUNT = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);

const int X_CENTER = 508;
const int Y_CENTER = 513;
const int STICK_DEADBAND = 35;

unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(BLUETOOTH_BAUD_RATE);

  for (byte i = 0; i < BUTTON_COUNT; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  pinMode(STICK_X_PIN, INPUT);
  pinMode(STICK_Y_PIN, INPUT);
}

void loop() {
  const unsigned long now = millis();

  if (now - lastSendTime < SEND_INTERVAL_MS) {
    return;
  }

  lastSendTime = now;

  const int x = readStickAxis(STICK_X_PIN, X_CENTER);
  const int y = readStickAxis(STICK_Y_PIN, Y_CENTER);
  const byte buttons = readButtons();

  sendControlPacket(x, y, buttons);
}

int readStickAxis(byte pin, int center) {
  const int raw = analogRead(pin);
  int delta = raw - center;

  if (abs(delta) <= STICK_DEADBAND) {
    return 0;
  }

  if (delta > 0) {
    return map(delta, STICK_DEADBAND + 1, 1023 - center, 0, 255);
  }

  return map(delta, -STICK_DEADBAND - 1, -center, 0, -255);
}

byte readButtons() {
  byte mask = 0;

  for (byte i = 0; i < BUTTON_COUNT; i++) {
    if (digitalRead(BUTTON_PINS[i]) == LOW) {
      mask |= (1 << i);
    }
  }

  return mask;
}

void sendControlPacket(int x, int y, byte buttons) {
  Serial.print('<');
  Serial.print(constrain(x, -255, 255));
  Serial.print(',');
  Serial.print(constrain(y, -255, 255));
  Serial.print(',');
  Serial.print(buttons);
  Serial.println('>');
}
