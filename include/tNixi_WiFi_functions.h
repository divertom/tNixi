#ifndef _TNIXI_WIFI_FUNCTIONS_H_
    #define _TNIXI_WIFI_FUNCTIONS_H_

#include <Arduino.h>
#include "tNixi.h"

#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Timezone.h>    // https://github.com/JChristensen/Timezone

extern Timezone myTZ;

unsigned long GetNTPTime();
void WiFiInit(String SSID, String Password);

#endif
