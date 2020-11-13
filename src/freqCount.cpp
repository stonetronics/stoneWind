#include "freqCount.h"
#include "pindef.h"

volatile uint16_t milliseconds = 0;
volatile uint16_t count = 0;
volatile uint16_t countMilliseconds = 100;
volatile uint32_t totalMillis = 0;

void freqCountInit( void )
{
  //have the counter pin as input
  pinMode(PIN_CNT, INPUT);

  //set up the counter
  TCCR1A = 0x00;
  //reset counter
  TCNT1 = 0;
  //start counting on falling edge
  TCCR1B |= (5<<CS10);

  //set up the timer
  TCCR0A |= (2<<WGM00); //ctc mode
  TCCR0B |= (3<<CS00); //prescaler 64, 256 Ticks = 1.024ms
  OCR0A = 250; //1.024ms * 250/256 = 1ms
  TIMSK0 |= (1<<OCIE0A); //make the overflow interrupt available
}

void freqCountSetTimebase( uint16_t timebase)
{
    countMilliseconds = timebase;
}

float freqCountGetFreq( void )
{
  return 1000.0 * ((float) count) / ((float)countMilliseconds);
}

uint32_t freqCountGetTotalMillis( void )
{
  return totalMillis;
}

ISR(TIMER0_COMPA_vect) {
  if (totalMillis == 4294967295) //biggest 32 bit integer
    totalMillis = 0;
  else
    totalMillis ++;
  milliseconds ++;
  if (milliseconds == countMilliseconds) {
    milliseconds = 0;
    count = TCNT1;
    TCNT1 = 0;
  }
}
