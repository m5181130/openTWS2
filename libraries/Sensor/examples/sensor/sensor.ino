#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

#include <Sensor.h>
#include <Wire.h>

HIH9131 hih9131;
TMP102 tmp102;
ADT7420 adt7420;
BMP280 bmp280;

void setup() {
 portA.begin(9600);
 hih9131.begin();
 tmp102.begin();
 adt7420.begin();
 bmp280.begin();
}

void loop() {
 double p0;
 double h0;
 double t0,t1,t2,t3;
 hih9131.read(h0, t0);
 tmp102.read(t1);
 adt7420.read(t2);
 bmp280.read(t3,p0);
 
 portA.print(h0);
 portA.print(" ");
 portA.print(t0);
 portA.print(" ");
 portA.print(t1);
 portA.print(" ");
 portA.print(t2);
 portA.print(" ");
 portA.print(t3);
 portA.print(" ");
 portA.print(p0);
 portA.print(" ");
 portA.println();  
 delay(2000);
}
