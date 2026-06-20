# Mailbox Telemetry Device (MBTD)

[cite_start]An ultra-low power, wireless mailbox notification and telemetry system built around the ATmega2561 microcontroller[cite: 19]. [cite_start]The system reliably detects incoming mail, monitors environmental parameters, and transmits data to a home-based receiver via a 2.4 GHz RF link[cite: 16, 21].

## 📌 Project Overview
Checking a physical mailbox regularly can be inconvenient. This project solves that by using a two-module hardware architecture:
1. **Transmitter Module (Tx):** Battery-powered, mounted inside the mailbox. [cite_start]It remains in deep sleep mode to conserve energy and wakes up on hardware interrupts triggered by reed switches[cite: 18, 20].
2. [cite_start]**Receiver Module (Rx):** Stationed indoors, features an OLED display with a custom UI to visually and acoustically notify the user of new mail and external weather conditions[cite: 17, 56, 446].

## ⚙️ Key Features
* [cite_start]**Event-Driven Mail Detection:** Uses magnetic reed switches for passive detection of mail insertion and mailbox opening (clearing the status)[cite: 18].
* [cite_start]**Ultra-Low Power Consumption:** The Tx module utilizes `SLEEP_MODE_IDLE` and specific power-down methods for the RF module, achieving up to 25 hours of continuous operation on a small 900mAh Li-Po test battery[cite: 1367, 1582, 1589].
* [cite_start]**Custom SPI Drivers:** Developed lightweight, custom C libraries for SPI communication to handle the nRF24L01+, BME280, and SSD1309 without relying entirely on high-level frameworks[cite: 912].
* [cite_start]**Environmental Telemetry:** Integrates a BME280 MEMS sensor to provide real-time external temperature and pressure readings[cite: 393].
* [cite_start]**Robust Hardware Design:** Custom PCB layout featuring a Step-Down XL7015E1 converter for efficient 5V logic supply and cascaded LDOs for 3.3V domains[cite: 310, 788]. [cite_start]Includes logic level shifters (BSS138) for safe communication[cite: 437].

## 🛠️ Hardware Stack
* [cite_start]**Microcontroller:** 8-bit AVR ATmega2561-16AU (Tx & Rx) [cite: 133, 449]
* [cite_start]**Wireless Communication:** nRF24L01+ (2.4 GHz) [cite: 19]
* [cite_start]**Display:** 2.42" OLED (SSD1309 Controller) [cite: 622]
* [cite_start]**Sensors:** BME280 (Temperature & Pressure), Reed Switches [cite: 18, 21]
* [cite_start]**Power Management:** 2S Li-Po battery, XL7015E1 Step-Down Converter, LM1117-5.0 & LM1117-3.3 LDOs [cite: 107, 310, 765, 766]
* [cite_start]**Expandability:** Rx module includes an unpopulated ESP8266 footprint and a hardware Real-Time Clock (DS3231) for future Smart Home integration[cite: 558, 722].

## 💻 Software & Libraries
[cite_start]The firmware was developed in C using the Arduino IDE environment[cite: 22].
* [cite_start]`SPI.h` - Custom-built hardware SPI interface implementation[cite: 912].
* [cite_start]`BME280.h` - Custom library for fetching and processing calibration data from the sensor[cite: 1088].
* [cite_start]`SSD1309.h` - Custom OLED graphics library with primitive drawing functions (Bresenham's algorithm) and bitmap rendering[cite: 1154, 1218, 1220].
* [cite_start]`JC_Button` - Used for robust software debouncing of the hardware reed switches[cite: 900, 905].

## 🚀 Future Improvements
- [ ] [cite_start]Activating the onboard ESP8266 for Wi-Fi and IoT connectivity (e.g., MQTT push notifications)[cite: 558, 1635].
- [ ] [cite_start]Integrating the DFPlayer Mini for voice notifications[cite: 1640].
- [ ] [cite_start]Shifting the Tx module to a deeper `PowerDown` mode to further extend battery life[cite: 1646].
