#include <Arduino.h>

String FirmwareVersion(String(__DATE__) + String(" ") + String(__TIME__));

#include "tNixi.h"

#define FS_NO_GLOBALS
#include <FS.h>

#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

#include <JPEGDecoder.h>
#include <TFT_eSPI.h>    
#include <RTClib.h>
#include <ArduinoOTA.h>

#include "JPEG_functions.h"
#include "SPIFFS_functions.h"
#include "tNixi_Digit.h"
#include "tNixi_WiFi_functions.h"

#include "o:\GlobalSettings_privat.h"
  /**** GlobalSettings_private.h ******
  #define WIFI_SSID   <your WiFi SSID>
  #define WIFI_PASSWORD <your WiFi password>
  */


//define CS for displays
#define TFT_CS_Digit_0  5   //seconds 1
#define TFT_CS_Digit_1  17  //seconds 10
#define TFT_CS_Digit_2  25  //minutes 1
#define TFT_CS_Digit_3  12  //minutes 10
#define TFT_CS_Digit_4  36  //hours 1
#define TFT_CS_Digit_5  14  //hours 10


#define LED_ORIENTATION 2 // 0 & 2 Portrait. 1 & 3 landscape

#define NIXI_TYPE "/NixiA_"
//#define NIXI_TYPE "/NixiB_"
//#define NIXI_TYPE "/NixiC_"

#define BOOT_SCREEN_TIMEOUT 10 //show boot screen for 10s

TFT_eSPI tft = TFT_eSPI(); 

//TFT backlight settings 
#define TFT_BL_PIN 27  //TFT backlight LEDs
#define TFT_BL_CHANNEL 0 //PWM channel for TFT backlight 
#define TFT_BL_FREQ 5000 //PWM frequency for TFT backlight 4kHz
#define TFT_BL_RERSOLUTION 10 //PWM resolution for TFT backlight
#define TFT_BL_INITIAL_LEVEL 1024 

int tft_BL_brightness = TFT_BL_INITIAL_LEVEL;    // how bright the TFT
int tft_BL_fade = 100;

int tft_ActiveCS = TFT_CS_Digit_0;

//====================================================================================
//                                    Backlight 
//====================================================================================
#define BL_PHOTO_PIN 36 //BL photo resistor is on pin 36
#define BL_INITIAL_BRIGNESS_LEVEL 3 // initial BL level

//Define backlight hysteresis table 
#define BL_NUM_HYST_LEVELS 4 // 0..3
const uint16_t blHystRaisingLevels[ BL_NUM_HYST_LEVELS] = {550 , 1200, 2000, 3000} ;
const uint16_t blHystFalingLevels [ BL_NUM_HYST_LEVELS] = { 400, 1000, 1500, 2800} ;
const uint16_t blBrightness [ BL_NUM_HYST_LEVELS]       = {  10,  200, 1000, 4000} ;

uint16_t blCrrentLevel = BL_INITIAL_BRIGNESS_LEVEL; //current BL brighness level from list blBrightness

// Set TFT Brightness 
void blSetBrightness(void)
{
    //read photo resistor
    uint16_t blNewBrighnessLevel = 0;
    uint16_t blPhotoResistorLevel = analogRead(BL_PHOTO_PIN);
    //Serial.println(blPhotoResistorLevel);
    
    //check if brighnes needs to go up
    for( uint16_t i = 0; i < BL_NUM_HYST_LEVELS; i++ )
    {
        if ( blPhotoResistorLevel > blHystRaisingLevels[i]) blNewBrighnessLevel = i;
        else break;
    }

    if (blNewBrighnessLevel > blCrrentLevel) 
    {
        blCrrentLevel = blNewBrighnessLevel;    //set the new BL level
        Serial.print("New BL level up: "); Serial.println(blCrrentLevel);
        ledcWrite(TFT_BL_CHANNEL, blBrightness[blCrrentLevel]); // set the brightness of the LED
        return;     //we are done, don't need to check for falling level
    }

    //check if brigness needs to go down
    for( uint16_t i = 0; i < BL_NUM_HYST_LEVELS; i++ )
    {
        if ( blPhotoResistorLevel > blHystFalingLevels[i]) blNewBrighnessLevel = i;
        else break;
    }

    if (blNewBrighnessLevel < blCrrentLevel) 
    {
        blCrrentLevel = blNewBrighnessLevel;    //set the new BL level
        Serial.print("New BL level down: "); Serial.println(blCrrentLevel);
        ledcWrite(TFT_BL_CHANNEL, blBrightness[blCrrentLevel]); // set the brightness of the LED
    }
}

