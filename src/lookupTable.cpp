#include "lookupTable.h"
#include <SPI.h>
#include <SD.h>

void LookupTable::begin(uint8_t length)
{
  float* ptr;
  this->lutLength = length;
  //init array of pointers and the cells where the data lies in
  this->lut = (float**)malloc(sizeof(float *) * length + sizeof(float) * 2 * length);
  ptr = (float*)(this->lut+length);
  //point rows pointer to the appropriate location
  for (uint8_t i = 0; i < length; i++)
  {
    this->lut[i] = (ptr + 2*i);
  }
}

void LookupTable::close( void )
{
  if(this->lut)
    free(this->lut);
}

void LookupTable::setInterpolationPoint(uint8_t index, float y, float x)
{
  if (index < this->lutLength)
  {
    this->lut[index][0] = y;
    this->lut[index][1] = x;
  }
}

void LookupTable::loadDefault( void )
{
  for (uint8_t i = 0; i < this->lutLength; i ++)
  { //build an arbitrary function into the lookup table
    //lut[i][0] = i * i * 2.5;
    //lut[i][1] = i * i * 0.9 + i * 5;
    //this->lut[i][0] = 3.5*i;
    //this->lut[i][1] = 4.32*i;
    this->lut[i][0] = i * 10;
    this->lut[i][1] = i * 10;
  }
  return;
}

float LookupTable::getYfromX( float x )
{
  uint8_t lowerIndex;
  float lowerX;
  float upperX;
  float lowerY;
  float upperY;
  float interpolatedY;

  //determine the interval where the x is in
  lowerIndex = 0;
  while ( !((lut[lowerIndex][1] < x) && (x <= lut[lowerIndex+1][1])))
  {
    lowerIndex++;
    if (lowerIndex == (this->lutLength-1))
    {
      Serial.println("reached end of LUT!");
      break;
    }
  }

  //read from the lut
  lowerX = lut[lowerIndex][1];
  lowerY = lut[lowerIndex][0];
  upperX = lut[lowerIndex+1][1];
  upperY = lut[lowerIndex+1][0];

  //interpolate
  interpolatedY = lowerY + (x-lowerX) * (upperY - lowerY) / (upperX-lowerX);

  return interpolatedY;
}

void LookupTable::printTable( void )
{
  uint8_t i;
  for (i = 0; i < this->lutLength; i++)
  {
    Serial.print("Entry ");
    Serial.print(i);
    Serial.print(": Y: ");
    Serial.print(this->lut[i][0]);
    Serial.print("| X: ");
    Serial.println(this->lut[i][1]);
  }
}

uint8_t LookupTable::loadFromFile(const char* filePath)
{
  File theFile;
  uint8_t row;

  // open the file for reading:
  theFile = SD.open(filePath);
  if (theFile) {
    // read from the file until there's nothing else in it:
    for (row = 0; row < this->lutLength; row++)
    {
      if (theFile.available()) {
        this->lut[row][0] = theFile.parseFloat();
        this->lut[row][1] = theFile.parseFloat();
      } else {
        Serial.print("invalid file format! there must be exactly");
        Serial.print(this->lutLength);
        Serial.println(" rows and 2 columns in the csv! (Y|X)");
        theFile.close();
        return 0;
      }
    }

    // close the file:
    theFile.close();
  } else {
    Serial.print("file: '");
    Serial.print(filePath);
    Serial.println("' couldn't be opened");
    return 0; //file coulddnt be opened -> error
  }
  return 1; //success
}
