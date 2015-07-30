#include <OpenTWS.h>
#include <GPS.h>
#include <Time.h>
#include <Wire.h>

#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

BINR binr(&Serial1);

void setup() {
 portA.begin(9600);
 pinMode(led1, OUTPUT);
 
 if ( !binr.begin(19200, SERIAL_8O1) )
  while(1) {
   digitalWrite(led1, !digitalRead(led1));
   delay(50);
  }
 binr.requestClear();
 binr.requestF5();
}

void loop() {
 while ( binr.available() )
  portA.print( binr.read(), HEX);
}
