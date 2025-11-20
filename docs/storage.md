# Removable USB Stick Concept

## Goals
- Acts as normal USB mass storage when plugged into a PC.
- When docked on the glove, the MCU can read/write profiles and configs.
- Optional: presence of the stick “arms” transmit features.

## Hardware Sketch
- Small micro on the stick (e.g., ATSAMD11/STM32F0/CH552) that exposes MSC to the PC.
- Dock connector (pogo/FFC) to glove wrist hub: 3V3, GND, UART (or I2C), stick-detect pin.
- Optional magnet/latch to keep it seated; LED to show safe-remove status.

## File Layout (on the stick)
- `/ir/*.ir` — IR captures.
- `/subghz/*.bin` — pulse trains for CC1101 replay.
- `/nfc/*.nfc` — NDEF/card profiles.
- `/hid/*.txt` — HID scripts (keystrokes).
- `/config.json` — gesture map, safety flags, defaults.
- `/logs/*.txt` — optional captures/status dumps.
- `/firmware.bin` — optional firmware update image (flashed on command).

## MicroSD Option
- Wire SPI microSD (e.g., HiLetgo 6-pin level-shifted module) to the MCU for bulk storage.
- Mirror the same directory layout as the stick; treat SD as local cache/bulk log area.
- On boot/wake, reconcile: prefer stick if present; otherwise use SD; finally fall back to internal flash.
- Use SD for larger captures and firmware staging; avoid excessive writes to internal flash.

## Wi-Fi Sync (ESP32/ESP8266)
- Run a small captive portal/AP or STA web server to browse/upload/download files in the same layout.
- Expose REST endpoints or WebDAV for `/ir`, `/subghz`, `/nfc`, `/hid`, `/config.json`, `/logs`.
- Guard writes behind an “armed” state or PIN; provide “safe to remove” status for the stick/SD when flushing.
- OTA: accept `/firmware.bin` upload and trigger a flash from SD/stick buffer.

## Dock Protocol (UART example)
- 115200 8N1, newline-delimited JSON or CBOR-lite.
- Messages:
  - `PING` / `PONG`
  - `LIST path`
  - `READ path offset length`
  - `WRITE path offset data_base64`
  - `COMMIT path size hash` (stick-side flush to flash)
  - `STATE {stick_present:bool, safe_to_remove:bool}`
- MCU flow:
  1) On stick-detect, send `PING`; wait for `PONG`.
  2) Mount paths to local cache; deny TX while writing.
  3) Before removal, send `STATE safe_to_remove=true`; blink LED/vibe.
  4) If stick is absent, fall back to internal flash cache (read-only or cached data).

## Safety
- Block removal during writes; keep a small capacitive hold-up on the stick micro.
- Add checksum/hash on writes; keep backups of critical configs on MCU flash.
- Optionally require stick-present to allow any TX modes; allow read-only if absent.
