# Mailbox Telemetry Device (MBTD)

An ultra-low power, wireless mailbox notification and telemetry system built around the ATmega2561 microcontroller. The system reliably detects incoming mail, monitors environmental parameters, and transmits data to a home-based receiver via a 2.4 GHz RF link.

## 📌 Project Overview
Checking a physical mailbox regularly can be inconvenient. This project solves that by using a two-module hardware architecture:
1. **Transmitter Module (Tx):** Battery-powered, mounted inside the mailbox. It remains in deep sleep mode to conserve energy and wakes up on hardware interrupts triggered by reed switches.
2. **Receiver Module (Rx):** Stationed indoors, features an OLED display with a custom UI to visually and acoustically notify the user of new mail and external weather conditions.

## ⚙️ Key Features
* **Event-Driven Mail Detection:** Uses magnetic reed switches for passive detection of mail insertion and mailbox opening (clearing the status).
* **Ultra-Low Power Consumption:** The Tx module utilizes `SLEEP_MODE_IDLE` and specific power-down methods for the RF module, achieving up to 25 hours of continuous operation on a small 900mAh Li-Po test battery.
* **Custom SPI Drivers:** Developed lightweight, custom C libraries for SPI communication to handle the nRF24L01+, BME280, and SSD1309 without relying entirely on high-level frameworks.
* **Environmental Telemetry:** Integrates a BME280 MEMS sensor to provide real-time external temperature and pressure readings.
* **Robust Hardware Design:** Custom PCB layout featuring a Step-Down XL7015E1 converter for efficient 5V logic supply and cascaded LDOs for 3.3V domains. Includes logic level shifters (BSS138) for safe communication.

## 🛠️ Hardware Stack
* **Microcontroller:** 8-bit AVR ATmega2561-16AU (Tx & Rx)
* **Wireless Communication:** nRF24L01+ (2.4 GHz)
* **Display:** 2.42" OLED (SSD1309 Controller)
* **Sensors:** BME280 (Temperature & Pressure), Reed Switches
* **Power Management:** 2S Li-Po battery, XL7015E1 Step-Down Converter, LM1117-5.0 & LM1117-3.3 LDOs
* **Expandability:** Rx module includes an unpopulated ESP8266 footprint and a hardware Real-Time Clock (DS3231) for future Smart Home integration.

## 💻 Software & Libraries
The firmware was developed in C using the Arduino IDE environment.
* `SPI.h` - Custom-built hardware SPI interface implementation.
* `BME280.h` - Custom library for fetching and processing calibration data from the sensor.
* `SSD1309.h` - Custom OLED graphics library with primitive drawing functions (Bresenham's algorithm) and bitmap rendering.
* `JC_Button` - Used for robust software debouncing of the hardware reed switches.

## 🚀 Future Improvements
- [ ] Activating the onboard ESP8266 for Wi-Fi and IoT connectivity (e.g., MQTT push notifications).
- [ ] Integrating the DFPlayer Mini for voice notifications.
- [ ] Shifting the Tx module to a deeper `PowerDown` mode to further extend battery life.
