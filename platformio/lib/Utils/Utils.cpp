// C++ program to illustrate how to concatenate two arrays
// into a single one
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <esp_wifi.h>
#include <sys/random.h>
#include <esp_random.h>
#include <esp_wifi_types.h>
#include "Utils.hpp"


using namespace std;

void setRandomMac() {
  // Generate random MAC
  static uint8_t macAddr[6];
  do {
    for (int i = 0; i < 6; i++) {
      macAddr[i] = esp_fill_random(void *buf, size_t 256); // Random byte
    }
    macAddr[0] &= 0xFC; // Clear the lowest two bits: use unicast & universally administered address
  } while (macAddr[0] == 0x00); // Prevent all-zero MAC address
  esp_wifi_set_mac(WIFI_IF_AP, macAddr);
  uint8_t last_mac5 = macAddr[5];
  do {
    macAddr[5] = random(256);
  } while (macAddr[5] == last_mac5);
  esp_wifi_set_mac(WIFI_IF_STA, macAddr);
}

int array_size(uint8_t array[]) {
  size_t size_charray = sizeof(array) / sizeof(array[0]);
  return size_charray;
}

int comb_array(int arr1[], int arr2[]) {
    // Determine the size of the arrays
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);

    // Create a new array to hold the concatenated elements
    int arr3[n1 + n2];

    // Copy the elements from the first array
    for (int i = 0; i < n1; i++)
        arr3[i] = arr1[i];

    // Copy the elements from the second array
    for (int i = 0; i < n2; i++)
        arr3[n1 + i] = arr2[i];

    // Print the concatenated array
    cout << "Concatenated Array: ";
    for (int i = 0; i < n1 + n2; i++)
        cout << arr3[i] << " ";
    cout << endl;

    return 0;
};