#ifndef _JPEG_FUNCTIONS_H_
    #define _JPEG_FUNCTIONS_H_

    #include <Arduino.h>
    #include <TFT_eSPI.h>

    extern TFT_eSPI tft;

    // Return the minimum of two values a and b
    #define minimum(a,b)     (((a) < (b)) ? (a) : (b))

    //====================================================================================
    //   Opens the image file and prime the Jpeg decoder
    //====================================================================================
    void drawJpeg(const char *filename, int xpos, int ypos);

    //====================================================================================
    //   Decode and render the Jpeg image onto the TFT screen
    //====================================================================================
    void jpegRender(int xpos, int ypos);

    //====================================================================================
    //   Print information decoded from the Jpeg image
    //====================================================================================
    void jpegInfo() ;

    //====================================================================================
    //   Open a Jpeg file and send it to the Serial port in a C array compatible format
    //====================================================================================
    void createArray(const char *filename);

#endif