#ifndef _TNIXI_DIGIT_H_
    #define _TNIXI_DIGIT_H_

    #include <Arduino.h>
    #include "tNixi.h"
    #include <Adafruit_GFX.h>
    #include <string>


//**** define digit modes 

//Time modes
#define TNIXI_MODE_HOUR_10      1
#define TNIXI_MODE_HOUR_1       2
#define TNIXI_MODE_MINUTE_10    3
#define TNIXI_MODE_MINUTE_1     4
#define TNIXI_MODE_SECOND_10    5
#define TNIXI_MODE_SECOND_1     6

//**** Display modes, using bitmap or fonts
#define TNIXI_DISPLAY_MODE_GRAPHIC  0   //use jpg file to show display information
#define TNIXI_DISPLAY_MODE_TEXT     1   //use fonts to show dispaly information
#define TNIXI_DEFAULT_COLOR         0xFFB901 //picked this color from a Nixi tube picture

class tNixi_Digit; 

//The calss definds a tube posiution. There will be one instance for each tube
class tNixi_Tube
{
    public:
        tNixi_Tube();
        void Init(int xThisDigitCS);
        void SetDigit(tNixi_Digit *Digit);
        void Refresh();

    private:
        tNixi_Digit *currentDigit;
        int ChipSelect;

};

class tNixi_Digit
{
    public:
        tNixi_Digit();

        virtual bool Refresh(bool xForce=false); //Refresh the display, if xForece set to true the display will be refreshed even if there is no change in the DigitData 

};

class tNixi_Digit_Time : public tNixi_Digit
{
    public:
        bool InitGraphicMode(String xNixiPictureFileName);
        bool InitTextMode(uint8_t xTextFont, uint32_t xTextColor = TNIXI_DEFAULT_COLOR);
        bool TextMode(void);
        bool GraphicMode(void);
    
    protected:
        int GetTimeDigit(int TimeDigit);    //Retunrs the selceted digit of the current time
        
        int CurrentDigitNumber = -1; //holds the current value that is displayed to reduce unnecessary redrawing 

        String NixiPictureFileName = ""; //base name of the Nixi pictuer files e.g. to show the number "0", file name is "NixiA_0.jpg". The base name will be "/NixiA_"
        uint8_t TextFont = 0; //check if 0 is the right font number 
        uint32_t TextColor = TNIXI_DEFAULT_COLOR;
        int DisplayMode = TNIXI_DISPLAY_MODE_TEXT;
        
        void DrawNumber(int Number);
        void DrawNumberPicture(int Number);
        void DrawNumberText(int Number);
};

class tNixi_Second_1 : public tNixi_Digit_Time
{
    bool Refresh(bool xForce=false); 
};

class tNixi_Second_10 : public tNixi_Digit_Time
{
    bool Refresh(bool xForce=false); 
};

class tNixi_Minute_1 : public tNixi_Digit_Time
{
    bool Refresh(bool xForce=false); 
};

class tNixi_Minute_10 : public tNixi_Digit_Time
{
    bool Refresh(bool xForce=false); 
};

class tNixi_Hour_1 : public tNixi_Digit_Time
{
    bool Refresh(bool xForce=false); 
};

class tNixi_Hour_10 : public tNixi_Digit_Time
{
    bool Refresh(bool xForce=false); 
};


class tNixi_Digit_BootScreen : public tNixi_Digit
{
    public:
        bool Refresh(bool xForce=false); 
    private:

};

#endif
