/**
register details.
http://pdf.datasheetcatalog.com/datasheet/analogdevices2/ADT7420.pdf, p13
NEXT --> Specify registers
http://bunkeina.blogspot.jp/2013/04/adt7420i2c.html
**/

#include <Wire.h>

#define ADT7420_I2C_ADDRESS 0x4B
//#define ADT7420_I2C_ADDRESS_READ  0xC8

void setup() {
 Serial.begin(9600);
 Wire.begin();
 
 Wire.beginTransmission(ADT7420_I2C_ADDRESS);
 Wire.endTransmission();
}

void loop() {
 uint16_t T_bits;
 uint8_t hi,lo,sign;
 float T_val;
 uint8_t offset = 90;
 
 Wire.requestFrom(ADT7420_I2C_ADDRESS, 2); 
 hi = Wire.read();
 lo = Wire.read();

/**
 Temperature Value MSB Register
 | Sign[15] |               Temp[14:8]                |
 Temperature Value LSB Register
 |      Temp[7:3]      | TCRIT[2] | THIGH[1]| TLOW[0] |
**/
 
 T_bits = (((uint16_t)hi) << 8 ) | lo;
 T_bits >>= 3;
/*
 T_bits = 0x1D80; // --> -40
 T_bits = 0x1FFF; // --> 0.0625
 T_bits = 0x001;  // --> 0.0625
 T_bits = 0x190;  // --> +25
 T_bits = 0x960;  // --> +150
 */
 sign = T_bits>>12;

 if (sign == 0)
  T_val = float(T_bits) * 0.0625;
 else
  T_val = -float((~T_bits+1) & 0x0FFF) * 0.0625;
 
 //Serial.print(T_bits); Serial.print("\t");
 Serial.println(T_val,2);
 
 delay(2000);
}
 
 //  T_val = float(long(word(hi,lo)) * 100/128)/100.00;
 /*
  T_bits = 0x0FFF & 0x1D80;
 //T_bits = (~T_bits+1) & 0x0FFF;
 Serial.print(T_bits, BIN);
 Serial.print("\t");
 Serial.print((~(T_bits)+1) & 0x0FFF , BIN);
 Serial.print("\t");
 Serial.print(-float((~(T_bits)+1) & 0x0FFF)/16);
 Serial.print("\t");
 */
