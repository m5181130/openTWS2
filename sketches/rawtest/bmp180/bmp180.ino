/**
 Simple test for BMP180.
 Datasheet
 http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
**/
#include <Wire.h>

#define BMP180_I2C_ADDRESS 0x77

#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

// the values of the calibration coefficients
int16_t AC1, AC2, AC3, VB1, VB2, MB, MC, MD;
uint16_t AC4, AC5, AC6;

int32_t _T_bits, _P_bits;

void setup() {
 portA.begin(9600);
 Wire.begin();

 // see flowchart on the datasheet p15.
 // 1. read callibration data. 
 readCoefficients();
}

void loop() {
 // 2. read uncompensated temperature value.
 readTemperature();

 // 3. read uncompensated pressure value.
 readPressure(3);

 // 4. calculate temperature
 double T_val = getTemperature(); // temp in 0.1 degree.

 // 5. calculate pressure
 double P_val = getPressure(3);

 // 6. display values
 portA.print( T_val / 10.0 ); // celsius degree
 portA.print(" ");
 portA.print( P_val / 100.0 ); // hPa
 portA.println();

 delay(2000);
}

void readTemperature()
{
 // 1. start messurement
 Wire.beginTransmission(BMP180_I2C_ADDRESS);
 /*
   Register Adress F4h
   oss<1:0> | sco | measurement control
   oss : controls the oversampling ratio of the PRESSURE measurement
         (00b: single, 01b: 2 times, 10b: 4 times, 11b: 8 times)
   sco : Start of conversion. The value of this bit stays “1”
         during conversion and is reset to “0” after conversion
         is complete (data registers are filled).
   measurement control : 0E (01110) for temperature, 14 (10100) for pressure.
  */
 Wire.write(0xF4);
 Wire.write(0x2E); // 00 1 01110
 Wire.endTransmission();

 _delay_ms(4.5);

 // 2. set register
 Wire.beginTransmission(BMP180_I2C_ADDRESS);
 Wire.write(0xF6);
 Wire.endTransmission();

 uint8_t hi, lo;

 // 3. read values
 Wire.requestFrom(BMP180_I2C_ADDRESS, 2);
 hi = Wire.read();
 lo = Wire.read();

 _T_bits = ( (uint16_t)hi<<8 ) | lo;
}

void readPressure(uint8_t oss)
{
 // 1. start messurement
 Wire.beginTransmission(BMP180_I2C_ADDRESS);
 /*
   Register Adress F4h
   oss<1:0> | sco | measurement control
   oss : controls the oversampling ratio of the pressure measurement
         (00b: single, 01b: 2 times, 10b: 4 times, 11b: 8 times)
   sco : Start of conversion. The value of this bit stays “1”
         during conversion and is reset to “0” after conversion
         is complete (data registers are filled).
   measurement control : 0E (01110) for temperature, 14 (10100) for pressure.
  */
 Wire.write(0xF4);
 Wire.write(0x34 | (oss << 6)); // 00 1 10100
 Wire.endTransmission();

 switch (oss) {
  case 0: _delay_ms(4.5); break;
  case 1: _delay_ms(7.5); break;
  case 2: _delay_ms(13.5); break;
  case 3: _delay_ms(25.5); break;
 }

 // 2. set register
 Wire.beginTransmission(BMP180_I2C_ADDRESS);
 Wire.write(0xF6);
 Wire.endTransmission();

 uint8_t hi, lo, xlo;

 // 3. read values
 Wire.requestFrom(BMP180_I2C_ADDRESS, 2);
 hi = Wire.read();
 lo = Wire.read();
 xlo = Wire.read(); // <7:3>

 _P_bits = (( (int32_t)hi<<16 ) | ((uint16_t)lo<<8) | xlo ) >> (8-oss);
}

double getTemperature()
{
 int32_t T_val;
 int32_t b5, x1, x2;

 x1 = (_T_bits - AC6) * AC5 / pow(2, 15);
 x2 = MC * pow(2,11) / (x1+MD);
 b5 = x1 + x2;
 T_val = (b5+8)/16;

 return double(T_val);
}

