#include "tNixi_Digit.h"

#include <Arduino.h>
#include <RTClib.h>
#include <TFT_eSPI.h>
#include <string>
#include <JPEGDecoder.h>
#include "JPEG_functions.h"


//***************************************************************
//****** Tube functions *****************************************
//***************************************************************

tNixi_Tube::tNixi_Tube()
{

}

void tNixi_Tube::Init(int xThisDigitCS, tNixi_Clock_Config *xNixiClockConfig)
{
    ChipSelect = xThisDigitCS;
    ClockConfig = xNixiClockConfig;

    *(ClockConfig->ActiveTFT) = ChipSelect;  //set this digit as the active TFT
    ClockConfig->TFT->setRotation(LED_ORIENTATION); // assuming that all TFTs have the same orientation
    ClockConfig->TFT->fillScreen(TFT_BLACK);
}

void tNixi_Tube::SetDigit(tNixi_Digit *Digit)
{
    currentDigit = Digit;
}

void tNixi_Tube::Refresh()
{
    *(ClockConfig->ActiveTFT) = ChipSelect;  //set this digit as the active TFT
    currentDigit->Refresh();
}


//Initializing digit without defined  mode
tNixi_Digit::tNixi_Digit()
{

}

//Initializing digit wiht defiend mode, e.g. hour_10
bool tNixi_Digit::Init(tNixi_Clock_Config *xNixiClockConfig, int xDigitMode)
{
    NixiClockConfig = xNixiClockConfig;
    DigitMode = xDigitMode;     //Set what this digit will show 
    DigitData.TimeDigit = -1;   //just setting some number that will not be show

    //Initialize TFT display --- old code remove 
    //*(NixiClockConfig->ActiveTFT) = DigitCS;  //set this digit as the active TFT
    //NixiClockConfig->TFT->setRotation(LED_ORIENTATION); // assuming that all TFTs have the same orientation
    //NixiClockConfig->TFT->fillScreen(TFT_BLACK);

    DigitIsIntitialized = true;
    
    //Serial.print("TFT Initialized "); Serial.println(*(NixiClockConfig->ActiveTFT));   
    return true;
}

bool tNixi_Digit::InitGraphicMode(String xNixiPictureFileName)
{
    NixiPictureFileName = xNixiPictureFileName;
    return true;
}

bool tNixi_Digit::InitTextMode(uint8_t xTextFont, uint32_t xTextColor)
{
    TextFont = xTextFont;
    TextColor = xTextColor;
    return true;
}

bool tNixi_Digit::TextMode(void)
{
    DisplayMode = TNIXI_DISPLAY_MODE_TEXT;
    return true;
}

bool tNixi_Digit::GraphicMode(void)
{
    if(NixiPictureFileName.isEmpty()) return false;
    DisplayMode = TNIXI_DISPLAY_MODE_GRAPHIC;
    return true;
}

bool tNixi_Digit::SetDigitMode(int xDigitMode)
{
    switch (DigitMode)
    {
        case TNIXI_MODE_BOOT:
        case TNIXI_MODE_HOUR_10: 
        case TNIXI_MODE_HOUR_1: 
        case TNIXI_MODE_MINUTE_10: 
        case TNIXI_MODE_MINUTE_1: 
        case TNIXI_MODE_SECOND_10: 
        case TNIXI_MODE_SECOND_1: 
        case TNIXI_MODE_DATE_MM_DD: 
        case TNIXI_MODE_DATE_MMMM_DD:
        case TNIXI_MODE_DATE_MMMM_DDDD: 
        case TNIXI_MODE_WEATHER_NOW: 
        case TNIXI_MODE_WEATHER_TODAY:
        case TNIXI_MODE_WEATHER_3DAY: 
        case TNIXI_MODE_STOCK:
        case TNIXI_MODE_CRYPTO_CURRENCY:
            DigitMode = xDigitMode;
            DoDisplayRefresh = true;    //refresh the display next time the Refresh function is called
        default:
            return false;   //unsupported mode 
        
    }
    return false;
}

//Refresh the display depdenign on the current display configuration 
bool tNixi_Digit::Refresh(bool xForce)
{
    IS_DIGIT_INITIALIZED
    
    //*(NixiClockConfig->ActiveTFT) = DigitCS;  //set this digit as the active TFT
    //Serial.print("Active Display: "); Serial.println(*(NixiClockConfig->ActiveTFT));
        
    switch (DigitMode)
    {
       
        //time number displays    
        case TNIXI_MODE_HOUR_10: DrawNumber(GetTimeDigit(5)); break;
        case TNIXI_MODE_HOUR_1: DrawNumber(GetTimeDigit(4)); break;
        case TNIXI_MODE_MINUTE_10: DrawNumber(GetTimeDigit(3)); break;
        case TNIXI_MODE_MINUTE_1: DrawNumber(GetTimeDigit(2)); break;
        case TNIXI_MODE_SECOND_10: DrawNumber(GetTimeDigit(1)); break;
        case TNIXI_MODE_SECOND_1: DrawNumber(GetTimeDigit(0)); break;

        //date displays
        case TNIXI_MODE_DATE_MM_DD: ShowScreen_Date_MM_DD(); break;
        case TNIXI_MODE_DATE_MMMM_DD:
        case TNIXI_MODE_DATE_MMMM_DDDD: ShowScreen_Date_MMMM_DDDD(); break;

        //weather displays
        case TNIXI_MODE_WEATHER_NOW: 
        case TNIXI_MODE_WEATHER_TODAY:
        case TNIXI_MODE_WEATHER_3DAY: ShowScreen_Weather_Now(); break;

        //other displays, e.g. stocks, new messages, ...
        case TNIXI_MODE_STOCK:
        case TNIXI_MODE_CRYPTO_CURRENCY:

        
        default:
            ShowScreen_Boot();
        
    }
    return true;
}

