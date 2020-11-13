#include <Arduino.h>
#include "sevenSegment.h"
#include "pindef.h"


volatile static int segmentPins[8] = {PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_DP};
volatile static int digitPins[3] = {PIN_DIG1, PIN_DIG2, PIN_DIG3};

volatile static uint8_t digitPatterns[18] = {  0x3F, //0
                                      0x06, //1
                                      0x5B, //2
                                      0x4F, //3
                                      0x66, //4
                                      0x6D, //5
                                      0x7D, //6
                                      0x07, //7
                                      0x7F, //8
                                      0x6F, //9
                                      0x77, //A
                                      0x7C, //b
                                      0x39, //C
                                      0x5E, //d
                                      0x79, //E
                                      0x71, //F
                                      0x00, //blank
                                      0x40}; //-

volatile int currentDigit = 0;
volatile uint8_t digitsToDisplay[3] = {16, 16, 16};
volatile uint8_t digitWithDp = 0;

void writeDigitPattern(uint8_t digit) {
  if (digit < 18){
    for (uint8_t i = 0; i < 7; i++) {
      //write the pattern for all the pins on the pins
      digitalWrite(segmentPins[i], !(((digitPatterns[digit])>>i) & 0x01));
    }
  }
}

void sevenSegmentInit( void )
{
  //initialize portpins
  uint8_t i;
  for(i = 0; i < 8; i++){
    digitalWrite(segmentPins[i], HIGH);
    pinMode(segmentPins[i], OUTPUT);
  }
  for(i = 0; i < 3; i++){
    digitalWrite(digitPins[i], HIGH);
    pinMode(digitPins[i], OUTPUT);
  }
  //set up timer
  TCCR2A = 0x00;
}

void sevenSegmentEnable( void ) //enable display
{
  //set the first digit
  currentDigit = 0;
  //enable overflow interrupt
  TIFR2 = (1<<TOV2); //clear the interrupt flag first
  TIMSK2 |= (1<<TOIE2);
  //turn on the timer
  TCCR2B |= (5<<CS20);
}

void sevenSegmentDisable( void ) //disable display
{
  uint8_t i;

  //stop timer
  TCCR2B &= ~(5<<CS20);
  //disable timer interrupt
  TIMSK2 &= ~(1<<TOIE2);
  //reset portpins
  for(i = 0; i < 8; i++){
    digitalWrite(segmentPins[i], HIGH);
  }
  for(i = 0; i < 3; i++){
    digitalWrite(digitPins[i], HIGH);
  }
}

//calculate the number to display
void sevenSegmentDisplayNumber(int16_t number, uint8_t leadingZeros)
{
  if ((number > -100) && (number <= -10)) {
    digitsToDisplay[0] = 17; //-
    digitsToDisplay[1] = abs(number) / 10;
    digitsToDisplay[2] = abs(number) % 10;
  } else if ((number > -10) && (number <= -1)) {
    if (leadingZeros) {
      digitsToDisplay[0] = 17; //-
      digitsToDisplay[1] = 0; //zero
    } else {
      digitsToDisplay[0] = 16; //blank
      digitsToDisplay[1] = 17; //-
    }
    digitsToDisplay[2] = abs(number) % 10;
  } else if ((number >= 0) && (number < 10)) {
    if (leadingZeros) {
      digitsToDisplay[0] = 0; //zero
      digitsToDisplay[1] = 0; //zero
    } else {
      digitsToDisplay[0] = 16; //blank
      digitsToDisplay[1] = 16; //blank
    }
    digitsToDisplay[2] = number % 10;
  } else if ((number >= 10) && (number < 100)) {
    if (leadingZeros)
      digitsToDisplay[0] = 0; //zero
    else
      digitsToDisplay[0] = 16; //blank
    digitsToDisplay[1] = number / 10;
    digitsToDisplay[2] = number % 10;
  } else if ((number >= 100) && (number < 1000)) {
    digitsToDisplay[0] = number / 100;
    digitsToDisplay[1] = (number / 10) % 10;
    digitsToDisplay[2] = number % 10;
  } else {
    digitsToDisplay[0] = 17; //-
    digitsToDisplay[1] = 17; //-
    digitsToDisplay[2] = 17; //-
  }
}

void sevenSegmentSetDp(uint8_t digit)
{
  digitWithDp = digit;
}

void sevenSegmentDisplayNumberWithDp(float number)
{
  if ((number > -100.0) && (number <= -10) ) {
    sevenSegmentDisplayNumber((int16_t)number, 1);
    sevenSegmentSetDp(2);
  } else if ((number > -10.0) && (number < 0.0) ) {
    sevenSegmentDisplayNumber((int16_t) (number * 10.0), 1);
    sevenSegmentSetDp(1);
  } else if (number == 0.0)  {
    sevenSegmentDisplayNumber((int16_t) (0), 0);
    sevenSegmentSetDp(-1);
  } else if ((number > 0.0) && (number < 10.0))  {
    sevenSegmentDisplayNumber((int16_t) (number * 100.0), 1);
    sevenSegmentSetDp(0);
  } else if ((number >= 10.0) && (number < 100.0))  {
    sevenSegmentDisplayNumber((int16_t) (number * 10.0), 1);
    sevenSegmentSetDp(1);
  } else if ((number >= 100.0) && (number < 1000.0))  {
    sevenSegmentDisplayNumber((int16_t) (number * 1.0), 1);
    sevenSegmentSetDp(-1);
  } else {
    sevenSegmentDisplayNumber(1000, 1); //display invalid number
    sevenSegmentSetDp(-1); //no Dp
  }
}

void sevenSegmentClear( void )
{
  digitsToDisplay[0] = 16;
  digitsToDisplay[1] = 16;
  digitsToDisplay[2] = 16;
  //set dp to an invalid position
  sevenSegmentSetDp(-1);
}


ISR(TIMER2_OVF_vect)
{
  digitalWrite(digitPins[currentDigit], HIGH);
  currentDigit++;
  if (currentDigit == 3) {
    currentDigit = 0;
  }
  writeDigitPattern(digitsToDisplay[currentDigit]);
  digitalWrite(PIN_DP, !(digitWithDp == currentDigit));
  digitalWrite(digitPins[currentDigit], LOW);
}
