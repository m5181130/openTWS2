/**
 Simple test for TMP102.
 Datasheet
 https://www.sparkfun.com/datasheets/Sensors/Temperature/tmp102.pdf
 http://www.mike-stirling.com/2013/04/low-cost-high-accuracy-temperature-sensing-with-a-thermistor/

 EM=0
 MSB |         Temp[11:4]        |
 LSB | Temp[3:0] | x | x | x | x |
 EM=1
 MSB |         Temp[12:5]        |
 LSB |   Temp[4:0]   | x | x | x |
 
**/
#include <Wire.h>
#include "OpenTWS.h"

#define TMP102_I2C_ADDRESS 0x48

// Configuration Register 
#define TMP_OS  7
#define TMP_R1  6
#define TMP_R0  5
#define TMP_F1  4
#define TMP_F0  3
#define TMP_POL 2
#define TMP_TM  1
#define TMP_SD  0

#define TMP_CR1 7
#define TMP_CR0 6
#define TMP_AL  5
#define TMP_EM  4

#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

void setup()
{
 portA.begin(9600);
 Wire.begin();
 
 // printConfig();
}

void loop()
{
 uint16_t T_bits;
 uint8_t hi, lo;
 float T_val;
 
 requestFrom(0b00); // Temperature Register 12- or 13-bit
 hi=Wire.read() & 0xFF;
 lo=Wire.read() & 0xF0;
 
 T_bits = (((uint16_t)hi) << 8) | lo;
 T_bits >>= 4;
 uint8_t sign = T_bits >> 11;

 if (sign == 0)
  T_val = float(T_bits) * 0.0625; //0.0475;
 else {
  T_val = -float((~T_bits+1) & 0x0FFF) * 0.0625;
 }
 //Serial.print(T_bits); Serial.print("\t");
 portA.println(T_val);
 printConfig();
 printLimits();
 delay(2000);
}

uint8_t reverse_bits(uint8_t v) {
 uint8_t r = 0;
 r |= (_BV(7) & v) >> 7;
 r |= (_BV(6) & v) >> 5;
 r |= (_BV(5) & v) >> 3;
 r |= (_BV(4) & v) >> 1;
 r |= (_BV(3) & v) << 1;
 r |= (_BV(2) & v) << 3;
 r |= (_BV(1) & v) << 5;
 r |= (_BV(0) & v) << 7;
 return r;
}

void requestFrom(uint8_t reg) {
  Wire.beginTransmission(TMP102_I2C_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(TMP102_I2C_ADDRESS, 2);
}

void printConfig() {
 uint16_t Config_bits;
 uint8_t hi, lo;
 requestFrom(0b01); // 2. Configuration Register 16-bit
 hi=Wire.read();
 lo=Wire.read();
 
 portA.print("One-Shot:\t");
 portA.println((hi & _BV(TMP_OS))>> 7 , BIN);
 portA.print("Resolution:\t");
 portA.println((hi & (_BV(TMP_R1) | _BV(TMP_R0)))>> 5 , BIN);
 portA.print("Fault Queue:\t");
 portA.println((hi & (_BV(TMP_F1) | _BV(TMP_F0)))>> 3 , BIN);
 portA.print("Polarity:\t");
 portA.println((hi & _BV(TMP_POL))>> 2 , BIN);
 portA.print("Thermostat:\t");
 portA.println((hi & _BV(TMP_TM))>> 1 , BIN);
 portA.print("Shutdown:\t");
 portA.println((hi & _BV(TMP_SD)) , BIN);
 portA.print("Rate:\t\t");
 portA.println((lo & (_BV(TMP_R1) | _BV(TMP_R0)))>> 6 , BIN);
 portA.print("Alert:\t\t");
 portA.println((lo & _BV(TMP_AL))>> 5 , BIN);
 portA.print("Extended Mode:\t");
 portA.println((lo & _BV(TMP_EM))>> 4 , BIN);
 portA.println();
}

void printLimits() {
 uint16_t T_bits;
 uint8_t hi, lo;
 float T_val; 
 
 requestFrom(0b10);
 hi=Wire.read();
 lo=Wire.read();
 T_bits = (hi << 4) | (lo >> 4);
 T_val = T_bits*0.0625;
 // fahrenheit to celsius
 T_val = (T_val-32) * 5/9;
 portA.print(T_val);
 portA.print("\t");
 
 requestFrom(0b11); // 4. T_HIGH-Limit Register
 hi=Wire.read();
 lo=Wire.read();
 T_bits = (hi << 4) | (lo >> 4);
 T_val = T_bits*0.0625;
 // fahrenheit to celsius
 T_val = (T_val-32) * 5/9;
 portA.print(T_val);
 portA.println();
 
}
