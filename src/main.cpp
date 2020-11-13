#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "pindef.h"
#include "sevenSegment.h"
#include "freqCount.h"
#include "lookupTable.h"

LookupTable windFreqLUT;
LookupTable tempAdcLUT;

void setup() {
  Serial.begin(9600); //init serial
  Serial.println("stoneWind Anemomenter");

  //sd card init

  Serial.println("initializing SD card");
  if (!SD.begin(PIN_CS)) {
    Serial.println("initialization failed!");
    Serial.println("please insert sd card an reset");
    while (1);
  }
  Serial.println("initialization done.");

  //load lookup tables

  Serial.println("loading wind/frequency lookup table from /LUT/windFreq.csv");
  windFreqLUT.begin(12);
  if (windFreqLUT.loadFromFile("LUT/windFreq.csv") )
  {
    Serial.println("loading successful!");
    Serial.println("lookup table values:");
    windFreqLUT.printTable();
  } else {
    Serial.println("couldn't load wind/freq LUT!");
    while(1);
  }

  Serial.println("loading temperature/ADC lookup table from /LUT/tempAdc.csv");
  tempAdcLUT.begin(12);
  if (tempAdcLUT.loadFromFile("LUT/tempAdc.csv") )
  {
    Serial.println("loading successful!");
    Serial.println("lookup table values:");
    tempAdcLUT.printTable();
  } else {
    Serial.println("couldn't load temp/adc LUT!");
    while(1);
  }

  //initialize pushbutton

  pinMode(PIN_DISPLAY, INPUT);

  //initialize 7 segment and freq counter

  sevenSegmentInit(); //setup portpins, timer, etc
  sevenSegmentEnable();
  freqCountInit();
  freqCountSetTimebase(2000);
  sei(); //enable interrupts
}

void loop() {
  static uint32_t millisBefore = 0;
  static uint32_t millisBtnPush = 0;
  static uint8_t displayMode = 0; //0 = no display, 1 = wind, 2 = temperature
  //read time and measure frequency and temperature adc
  uint32_t millisNow = freqCountGetTotalMillis();
  float measuredFrequency = freqCountGetFreq();
  uint16_t measuredADC = analogRead(PIN_TEMPERATURE);

  //calculate the wind
  float calculatedWind = windFreqLUT.getYfromX(measuredFrequency);
  float calculatedTemp = tempAdcLUT.getYfromX(measuredADC);

  //read button for display mode
  static uint8_t btnOld = 0;
  uint8_t btn = digitalRead(PIN_DISPLAY);

  if (btnOld != btn)
  {
    btnOld = btn;
    if (!btn) //button onPush action
    {
      millisBtnPush = millisNow;
      //switch through display modes
      displayMode++;
      if (displayMode == 3)
        displayMode = 0;
    }
  }

  //display the measured value depending on the display mode
  switch (displayMode)
  {
      case 0:
        sevenSegmentClear();
        break;
      case 1:
        //display the measured wind speed
        sevenSegmentDisplayNumberWithDp(calculatedWind);
        break;
      case 2:
        //display the measured temperature
        sevenSegmentDisplayNumberWithDp(calculatedTemp);
        break;
      default:
        break;
  }

  if (millisNow - millisBtnPush > 35000)  //reset the display after a while
  {
    //reset display mode
    displayMode = 0;
  }

  if (millisNow - millisBefore > 5000) //every 5s
  {
    //print
    Serial.print(millisNow);
    Serial.print(": ");
    Serial.print("Wind: ");
    Serial.print(calculatedWind);
    Serial.print(" km/h | Temperature: ");
    Serial.print(calculatedTemp);
    Serial.println(" deg C");

    //open csv and write to it
    File logFile = SD.open("log.csv", FILE_WRITE);
    if (logFile)
    {
      logFile.print(millisNow);
      logFile.print(",");
      logFile.print(calculatedWind);
      logFile.print(",");
      logFile.println(calculatedTemp);
      logFile.close();
    }
    else
    {
      Serial.println("Couldn't open log file 'log.csv'");
    }


    //save the time now for the next iteration
    millisBefore = millisNow;
  }

}
