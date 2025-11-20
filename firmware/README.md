# Firmware Scaffold

Target: ESP32-S3 (Arduino core) with BLE + native USB. Adapt pins and drivers as needed.

## Features to Implement (order of bring-up)
1) Inputs/feedback: buttons, IMU, flex/Hall sensors, vibra, OLED.
2) Safety: arm/panic gestures, TX gate, cooldowns.
3) IR: learn (edge timing) + send (PWM carrier).
4) BLE/USB HID: keyboard/mouse + script runner.
5) Sub-GHz (CC1101): capture/replay (OOK/FSK); start with fixed params.
6) NFC/HF (PN532) + LF read/emulate.
7) Storage: removable USB stick sync (UART/I2C protocol) + optional microSD SPI module.
8) Wi-Fi (ESP32/ESP8266): captive portal/web UI/API, OTA, file sync.
9) Power: deep sleep on idle/gesture; wake via RTC-capable pin or IMU INT; disarm on wake.

## Build Notes (ESP32-S3 + Arduino core)
- Install ESP32 board support (2.x) in Arduino IDE or PlatformIO (`framework = arduino`, `board = esp32-s3-devkitc-1` or similar).
- Enable USB CDC/HID: `USB_CDC_ON_BOOT=1`, `ARDUINO_USB_MODE=1`, `ARDUINO_USB_CDC_ON_BOOT=1`, `USB_HID` if using combined HID/CDC.
- Add libraries as you integrate: `Wire`, `SPI`, `Adafruit_SSD1306` (optional), `Adafruit_PN532` or `NDEF` lib, `RadioHead`/`ELECHOUSE_CC1101` (or custom), `Preferences`/`LittleFS`.
- For Wi-Fi/AP/OTA: `WiFi.h`, `WebServer`/`ESPAsyncWebServer` (if available), `Update.h`.
- For SD: `SD.h` (SPI); set `PIN_SD_CS` to your wiring.
- Logging: use `Serial` (CDC) and optionally BLE UART; keep rate-limited to avoid watchdogs.
- If you only have an Arduino + Wi-Fi module (ESP8266/ESP32 breakout), run a UART/SPI bridge: the main Arduino handles gestures/outputs; the Wi-Fi module serves the web UI/API and SD/USB proxy commands.

## Structure
- `esp32/main.cpp` — minimal loop and gesture router stub.
- Add drivers/modules as you go: `gesture.cpp`, `ir.cpp`, `cc1101.cpp`, `nfc.cpp`, `hid.cpp`, `storage.cpp`, `wifi.cpp`, `power.cpp`.

## Safety Reminders
- Keep TX off until explicitly armed; auto-disarm after timeout.
- Add a panic gesture/button to drop to receive-only.
- Guard writes to the removable stick; mark safe-to-remove before undocking.
- Clear armed state on wake from deep sleep; give vibra/LED cues for sleep/wake.
