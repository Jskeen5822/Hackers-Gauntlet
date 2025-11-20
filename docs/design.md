# THE-HAND Design

## Goals
- Gesture-driven glove that mirrors core Flipper-style features: IR learn/send, sub-GHz capture/replay, NFC/LF read/emulate, USB/BLE HID scripts.
- Removable USB stick for configs/logs/profiles; doubles as an “arm key” if desired.
- Safety by design: explicit arming, modifiers, and TX interlocks.

## System Architecture
- **MCU**: ESP32-S3 (BLE + USB), or RP2040 + BT coprocessor, or nRF52840 (BLE HID). Needs enough RAM/flash for captures.
- **Inputs**: flex sensors (2–4), Hall sensors/magnets for finger touches, IMU (MPU6050/ICM-20948) for tilt/knock, tactiles near thumb/index as safety modifiers.
- **Outputs/Features**:
  - IR send/learn (high-power IR LED + receiver).
  - Sub-GHz CC1101 for 315/433/868/915 MHz ASK/FSK capture/replay.
  - NFC/HF (PN532/PN7150) card/reader; LF (125 kHz) reader/emulate.
  - USB/BLE HID gadget; optional mouse-jiggler/presenter modes.
  - Wi-Fi (if ESP32/ESP8266): captive portal web UI, REST/WebSocket API, file sync, OTA updates, MQTT/log sink.
  - Display (0.66–0.96" OLED) + vibra motor for feedback.
- **Storage**: onboard flash (LittleFS) for profiles; removable USB stick docked via pogo/FFC for file sync; optional microSD SPI module for bulk captures/updates.
- **Power**: 3.7V LiPo on wrist, charger IC (MCP73831-class), buck/boost to 3.3V; USB-C for power/CDC/HID.
- **Sleep**: deep sleep after idle timeout or gesture; wake on IMU tap, Hall/tactile, timer, or USB; clear armed state on wake for safety.

## Software Architecture (ESP32-style)
- Tasks/services:
  - `gesture_input`: read IMU/flex/Hall, debounce, detect taps/double-taps/holds.
  - `gesture_router`: maps gestures to actions; enforces arm/safety rules and cooldowns.
  - `radio_subghz`: CC1101 driver, capture buffer, replay (OOK/FSK).
  - `nfc_hf` and `nfc_lf`: PN532 interface + LF front-end for read/emulate.
  - `ir`: learn (edge timing) + send (carrier PWM).
  - `hid`: USB/BLE HID (keyboard/mouse/scripts), plus CDC command shell.
  - `storage`: LittleFS + optional microSD + docked USB stick sync (see `docs/storage.md`).
  - `wifi`: web UI/API, OTA, file sync, MQTT; can be disabled for pure Arduino boards without Wi-Fi.
  - `ui`: display + vibra patterns + status LEDs.
- Data: profile files (`/ir/*.ir`, `/subghz/*.bin`, `/nfc/*.nfc`, `/hid/*.txt`), `config.json` for gesture map and safety flags.
- Command surfaces: USB CDC + BLE UART for configuration and logs; optional web UI over Wi-Fi AP or STA mode if using ESP32/ESP8266.

## Safety/Interlocks
- Require an `armed` flag set by a modifier or special gesture before any TX (IR/sub-GHz/NFC/HID script).
- Timeout auto-disarm; block TX when USB stick is mid-write or battery is low/high temp.
- Provide vibra or LED confirmation before/after any emission; allow a “panic” gesture to drop into receive-only.

## Mechanical/Layout Notes
- Wrist hub: battery, charger, MCU (and USB-C), power switches.
- Dorsal-hand PCB: CC1101 + antenna, IR LED + receiver, OLED, vibra, tactiles.
- Flex antennas: LF in palm, HF/NFC loop around cuff; keep orthogonal orientation for coupling options.
- Use FFC/pogo between wrist and dorsal boards; JST-SH for finger sensors. Keep RF away from battery leads; add LC filters on rails.
