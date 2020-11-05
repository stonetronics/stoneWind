#ifndef __sevenSegment_h__
#define __sevenSegment_h__

#include <Arduino.h>

void sevenSegmentInit( void ); //setup portpins, timer, etc
void sevenSegmentEnable( void ); //enable display
void sevenSegmentDisable( void ); //disable display
void sevenSegmentDisplayNumber(int16_t number, uint8_t leadingZeros); //set the number to display
void sevenSegmentSetDp(uint8_t digit); //set the digit where a decimal point shall be displayed //-1 = none, 0 == x.xx, 2 == xxx.
void sevenSegmentDisplayNumberWithDp(float number); //display a number with a decimal point

#endif
