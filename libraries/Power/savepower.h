#ifndef savepower_h
#define savepower_h

#include <Arduino.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// watchdog intervals
// sleep bit patterns for WDTCSR
enum 
{
 WDT_16_MS  =  0b000000,
 WDT_32_MS  =  0b000001,
 WDT_64_MS  =  0b000010,
 WDT_128_MS =  0b000011,
 WDT_256_MS =  0b000100,
 WDT_512_MS =  0b000101,
 WDT_1_SEC  =  0b000110,
 WDT_2_SEC  =  0b000111,
 WDT_4_SEC  =  0b100000,
 WDT_8_SEC  =  0b100001,
};  // end of WDT intervals enum

void setSleepInterrupt(void)
{
 cli();
 MCUSR &= ~(1<<WDRF);
 WDTCSR |= (1<<WDCE) | (1<<WDE);
 WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
 WDTCSR |= _BV(WDIE);
 sei();
}

void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  sleep_disable();
  power_all_enable();
}

ISR(WDT_vect)
{
}
#endif
