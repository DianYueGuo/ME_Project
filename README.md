# ME_Project

## Arduino Sketches

- `RemoteControllerUNO/RemoteControllerUNO.ino` reads the UNO controller buttons and XY stick, then sends joystick packets through an HC-05 Bluetooth module.
- `RCCarControllerNano/RCCarControllerNano.ino` receives those packets on the Nano and drives the TB6612FNG motor driver with differential steering.
- `MotorTestNano/MotorTestNano.ino` tests the Nano, TB6612FNG, motor power, and motor wiring without Bluetooth.

Remote controller HC-05 pins used by the sketch:
- **D0 (RX)** ← HC-05 TX
- **D1 (TX)** → HC-05 RX (via 1k/2k resistor divider)
- Bluetooth baud rate: **9600**

Both boards use their hardware UART pins for HC-05 Bluetooth. Disconnect the HC-05 RX/TX wires while uploading sketches if the IDE cannot upload.

## Motor Speed Limit

Set the car's maximum motor PWM in `RCCarControllerNano/RCCarControllerNano.ino`:

```cpp
const int MAX_MOTOR_PWM = 160;
```

The remote still sends joystick commands from `-255` to `255`, and the car scales them to the configured maximum. For example, with `MAX_MOTOR_PWM = 160`, full stick produces PWM `160`, half stick produces about PWM `80`, and neutral produces PWM `0`.

If one side is stronger than the other, trim the motors separately:

```cpp
const int MAX_LEFT_MOTOR_PWM = MAX_MOTOR_PWM;
const int MAX_RIGHT_MOTOR_PWM = MAX_MOTOR_PWM;
```

## Compile and Upload

These commands use `arduino-cli`. Install the Arduino AVR core once if it is not already installed:

```sh
arduino-cli core update-index
arduino-cli core install arduino:avr
```

List connected boards and note the serial port, for example `/dev/cu.usbmodem...` on macOS or `COM3` on Windows:

```sh
arduino-cli board list
```

Compile the remote controller UNO sketch:

```sh
arduino-cli compile --fqbn arduino:avr:uno RemoteControllerUNO
```

Upload the remote controller UNO sketch:

```sh
arduino-cli upload -p <UNO_PORT> --fqbn arduino:avr:uno RemoteControllerUNO
```

If this board only uploads in Arduino IDE when **Processor** is set to **ATmega328P (Old Bootloader)**, use the Nano old-bootloader FQBN instead:

```sh
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old RemoteControllerUNO
arduino-cli upload -p <UNO_PORT> --fqbn arduino:avr:nano:cpu=atmega328old RemoteControllerUNO
```

Compile the RC car Nano sketch:

```sh
arduino-cli compile --fqbn arduino:avr:nano RCCarControllerNano
```

Upload the RC car Nano sketch:

```sh
arduino-cli upload -p <NANO_PORT> --fqbn arduino:avr:nano RCCarControllerNano
```

Some Nano boards use the old bootloader. If upload fails with a sync or timeout error, try this FQBN instead:

```sh
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old RCCarControllerNano
arduino-cli upload -p <NANO_PORT> --fqbn arduino:avr:nano:cpu=atmega328old RCCarControllerNano
```

Compile and upload the motor-only Nano test:

```sh
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old MotorTestNano
arduino-cli upload -p <NANO_PORT> --fqbn arduino:avr:nano:cpu=atmega328old MotorTestNano
```

The motor test repeats this sequence:
- both motors forward for 2 seconds
- stop for 1 second
- both motors reverse for 2 seconds
- stop for 1 second
- motor A only for 2 seconds
- stop for 1 second
- motor B only for 2 seconds
- stop for 1 second

## Remote Controller Details

Button A: pressed D2=0, released D2=1
Button B: pressed D3=0, released D3=1
Button C: pressed D4=0, released D4=1
Button D: pressed D5=0, released D5=1
Button E: pressed D6=0, released D6=1
Button F: pressed D7=0, released D7=1
Button K: pressed D8=0, released D8=1

Stick X: Full left A0=0, Neutral A0=507-508, Full right A0=1023
Stick Y: Full down A1=0, Neutral A1=513, Full up A1=1023

## RC Car Controller Arduino Nano Pin Connections

### Power
- **VIN** → None
- **GND** → Common ground  
- **+5V** → From logic DC-DC converter (CVTR_LOGIC output)

### UART (HC-05 Bluetooth)
- **D1 (TX)** → HC-05 RX (via 1k/2k resistor divider)  
- **D0 (RX)** → HC-05 TX  

### Digital Control Signals
- **D2** → S_PICKER  
- **D3** → S_STRICKER  
- **D4** → S_PITCH  

### Motor Driver (TB6612FNG)
- **D5** → PWMA  
- **D6** → AIN2  
- **D7** → AIN1  
- **D8** → STBY  
- **D9** → BIN1  
- **D10** → BIN2  
- **D11** → PWMB  

### Other Control
- **D12** → S_GATE  
- **D13** → HC-05 STATE  

### Analog Pins
- **A0** → HC-05 ENABLE  
- **A1–A7** → Not connected  
