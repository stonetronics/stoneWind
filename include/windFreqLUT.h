#ifndef __windFreqLUT_h__
#define __windFreqLUT_h__

#include <Arduino.h>

#define WINDFREQLUT_SIZE 15

/*
The interpolation points shall be ordered from
0 - 0 wind, 0 frequency
WINDFREQLUT_SIZE - highest measured wind, highest measured frequency
*/

void lutSetInterpolationPoint(uint8_t point, float windSpeed, float frequency);
float lutGetWindSpeed( float frequency );
void lutLoadDefault( void );

#endif
