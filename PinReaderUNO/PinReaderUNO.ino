/*
  Arduino UNO pin reader

  Prints the state of the Arduino UNO digital pins and analog inputs to the
  USB serial port.

  Notes:
  - Digital pins 0 and 1 are used by USB serial. They are read but not
    reconfigured because changing their pin mode can interfere with Serial.
  - Analog pins A0-A5 are also digital pins 14-19 on the UNO. This sketch
    prints them as both analog values and digital states.
*/

const unsigned long SERIAL_BAUD_RATE = 115200;
const unsigned long SAMPLE_INTERVAL_MS = 500;

unsigned long lastSampleTime = 0;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  for (byte pin = 2; pin <= 13; pin++) {
    pinMode(pin, INPUT_PULLUP);
  }

  for (byte analogIndex = 0; analogIndex < 6; analogIndex++) {
    pinMode(A0 + analogIndex, INPUT);
  }

  Serial.println(F("Arduino UNO pin reader"));
  Serial.println(F("D2-D13 use INPUT_PULLUP: LOW means connected to GND, HIGH means open or driven HIGH."));
  Serial.println(F("D0-D1 are USB serial RX/TX and are read without changing their mode."));
  Serial.println(F("Analog inputs are printed as ADC values from 0 to 1023."));
  Serial.println();
}

void loop() {
  const unsigned long now = millis();

  if (now - lastSampleTime < SAMPLE_INTERVAL_MS) {
    return;
  }

  lastSampleTime = now;

  printDigitalPins();
  Serial.print(F(" | "));
  printAnalogPins();
  Serial.println();
}

void printDigitalPins() {
  Serial.print(F("D:"));

  for (byte pin = 0; pin <= 13; pin++) {
    Serial.print(F(" D"));
    Serial.print(pin);
    Serial.print(F("="));
    Serial.print(digitalRead(pin));
  }

  for (byte analogIndex = 0; analogIndex < 6; analogIndex++) {
    const byte pin = A0 + analogIndex;

    Serial.print(F(" A"));
    Serial.print(analogIndex);
    Serial.print(F("(D"));
    Serial.print(pin);
    Serial.print(F(")="));
    Serial.print(digitalRead(pin));
  }
}

void printAnalogPins() {
  Serial.print(F("A:"));

  for (byte analogIndex = 0; analogIndex < 6; analogIndex++) {
    const byte pin = A0 + analogIndex;

    Serial.print(F(" A"));
    Serial.print(analogIndex);
    Serial.print(F("="));
    Serial.print(analogRead(pin));
  }
}
