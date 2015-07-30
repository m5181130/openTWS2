static const char *NMEA_TEST  = "$PORZB,RMC,1*38\r\n";
static const char *NMEA_CLEAR = "$PORZB*55\r\n";

#include "OpenTWS.h"
#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

void setup() {
 portA.begin(9600);
 Serial.begin(9600, SERIAL_8N1);
 nmea_clear();
 nmea_test();
 
 pinMode(led1, OUTPUT);
}

void loop() {
 if (Serial.available() )
  digitalWrite(led1, HIGH);
 else
  digitalWrite(led1, LOW);
 
 while ( Serial.available() )
  portA.write( Serial.read() );
}

void nmea_test()
{
 Serial.write( NMEA_TEST );
}

void nmea_clear()
{
 Serial.write( NMEA_CLEAR );
}

