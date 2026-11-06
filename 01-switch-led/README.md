# ESP32 Switch-Controlled LED

This PlatformIO project uses ESP-IDF and C to read a slide switch on an ESP32 DevKit and control a red LED. The switch input uses the ESP32's internal pull-up resistor, so the input reads `LOW` when the switch connects it to ground. The firmware checks the switch every 100 ms.

## Required components

- ESP32 DevKit
- Red LED
- 230 ohm resistor
- 3-pin SPDT slide switch
- Breadboard and jumper wires
- USB cable for the ESP32 DevKit

## Wiring

### Slide switch

| Breadboard position | Connection |
| --- | --- |
| J10 | Switch pin |
| J11 | Switch pin |
| J12 | Switch pin (not connected) |
| H10 | ESP32 GPIO18 (`D18`) |
| H11 | ESP32 GND |

`H10` and `J10` share breadboard row 10, and `H11` and `J11` share row 11. With the switch in the ON position, it connects the GPIO18 side to ground.

### LED circuit

| Breadboard position | Connection |
| --- | --- |
| J20 | LED anode (+) |
| J21 | LED cathode (-) |
| H17 to H20 | 230 ohm resistor |
| F17 | ESP32 GPIO23 (`D23`) |
| H21 | ESP32 GND |

`F17` connects GPIO23 to the resistor through row 17. The resistor connects to the LED anode through row 20, and the LED cathode is grounded through row 21.

## Circuit diagram

```text
Switch input (internal pull-up enabled)

ESP32 GPIO18 (D18) ---- H10 = J10 ----[ SPDT switch ]---- J11 = H11 ---- GND
                                               |
                                              J12
                                         not connected

LED output

ESP32 GPIO23 (D23) ---- F17 = H17 ----[ 230 ohm ]---- H20 = J20 ----|>|---- J21 = H21 ---- GND
                                                                  Red LED
```

## Behavior

- Switch ON: the switch connects GPIO18 to GND, the input reads `LOW`, and the LED turns ON.
- Switch OFF: GPIO18 is held `HIGH` by the internal pull-up, and the LED turns OFF.
- GPIO18 is configured as an input with the internal pull-up enabled.
- GPIO23 is configured as the LED output.
- The switch state is checked every 100 ms.

## Build, upload, and monitor

The project configuration defines the `esp32dev` environment, uses the `espressif32` PlatformIO platform with the ESP-IDF framework, and sets the serial monitor speed to 115200 baud.

From this directory:

```sh
cd 01-switch-led
pio run
pio run --target upload
pio device monitor
```

If PlatformIO cannot automatically find the board, add the appropriate serial port, for example:

```sh
pio run --target upload --upload-port /dev/ttyUSB0
pio device monitor --port /dev/ttyUSB0 --baud 115200
```

The serial monitor prints `LED ON` or `LED OFF` as the switch is read.

## Learning outcomes

- Configure ESP32 GPIO pins with ESP-IDF in a PlatformIO project.
- Use an internal pull-up resistor for a digital switch input.
- Understand active-low input logic and grounding a switch input.
- Drive an LED safely from a GPIO output through a current-limiting resistor.
- Build, upload, and monitor an ESP32 application with PlatformIO.
