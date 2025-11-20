# Gestures and Routing

## Inputs
- **Flex sensors**: measure bend; use thresholds + hysteresis.
- **Hall sensors**: detect finger/thumb contact via small magnets; use as “chords.”
- **IMU**: detect tilt (left/right/up), knocks/taps (accel spikes), and double-taps by timing windows.
- **Tactile modifiers**: thumb/index buttons to arm or shift modes.

## Detection Approach
- Sample at ~100–200 Hz; low-pass IMU a bit; debounce Hall/tactiles.
- Derive events: `tap`, `double_tap`, `tilt_left/right`, `fist` (multiple flex over threshold), `pinch` (Hall sense).
- Maintain state: `armed` flag, last gesture time, cooldown timers to prevent thrash.
- Use hysteresis on flex thresholds to avoid noise; log raw values over serial for calibration.

## Example Gesture Map
- `thumb+index button` (hold 1s): arm system (sets `armed=true` for 30s).
- `fist`: IR send `tv_power.ir`.
- `tilt_left/right`: prev/next slide (HID key).
- `double_tap` (knock): run HID script `open_term.txt`.
- `pinch` (thumb-index Hall): emulate NFC profile `badge_1.nfc`.
- `index flex only`: start capture mode (IR learn or sub-GHz pulse log).
- `panic` (triple tap): disarm, stop TX tasks.
- `sleep` (long-hold thumb+index): enter deep sleep; wake via dedicated RTC pin (button/IMU INT) and clear armed state on wake.

## Routing Pseudocode
```cpp
struct Gesture { const char* id; unsigned long t; };

bool armed = false;
unsigned long armedUntil = 0;

void route(const Gesture& g) {
  unsigned long now = millis();
  if (g.id == String("arm")) { armed = true; armedUntil = now + 30000; vibe("armed"); return; }
  if (g.id == String("panic")) { armed = false; vibe("panic"); return; }
  if (!armed || now > armedUntil) { armed = false; return; }

  if (g.id == String("fist")) sendIR("tv_power.ir");
  else if (g.id == String("tilt_left")) hidKey("prev");
  else if (g.id == String("tilt_right")) hidKey("next");
  else if (g.id == String("double_tap")) runHIDScript("open_term.txt");
  else if (g.id == String("pinch")) emulateNFC("badge_1.nfc");
  else if (g.id == String("index_flex")) startCapture("ir");
  else if (g.id == String("sleep")) goToSleep();
}
```

## Calibration Flow
- Log raw flex/IMU data over USB CDC while you perform candidate gestures.
- Set per-sensor thresholds and hysteresis; tune tap timing windows (e.g., 80–250 ms gap for double-tap).
- Add per-gesture cooldowns (e.g., 500–1000 ms) to reduce repeats.
