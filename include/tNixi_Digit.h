#ifndef _TNIXI_DIGIT_H_
    #define _TNIXI_DIGIT_H_

    #include <Arduino.h>
    #include <TFT_eSPI.h>
    #include <string>

#define LED_ORIENTATION 2 // 0 & 2 Portrait. 1 & 3 landscape

#define TNIXI_TIME_FORMAT_24  0 //24 hour time display
#define TNIXI_TIME_FORMAT_12  1 //12 hour time display

#define TNIXI_DATE_FORMAT_US    0 //US date format MM/DD/YYYY
#define TNIXI_DATE_FORMAT_WORLD 1 //I guess, the rest of the world uses DD/MM/YYYY

//**** define digit modes 
#define TNIXI_MODE_BOOT         0

//Time modes
#define TNIXI_MODE_HOUR_10      1
#define TNIXI_MODE_HOUR_1       2
#define TNIXI_MODE_MINUTE_10    3
#define TNIXI_MODE_MINUTE_1     4
#define TNIXI_MODE_SECOND_10    5
#define TNIXI_MODE_SECOND_1     6

//Date modes 
#define TNIXI_MODE_DATE_MM_DD       10  //show "12/30" or "30.12" 
#define TNIXI_MODE_DATE_MMMM_DD     11  //show "Dec 30"
#define TNIXI_MODE_DATE_MMMM_DDDD   12  //show "Mon Dec 30"

//Weather modes
#define TNIXI_MODE_WEATHER_NOW      20
#define TNIXI_MODE_WEATHER_TODAY    21
#define TNIXI_MODE_WEATHER_3DAY     22

//Any other modes
#define TNIXI_MODE_STOCK            30
#define TNIXI_MODE_CRYPTO_CURRENCY  31


//**** Display modes, using bitmap or fonts
#define TNIXI_DISPLAY_MODE_GRAPHIC  0   //use jpg file to show display information
#define TNIXI_DISPLAY_MODE_TEXT     1   //use fonts to show dispaly information
#define TNIXI_DEFAULT_COLOR         0xFFB901 //picked this color from a Nixi tube picture


#define MIC_SIZE    4   //market identifier code (MIC) length, defined as 4 characters
#define CCS_SIZE    4   //Crupto Currency Symbol Size

#define IS_DIGIT_INITIALIZED    if(!DigitIsIntitialized) return false;

//Structure contains all variabls and settings that are common across all digit TFTs
struct tNixi_Clock_Config
{
    //TFT configuration
    int *ActiveTFT;
    TFT_eSPI *TFT;

    //
    int TimeFormat = TNIXI_TIME_FORMAT_24;
    int DateFormat = TNIXI_DATE_FORMAT_US;
    String WiFiSSID ="";    // some screen will show the current SSID
    time_t CurrentTime;     // snapshot of the time that wil be displayed. This needs to be refreshed for each new time that is displayed.
};

union tNixi_Digit_Data
{
    int TimeDigit;  //any of the time and date digits 

    char Stock[MIC_SIZE + 1];   //Stock symbol 
    char CryptoCurrency[CCS_SIZE+ 1]; //Crypro Currency Symbol
};

class tNixi_Digit
{
    public:
        tNixi_Digit();
        
        bool Init(int xThisDigitCS, tNixi_Clock_Config *xNixiClockConfig, int xDigitMode);
        bool InitGraphicMode(String xNixiPictureFileName);
        bool InitTextMode(uint8_t xTextFont, uint32_t xTextColor = TNIXI_DEFAULT_COLOR);
        bool TextMode(void);
        bool GraphicMode(void);
        bool Refresh(bool xForce=false); //Refresh the display, if xForece set to true the display will be refreshed even if there is no change in the DigitData 
        bool SetDigitMode(int xDigitMode = TNIXI_MODE_BOOT);


    private:
        bool DigitIsIntitialized = false;
        int DigitCS;    //CS pin for this digit, configured via Init procedure 
        int DigitMode = TNIXI_MODE_BOOT;
        tNixi_Clock_Config *NixiClockConfig;
        
        tNixi_Digit_Data DigitData; //holds the current value that is displayed to reduce unnecessary redrawing 
        bool DoDisplayRefresh = false;

        String NixiPictureFileName = ""; //base name of the Nixi pictuer files e.g. to show the number "0", file name is "NixiA_0.jpg". The base name will be "/NixiA_"
        uint8_t TextFont = 0; //check if 0 is the right font number 
        uint32_t TextColor = TNIXI_DEFAULT_COLOR;
        int DisplayMode = TNIXI_DISPLAY_MODE_TEXT;
        


        void ShowScreen_Boot();
        void ShowScreen_Date_MM_DD();
        void ShowScreen_Date_MMMM_DD();
        void ShowScreen_Date_MMMM_DDDD();

        void ShowScreen_Weather_Now();

        void DrawNumber(int Number);
        void DrawNumberPicture(int Number);
        void DrawNumberText(int Number);
        int GetTimeDigit(int TimeDigit);    //Retunrs the selceted digit of the current time
};


#endif
