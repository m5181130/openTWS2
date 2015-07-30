#include <Sensor.h>
#include <Wire.h>

Sensor::Sensor(uint8_t addr)
 : _addr(addr)
{}

bool Sensor::begin()
{
 Wire.begin();
 Wire.beginTransmission(_addr);
 if ( Wire.endTransmission() != 0 ) return false;
 return true;
}

//--- HIH9131 ----------------------------------
HIH9131::HIH9131()
 : Sensor(HIH9131_I2C_ADDRESS)
{}

void HIH9131::read(double &T,double &H)
{
 uint16_t H_bits, T_bits;
 uint8_t hi0, lo0, hi1, lo1;

 Wire.beginTransmission(_addr);
 Wire.endTransmission();
 delay(55); // no delay gives us stale data

 Wire.requestFrom(_addr, 4);
 hi0 = Wire.read();
 lo0 = Wire.read();
 hi1 = Wire.read();
 lo1 = Wire.read();

 H_bits = ((uint16_t)hi0 << 8) | lo0;
 H_bits &= 0x3FFF;
 T_bits = ((uint16_t)hi1 << 8) | lo1;
 T_bits >>= 2;

 H = double(H_bits) * 6.10e-3;
 T = double(T_bits) * 1.007e-2 - 40.0;
}

//--- TMP102 ----------------------------------
TMP102::TMP102()
 : Sensor(TMP102_I2C_ADDRESS)
{}

void TMP102::read(double &T)
{
 uint16_t T_bits;
 uint8_t hi, lo, sign;

 // Set Pointer Register
 Wire.beginTransmission(_addr);
 Wire.write(0x00);
 Wire.endTransmission();

 // Data Fetch Request
 Wire.requestFrom(_addr, 2);
 hi = Wire.read();
 lo = Wire.read();
 T_bits = (((uint16_t)hi) << 8) | lo;
 T_bits >>= 4;
 sign = T_bits >> 11;

 if (sign == 0)
  T = double(T_bits) * 0.0625;
 else
  T = -double((~T_bits+1) & 0x0FFF) * 0.0625;
}

//--- ADT7420 ----------------------------------
ADT7420::ADT7420()
 : Sensor(ADT7420_I2C_ADDRESS)
{}

void ADT7420::read(double &T)
{
 uint16_t T_bits;
 uint8_t hi,lo,sign;

 // Set Pointer Register
 Wire.beginTransmission(_addr);
 Wire.write(0x00);
 Wire.endTransmission();

 // Data Fetch Request
 Wire.requestFrom(_addr, 2); 
 hi = Wire.read();
 lo = Wire.read();
 T_bits = (((uint16_t)hi) << 8 ) | lo;
 T_bits >>= 3;
 sign = T_bits>>12;

 if (sign == 0)
  T = double(T_bits) * 0.0625;
 else
  T = -double((~T_bits+1) & 0x0FFF) * 0.0625;
}

//--- BMP280 ----------------------------------
BMP280::BMP280()
 : Sensor(BMP280_I2C_ADDRESS)
 , dig_T2(0),dig_T3(0),dig_T4(0)
 , dig_P2(0),dig_P3(0),dig_P4(0),dig_P5(0),dig_P6(0),dig_P7(0),dig_P8(0),dig_P9(0)
 , osrs_p(0b001),osrs_t(0b001)
 , mode(0b01)
{}

bool BMP280::begin()
{
 Wire.begin();
 Wire.beginTransmission(_addr);
 if ( Wire.endTransmission() != 0 ) return false;

 readUInt(0x88, dig_T1) ;
 readInt( 0x8A, dig_T2) ;
 readInt( 0x8C, dig_T3) ;
 readUInt(0x8E, dig_P1) ;
 readInt( 0x90, dig_P2) ;
 readInt( 0x92, dig_P3) ;
 readInt( 0x94, dig_P4) ;
 readInt( 0x96, dig_P5) ;
 readInt( 0x98, dig_P6) ;
 readInt( 0x9A, dig_P7) ;
 readInt( 0x9C, dig_P8) ;
 readInt( 0x9E, dig_P9) ;

 return true;
}

void BMP280::read(double &T, double &P)
{
 int32_t adc_T, adc_P;
 uint8_t hi, lo, xlo;

 // 1. start messurement
 Wire.beginTransmission(_addr);
 Wire.write(0xF4);
 Wire.write( (osrs_t << 5) | (osrs_p<<2) | mode );
 Wire.endTransmission(); 
 _delay_ms(6.4); // wait for measurement
 
 // 2. set register for reading ADC bits
 Wire.beginTransmission(_addr);
 Wire.write(0xF7);
 Wire.endTransmission();

 // 3. read values
 Wire.requestFrom(_addr, 6);
 hi  = Wire.read();
 lo  = Wire.read();
 xlo = Wire.read();
 adc_P = (( (uint32_t)hi<<16 ) | ( (uint32_t)lo<<8 ) | xlo ) >> 4;
 hi  = Wire.read();
 lo  = Wire.read();
 xlo = Wire.read();
 adc_T = (( (uint32_t)hi<<16 ) | ( (uint32_t)lo<<8 ) | xlo ) >> 4;

 // 4. calculate
 T = (double)compensate_T(adc_T)/100.0;
 P = (double)compensate_P(adc_P)/100.0;
}

int32_t BMP280::compensate_T(int32_t adc_T)
{
 int32_t var1, var2, t;
 var1 = ((((adc_T>>3)-((int32_t)dig_T1<<1)))*((int32_t)dig_T2))>> 11;
 var2 = (((((adc_T>>4)-((int32_t)dig_T1))*((adc_T>>4)-((int32_t)dig_T1)))>>12)*((int32_t)dig_T3))>> 14;
 t_fine = var1+var2;
 t = (t_fine * 5 + 128) >> 8;

 return t;
}

uint32_t BMP280::compensate_P(int32_t adc_P)
{
 int32_t var1, var2;
 uint32_t p;
 var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
 var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)dig_P6);
 var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
 var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
 var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) * var1)>>1))>>18;
 var1 =((((32768+var1))*((int32_t)dig_P1))>>15);
 p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
 p = (p < 0x80000000) ? (p << 1) / ((uint32_t)var1) : (p / (uint32_t)var1) * 2;
 var1 = (((int32_t)dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
 var2 = (((int32_t)(p>>2)) * ((int32_t)dig_P8))>>13;
 p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));

 return p;
}

void BMP280::readUInt(char reg, uint16_t &coeff_bits)
{
 uint8_t hi, lo;
 Wire.beginTransmission(_addr);
 Wire.write(reg);
 Wire.endTransmission();

 Wire.requestFrom(_addr, 2);
 lo = Wire.read(); // ! LSB is first, different from BMP180
 hi = Wire.read();

 coeff_bits = ( (uint16_t)hi<<8 ) | lo;
}

void BMP280::readInt(char reg, int16_t &coeff_bits)
{
 uint8_t hi, lo;
 Wire.beginTransmission(_addr);
 Wire.write(reg);
 Wire.endTransmission();

 Wire.requestFrom(_addr, 2);
 lo = Wire.read();
 hi = Wire.read();

 coeff_bits = ( (int16_t)hi<<8 ) | lo ;
}
