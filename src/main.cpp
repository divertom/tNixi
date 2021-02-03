#include <Arduino.h>

#define FS_NO_GLOBALS
#include <FS.h>

#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

#include "JPEG_functions.h"
#include "SPIFFS_functions.h"
#include <JPEGDecoder.h>
#include <TFT_eSPI.h>      
#include "tNixi_Digit.h"


#include "o:\GlobalSettings_privat.h"
/**** GlobalSettings_private.h ******
#define WIFI_SSID   <your WiFi SSID>
#define WIFI_PASSWORD <your WiFi password>
*/

//define CS for displays
#define TFT_CS_Digit_0  5   //seconds 1
#define TFT_CS_Digit_1  17  //seconds 10
#define TFT_CS_Digit_2  19  //minutes 1
//#define TFT_CS_Digit_3  19  //minutes 10
//#define TFT_CS_Digit_4  19  //hours 1
//#define TFT_CS_Digit_5  19  //hours 10


#define LED_ORIENTATION 2 // 0 & 2 Portrait. 1 & 3 landscape

#define NIXI_TYPE "/NixiA_"
//#define NIXI_TYPE "/NixiB_"
//#define NIXI_TYPE "/NixiC_"

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

tNixi_Digit Digit0;
tNixi_Digit Digit1;
tNixi_Digit Digit2;

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

  //**** Initialize digits
  // I/O pin setup 
  pinMode(TFT_CS_Digit_0, OUTPUT); 
  pinMode(TFT_CS_Digit_1, OUTPUT); 
  pinMode(TFT_CS_Digit_2, OUTPUT); 

  //set all CS active (low) to be initialized at the same time
  digitalWrite(TFT_CS_Digit_0, LOW);  
  digitalWrite(TFT_CS_Digit_1, LOW);
  digitalWrite(TFT_CS_Digit_2, LOW);

  tft.begin();

  //set all CS inactive (hight) 
  digitalWrite(TFT_CS_Digit_0, HIGH);
  digitalWrite(TFT_CS_Digit_0, HIGH);
  digitalWrite(TFT_CS_Digit_0, HIGH);

  //prep clock configurations
  ClockConfig.ActiveTFT = &tft_ActiveCS;
  ClockConfig.TFT = &tft;
  ClockConfig.TimeFormat = TNIXI_TIME_FORMAT_24;
  ClockConfig.DateFormat = TNIXI_DATE_FORMAT_WORLD;
  ClockConfig.WiFiSSID = WIFI_SSID; //SSID come from GlobalSettings_privat.h

  //create digits
  Digit0.Init(TFT_CS_Digit_0, &ClockConfig,TNIXI_MODE_SECOND_1);
  Digit0.InitGraphicMode(NIXI_TYPE);
  Digit0.GraphicMode();

  Digit1.Init(TFT_CS_Digit_1, &ClockConfig,TNIXI_MODE_SECOND_10);
  Digit1.InitGraphicMode(NIXI_TYPE);
  Digit1.GraphicMode();

  Digit2.Init(TFT_CS_Digit_2, &ClockConfig,TNIXI_MODE_MINUTE_1);
  Digit2.InitGraphicMode(NIXI_TYPE);
  Digit2.GraphicMode();

  //TFT backligth setup
  ledcSetup(TFT_BL_CHANNEL, TFT_BL_FREQ, TFT_BL_RERSOLUTION);
  ledcAttachPin(TFT_BL_PIN, TFT_BL_CHANNEL);
  ledcWrite(TFT_BL_CHANNEL, TFT_BL_INITIAL_LEVEL); // set the brightness of the LED
}

//====================================================================================
//                                    The Loop
//====================================================================================
void loop()
{
  blSetBrightness();
  Digit0.Refresh();
  Digit1.Refresh();
  Digit2.Refresh();

  delay(200);
 
}
//====================================================================================

