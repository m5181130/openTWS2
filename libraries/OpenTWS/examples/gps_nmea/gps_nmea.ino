#include <OpenTWS.h>
#include <GPS.h>
#include <Time.h>
#include <Wire.h>

#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

NMEA nmea(&Serial);

void setup() {
 portA.begin(9600);
 pinMode(led1, OUTPUT);
 
 if ( !nmea.begin(9600, SERIAL_8N1) )
  while(1) {
   digitalWrite(led1, !digitalRead(led1));
   delay(50);
  }
 
 nmea.requestClear();
 nmea.requestRMC();
}

void loop() {
 while ( nmea.available() )
  portA.write( nmea.read() );
}
