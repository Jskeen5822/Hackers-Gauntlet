# Hackers-Gauntlet

Glove-mounted multi-tool inspired by Flipper-style features: gesture-triggered actions, radios (IR, sub-GHz), NFC/LF, HID, and a removable USB key for config/logs.

## Repo Map
- `docs/design.md` – system architecture, module breakdown, safety.
- `docs/hardware.md` – suggested BOM, pin map ideas, wiring notes.
- `docs/gestures.md` – gesture types, detection/state machine, sample mappings.
- `docs/storage.md` – removable USB key layout, microSD option, and sync protocol.
- `firmware/` – implementation notes and starter sketch (ESP32/Arduino style with Wi-Fi hooks).

## Quick Start (prototype concept)
1) Pick MCU with BLE/USB: ESP32-S3 dev board is the default; add CC1101 (sub-GHz), PN532 (NFC), IR LED + receiver, vibra motor, IMU (MPU6050), and 2–4 flex/Hall sensors. If you only have an Arduino + Wi-Fi module (ESP8266/ESP32 breakout), use UART/SPI to bridge Wi-Fi features.
2) Power: small LiPo on wrist, charger (MCP73831-class), buck/boost for 3.3V. Expose USB-C for power/CDC/debug.
3) Inputs: place tactiles near thumb/index as safety modifiers; flex/Hall on fingers; IMU for tilt/knock.
4) Outputs: IR blaster/learner, CC1101 antenna on dorsal PCB, NFC/LF flex antennas in cuff/palm, small OLED and vibe; optional Wi-Fi web UI/API.
5) Storage: removable USB stick docked on the wrist; optional microSD SPI module for bulk captures and firmware staging.
6) Firmware: see `firmware/` for the task map and starter loop; add Wi-Fi/OTA/file sync; keep a safety “arm” gesture/button before any transmit.
7) Power saving: implement deep sleep on idle/gesture; wake via RTC-capable tactile or IMU interrupt and clear armed state.

## Safety & Legal
- Obey local regulations for sub-GHz/NFC/RFID transmission; prefer receive-first modes.
- Add current limiting and thermal/charge protection; block removal of the USB stick during writes.
- Use explicit arm gestures/modifiers and timeouts to avoid accidental TX.

## Next Steps
- Flesh out BOM and pin map for your chosen MCU board.
- Implement gesture router and one output at a time: IR send/learn → BLE/USB HID → sub-GHz → NFC/LF.
- Prototype the removable USB stick bridge and file sync flow (see `docs/storage.md`).