double getPressure(uint8_t oss)
{
 int32_t P_val;
 int32_t b3, b5, b6, x1, x2, x3;
 uint32_t b4, b7;

 x1 = (_T_bits - AC6) * AC5 / pow(2, 15);
 x2 = MC * pow(2,11) / (x1+MD);
 b5 = x1 + x2;
 b6 = b5 - 4000;
 x1 = ( VB2 * ( pow(b6,2)/pow(2,12) )) / pow(2,11);
 x2 = AC2*b6 / pow(2,11);
 x3 = x1 + x2;
 b3 = (( (AC1*4+(uint32_t)x3) << oss ) + 2 ) / 4;
 x1 = AC3 * b6 / pow(2,13);
 x2 = (VB1 * ( pow(b6,2)/pow(2,12) ))/pow(2,16);
 x3 = ((x1+x2)+2)/4;
 b4 = AC4 * (uint32_t)(x3+32768)/pow(2,15);
 b7 = ((uint32_t)_P_bits-b3) * (50000 >> oss);
 P_val = (b7 < 0x80000000) ? (b7*2)/b4 : (b7/b4)*2;
 x1 = (P_val/256) * (P_val/256);
 x1 = (x1*3038)/pow(2,16);
 x2 = (-7537*P_val)/pow(2,16);
 P_val += (x1+x2+3791)/16; 

 return double(P_val);
}

/*
 Every sensor module has individual coefficients. Before the first
 calculation of temperature and pressure, the master reads out the
 E2PROM data. The data communication can be checked by checking that
 none of the words has the value 0 or 0xFFFF.
*/
bool readCoefficients()
{
 if (
 !( readInt(0xAA, AC1)
  & readInt(0xAC, AC2)
  & readInt(0xAE, AC3)
  & readUInt(0xB0, AC4)
  & readUInt(0xB2, AC5)
  & readUInt(0xB4, AC6)
  & readInt(0xB6, VB1)
  & readInt(0xB8, VB2)
  & readInt(0xBA, MB)
  & readInt(0xBC, MC)
  & readInt(0xBE, MD)
 )) return false;

 return true;
}

bool readUInt(char reg, uint16_t &coeff_bits)
{
 uint8_t hi, lo;
 Wire.beginTransmission(BMP180_I2C_ADDRESS);
 Wire.write(reg);
 Wire.endTransmission();

 Wire.requestFrom(BMP180_I2C_ADDRESS, 2);
 hi = Wire.read();
 lo = Wire.read();

 coeff_bits = ( (uint16_t)hi<<8 ) | lo ;

 return ((coeff_bits == 0xFFFF) | coeff_bits==0 ) ? false : true;
}

bool readInt(char reg, int16_t &coeff_bits)
{
 uint8_t hi, lo;
 Wire.beginTransmission(BMP180_I2C_ADDRESS);
 Wire.write(reg);
 Wire.endTransmission();

 Wire.requestFrom(BMP180_I2C_ADDRESS, 2);
 hi = Wire.read();
 lo = Wire.read();

 coeff_bits = ( (int16_t)hi<<8 ) | lo ;

 return ((coeff_bits == 0xFFFF) | coeff_bits==0 ) ? false : true;
}

void printCoefficients()
{
 portA.print("AC1: "); portA.println(AC1);
 portA.print("AC2: "); portA.println(AC2);
 portA.print("AC3: "); portA.println(AC3);
 portA.print("AC4: "); portA.println(AC4);
 portA.print("AC5: "); portA.println(AC5);
 portA.print("AC6: "); portA.println(AC6);
 portA.print("VB1: "); portA.println(VB1);
 portA.print("VB2: "); portA.println(VB2);
 portA.print("MB: "); portA.println(MB);
 portA.print("MC: "); portA.println(MC);
 portA.print("MD: "); portA.println(MD);
 portA.println();
}
