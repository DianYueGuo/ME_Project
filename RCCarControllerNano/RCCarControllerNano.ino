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
  0xAA, x, y, buttons, checksum

  x and y are encoded bytes where 0 is full negative, 127 is neutral,
  and 254 is full positive.
  y controls throttle: positive forward, negative reverse.
  x controls steering: positive right, negative left.
  checksum is x ^ y ^ buttons.
*/

const unsigned long BLUETOOTH_BAUD_RATE = 9600;
const unsigned long COMMAND_TIMEOUT_MS = 800;
const byte PACKET_START = 0xAA;

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

enum PacketState {
  WAIT_FOR_START,
  READ_X,
  READ_Y,
  READ_BUTTONS,
  READ_CHECKSUM
};

PacketState packetState = WAIT_FOR_START;
byte packetX = 127;
byte packetY = 127;
byte packetButtons = 0;

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
    const byte incoming = Serial.read();

    switch (packetState) {
      case WAIT_FOR_START:
        if (incoming == PACKET_START) {
          packetState = READ_X;
        }
        break;

      case READ_X:
        packetX = incoming;
        packetState = READ_Y;
        break;

      case READ_Y:
        packetY = incoming;
        packetState = READ_BUTTONS;
        break;

      case READ_BUTTONS:
        packetButtons = incoming;
        packetState = READ_CHECKSUM;
        break;

      case READ_CHECKSUM:
        parseControlPacket(packetX, packetY, packetButtons, incoming);
        packetState = WAIT_FOR_START;
        break;
    }
  }
}

void parseControlPacket(byte encodedX, byte encodedY, byte buttons, byte checksum) {
  if ((encodedX ^ encodedY ^ buttons) != checksum) {
    return;
  }

  commandX = decodeAxis(encodedX);
  commandY = decodeAxis(encodedY);
  commandButtons = buttons;
  lastCommandTime = millis();
}

int decodeAxis(byte value) {
  return map(constrain(value, 0, 254), 0, 254, -255, 255);
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
