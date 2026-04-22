/*
  TB6612FNG motor test for the RC car Arduino Nano.

  Upload this sketch to test the motor driver, motor power, and motor wiring
  without Bluetooth or the remote controller.

  Nano wiring follows README.md:
  - D5  -> TB6612FNG PWMA
  - D6  -> TB6612FNG AIN2
  - D7  -> TB6612FNG AIN1
  - D8  -> TB6612FNG STBY
  - D9  -> TB6612FNG BIN1
  - D10 -> TB6612FNG BIN2
  - D11 -> TB6612FNG PWMB
*/

const byte PWMA_PIN = 5;
const byte AIN2_PIN = 6;
const byte AIN1_PIN = 7;
const byte STBY_PIN = 8;
const byte BIN1_PIN = 9;
const byte BIN2_PIN = 10;
const byte PWMB_PIN = 11;

const int TEST_SPEED = 180;
const unsigned long RUN_TIME_MS = 2000;
const unsigned long STOP_TIME_MS = 1000;

void setup() {
  pinMode(PWMA_PIN, OUTPUT);
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(STBY_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);
  pinMode(PWMB_PIN, OUTPUT);

  stopMotors();
  digitalWrite(STBY_PIN, HIGH);
}

void loop() {
  driveBoth(TEST_SPEED);
  delay(RUN_TIME_MS);

  stopMotors();
  delay(STOP_TIME_MS);

  driveBoth(-TEST_SPEED);
  delay(RUN_TIME_MS);

  stopMotors();
  delay(STOP_TIME_MS);

  setMotor(PWMA_PIN, AIN1_PIN, AIN2_PIN, TEST_SPEED);
  setMotor(PWMB_PIN, BIN1_PIN, BIN2_PIN, 0);
  delay(RUN_TIME_MS);

  stopMotors();
  delay(STOP_TIME_MS);

  setMotor(PWMA_PIN, AIN1_PIN, AIN2_PIN, 0);
  setMotor(PWMB_PIN, BIN1_PIN, BIN2_PIN, TEST_SPEED);
  delay(RUN_TIME_MS);

  stopMotors();
  delay(STOP_TIME_MS);
}

void driveBoth(int speed) {
  setMotor(PWMA_PIN, AIN1_PIN, AIN2_PIN, speed);
  setMotor(PWMB_PIN, BIN1_PIN, BIN2_PIN, speed);
}

void setMotor(byte pwmPin, byte in1Pin, byte in2Pin, int speed) {
  const int pwm = constrain(abs(speed), 0, 255);

  if (speed > 0) {
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
  } else if (speed < 0) {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
  } else {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, LOW);
  }

  analogWrite(pwmPin, pwm);
}

void stopMotors() {
  setMotor(PWMA_PIN, AIN1_PIN, AIN2_PIN, 0);
  setMotor(PWMB_PIN, BIN1_PIN, BIN2_PIN, 0);
}
