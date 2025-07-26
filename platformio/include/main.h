#ifndef MAIN_H
#define MAIN_H

void loop(void);
void setup(void);

#define CHANNEL_LIST_SIZE 5
static uint8_t CHANNEL_LIST[CHANNEL_LIST_SIZE] = {1, 3, 6, 9, 11};

#define DEAUTH_REASON 0x06

// Power Save Constants
// -----------------------------------------
#define uS_TO_S_FACTOR 1000000ULL
const int TIME_TO_SLEEP = 300;

const int LED_PIN = 2;
const int SCAN_ROUNDS = 5;
const int CHANNEL_MAX = 11;

#endif