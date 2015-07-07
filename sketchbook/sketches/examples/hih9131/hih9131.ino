/**
 Simple test for HIH9131.
 Datasheet
 http://sensing.honeywell.com/index.php?ci_id=147073
 Technical Note for I2C Communication
 http://sensing.honeywell.com/index.php?ci_id=142171
 http://sensing.honeywell.com/index.php?ci_id=142170

 RH   MSB | ST[15:14] | RH[13:8]  |
 RH   LSB |         RH[7:0]       |
 TEMP MSB |       TEMP[13:6]      |
 TEMP LSB |   TEMP[5:0]   | x | x |
**/

#include <Wire.h>

#define HIH9131_I2C_ADDRESS 0x27

#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

void setup()
{
 portA.begin(9600);
 Wire.begin();
}

void loop() {
 read_data();
 delay(2000);
}

void read_data() {
 uint16_t H_bits, T_bits; // ADC output for humidity and temperature
 float H_val, T_val; 
 uint8_t hi, lo;
 uint8_t stat;

 Wire.beginTransmission(HIH9131_I2C_ADDRESS);
 Wire.endTransmission();
 delay(50); // no delay causes stale data

 Wire.requestFrom(HIH9131_I2C_ADDRESS, 2);
 hi = Wire.read();
 lo = Wire.read();
 stat = (hi >> 6) & 0b11;
 H_bits = (((uint16_t)hi) << 8) | lo;
 H_bits &= 0x3FFF;

 hi = Wire.read();
 lo = Wire.read();
 T_bits = (((uint16_t)hi) << 8) | lo;
 T_bits >>= 2;

 H_val = (float) H_bits * 6.10e-3;
 T_val = (float) T_bits * 1.007e-2 - 40.0;

 portA.print(stat);      portA.print("\t");
 portA.print(H_val, 2);  portA.print("\t");
 portA.print(T_val, 2);  portA.print("\t");
 //Serial.print(T_bits);
 portA.println();
}
