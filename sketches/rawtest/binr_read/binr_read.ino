static const char *BINR_F4    = "\x10\xF4\x0A\x10\x03";
static const char *BINR_CLEAR = "\x10\x0E\x10\x03";

#include "OpenTWS.h"
#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

void setup() {
 portA.begin(9600);
 Serial1.begin(19200,SERIAL_8O1);
 binr_clear();
 binr_test();
 
 pinMode(led1, OUTPUT);
}

void loop() {
 if (Serial1.available() )
  digitalWrite(led1, HIGH);
 else
  digitalWrite(led1, LOW);
 
 while ( Serial1.available() )
  portA.print( Serial1.read() , HEX);
}

void binr_test()
{
 Serial1.write( BINR_F4 );
}

void binr_clear()
{
 Serial1.write( BINR_CLEAR );
}
