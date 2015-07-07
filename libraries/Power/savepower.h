#ifndef savepower_h
#define savepower_h

#include <Arduino.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

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
