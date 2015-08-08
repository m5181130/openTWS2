#include "Arduino.h"
#include "PCF8523.h"
#include <Wire.h>
#include <Time.h>

PCF8523::PCF8523()
 : I2CI(PCF8523_I2C_ADDRESS)
 , _tmrctrl(RTC_CLKOUT_DISABLED)
{}

bool PCF8523::begin()
{
 if ( !I2CI::begin() ) return false;

 uint8_t rc1 = 0b00000000;
 uint8_t rc2 = 0b00000000;
 uint8_t rc3 = 0b11100000;
 uint8_t data[3] = {rc1,rc2,rc3};

 writeBytes(0x00,0x02,data);

 disableTimer(RTC_WTA | RTC_CTA | RTC_CTB);

 return true;
}

bool PCF8523::read(tmElements_t &tm)
{
 uint8_t data[7];

 readBytes(0x03,0x09,data);
 
 if (data[0] & 0x80) return false; // oscillator stopped

 tm.Second = btod( data[0] & 0x7f );  
 tm.Minute = btod( data[1] & 0x7F );
 tm.Hour   = btod( data[2] & 0x3F );
 tm.Day    = btod( data[3] & 0x3F );
 tm.Wday   = btod( data[4] & 0x07 );
 tm.Month  = btod( data[5] & 0x1F );
 tm.Year   = y2kYearToTm( btod( data[6] & 0xFF ) );

 return true;
}

void PCF8523::write(tmElements_t &tm)
{
 uint8_t data[7] = {
  dtob( tm.Second ),
  dtob( tm.Minute ),
  dtob( tm.Hour   ),
  dtob( tm.Day    ),
  dtob(     0     ), // don't care
  dtob( tm.Month  ),
  dtob( tmYearToY2k(tm.Year) )
 };

 writeBytes(0x03,0x09,data);
}

void PCF8523::write(tmElements_t &tm, uint8_t utc_offset)
{
 time_t t = makeTime(tm) + utc_offset * SECS_PER_HOUR;
 breakTime(t, tm);
 write(tm);
}

void PCF8523::enableTimer(uint8_t rtc_ct)
{
 _tmrctrl |= rtc_ct;

 writeByte(0x0F, _tmrctrl);
}

void PCF8523::disableTimer(uint8_t rtc_ct)
{
 _tmrctrl &= ~rtc_ct;

 writeByte(0x0F, _tmrctrl);
}

void PCF8523::setTimer(uint8_t val, uint8_t rtc_tq, uint8_t rtc_ct)
{
 uint8_t data[2] = {rtc_tq, val};
 
 if (rtc_ct & (RTC_CTA|RTC_WTA))
  writeBytes(0x10,0x11,data);
 else
  writeBytes(0x12,0x13,data);
}

void PCF8523::clearFlag(uint8_t rtc_f)
{
 uint8_t reset = ~_BV(rtc_f) & 0x78;

 writeByte(0x01, reset);
}

bool PCF8523::getFlag(uint8_t rtc_f)
{
 uint8_t flag;
 readByte(0x01, flag);
 return flag & _BV(rtc_f) ? true : false;
}

uint8_t PCF8523::btod(uint8_t bcd)
{
  return ((bcd/16 * 10) + (bcd % 16));
}

uint8_t PCF8523::dtob(uint8_t decimal)
{
  return ((decimal/10 * 16) + (decimal % 10));
}
