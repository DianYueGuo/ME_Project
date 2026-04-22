/*
  Simplest car motor test over Bluetooth.

  Board: Arduino Nano
  Bluetooth: HC-05 on hardware serial D0/D1, 9600 baud

  Received command:
  - '1' -> both motors forward
  - '0' -> stop motors
*/

const unsigned long BLUETOOTH_BAUD_RATE = 9600;
const unsigned long COMMAND_TIMEOUT_MS = 500;

const byte PWMA_PIN = 5;
const byte AIN2_PIN = 6;
const byte AIN1_PIN = 7;
const byte STBY_PIN = 8;
const byte BIN1_PIN = 9;
const byte BIN2_PIN = 10;
const byte PWMB_PIN = 11;

const int TEST_MOTOR_PWM = 150;

bool motorEnabled = false;
unsigned long lastCommandTime = 0;

void setup() {
  Serial.begin(BLUETOOTH_BAUD_RATE);

  pinMode(PWMA_PIN, OUTPUT);
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(STBY_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);
  pinMode(PWMB_PIN, OUTPUT);

  stopMotors();
}

void loop() {
  readCommand();

  if (millis() - lastCommandTime > COMMAND_TIMEOUT_MS) {
    motorEnabled = false;
  }

  if (motorEnabled) {
    driveBothForward(TEST_MOTOR_PWM);
  } else {
    stopMotors();
  }
}

void readCommand() {
  while (Serial.available() > 0) {
    const char incoming = Serial.read();

    if (incoming == '1') {
      motorEnabled = true;
      lastCommandTime = millis();
    } else if (incoming == '0') {
      motorEnabled = false;
      lastCommandTime = millis();
    }
  }
}

void driveBothForward(int pwm) {
  digitalWrite(STBY_PIN, HIGH);

  digitalWrite(AIN1_PIN, HIGH);
  digitalWrite(AIN2_PIN, LOW);
  analogWrite(PWMA_PIN, constrain(pwm, 0, 255));

  digitalWrite(BIN1_PIN, HIGH);
  digitalWrite(BIN2_PIN, LOW);
  analogWrite(PWMB_PIN, constrain(pwm, 0, 255));
}

void stopMotors() {
  analogWrite(PWMA_PIN, 0);
  analogWrite(PWMB_PIN, 0);

  digitalWrite(AIN1_PIN, LOW);
  digitalWrite(AIN2_PIN, LOW);
  digitalWrite(BIN1_PIN, LOW);
  digitalWrite(BIN2_PIN, LOW);
  digitalWrite(STBY_PIN, LOW);
}
