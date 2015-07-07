/**
 Simple test for ADT7420.
 Datasheet
 http://pdf.datasheetcatalog.com/datasheet/analogdevices2/ADT7420.pdf

 MSB | Sign[15] |               Temp[14:8]                 |
 LSB |      Temp[7:3]      | TCRIT[2] | THIGH[1] | TLOW[0] |
**/

#include <Wire.h>

#define ADT7420_I2C_ADDRESS 0x4B

#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

void setup() {
 portA.begin(9600);
 Wire.begin();

 Wire.beginTransmission(ADT7420_I2C_ADDRESS);
 Wire.endTransmission();
}

void loop() {
 uint16_t T_bits;
 uint8_t hi,lo,sign;
 float T_val;

 Wire.requestFrom(ADT7420_I2C_ADDRESS, 2); 
 hi = Wire.read();
 lo = Wire.read();

 T_bits = (((uint16_t)hi) << 8 ) | lo;
 T_bits >>= 3;
 sign = T_bits>>12;

 if (sign == 0)
  T_val = float(T_bits) * 0.0625;
 else
  T_val = -float((~T_bits+1) & 0x0FFF) * 0.0625;

 portA.println(T_val, 2);

 delay(2000);
}
