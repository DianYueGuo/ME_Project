# ME_Project

## Compile and Upload with arduino-cli

Install the Arduino AVR core if it is not already installed:

```sh
arduino-cli core install arduino:avr
```

List connected Arduino serial ports:

```sh
arduino-cli board list
```

Compile the UNO sender:

```sh
arduino-cli compile --fqbn arduino:avr:uno SenderUNO
```

Upload the UNO sender, replacing `/dev/cu.usbmodemXXXX` with the UNO port from `arduino-cli board list`:

```sh
arduino-cli upload -p /dev/cu.usbmodemXXXX --fqbn arduino:avr:uno SenderUNO
```

Compile the Nano receiver with the old ATmega328P bootloader:

```sh
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old --libraries /Users/joseph/Library/Arduino15/libraries ReceiverNano
```

Upload the Nano receiver, replacing `/dev/cu.usbserialXXXX` with the Nano port from `arduino-cli board list`:

```sh
arduino-cli upload -p /dev/cu.usbserialXXXX --fqbn arduino:avr:nano:cpu=atmega328old ReceiverNano
```

The receiver uses `Servo.h`. On this machine, `Servo.h` is installed under `/Users/joseph/Library/Arduino15/libraries`, but `arduino-cli lib list` does not include it by default, so the Nano compile command passes that folder explicitly.

If upload fails while the HC-05 is connected to D0/D1, disconnect the HC-05 TX/RX wires, upload again, then reconnect them.

## Remote Controller Details (Sender)

Button A: pressed D2=0, released D2=1
Button B: pressed D3=0, released D3=1
Button C: pressed D4=0, released D4=1
Button D: pressed D5=0, released D5=1
Button E: pressed D6=0, released D6=1
Button F: pressed D7=0, released D7=1
Button K: pressed D8=0, released D8=1

Stick X: Full left A0=0, Neutral A0=507-508, Full right A0=1023
Stick Y: Full down A1=0, Neutral A1=513, Full up A1=1023

## RC Car Controller Arduino Nano Pin Connections (Receiver)

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