//====================================================================================
//                                    Global Variables
//====================================================================================
tNixi_Clock_Config ClockConfig;
long bootTime = 0;
bool showBootbootTimeScreen = true;

//Create a tNixi_Tube object for each tube. This should be change to a list of objects
tNixi_Tube Tube0;
tNixi_Tube Tube1;
tNixi_Tube Tube2;
tNixi_Tube Tube3;
tNixi_Tube Tube4;
tNixi_Tube Tube5;

tNixi_Digit DigitSec1;
tNixi_Digit DigitSec10;
tNixi_Digit DigitMin1;
tNixi_Digit DigitMin10;
tNixi_Digit DigitHour1;
tNixi_Digit DigitHour10;
tNixi_Digit DigitBoot;

//====================================================================================
//                                    Display Stuff
//====================================================================================
bool TFTsInit()
{
  //**** Initialize digits
  // I/O pin setup 
  pinMode(TFT_CS_Digit_0, OUTPUT); 
  pinMode(TFT_CS_Digit_1, OUTPUT); 
  pinMode(TFT_CS_Digit_2, OUTPUT); 
  pinMode(TFT_CS_Digit_3, OUTPUT); 
  pinMode(TFT_CS_Digit_4, OUTPUT); 
  pinMode(TFT_CS_Digit_5, OUTPUT);  

  //set all CS active (low) to be initialized at the same time
  digitalWrite(TFT_CS_Digit_0, LOW);  
  digitalWrite(TFT_CS_Digit_1, LOW);
  digitalWrite(TFT_CS_Digit_2, LOW);
  digitalWrite(TFT_CS_Digit_3, LOW);  
  digitalWrite(TFT_CS_Digit_4, LOW);
  digitalWrite(TFT_CS_Digit_5, LOW);

  tft.begin();

  //set all CS inactive (hight) 
  digitalWrite(TFT_CS_Digit_0, HIGH);
  digitalWrite(TFT_CS_Digit_1, HIGH);
  digitalWrite(TFT_CS_Digit_2, HIGH);
  digitalWrite(TFT_CS_Digit_3, HIGH);
  digitalWrite(TFT_CS_Digit_4, HIGH);
  digitalWrite(TFT_CS_Digit_5, HIGH);


  return true;
}

//====================================================================================
//                                    Time Stuff
//====================================================================================
RTC_DS3231 RTC;

uint32_t lastRTC_NTPSync = 0;
#define RTC_NTP_SYNC_INTERVAL 6 * 3600  //sync RTC from NTP every 6h
#define RTC_SYSTIM_SYNC_INTERVAL time_t(5)  //after how many seconds the system time is synced with the RTC time

time_t getRTC()
{
  return RTC.now().unixtime();
}

//Check if RTC shoudl be synced with NTP
bool RTC_NTPSyncNeeded()
{
  if((RTC.now().unixtime() - lastRTC_NTPSync) > RTC_NTP_SYNC_INTERVAL) return true;
  return false;
}

//Sync the RTC time wiht the NTP time if needed
void SyncRTC_NTP(void)
{
  //sync RTC ... only if it's time to do this or RTC lost power ... and only if there is WiFi
  if ((RTC_NTPSyncNeeded() || RTC.lostPower()) && ClockConfig.WiFiConnected)
  {
    Serial.print("Adjust RTC with ");
    RTC.adjust(DateTime(GetNTPTime()));
    lastRTC_NTPSync = RTC.now().unixtime();
  }
}

