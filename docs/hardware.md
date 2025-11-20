# Hardware Notes

## Minimal Prototype BOM (bench)
- MCU: ESP32-S3 dev board (USB + BLE) or RP2040 if you prefer; breadboard-friendly.
- IMU: MPU6050 breakout (I2C).
- Flex sensors: 2–4 pcs + resistor dividers; or Hall sensors (A3144) + small magnets on fingertips.
- IR: IR LED (high-power 940 nm) + NPN/MOSFET driver + resistor; IR receiver module (VS1838B/TSOP38238).
- Sub-GHz: CC1101 module breakout; simple 433/868 MHz whip or compact PCB antenna.
- NFC/HF: PN532 breakout; LF: T5577/EM4100-compatible front-end or buy a small 125 kHz reader module. Flex loop antennas stitched into glove.
- Storage: optional microSD SPI module (e.g., HiLetgo 6-pin level-shifted) for bulk captures/updates; removable USB stick for user-facing files.
- Feedback: 3V vibe motor + transistor; small OLED (SSD1306 0.66–0.96").
- Power: LiPo + charger IC (MCP73831/TP4056) + buck/boost (TPS63020-class) to 3.3V; switch and protection board.
- Connectors: FFC/pogo for removable USB stick and dorsal board; JST-SH for sensors; USB-C on wrist.
- Wi-Fi option: if you only have an Arduino + Wi-Fi module (ESP8266/ESP32-C3 breakout), run SPI/UART to it for captive portal/API; ESP32 dev board remains the simplest all-in-one.

## Suggested Pin Map (ESP32-S3 example; adjust to your board)
- I2C: SDA/SCL → IMU + OLED (shared bus).
- SPI: SCK/MOSI/MISO + CS → CC1101; separate CS for PN532 SPI if you choose SPI (or leave PN532 on I2C); extra CS for microSD module.
- GPIO (analog): A0–A3 → flex sensor dividers.
- GPIO (digital): GPIOx → Hall sensors; GPIOy → tactiles (thumb/index modifiers).
- PWM: GPIOz → vibra motor driver.
- IR: one GPIO → IR LED PWM (carrier), one GPIO → IR receiver input.
- USB: native USB for CDC/HID (ESP32-S3) or via USB-UART for logs if on RP2040/other.
- Dock: UART/I2C pair for removable USB stick micro (bridge).
- Wake: choose an RTC-capable GPIO (ESP32: RTC IO) tied to IMU INT or a tactile for deep-sleep wake; add VBUS detect if you want wake on USB plug.

## Physical/Layout Tips
- Wrist hub hosts battery, charge/power, MCU, USB-C, dock connector for removable stick.
- Dorsal board handles antennas, IR, display, vibra, buttons. Keep IR LED near knuckles with line of sight.
- Route HF/LF antennas in fabric channels; keep RF front-ends away from battery leads and large ground pours.
- Add TVS/fuse on USB-C, series resistors on LEDs, and LC filters on sensitive rails.
- Label pogo/FFC pins clearly for service: VBUS, 3V3, GND, UART/I2C, detect pin for stick presence.
- MicroSD wiring (HiLetgo 6-pin): VCC→3.3V, GND→GND, CS→free GPIO, SCK→SPI SCK, MOSI→SPI MOSI, MISO→SPI MISO; leave 5V unconnected on ESP32-class boards.
