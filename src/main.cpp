#include <Arduino.h>
#include "pindef.h"
#include "sevenSegment.h"
#include "freqCount.h"
#include "windFreqLUT.h"

void setup() {
  Serial.begin(9600); //init serial
  Serial.println("stoneWind Anemomenter");
  lutLoadDefault(); //load default lookup table for testing
  sevenSegmentInit(); //setup portpins, timer, etc
  sevenSegmentEnable();
  freqCountInit();
  freqCountSetTimebase(2000);
  sei(); //enable interrupts
}

void loop() {
  static uint32_t millisBefore = 0;
  //read time and measure frequency
  uint32_t millisNow = freqCountGetTotalMillis();
  float measuredFrequency = freqCountGetFreq();

  //calculate the wind
  float calculatedWind = lutGetWindSpeed(measuredFrequency);

  //display the measured frequency
  sevenSegmentDisplayNumberWithDp(calculatedWind);

  if (millisNow - millisBefore > 500) //every 500 ms
  {
    Serial.print(millisNow);
    Serial.print(": ");
    Serial.print("The measured frequency is: ");
    Serial.print(measuredFrequency);
    Serial.print(" This equals a wind of: ");
    Serial.println(calculatedWind);

    //save the time now for the next iteration
    millisBefore = millisNow;
  }

}
