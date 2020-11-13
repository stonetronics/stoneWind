#ifndef __freqCount_h__
#define __freqCount_h__

#include <Arduino.h>

void freqCountInit( void ); //initialize portpins, timer and counter
void freqCountSetTimebase( uint16_t timebase); //set the gate time (ms) for the frequency counting
float freqCountGetFreq( void ); //get the counted frequency
uint32_t freqCountGetTotalMillis( void ) ; //get total elapsed milliseconds

#endif