//Show digit and clock configuration
void tNixi_Digit::ShowScreen_Boot()
{
    NixiClockConfig->TFT->setCursor(0, 0);
    NixiClockConfig->TFT->setTextColor(TFT_WHITE, TFT_BLACK);
    NixiClockConfig->TFT->setTextDatum(TR_DATUM);
    NixiClockConfig->TFT->setTextSize(2);
    int padding = NixiClockConfig->TFT->textWidth("                    ", 2); // get the width of the text in pixels;
    NixiClockConfig->TFT->setTextPadding(padding);

    //NixiClockConfig->TFT->print("Chip Select: "); NixiClockConfig->TFT->println(DigitCS);
    NixiClockConfig->TFT->print("SSID: "); NixiClockConfig->TFT->println(NixiClockConfig->WiFiSSID);
    NixiClockConfig->TFT->print("IP: ");NixiClockConfig->TFT->println(NixiClockConfig->IPAddress);

    char TimeDateStr[20];
    DateTime CurrentTime = NixiClockConfig->CurrentTime;
    sprintf(TimeDateStr, "%02d:%02d:%02d %02d/%02d/%02d",   CurrentTime.hour(), 
                                                            CurrentTime.minute(), 
                                                            CurrentTime.second(), 
                                                            CurrentTime.day(), 
                                                            CurrentTime.month(), 
                                                            CurrentTime.year());

    NixiClockConfig->TFT->println(TimeDateStr);
}

//Show date in numbers only: "12/30" or "30.12"
void tNixi_Digit::ShowScreen_Date_MM_DD()
{

}

//Show date wiht Month short name: "Dec 30"
void tNixi_Digit::ShowScreen_Date_MMMM_DD()
{

}

//Show date with Month short name and day of week name: " Monday Dec 30"
void tNixi_Digit::ShowScreen_Date_MMMM_DDDD()
{

}


//Show Weather data 
void tNixi_Digit::ShowScreen_Weather_Now()
{

}



//*****************************************************************
//   Supporting function for single number per digit output
//*****************************************************************

//Display the single number on the screen depeding on the current display mode
void tNixi_Digit::DrawNumber(int Number)
{
      
    if (DisplayMode == TNIXI_DISPLAY_MODE_GRAPHIC) 
    {
        //Serial.print("DrawNumberPicture "); Serial.println(Number);
        DrawNumberPicture(Number);
        return;
    } 
    if (DisplayMode == TNIXI_DISPLAY_MODE_TEXT) 
    {
        //Serial.println("DrawNumberText "); Serial.println(Number);
        DrawNumberText(Number);
        return;
    }

    // some undefined display mode. Maybe add an error printout here 
    return;
}

//Draw the digit number as bitmap. Used for single diget on display.
void tNixi_Digit::DrawNumberPicture(int Number)
{
    //check if the number is different then before. Drawing stuff takes a lot of time so we try to reduce this
    if (DigitData.TimeDigit != Number)
    {
        //new number to display
        String FileName = NixiPictureFileName + Number + ".jpg";
        drawJpeg(FileName.c_str(), 0 , 0); 

        DigitData.TimeDigit = Number;   //set new number that was just drawn
    }
}

//Draw the digit number as Text. Used for single diget on display.
void tNixi_Digit::DrawNumberText(int Number)
{
    NixiClockConfig->TFT->setTextFont(TextFont);
    NixiClockConfig->TFT->setCursor(0, 0);
    NixiClockConfig->TFT->setTextDatum(CC_DATUM); //place number in the middle of the screen
    NixiClockConfig->TFT->setTextColor(TFT_WHITE);  
    NixiClockConfig->TFT->setTextSize(5);
    NixiClockConfig->TFT->println(Number);
}

//***********************************************************************************
// Time functions
//***********************************************************************************
#define START_YEAR 1970

// Returns the specified digit in the of the current time wiht the Seconds lowes digit being index 0
// Digit Index:  14... .. .. . .. .. .0
// Time:          YYYY MM DD W HH MM SS 
int tNixi_Digit::GetTimeDigit(int TimeIndex)
{
    TimeElements timeElements;
    breakTime(NixiClockConfig->CurrentTime,timeElements);
    int DigitValue = 0;

    switch (TimeIndex) 
    {
        case 0: return DigitValue = timeElements.Second % 10;
        case 1: return DigitValue = timeElements.Second / 10;
        case 2: return DigitValue = timeElements.Minute % 10;
        case 3: return DigitValue = timeElements.Minute / 10;
        case 4: return DigitValue = timeElements.Hour % 10;
        case 5: return DigitValue = timeElements.Hour / 10;
        case 6: return DigitValue = timeElements.Wday;
        case 7: return DigitValue = timeElements.Day % 10;
        case 8: return DigitValue = timeElements.Day / 10;
        case 9: return DigitValue = timeElements.Month % 10;
        case 10: return DigitValue = timeElements.Minute / 10;
        case 11: return DigitValue = timeElements.Year % 10;
        case 12: return DigitValue = ((timeElements.Year + START_YEAR) / 10) % 10;
        case 13: return DigitValue = ((timeElements.Year + START_YEAR) / 100) % 10;
        case 14: return DigitValue = ((timeElements.Year + START_YEAR) / 1000) % 10;

        default: return DigitValue;

    }  

    return DigitValue;
}