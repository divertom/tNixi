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

void tNixi_Tube::Init(int xThisDigitCS)
{
    ChipSelect = xThisDigitCS;
    
    *(gClockConfig.ActiveTFT) = ChipSelect;  //set this digit as the active TFT
     gClockConfig.TFT->setRotation(LED_ORIENTATION); // assuming that all TFTs have the same orientation
     gClockConfig.TFT->fillScreen(TFT_BLACK);
}

void tNixi_Tube::SetDigit(tNixi_Digit *Digit)
{
    currentDigit = Digit;
}

void tNixi_Tube::Refresh()
{
    *(gClockConfig.ActiveTFT) = ChipSelect;  //set this digit as the active TFT
    currentDigit->Refresh();
}


//***************************************************************
//****** Digit functions ****************************************
//***************************************************************

//Initializing digit without defined  mode
tNixi_Digit::tNixi_Digit()
{

}


bool tNixi_Digit_Time::InitGraphicMode(String xNixiPictureFileName)
{
    NixiPictureFileName = xNixiPictureFileName;
    return true;
}

bool tNixi_Digit_Time::InitTextMode(uint8_t xTextFont, uint32_t xTextColor)
{
    TextFont = xTextFont;
    TextColor = xTextColor;
    return true;
}

bool tNixi_Digit_Time::TextMode(void)
{
    DisplayMode = TNIXI_DISPLAY_MODE_TEXT;
    return true;
}

bool tNixi_Digit_Time::GraphicMode(void)
{
    if(NixiPictureFileName.isEmpty()) return false;
    DisplayMode = TNIXI_DISPLAY_MODE_GRAPHIC;
    return true;
}

//Refresh the display depdenign on the current display configuration 
bool tNixi_Digit::Refresh(bool xForce)
{
    return true;
}



//*****************************************************************
//   Supporting function for single number per digit output
//*****************************************************************

//Display the single number on the screen depeding on the current display mode
void tNixi_Digit_Time::DrawNumber(int Number)
{
    if (DisplayMode == TNIXI_DISPLAY_MODE_GRAPHIC) 
    {
        DrawNumberPicture(Number);
        return;
    } 
    if (DisplayMode == TNIXI_DISPLAY_MODE_TEXT) 
    {
        DrawNumberText(Number);
        return;
    }

    // some undefined display mode. Maybe add an error printout here 
    return;
}

//Draw the digit number as bitmap. Used for single diget on display.
void tNixi_Digit_Time::DrawNumberPicture(int Number)
{
    //check if the number is different then before. Drawing stuff takes a lot of time so we try to reduce this
    if (CurrentDigitNumber != Number)
    {
        //new number to display
        String FileName = NixiPictureFileName + Number + ".jpg";
        drawJpeg(FileName.c_str(), 0 , 0); 

       CurrentDigitNumber = Number;   //set new number that was just drawn
    }
}

//Draw the digit number as Text. Used for single diget on display.
void tNixi_Digit_Time::DrawNumberText(int Number)
{
     gClockConfig.TFT->setTextFont(TextFont);
     gClockConfig.TFT->setCursor(0, 0);
     gClockConfig.TFT->setTextDatum(CC_DATUM); //place number in the middle of the screen
     gClockConfig.TFT->setTextColor(TFT_WHITE);  
     gClockConfig.TFT->setTextSize(5);
     gClockConfig.TFT->println(Number);
}

//***********************************************************************************
// Time functions
//***********************************************************************************
#define START_YEAR 1970

// Returns the specified digit in the of the current time wiht the Seconds lowes digit being index 0
// Digit Index:  14... .. .. . .. .. .0
// Time:          YYYY MM DD W HH MM SS 
int tNixi_Digit_Time::GetTimeDigit(int TimeIndex)
{
    TimeElements timeElements;
    breakTime( gClockConfig.CurrentTime,timeElements);
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

bool tNixi_Second_1::Refresh(bool xForce)  
{
    DrawNumber(GetTimeDigit(0));
    return true;
}

bool tNixi_Second_10::Refresh(bool xForce)  
{
    DrawNumber(GetTimeDigit(1));
    return true;
}

bool tNixi_Minute_1::Refresh(bool xForce)  
{
    DrawNumber(GetTimeDigit(2));
    return true;
}

bool tNixi_Minute_10::Refresh(bool xForce) 
{
    DrawNumber(GetTimeDigit(3));
    return true;
}

bool tNixi_Hour_1::Refresh(bool xForce) 
{
    DrawNumber(GetTimeDigit(4));
    return true;
}

bool tNixi_Hour_10::Refresh(bool xForce)
{
    DrawNumber(GetTimeDigit(5));
    return true;
}


//***********************************************************************************
// Boot Screen Class
//***********************************************************************************
bool tNixi_Digit_BootScreen::Refresh(bool xForce)
{
     gClockConfig.TFT->setCursor(0, 0);
     gClockConfig.TFT->setTextColor(TFT_WHITE, TFT_BLACK);    
     gClockConfig.TFT->setTextDatum(TR_DATUM);
     gClockConfig.TFT->setTextSize(2);
    int padding =  gClockConfig.TFT->textWidth("                    ", 2); // get the width of the text in pixels;
     gClockConfig.TFT->setTextPadding(padding);

    //show device name and FW version
         gClockConfig.TFT->println(DEVICE_NAME);
         gClockConfig.TFT->println(FW_VERSION);

    //show network configuration
         gClockConfig.TFT->print("SSID: ");  gClockConfig.TFT->println( gClockConfig.WiFiSSID);
         gClockConfig.TFT->print("IP: "); gClockConfig.TFT->println( gClockConfig.IPAddress);

    //show system time, time zone and date
        if ( gClockConfig.RTCPowerStatus)  gClockConfig.TFT->println("RTC lost power");
        
        char TimeDateStr[20];
        sprintf(TimeDateStr, "%s %02d:%02d:%02d",   "RTC",
                                                    hour(), 
                                                    minute(), 
                                                    second());
         gClockConfig.TFT->println(TimeDateStr);    
        
        
        DateTime CurrentTime =  gClockConfig.CurrentTime;

        sprintf(TimeDateStr, "%s %02d:%02d:%02d %s",   "Local",
                                                    CurrentTime.hour(), 
                                                    CurrentTime.minute(), 
                                                    CurrentTime.second(), 
                                                     gClockConfig.TimeZone.c_str());
         gClockConfig.TFT->println(TimeDateStr);
        
        sprintf(TimeDateStr, "%02d/%02d/%02d",   
                                            CurrentTime.day(), 
                                            CurrentTime.month(), 
                                            CurrentTime.year());
         gClockConfig.TFT->println(TimeDateStr);
        
        return true;
}