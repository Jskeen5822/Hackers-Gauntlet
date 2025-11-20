#include <Arduino.h>
#include <string.h>

struct Gesture {
  const char* id;
  bool valid;
  uint32_t t;
};

constexpr uint32_t ARM_MS = 30000;        // milliseconds to stay armed after arm gesture
constexpr uint32_t LOOP_DELAY_MS = 5;     // main loop delay
constexpr uint32_t SLEEP_TIMEOUT_MS = 30000; // idle timeout before deep sleep

static bool armed = false;
static uint32_t armedUntil = 0;
static uint32_t lastActivity = 0;

#ifdef ARDUINO_ARCH_ESP32
#include "esp_sleep.h"
const gpio_num_t WAKE_PIN = GPIO_NUM_0; // change to a real RTC-capable pin tied to button/IMU INT
#endif

// ---- Stubs to replace with real implementations ----
void initHardware() {
  // TODO: init IMU, flex/Hall sensors, buttons, display, vibe, radios, storage
}

Gesture readGesture() {
  // TODO: fuse IMU + flex/Hall + buttons into gesture IDs
  return {"", false, millis()};
}

void vibe(const char* /*pattern*/) {
  // TODO: implement haptic feedback patterns
}

void sendIR(const char* /*profile*/) {
  // TODO: load IR profile file and transmit
}

void hidKey(const char* /*key*/) {
  // TODO: send HID key over USB/BLE
}

void runHIDScript(const char* /*path*/) {
  // TODO: execute a stored HID script
}

void emulateNFC(const char* /*profile*/) {
  // TODO: start NFC/LF emulation from profile
}

void replaySubGhz(const char* /*path*/) {
  // TODO: replay sub-GHz capture via CC1101
}

void startCapture(const char* /*kind*/) {
  // TODO: begin capture mode (IR or sub-GHz) and log to storage
}

void serviceRadios() {
  // TODO: keep radio state machines alive
}

void serviceStorage() {
  // TODO: handle removable stick sync and filesystem tasks
}

void serviceUI() {
  // TODO: refresh display/LEDs
}

void serviceWifi() {
  // TODO: web UI/API, OTA, file sync when Wi-Fi-capable MCU/module is present
}

void goToSleep() {
#ifdef ARDUINO_ARCH_ESP32
  vibe("sleep");
  esp_sleep_enable_ext0_wakeup(WAKE_PIN, 0); // wake on low
  esp_sleep_enable_timer_wakeup(10ULL * 60ULL * 1000000ULL); // wake after 10 minutes
  esp_deep_sleep_start();
#else
  // TODO: add low-power mode for non-ESP32 boards if available
#endif
}

void servicePower(uint32_t now) {
  if (now - lastActivity > SLEEP_TIMEOUT_MS) {
    goToSleep();
  }
}
// ---- End stubs ----

void routeGesture(const Gesture& g, uint32_t now) {
  if (strcmp(g.id, "arm") == 0) {
    armed = true;
    armedUntil = now + ARM_MS;
    vibe("armed");
    return;
  }
  if (strcmp(g.id, "panic") == 0) {
    armed = false;
    vibe("panic");
    return;
  }
  if (!armed || now > armedUntil) {
    armed = false;
    return;
  }

  if (strcmp(g.id, "fist") == 0) sendIR("tv_power.ir");
  else if (strcmp(g.id, "tilt_left") == 0) hidKey("prev");
  else if (strcmp(g.id, "tilt_right") == 0) hidKey("next");
  else if (strcmp(g.id, "double_tap") == 0) runHIDScript("open_term.txt");
  else if (strcmp(g.id, "pinch") == 0) emulateNFC("badge_1.nfc");
  else if (strcmp(g.id, "index_flex") == 0) startCapture("ir");
  else if (strcmp(g.id, "garage") == 0) replaySubGhz("garage.bin");
  else if (strcmp(g.id, "sleep") == 0) goToSleep();
}

void setup() {
  Serial.begin(115200);
  delay(200);
  initHardware();
  vibe("boot");
  lastActivity = millis();
}

void loop() {
  const uint32_t now = millis();
  Gesture g = readGesture();
  if (g.valid) {
    routeGesture(g, now);
    lastActivity = now;
  }

  serviceRadios();
  serviceStorage();
  serviceUI();
  serviceWifi();
  servicePower(now);

  delay(LOOP_DELAY_MS);
}
