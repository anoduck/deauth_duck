// C++ program to illustrate how to concatenate two arrays
// into a single one
#include <iostream>
#include <esp_wifi.h>
#include "esp_wifi_types.h"

using namespace std;

const int arr1[0] = {};
const int arr2[0] = {};

int comb_array(arr1, arr2)
{
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