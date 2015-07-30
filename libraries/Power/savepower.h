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
 wdt_reset();
 MCUSR &= ~_BV(WDRF);
 WDTCSR = _BV(WDCE) | _BV(WDE);
 WDTCSR = _BV(WDIE) | WDT_8_SEC;
 sei();
}

void enterSleep(void)
{
 set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 sleep_mode();
}

ISR(WDT_vect)
{
}
#endif
