#ifndef CONTROL_PROTOCOL_H
#define CONTROL_PROTOCOL_H

#include <Arduino.h>
#include "ProjectConfig.h"

const byte PACKET_START_1 = 0xAA;
const byte PACKET_START_2 = 0x55;
const byte PACKET_LENGTH = 9;

struct ControlPacket {
  byte sequence;
  byte buttons;
  uint16_t stickX;
  uint16_t stickY;
};

inline byte crc8(const byte *data, byte length) {
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

inline void writeControlPacket(Stream &serial, byte sequence, byte buttons, uint16_t stickX, uint16_t stickY) {
  const byte packet[] = {
    PACKET_START_1,
    PACKET_START_2,
    sequence,
    byte(buttons & CONTROL_BUTTON_MASK),
    byte(stickX & 0xFF),
    byte((stickX >> 8) & 0xFF),
    byte(stickY & 0xFF),
    byte((stickY >> 8) & 0xFF)
  };
  const byte crc = crc8(packet, sizeof(packet));

  serial.write(packet, sizeof(packet));
  serial.write(crc);
}

class ControlPacketParser {
public:
  bool feed(byte incoming, ControlPacket &packet) {
    if (packetIndex == 0) {
      if (incoming == PACKET_START_1) {
        packetBuffer[packetIndex++] = incoming;
      }
      return false;
    }

    if (packetIndex == 1) {
      if (incoming == PACKET_START_2) {
        packetBuffer[packetIndex++] = incoming;
      } else if (incoming == PACKET_START_1) {
        packetBuffer[0] = incoming;
        packetIndex = 1;
      } else {
        packetIndex = 0;
      }
      return false;
    }

    packetBuffer[packetIndex++] = incoming;

    if (packetIndex < PACKET_LENGTH) {
      return false;
    }

    packetIndex = 0;

    if (packetBuffer[PACKET_LENGTH - 1] != crc8(packetBuffer, PACKET_LENGTH - 1)) {
      return false;
    }

    packet.sequence = packetBuffer[2];
    packet.buttons = packetBuffer[3] & CONTROL_BUTTON_MASK;
    packet.stickX = uint16_t(packetBuffer[4]) | (uint16_t(packetBuffer[5]) << 8);
    packet.stickY = uint16_t(packetBuffer[6]) | (uint16_t(packetBuffer[7]) << 8);
    return true;
  }

private:
  byte packetBuffer[PACKET_LENGTH];
  byte packetIndex = 0;
};

#endif
