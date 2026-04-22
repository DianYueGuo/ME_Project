/*
  RC car controller for Arduino Nano.

  Board: Arduino Nano
  Bluetooth: HC-05 paired with the remote controller's HC-05, 9600 baud

  Nano wiring follows README.md:
  - D0 RX <- HC-05 TX
  - D1 TX -> HC-05 RX through a resistor divider
  - D5  -> TB6612FNG PWMA
  - D6  -> TB6612FNG AIN2
  - D7  -> TB6612FNG AIN1
  - D8  -> TB6612FNG STBY
  - D9  -> TB6612FNG BIN1
  - D10 -> TB6612FNG BIN2
  - D11 -> TB6612FNG PWMB

  Packet format received from the remote:
  <x,y,buttons>

  x and y are signed values from -255 to 255.
  y controls throttle: positive forward, negative reverse.
  x controls steering: positive right, negative left.
*/

const unsigned long BLUETOOTH_BAUD_RATE = 9600;
const unsigned long COMMAND_TIMEOUT_MS = 300;

const int MAX_MOTOR_PWM = 160;
const int MAX_LEFT_MOTOR_PWM = MAX_MOTOR_PWM;
const int MAX_RIGHT_MOTOR_PWM = MAX_MOTOR_PWM;

const byte PWMA_PIN = 5;
const byte AIN2_PIN = 6;
const byte AIN1_PIN = 7;
const byte STBY_PIN = 8;
const byte BIN1_PIN = 9;
const byte BIN2_PIN = 10;
const byte PWMB_PIN = 11;

const byte S_PICKER_PIN = 2;
const byte S_STRICKER_PIN = 3;
const byte S_PITCH_PIN = 4;
const byte S_GATE_PIN = 12;
const byte HC05_STATE_PIN = 13;
const byte HC05_ENABLE_PIN = A0;

const byte SERIAL_BUFFER_SIZE = 32;

char serialBuffer[SERIAL_BUFFER_SIZE];
byte serialBufferIndex = 0;
bool receivingPacket = false;

int commandX = 0;
int commandY = 0;
byte commandButtons = 0;
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

  pinMode(S_PICKER_PIN, OUTPUT);
  pinMode(S_STRICKER_PIN, OUTPUT);
  pinMode(S_PITCH_PIN, OUTPUT);
  pinMode(S_GATE_PIN, OUTPUT);
  pinMode(HC05_STATE_PIN, INPUT);
  pinMode(HC05_ENABLE_PIN, OUTPUT);

  digitalWrite(HC05_ENABLE_PIN, LOW);
  digitalWrite(S_PICKER_PIN, LOW);
  digitalWrite(S_STRICKER_PIN, LOW);
  digitalWrite(S_PITCH_PIN, LOW);
  digitalWrite(S_GATE_PIN, LOW);

  stopMotors();
}

void loop() {
  readBluetooth();

  if (millis() - lastCommandTime > COMMAND_TIMEOUT_MS) {
    commandX = 0;
    commandY = 0;
    commandButtons = 0;
    stopMotors();
    applyButtonOutputs(commandButtons);
    return;
  }

  driveFromStick(commandX, commandY);
  applyButtonOutputs(commandButtons);
}

void readBluetooth() {
  while (Serial.available() > 0) {
    const char incoming = Serial.read();

    if (incoming == '<') {
      receivingPacket = true;
      serialBufferIndex = 0;
      continue;
    }

    if (!receivingPacket) {
      continue;
    }

    if (incoming == '>') {
      serialBuffer[serialBufferIndex] = '\0';
      parseControlPacket(serialBuffer);
      receivingPacket = false;
      serialBufferIndex = 0;
      continue;
    }

    if (serialBufferIndex < SERIAL_BUFFER_SIZE - 1) {
      serialBuffer[serialBufferIndex++] = incoming;
    } else {
      receivingPacket = false;
      serialBufferIndex = 0;
    }
  }
}

void parseControlPacket(char *packet) {
  char *xToken = strtok(packet, ",");
  char *yToken = strtok(NULL, ",");
  char *buttonsToken = strtok(NULL, ",");

  if (xToken == NULL || yToken == NULL || buttonsToken == NULL) {
    return;
  }

  commandX = constrain(atoi(xToken), -255, 255);
  commandY = constrain(atoi(yToken), -255, 255);
  commandButtons = constrain(atoi(buttonsToken), 0, 255);
  lastCommandTime = millis();
}

void driveFromStick(int x, int y) {
  const int leftSpeed = constrain(y + x, -255, 255);
  const int rightSpeed = constrain(y - x, -255, 255);

  setMotor(PWMA_PIN, AIN1_PIN, AIN2_PIN, leftSpeed, MAX_LEFT_MOTOR_PWM);
  setMotor(PWMB_PIN, BIN1_PIN, BIN2_PIN, rightSpeed, MAX_RIGHT_MOTOR_PWM);

  digitalWrite(STBY_PIN, HIGH);
}

void setMotor(byte pwmPin, byte in1Pin, byte in2Pin, int speed, int maxPwm) {
  const int safeMaxPwm = constrain(maxPwm, 0, 255);
  const int pwm = map(abs(speed), 0, 255, 0, safeMaxPwm);

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
  setMotor(PWMA_PIN, AIN1_PIN, AIN2_PIN, 0, MAX_LEFT_MOTOR_PWM);
  setMotor(PWMB_PIN, BIN1_PIN, BIN2_PIN, 0, MAX_RIGHT_MOTOR_PWM);
  digitalWrite(STBY_PIN, LOW);
}

void applyButtonOutputs(byte buttons) {
  digitalWrite(S_PICKER_PIN, bitRead(buttons, 0));
  digitalWrite(S_STRICKER_PIN, bitRead(buttons, 1));
  digitalWrite(S_PITCH_PIN, bitRead(buttons, 2));
  digitalWrite(S_GATE_PIN, bitRead(buttons, 3));
}
