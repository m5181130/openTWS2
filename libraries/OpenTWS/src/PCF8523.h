#ifndef PCF8523_h
#define PCF8523_h

#include <Time.h>
#include "i2c_interface.h"

#define PCF8523_I2C_ADDRESS 0x68

// Register Control_1
#define RTC_CAP_SEL 7
#define RTC_STOP    5
#define RTC_SR      4
#define RTC_12_24   3
#define RTC_SIE     2
#define RTC_AIE     1
#define RTC_CIE     0

// Register Control_2
#define RTC_WTAF    7
#define RTC_CTAF    6
#define RTC_CTBF    5
#define RTC_SF      4
#define RTC_AF      3
#define RTC_WTAIE   2
#define RTC_CTAIE   1
#define RTC_CTBIE   0

// Register Control_3
#define RTC_PM2     7
#define RTC_PM1     6
#define RTC_PM0     5
#define RTC_BSF     3
#define RTC_BLF     2
#define RTC_BSIE    1
#define RTC_BLIE    0

// timer control
#define RTC_CLKOUT_DISABLED 0x38
#define RTC_WTA 0x04 // timer A is configured as watchdog timer
#define RTC_CTA 0x02 // timer A is configured as countdown timer
#define RTC_CTB 0x01 // timer B is enabled

// timer frequency control 
#define RTC_TQH  0xE0
#define RTC_TQM  0x03
#define RTC_TQS  0x02 // 1 Hz
#define RTC_TQMS 0x01 // 64 Hz
#define RTC_TQUS 0x00 // 4.096 kHz

class PCF8523 : public I2CI
{
public:
 PCF8523();
 bool begin();

// time
 bool read(tmElements_t &tm);
 void write(tmElements_t &tm);
 void write(tmElements_t &tm, uint8_t utc_offset);

// timer
 void setTmrA(uint8_t val, uint8_t rtc_tq) { 
  setTimer(val, rtc_tq, RTC_CTA); 
 }
 void setTmrB(uint8_t val, uint8_t rtc_tq) {
  setTimer(val, rtc_tq, RTC_CTB);
 }
 void enableTmrA() { enableTimer(RTC_CTA); }
 void enableTmrB() { enableTimer(RTC_CTB); }
 void disableTmrB() { disableTimer(RTC_CTB); }
 void clearTmrAFlag() { clearFlag(RTC_CTAF); }
 void clearTmrBFlag() { clearFlag(RTC_CTBF); }
 bool getTmrAFlag() { return getFlag(RTC_CTAF); }
 bool getTmrBFlag() { return getFlag(RTC_CTBF); }

private:
 void enableTimer(uint8_t rtc_ct);
 void disableTimer(uint8_t rtc_ct);
 void setTimer(uint8_t val, uint8_t rtc_tq, uint8_t rtc_ct);
 void clearFlag(uint8_t rtc_f);
 bool getFlag(uint8_t rtc_f);

 uint8_t btod(uint8_t bcd);
 uint8_t dtob(uint8_t decimal);

 uint8_t _tmrctrl;
};

#endif
