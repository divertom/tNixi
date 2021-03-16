#include "tNixi_WiFi_functions.h"


// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "europe.pool.ntp.org";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

//Time Zones

    #define HOUR2MIN(hour)  hour * 60

    #define TZ_EST  -5  // Eastern Standard Time (USA)
    #define TZ_EDT  -4  // Eastern Daylight Time (USA)

    #define TZ_PST  -8  // Pacific Standard Time (USA)
    #define TZ_PDT  -7  // Pacific Daylight Time (USA)

//Define daylight savings time rules

    // US Eastern Time Zone (New York, Detroit)
    TimeChangeRule TimeRule_DST = {"EDT", Second, Sun, Mar, 2, HOUR2MIN(TZ_EDT)};    //Daylight time = UTC - 4 hours
    TimeChangeRule TimeRule_STD = {"EST", First, Sun, Nov, 2, HOUR2MIN(TZ_EST)};     //Standard time = UTC - 5 hours

    // US Pacific Time Zone (Seattle, Los Angeles)
    TimeChangeRule TimeRule_PDT = {"PDT", Second, Sun, Mar, 2, HOUR2MIN(TZ_PDT)};    //Daylight time = UTC - 4 hours
    TimeChangeRule TimeRule_PST = {"PST", First, Sun, Nov, 2, HOUR2MIN(TZ_PST)};     //Standard time = UTC - 5 hours


//Set my time zone + rule
    Timezone myTZ(TimeRule_PDT, TimeRule_PST);  //TBD make this configurabel


//NTP time 
    #define NTP_UPDATE_INTERVAL 1 * 3600 //update interval is 1h
    int TimeOffsetGMT = 0; //GMT/UTC
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

