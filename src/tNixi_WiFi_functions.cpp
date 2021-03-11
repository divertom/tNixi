#include "tNixi_WiFi_functions.h"


// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "europe.pool.ntp.org";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

//const int timeZone = 1;   // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)

#define NTP_UPDATE_INTERVAL 1 * 3600 //update interval is 1h
#define DST_OFFSET 3600              //one hour DST offset

int TimeOffsetGMT = (timeZone * 3600); //default time zone

unsigned int localPort = 8888;  // local port to listen for UDP packets

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServerName, TimeOffsetGMT, NTP_UPDATE_INTERVAL);

static void onWiFiEvent_GotIP(WiFiEvent_t event)
{
    Serial.print("Connected to "); Serial.print(WiFi.SSID()); Serial.print(" with IP address: "); Serial.println(WiFi.localIP()); 
    timeClient.begin(); //start NTP client
}

static void onWiFiEvent_Disconnected(WiFiEvent_t event)
{
    Serial.print("Lost Connected to "); Serial.println(WiFi.SSID());  
    timeClient.end(); //start NTP client
    WiFi.begin(); //try to connect again
}

void WiFiInit(String SSID, String Password)
{
    WiFi.onEvent(onWiFiEvent_GotIP, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(onWiFiEvent_Disconnected, SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.begin(SSID.c_str(), Password.c_str());
}

unsigned long GetNTPTime()
{
    timeClient.forceUpdate();
    Serial.print("NTP time: "); Serial.println(timeClient.getFormattedTime());
    return timeClient.getEpochTime();
}

