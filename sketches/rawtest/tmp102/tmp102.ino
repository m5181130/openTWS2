/**
 Simple test for TMP102.
 Datasheet
 https://www.sparkfun.com/datasheets/Sensors/Temperature/tmp102.pdf

 EM=0
 MSB |         TEMP[11:4]        |
 LSB | TEMP[3:0] | x | x | x | x |
 EM=1
 MSB |         TEMP[12:5]        |
 LSB |   TEMP[4:0]   | x | x | x |
**/
#include <Wire.h>

#define TMP102_I2C_ADDRESS 0x48

#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

void setup()
{
 portA.begin(9600);
 Wire.begin();
}

void loop()
{
 uint16_t T_bits;
 uint8_t hi, lo, sign;
 float T_val; 

 requestFrom(0);
 hi=Wire.read();
 lo=Wire.read();

 T_bits = (((uint16_t)hi) << 8) | lo;
 T_bits >>= 4;
 sign = T_bits >> 11;

 if (sign == 0)
  T_val = float(T_bits) * 0.0625;
 else
  T_val = -float((~T_bits+1) & 0x0FFF) * 0.0625;

 portA.println(T_val);

 delay(2000);
}

void requestFrom(uint8_t reg)
{
  Wire.beginTransmission(TMP102_I2C_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(TMP102_I2C_ADDRESS, 2);
}