bool RTCInit()
{
  if (!RTC.begin())
  {
      Serial.println("Couldn't find RTC");
      //TBD use NTP as time source
  }
  else
  {
    //RTC testing only
      //RTC.adjust(DateTime(F(__DATE__), F(__TIME__))); //just used once to init the RTC 
        //OR
      //RTC.adjust(DateTime(F(__DATE__), F("00:00:00"))); //testing RTC setting via NTP remove in production code
    //RTC testing 

    setSyncInterval(RTC_SYSTIM_SYNC_INTERVAL);
    setSyncProvider(getRTC);  //set RTC as time provider
    if(timeStatus()!= timeSet) 
      Serial.println("Unable to sync with the RTC");
    else
      Serial.println("RTC has set the system time");
  }

  //Just for testing 
  Serial.print(hour());Serial.print(":");
  Serial.print(minute());Serial.print(":");
  Serial.print(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 

  return true;
}

//====================================================================================
//                                    OTA Stuff
//====================================================================================

bool OTAInit()
{

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(DEVICE_NAME);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() 
    {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "Firmware";
      else // U_SPIFFS
      {
        type = "Data";
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        //SPIFFS.end(); // need to deal with this whiel showing the clock - need to stop reading data form SPIFFS
      }

      ClockConfig.OTAActive = false;
      ClockConfig.OTAComamand = type;  
      Serial.println("Start updating " + type);
    })
    
    .onEnd([]() 
    {
      ClockConfig.OTAActive = false;
      Serial.println("\nEnd");
    })
    
    .onProgress([](unsigned int progress, unsigned int total) 
    {
      ClockConfig.OTAPprogress = progress;
      ClockConfig.OTATotal = total;
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    
    .onError([](ota_error_t error) 
    {
      ClockConfig.OTAError = error;
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  return true;
}

//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(115200); 
  Serial.println("tNixi Clock Project");

  if (!SPIFFS.begin()) {
      Serial.println("SPIFFS initialisation failed!");
      while (1) yield(); 
  }
  Serial.println("SPIFFS Initialized");
  listFiles(); // Lists all files that ara availabel in the SPIFFS

  //Initializ time source
  RTCInit();

  //Initialize displays
  TFTsInit();

  //prep clock configurations
  ClockConfig.ActiveTFT = &tft_ActiveCS;
  ClockConfig.TFT = &tft;
  ClockConfig.TimeFormat = TNIXI_TIME_FORMAT_24;
  ClockConfig.DateFormat = TNIXI_DATE_FORMAT_WORLD;
  ClockConfig.WiFiSSID = WIFI_SSID; //SSID come from GlobalSettings_privat.h
  ClockConfig.WiFiPassword = WIFI_PASSWORD; //WiFI Password comes from GlobalSettings_privat.h

  //init Tubes
  Tube0.Init(TFT_CS_Digit_0, &ClockConfig);
  Tube1.Init(TFT_CS_Digit_1, &ClockConfig);
  Tube2.Init(TFT_CS_Digit_2, &ClockConfig);
  Tube3.Init(TFT_CS_Digit_3, &ClockConfig);
  Tube4.Init(TFT_CS_Digit_4, &ClockConfig);
  Tube5.Init(TFT_CS_Digit_5, &ClockConfig);


  //create digits 
  DigitSec1.Init(&ClockConfig,TNIXI_MODE_SECOND_1);
  DigitSec1.InitGraphicMode(NIXI_TYPE);
  DigitSec1.GraphicMode();

  DigitSec10.Init(&ClockConfig,TNIXI_MODE_SECOND_10);
  DigitSec10.InitGraphicMode(NIXI_TYPE);
  DigitSec10.GraphicMode();

  DigitMin1.Init(&ClockConfig,TNIXI_MODE_MINUTE_1);
  DigitMin1.InitGraphicMode(NIXI_TYPE);
  DigitMin1.GraphicMode();

  DigitMin10.Init(&ClockConfig,TNIXI_MODE_MINUTE_10);
  DigitMin10.InitGraphicMode(NIXI_TYPE);
  DigitMin10.GraphicMode();

  DigitHour1.Init(&ClockConfig,TNIXI_MODE_HOUR_1);
  DigitHour1.InitGraphicMode(NIXI_TYPE);
  DigitHour1.GraphicMode();

  DigitHour10.Init(&ClockConfig,TNIXI_MODE_HOUR_10);
  DigitHour10.InitGraphicMode(NIXI_TYPE);
  DigitHour10.GraphicMode();

  DigitBoot.Init(&ClockConfig,TNIXI_MODE_BOOT);
  DigitBoot.InitGraphicMode(NIXI_TYPE);
  DigitBoot.GraphicMode();

  //set inital tube displayes    
  Tube0.SetDigit(&DigitBoot); //start with the boot screen
  showBootbootTimeScreen = true;

/*
  Tube1.SetDigit(&DigitSec10);
  Tube2.SetDigit(&DigitMin1);
  Tube3.SetDigit(&DigitMin10);
  Tube4.SetDigit(&DigitHour1);
  Tube5.SetDigit(&DigitHour10);
*/

  //Test
  Tube1.SetDigit(&DigitMin10);
  Tube2.SetDigit(&DigitHour1);
  Tube3.SetDigit(&DigitHour10);
  //Tube4.SetDigit(&DigitHour1);
  //Tube5.SetDigit(&DigitHour10);
  //Test

  //TFT backligth setup
  ledcSetup(TFT_BL_CHANNEL, TFT_BL_FREQ, TFT_BL_RERSOLUTION);
  ledcAttachPin(TFT_BL_PIN, TFT_BL_CHANNEL);
  ledcWrite(TFT_BL_CHANNEL, TFT_BL_INITIAL_LEVEL); // set the brightness of the LED

  //WiFi Setup - we will not wait here for WiFi to connect
  WiFiInit(ClockConfig.WiFiSSID.c_str(), ClockConfig.WiFiPassword.c_str());

  OTAInit();  //Initializ over the air update

  bootTime = now();
}

//====================================================================================
//                                    The Loop
//====================================================================================
void loop()
{
  //************ Do some system housekeeping **********************
  blSetBrightness();  //adjust the TFT backlight
  SyncRTC_NTP();  //sync RTC time with NTP time if needed 

  //************ Prepare the display data *************************
  if (WiFi.isConnected())
  {
    ClockConfig.WiFiConnected = true;  //set current WiFi conenction status
    ClockConfig.IPAddress = WiFi.localIP().toString();
  }
  else 
  {
    ClockConfig.WiFiConnected = false;  //set current WiFi conenction status
    ClockConfig.IPAddress = String("0.0.0.0");
  }
  
  TimeChangeRule *TimeRule;        //pointer to the time change rule, use to get TZ abbrev
  ClockConfig.CurrentTime = myTZ.toLocal(now(), &TimeRule);  //adjust for time zone and daylight savings time
  ClockConfig.TimeZone = TimeRule->abbrev;

  //************ Set what every tube should show **************
  if (showBootbootTimeScreen)
  {
    if ((now() - bootTime) > BOOT_SCREEN_TIMEOUT)
    {
        //Tube0.SetDigit(&DigitSec1); //replace boot screen with second digit 1 display
        Tube0.SetDigit(&DigitMin1); //replace boot screen with second digit 1 display
        showBootbootTimeScreen = false;
    }
  }


  //************ The last thing to do, refresh all displays *******
  Tube0.Refresh();
  Tube1.Refresh();
  Tube2.Refresh();
  Tube3.Refresh();
  //Tube4.Refresh();
  //Tube5.Refresh();

  //**** finaly doing the OTA stuff
  ArduinoOTA.handle();
  
}
//====================================================================================

