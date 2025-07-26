

<pre style="font-family: monospace; align: center">
██████╗ ███████╗ █████╗ ██╗   ██╗████████╗██╗  ██╗    ██████╗ ██╗   ██╗ ██████╗██╗  ██╗
██╔══██╗██╔════╝██╔══██╗██║   ██║╚══██╔══╝██║  ██║    ██╔══██╗██║   ██║██╔════╝██║ ██╔╝
██║  ██║█████╗  ███████║██║   ██║   ██║   ███████║    ██║  ██║██║   ██║██║     █████╔╝ 
██║  ██║██╔══╝  ██╔══██║██║   ██║   ██║   ██╔══██║    ██║  ██║██║   ██║██║     ██╔═██╗ 
██████╔╝███████╗██║  ██║╚██████╔╝   ██║   ██║  ██║    ██████╔╝╚██████╔╝╚██████╗██║  ██╗
╚═════╝ ╚══════╝╚═╝  ╚═╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝    ╚═════╝  ╚═════╝  ╚═════╝╚═╝  ╚═╝
</pre>

# Deauth Duck

> PlatformIO variant of Deauth Duck, a WiFi deauthentication tool for ESP32-WROOM-32D boards, and cool kids like me.

<hr>

## Introduction

This is the root folder for the PlatformIO variant of Deauth Duck, built on top of the ESP-IDF framework. It consumes more space than the variant based on Arduino, but implements features not available in the Arduino variant.


## Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)
- [Contributing](../CONTRIBUTING.md)

## About <a name = "about"></a>

Was not pleased with other projects found on the hub. Some used a method of deauthentication that was questionable at best, while others were obviously designed for lab conditions as proofs of concept, and were not suitable for tests in the wild. Like most projects, it was intended to be a simple repurposing of pre-existing code, but it quickly got out of hand. Before I knew it, I was elbow deep in C++, and in foreign territory.

## Getting Started <a name = "getting_started"></a>

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See [deployment](#deployment) for notes on how to deploy the project on a live system.

### Prerequisites

The following software and hardware are required to run this project.

#### Hardware

This project was written for use with the ESP32-WROOM-32D board, which are currently really popular, and can be picked up for around $5 on some websites. If your device does not come with an external antenna jack, you may want to modify your board to provide one. 

#### Software

In order to use the plantformino variant, you will need platformio installed on your system. It can be easily installed in your system with uv or pipx.

```
uv tool install platformio
# OR
pipx install platformio
```

### Installing

A step by step series of examples that tell you how to get a development env running.

Say what the step will be

```
platformio upload
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo.

## Usage <a name = "usage"></a>

Once power is applied to the board, the program should immediately startup and begin working. It is intentionally designed to have all LED lights off by default, this way the board can be concealed as stealthly as possible.

## Developmental Notes <a name = "developmental_notes"></a>

Below are tidbits of information found relevant during the development of this project. They may be informative to users to gain further understanding.

### RSSI Signal Strength Thresholds

| RSSI Value Range | Wifi Signal Strength |
|------------------|----------------------|
| RSSI > -30 dBm   | Excellent            |
| RSSI < – 55 dBm  | Very Good            |
| RSSI < – 67 dBm  | Fairly Good          |
| RSSI < – 70 dBm  | Okay                 |
| RSSI < – 80 dBm  | Not Good             |
| RSSI < – 90 dBm  | Unusable             |


### Deauthentication Reason Codes

<table>
   <tr>
      <th>Code</th>
      <th>Meaning</th>
   </tr>
   <tr><td>0</td><td>Reserved.</td></tr>
   <tr><td>1</td><td>Unspecified reason.</td></tr>
   <tr><td>2</td><td>Previous authentication no longer valid.</td></tr>
   <tr><td>3</td><td>Deauthenticated because sending station (STA) is leaving or has left Independent Basic Service Set (IBSS) or ESS.</td></tr>
   <tr><td>4</td><td>Disassociated due to inactivity.</td></tr>
   <tr><td>5</td><td>Disassociated because WAP device is unable to handle all currently associated STAs.</td></tr>
   <tr><td>6</td><td>Class 2 frame received from nonauthenticated STA.</td></tr>
   <tr><td>7</td><td>Class 3 frame received from nonassociated STA.</td></tr>
   <tr><td>8</td><td>Disassociated because sending STA is leaving or has left Basic Service Set (BSS).</td></tr>
   <tr><td>9</td><td>STA requesting (re)association is not authenticated with responding STA.</td></tr>
   <tr><td>10</td><td>Disassociated because the information in the Power Capability element is unacceptable.</td></tr>
   <tr><td>11</td><td>Disassociated because the information in the Supported Channels element is unacceptable.</td></tr>
   <tr><td>12</td><td>Disassociated due to BSS Transition Management.</td></tr>
   <tr><td>13</td><td>Invalid element, that is, an element defined in this standard for which the content does not meet the specifications in Clause 8.</td></tr>
   <tr><td>14</td><td>Message integrity code (MIC) failure.</td></tr>
   <tr><td>15</td><td>4-Way Handshake timeout.</td></tr>
   <tr><td>16</td><td>Group Key Handshake timeout.</td></tr>
   <tr><td>17</td><td>Element in 4-Way Handshake different from (Re)Association Request/ Probe Response/Beacon frame.</td></tr>
   <tr><td>18</td><td>Invalid group cipher.</td></tr>
   <tr><td>19</td><td>Invalid pairwise cipher.</td></tr>
   <tr><td>20</td><td>Invalid AKMP.</td></tr>
   <tr><td>21</td><td>Unsupported RSNE version.</td></tr>
   <tr><td>22</td><td>Invalid RSNE capabilities.</td></tr>
   <tr><td>23</td><td>IEEE 802.1X authentication failed.</td></tr>
   <tr><td>24</td><td>Cipher suite rejected because of the security policy.</td></tr>
</table>

## Project Tree

```
platformio
├─ README.md
├─ include
│  ├─ README
│  ├─ deauth.h
│  ├─ definitions.h
│  ├─ main.h
│  ├─ sleep.h
│  └─ types.h
├─ lib
│  └─ README
├─ platformio.ini
├─ src
│  ├─ deauth.cpp
│  ├─ main.cpp
│  └─ sleep.cpp
└─ test
   └─ README

```

## TODO

Features that may or may not be implemented:

- [ ] Random Deauthentication reason code injection
- [ ] Implement Mac Address Spoofing
- [ ] Implement fix for Mac Address Spoofing Detection as with modern cellular devices
- [ ] Implement Evil Twin with ESPNOW protocol as WAN

## References

- [Tesa-Klebeband ESP32-Deauther](https://github.com/tesa-klebeband/ESP32-Deauther)
- [DeautherINO by ExPloit256](https://github.com/ExPloit256/DeautherINO)
- [Esp32Marauder Project](https://github.com/trisp3ar/ESP32Marauder)