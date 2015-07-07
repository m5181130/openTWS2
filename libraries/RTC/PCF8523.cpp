#include "Arduino.h"
#include "PCF8523.h"
#include <Wire.h>
#include <Time.h>

PCF8523::PCF8523()
 : _addr(PCF8523_I2C_ADDRESS)
 , _rc1(0b00000000)
 , _rc2(0b00000000)
 , _rc3(0b11100000)
 , _tmrctrl(RTC_CLKOUT_DISABLED)
{}

bool PCF8523::begin()
{
 Wire.begin();
 Wire.beginTransmission(_addr);
  Wire.write( 0x00 );
  Wire.write( _rc1 );
  Wire.write( _rc2 );
  Wire.write( _rc3 );
 if (Wire.endTransmission() != 0) return false;

 disableTimer(RTC_WTA | RTC_CTA | RTC_CTB);
 return true;
}

bool PCF8523::set(time_t t)
{
 tmElements_t tm;
 breakTime(t, tm);
 write(tm); 
}

bool PCF8523::read(tmElements_t &tm)
{
 uint8_t os_sec;

 Wire.beginTransmission(_addr);
 Wire.write( 0x03 );
 if (Wire.endTransmission() != 0) return false;
 
 Wire.requestFrom(_addr, 7);
 os_sec = Wire.read();
 tm.Second = btod( os_sec & 0x7f );  
 tm.Minute = btod( Wire.read() & 0x7F );
 tm.Hour   = btod( Wire.read() & 0x3F );
 tm.Day    = btod( Wire.read() & 0x3F );
             btod( Wire.read() & 0x07 );
 tm.Month  = btod( Wire.read() & 0x1F );
 tm.Year   = y2kYearToTm( btod( Wire.read() & 0xFF ) );

 if (os_sec & 0x80) return false; // the oscillator is stopped
 return true;
}

bool PCF8523::write(tmElements_t &tm)
{
 Wire.beginTransmission(_addr);
  Wire.write( 0x03 );
  Wire.write( dtob( tm.Second ) );
  Wire.write( dtob( tm.Minute ) );
  Wire.write( dtob( tm.Hour   ) );
  Wire.write( dtob( tm.Day    ) );
  Wire.write( dtob(  0     ) ); // don't care
  Wire.write( dtob( tm.Month  ) );
  Wire.write( dtob( tmYearToY2k(tm.Year) ) );
 if (Wire.endTransmission() != 0) return false;
 return true;
}

//--- PRIVATE FUNCTIONS
bool PCF8523::enableTimer(uint8_t RTC_CT)
{
 if ( RTC_CT == (RTC_CTA | RTC_WTA) ) return false;

 _tmrctrl |= RTC_CT;

 Wire.beginTransmission(_addr);
  Wire.write( 0x0F );
  Wire.write( _tmrctrl );
 if (Wire.endTransmission() != 0) return false;
 return true;
}

void PCF8523::setTimer(uint8_t val, uint8_t RTC_TQ, uint8_t RTC_CT)
{
 uint8_t freq;

 freq = (RTC_TQ == RTC_TQH) ? RTC_TQH
      : (RTC_TQ == RTC_TQM) ? RTC_TQM
      : (RTC_TQ == RTC_TQS) ? RTC_TQS
      : (RTC_TQ == RTC_TQMS) ? RTC_TQMS
      : RTC_TQUS;
 
 Wire.beginTransmission(_addr);
 if (RTC_CT & (RTC_CTA|RTC_WTA))
  Wire.write( 0x10 );
 else
  Wire.write( 0x12 );

  Wire.write( freq );
  Wire.write( val );
 Wire.endTransmission();
}

void PCF8523::disableTimer(uint8_t RTC_CT)
{
 _tmrctrl &= ~RTC_CT;
 Wire.beginTransmission(_addr);
  Wire.write( 0x0F );
  Wire.write( _tmrctrl );
 Wire.endTransmission();
}

void PCF8523::clearFlag(uint8_t RTC_F)
{
 Wire.beginTransmission(_addr);
  Wire.write(0x01);
  Wire.write(~_BV(RTC_F) & 0x78);
 Wire.endTransmission();
}

bool PCF8523::getFlag(uint8_t RTC_F)
{
 Wire.beginTransmission(_addr);  
 Wire.write(0x01);
 if (Wire.endTransmission() != 0) return false;

 Wire.requestFrom(_addr, 1);
 return Wire.read() & _BV(RTC_F) ? true : false;
}

uint8_t PCF8523::btod(uint8_t bcd)
{
  return ((bcd/16 * 10) + (bcd % 16));
}

uint8_t PCF8523::dtob(uint8_t decimal)
{
  return ((decimal/10 * 16) + (decimal % 10));
}

PCF8523 RTC = PCF8523(); // create an instance for the user
