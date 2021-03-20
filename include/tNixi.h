#ifndef _TNIXI_H_
    #define _TNIXI_H_

#include <TFT_eSPI.h>   
#include <ArduinoOTA.h>

extern String FirmwareVersion;

#define FW_VERSION FirmwareVersion
#define DEVICE_NAME "Tom_Nixi_Clock"

#define LED_ORIENTATION 2 // 0 & 2 Portrait. 1 & 3 landscape

#define TNIXI_TIME_FORMAT_24  0 //24 hour time display
#define TNIXI_TIME_FORMAT_12  1 //12 hour time display

#define TNIXI_DATE_FORMAT_US    0 //US date format MM/DD/YYYY
#define TNIXI_DATE_FORMAT_WORLD 1 //I guess, the rest of the world uses DD/MM/YYYY

//Structure contains all variabls and settings that are common across all digit TFTs
struct tNixi_Clock_Config
{
    //TFT configuration
    int *ActiveTFT;
    TFT_eSPI *TFT;

    //Time and Date
    int TimeFormat = TNIXI_TIME_FORMAT_24;
    int DateFormat = TNIXI_DATE_FORMAT_US;
    time_t CurrentTime;     // snapshot of the time that wil be displayed. This needs to be refreshed for each new time that is displayed.
    String TimeZone;
    bool RTCPowerStatus;
    
    //WiFi 
    String WiFiSSID ="";    // some screen will show the current SSID
    String WiFiPassword = ""; 
    String IPAddress = "";  
    bool WiFiConnected = false;


    //OTA Status
    bool OTAActive = false;
    String OTAComamand = "";
    unsigned int OTAPprogress = 0;
    unsigned int OTATotal = 0;
    ota_error_t OTAError;
};

extern tNixi_Clock_Config gClockConfig;

#endif