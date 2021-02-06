#ifndef _TNIXI_WIFI_FUNCTIONS_H_
    #define _TNIXI_WIFI_FUNCTIONS_H_

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

unsigned long GetNTPTime();
void WiFiInit(String SSID, String Password);

#endif
