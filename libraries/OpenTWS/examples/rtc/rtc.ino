#include <OpenTWS.h>
#include <Time.h>
#include <Wire.h>
#include <PCF8523.h>

#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

PCF8523 rtc;
tmElements_t tm;

void setup() {
 portA.begin(9600);
 rtc.begin();
 rtc.setTmrA(5, RTC_TQS);
 
 tm.Hour   = 23;
 tm.Minute = 59;
 tm.Second = 50;
 tm.Day = 1;
 tm.Month = 1;
 tm.Year = CalendarYrToTm(2015);
 rtc.write(tm);
 rtc.enableTmrA();
}

void loop() {

 if ( rtc.getTmrAFlag() ) {
  printTime();
  rtc.clearTmrAFlag();
 } else
  portA.println('.');
 delay(1000);
}

void printTime()
{
 rtc.read(tm);
 portA.print(tm.Hour);   portA.print(" ");
 portA.print(tm.Minute); portA.print(" ");
 portA.print(tm.Second); portA.print(" ");
 portA.print(tm.Day);    portA.print(" ");
 portA.print(tm.Month);  portA.print(" ");
 portA.print(tmYearToCalendar(tm.Year)); portA.print(" ");
 portA.println();
}
