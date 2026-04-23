#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <Arduino.h>

const unsigned long SERIAL_BAUD_RATE = 9600;
const unsigned long SEND_INTERVAL_MS = 25;
const unsigned long FAILSAFE_TIMEOUT_MS = 300;
const unsigned long RECEIVE_TIMEOUT_MS = 300;
const unsigned long HOLD_REPEAT_DELAY_MS = 400;
const unsigned long HOLD_REPEAT_INTERVAL_MS = 150;
const unsigned long SWING_INTERVAL_MS = 20;

const byte HC05_STATE_PIN = 13;
const bool REQUIRE_HC05_STATE_CONNECTED = false;
const byte HC05_ENABLE_PIN = A0;
const bool HC05_DRIVE_ENABLE_PIN = false;
const byte HC05_ENABLE_NORMAL_LEVEL = LOW;

const byte BUTTON_A_PIN = 2;
const byte BUTTON_B_PIN = 3;
const byte BUTTON_C_PIN = 4;
const byte BUTTON_D_PIN = 5;
const byte BUTTON_E_PIN = 6;
const byte BUTTON_F_PIN = 7;
const byte BUTTON_K_PIN = 8;

const byte BUTTON_A_MASK = 0x01;
const byte BUTTON_B_MASK = 0x02;
const byte BUTTON_C_MASK = 0x04;
const byte BUTTON_D_MASK = 0x08;
const byte BUTTON_E_MASK = 0x10;
const byte BUTTON_F_MASK = 0x20;
const byte BUTTON_K_MASK = 0x40;
const byte CONTROL_BUTTON_MASK = BUTTON_A_MASK | BUTTON_B_MASK | BUTTON_C_MASK | BUTTON_D_MASK | BUTTON_E_MASK | BUTTON_F_MASK | BUTTON_K_MASK;

const byte PICKER_SERVO_PIN = 2;
const byte STRICKER_SERVO_PIN = 3;
const byte PITCH_SERVO_PIN = 4;
const byte GATE_SERVO_PIN = 12;

const int PICKER_MIN_ANGLE = 10;
const int PICKER_MAX_ANGLE = 180;
const int PICKER_START_ANGLE = PICKER_MIN_ANGLE;
const int PICKER_STEP_ANGLE = 10;

const int PITCH_MIN_ANGLE = 35;
const int PITCH_MAX_ANGLE = 133;
const int PITCH_START_ANGLE = PITCH_MIN_ANGLE;
const int PITCH_STEP_ANGLE = 5;
const int PITCH_SWING_STEP_ANGLE = 1;

const int GATE_E_ANGLE = 80;
const int GATE_F_ANGLE = 45;
const int GATE_START_ANGLE = GATE_E_ANGLE;

const int STRICKER_NEUTRAL_ANGLE = 90;
const int STRICKER_ACTIVE_ANGLE = 40;
const int STRICKER_START_ANGLE = STRICKER_NEUTRAL_ANGLE;

#endif
