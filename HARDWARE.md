
# Hardware Setup

## Required Parts
- ESP32-C3
- 2.8" ST7789 Touch Display
- SD Card or SPIFFS Flash
- Wi-Fi

## Wiring (Example)
| Display | ESP32-C3 |
|--------|----------|
| VCC | 3.3V |
| GND | GND |
| SCK | GPIO6 |
| MOSI | GPIO7 |
| CS | GPIO10 |
| DC | GPIO9 |
| RST | GPIO8 |

Touch uses XPT2046 (SPI).
