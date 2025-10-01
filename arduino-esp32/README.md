# Arduino deauth_duck 

``` C
/home/vassilios/Sandbox/deauth_duck/arduino-esp32/arduino-esp32.ino: In function 'void start_deauth(int, uint16_t)':
/home/vassilios/Sandbox/deauth_duck/arduino-esp32/arduino-esp32.ino:184:33: error: no matches converting function 'sniffer' to type 'wifi_promiscuous_cb_t' {aka 'void (*)(void*, enum wifi_promiscuous_pkt_type_t)'}
  184 |   esp_wifi_set_promiscuous_rx_cb(&sniffer);
      |   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~
/home/vassilios/Sandbox/deauth_duck/arduino-esp32/arduino-esp32.ino:143:16: note: candidates are: 'void sniffer(void*, wifi_promiscuous_pkt_t)'
  143 | IRAM_ATTR void sniffer(void *buf, wifi_promiscuous_pkt_t type) {
      |                ^~~~~~~
/home/vassilios/Sandbox/deauth_duck/arduino-esp32/arduino-esp32.ino:78:16: note:                 'void sniffer()'
   78 | IRAM_ATTR void sniffer();
      |                ^~~~~~~
exit status 1

Compilation error: no matches converting function 'sniffer' to type 'wifi_promiscuous_cb_t' {aka 'void (*)(void*, enum wifi_promiscuous_pkt_type_t)'}
```