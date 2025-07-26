#ifndef DEFINITIONS_H
#define DEFINITIONS_H

void loop(void);
void setup(void);

// #define AP_SSID "ESP32-Deauther"
// #define AP_PASS "esp32wroom32"
#define LED_PIN 2
const int CHANNEL_MAX = 11;
const int NUM_FRAMES_PER_DEAUTH = 16;
const int DEAUTH_BLINK_TIMES = 2;
const int DEAUTH_BLINK_DURATION = 20;
const int DEAUTH_TYPE_SINGLE = 0;
const int DEAUTH_TYPE_ALL = 1;
const int MAXIMUM_AP = 20;
const int TIME_TO_SLEEP = 300;
const int LED_PIN = 2;
const int SCAN_ROUNDS = 5;
static const char *TAG = "scan";

#define CHANNEL_LIST_SIZE 5
static uint8_t CHANNEL_LIST[CHANNEL_LIST_SIZE] = {1, 3, 6, 9, 11};

#define MAX_LIST_SIZE 0x32
#define DEAUTH_REASON 0x06

// Power Save Constants
// -----------------------------------------
#define uS_TO_S_FACTOR 1000000ULL

#define SERIAL_DEBUG

#ifdef SERIAL_DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#endif
#ifndef SERIAL_DEBUG
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif
#ifdef LED_PIN
#define BLINK_LED(num_times, blink_duration) blink_led(num_times, blink_duration)
#endif
#ifndef LED_PIN
#define BLINK_LED()
#endif

// void blink_led(int num_times, int blink_duration);

#endif
