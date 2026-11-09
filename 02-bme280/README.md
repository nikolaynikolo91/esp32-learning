# 02 - BME280 Environmental Sensor

## Overview

This project demonstrates how to interface a **BME280 environmental sensor** with an **ESP32** using the **I²C communication protocol** and **ESP-IDF**.

The ESP32 reads three environmental measurements:

- Temperature (°C)
- Relative humidity (%)
- Atmospheric pressure (hPa)

Measurements are displayed in the serial monitor every 3 seconds.

This exercise is part of my ESP32 learning journey and will eventually contribute to a smart beehive monitoring system.

## Hardware

- ESP32 DevKit
- BME280 sensor module
- Breadboard
- Jumper wires
- USB cable

## Wiring

| BME280 Pin | ESP32 Pin | Description |
|---|---|---|
| VCC | 3V3 | 3.3V power supply |
| GND | GND | Ground |
| SCL | GPIO22 | I²C clock |
| SDA | GPIO21 | I²C data |
| CSB | 3V3 | Enable I²C mode |
| SDO | GND | Select I²C address 0x76 |

**I²C address:** `0x76`

## Software

- Language: C
- Framework: ESP-IDF
- Development environment: VS Code + PlatformIO
- Sensor driver: Bosch Sensortec BME280 Sensor API

## How It Works

1. Initialize the ESP32 I²C controller at 100 kHz.
2. Verify the BME280 chip ID (`0x60`).
3. Initialize the Bosch BME280 driver.
4. Configure temperature, humidity and pressure measurements.
5. Read calibrated sensor values.
6. Print the measurements every 3 seconds.

## Build and Run

Open the `02-bme280` directory as a PlatformIO project.

**Build:**

```bash
pio run
```

**Upload:**

```bash
pio run -t upload
```

**Serial monitor:**

```bash
pio device monitor -b 115200
```

## Example Output

The following measurements were captured from the sensor:

```text
Temperature: 25.01 °C
Humidity: 33.99 %
Pressure: 974.15 hPa

----------------------------

Temperature: 25.02 °C
Humidity: 34.00 %
Pressure: 974.15 hPa
```

## What I Learned

- How the I²C protocol works with ESP32.
- How to configure SDA and SCL pins.
- How to scan an I²C bus and detect connected devices.
- How to verify a sensor using its chip ID.
- How to integrate a sensor driver with ESP-IDF.
- How to read and display calibrated environmental measurements.
- How to build, upload and monitor ESP32 firmware using PlatformIO.

## Next Steps

- Add more sensors for the smart beehive project.
- Send measurements to a Raspberry Pi.
- Store historical sensor data.
- Build a dashboard to visualize environmental conditions.

## Project Status

**Working prototype** — successfully reading temperature, humidity and atmospheric pressure from the BME280 sensor.
