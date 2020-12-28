#ifndef _SPIFFS_FUNCTIONS_H_
    #define _SPIFFS_FUNCTIONS_H_

    #include <FS.h>


    #ifdef ESP8266
        void listFiles(void);
    #endif

    //====================================================================================

    #ifdef ESP32
        #include "SPIFFS.h" // ESP32 only
        void listFiles(void);

        //void listDir(fs &fs::FS, const char * dirname, uint8_t levels);
        void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
    #endif
#endif