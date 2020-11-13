#ifndef __lookupTable_h__
#define __lookupTable_h__

#include <Arduino.h>

/*
This class provides a lookup table abstraction
The lookup table can be stored as a csv, with the first column y values, second column x values
The interpolation points shall be ordered from
0 - 0 x, 0 y
length - highest measured x, highest measured y
*/

class LookupTable {
  public:

    void begin(uint8_t length);
    void close( void );
    void setInterpolationPoint(uint8_t index, float y, float x);
    float getYfromX( float x );
    void loadDefault( void );
    uint8_t loadFromFile(const char* filePath); //SD card must be initialized to use this; returns 1 on success, 0 on fail
    void printTable( void );

  private:
    float** lut;
    uint8_t lutLength;
};



#endif
