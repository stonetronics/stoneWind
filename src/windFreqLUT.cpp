#include "windFreqLUT.h"

volatile float lut[WINDFREQLUT_SIZE][2]; //lines are the points, column 0: wind, column 1: frequency

void lutSetInterpolationPoint(uint8_t point, float windSpeed, float frequency)
{
  lut[point][0] = windSpeed;
  lut[point][1] = frequency;
}

void lutLoadDefault( void )
{
  for (uint8_t i = 0; i < WINDFREQLUT_SIZE; i ++)
  { //build an arbitrary function into the lookup table
    //lut[i][0] = i * i * 2.5;
    //lut[i][1] = i * i * 0.9 + i * 5;
    lut[i][0] = 3.5*i;
    lut[i][1] = 4.32*i;
  }
  return;
}

float lutGetWindSpeed( float frequency )
{
  uint8_t lowerIndex;
  float lowerFreq;
  float upperFreq;
  float lowerWind;
  float upperWind;
  float interpolatedWind;

  //determine the interval where the frequency is in
  lowerIndex = 0;
  while ((lut[lowerIndex][1] < frequency) && (frequency <= lut[lowerIndex+1][1]) && lowerIndex < (WINDFREQLUT_SIZE-1) )
    lowerIndex++;

  //read from the lut
  lowerFreq = lut[lowerIndex][1];
  lowerWind = lut[lowerIndex][0];
  upperFreq = lut[lowerIndex+1][1];
  upperWind = lut[lowerIndex+1][0];

  //interpolate
  interpolatedWind = lowerWind + (frequency-lowerFreq) * (upperWind - lowerWind) / (upperFreq-lowerFreq);

  return interpolatedWind;
}
