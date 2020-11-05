#include <Arduino.h>
#include "pindef.h"
#include "sevenSegment.h"
#include "freqCount.h"

void setup() {
  Serial.begin(9600); //init serial
  Serial.println("stoneWind Anemomenter");
  sevenSegmentInit(); //setup portpins, timer, etc
  sevenSegmentEnable();
  freqCountInit();
  freqCountSetTimebase(2000);
  sei(); //enable interrupts
}

void loop() {
  static uint32_t millisBefore = 0;
  //read time and measure frequency
  uint32_t millis = freqCountGetTotalMillis();
  float measuredFrequency = freqCountGetFreq();

  //display the measured frequency
  sevenSegmentDisplayNumberWithDp(measuredFrequency);
  Serial.print("The measured frequency is: ");
  Serial.println(measuredFrequency);

  //save the time now for the next iteration
  millisBefore = millis;
}
