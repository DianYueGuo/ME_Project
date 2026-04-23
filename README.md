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
arduino-cli compile --fqbn arduino:avr:uno --library libraries/MEProjectShared SenderUNO
```

Compile and upload the UNO sender, replacing `/dev/cu.usbmodemXXXX` with the UNO port from `arduino-cli board list`:

```sh
arduino-cli compile --upload -p /dev/cu.usbmodemXXXX --fqbn arduino:avr:uno --library libraries/MEProjectShared SenderUNO
```

Compile the Nano receiver with the old ATmega328P bootloader:

```sh
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old --library libraries/MEProjectShared --library ~/Library/Arduino15/libraries/Servo ReceiverNano
```

Compile and upload the Nano receiver, replacing `/dev/cu.usbserialXXXX` with the Nano port from `arduino-cli board list`:

```sh
arduino-cli compile --upload -p /dev/cu.usbserialXXXX --fqbn arduino:avr:nano:cpu=atmega328old --library libraries/MEProjectShared --library ~/Library/Arduino15/libraries/Servo ReceiverNano
```

The receiver uses `Servo.h` and the repo-local `MEProjectShared` library. On this machine, `Servo.h` is installed under `~/Library/Arduino15/libraries/Servo`, but `arduino-cli lib list` does not include it by default, so Nano commands pass that folder explicitly.

Shared project settings and helpers live in:

```text
libraries/MEProjectShared/src
```

Change shared baud rate, HC-05 pins, packet format, servo limits, or diagnostic swing behavior there so the main sketches and test sketches stay aligned.

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

### HC-05 Control Pins
- Receiver Bluetooth sketches configure **D13** as `INPUT` for HC-05 `STATE`.
- Receiver Bluetooth sketches leave **A0** as `INPUT` by default so the code does not accidentally disable the HC-05 or force AT mode.
- If you want the Nano to drive HC-05 `ENABLE/KEY`, set `HC05_DRIVE_ENABLE_PIN` to `true` in `libraries/MEProjectShared/src/ProjectConfig.h`, then set `HC05_ENABLE_NORMAL_LEVEL` for your breakout.
