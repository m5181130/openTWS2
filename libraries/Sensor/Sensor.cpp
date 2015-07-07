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

void HIH9131::read(double &H)
{
 uint16_t H_bits;
 uint8_t hi, lo;

 Wire.beginTransmission(_addr);
 Wire.endTransmission();
 delay(55); // no delay gives us stale data

 Wire.requestFrom(_addr, 2);
 hi = Wire.read();
 lo = Wire.read();

 H_bits = ((uint16_t)hi << 8) | lo;
 H_bits &= 0x3FFF;

 H = (double)H_bits * 6.10e-3;
}

void HIH9131::read(double &H, double &T)
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

//--- BMP180 ----------------------------------
BMP180::BMP180()
 : Sensor(BMP180_I2C_ADDRESS)
 ,_AC1(0),_AC2(0),_AC3(0),_B1(0),_B2(0),_MB(0),_MC(0),_MD(0)
 ,_AC4(0),_AC5(0),_AC6(0)
 ,_T_bits(0), _P_bits(0)
 ,_oss(0)
{}

bool BMP180::begin()
{
 Wire.begin();
 Wire.beginTransmission(_addr);
 if ( Wire.endTransmission() != 0 ) return false;

 if (
 !( readInt(0xAA, _AC1)
  & readInt(0xAC, _AC2)
  & readInt(0xAE, _AC3)
  & readUInt(0xB0, _AC4)
  & readUInt(0xB2, _AC5)
  & readUInt(0xB4, _AC6)
  & readInt(0xB6, _B1)
  & readInt(0xB8, _B2)
  & readInt(0xBA, _MB)
  & readInt(0xBC, _MC)
  & readInt(0xBE, _MD)
 )) return false;

 return true;
}

void BMP180::read(double &T)
{
 int32_t T_val;
 uint8_t hi, lo;
 int32_t b5, x1, x2;

 // 1. start messurement
 Wire.beginTransmission(_addr);
 Wire.write(0xF4);
 Wire.write(0x2E);
 Wire.endTransmission();
 
 _delay_ms(4.5);
 
 // 2. set register
 Wire.beginTransmission(_addr);
 Wire.write(0xF6);
 Wire.endTransmission();
 
 // 3. read values
 Wire.requestFrom(_addr, 2);
 hi = Wire.read();
 lo = Wire.read();
 _T_bits = ( (uint16_t)hi<<8 ) | lo;

 // 4. calculate temperature
 x1 = (_T_bits - _AC6) * _AC5 / pow(2, 15);
 x2 = _MC * pow(2,11) / (x1+_MD);
 b5 = x1 + x2;
 T_val = (b5+8)/16;
 
 // 5. result
 T = double(T_val) / 10.0;
}

void BMP180::read(double &T, double &P)
{
 int32_t P_val;
 uint8_t hi, lo, xlo;
 int32_t b3, b5, b6, x1, x2, x3;
 uint32_t b4, b7;

 // 0. read temperature
 BMP180::read(T);
 
 // 1. start messurement
 Wire.beginTransmission(_addr);
 Wire.write(0xF4);
 Wire.write(0x34 | (_oss << 6)); // 00 1 10100
 Wire.endTransmission();
 
 switch (_oss) {
  case 0: _delay_ms(4.5); break;
  case 1: _delay_ms(7.5); break;
  case 2: _delay_ms(13.5); break;
  case 3: _delay_ms(25.5); break;
 }

 // 2. set register
 Wire.beginTransmission(_addr);
 Wire.write(0xF6);
 Wire.endTransmission();

 // 3. read values
 Wire.requestFrom(_addr, 2);
 hi = Wire.read();
 lo = Wire.read();
 xlo = Wire.read(); // <7:3>

 _P_bits = (( (int32_t)hi<<16 ) | ((uint16_t)lo<<8) | xlo ) >> (8-_oss);

 // 4. calculate pressure
 x1 = (_T_bits - _AC6) * _AC5 / pow(2, 15);
 x2 = _MC * pow(2,11) / (x1+_MD);
 b5 = x1 + x2;
 b6 = b5 - 4000;
 x1 = ( _B2 * ( pow(b6,2)/pow(2,12) )) / pow(2,11);
 x2 = _AC2*b6 / pow(2,11);
 x3 = x1 + x2;
 b3 = (( (_AC1*4+(uint32_t)x3) << _oss ) + 2 ) / 4;
 x1 = _AC3 * b6 / pow(2,13);
 x2 = (_B1 * ( pow(b6,2)/pow(2,12) ))/pow(2,16);
 x3 = ((x1+x2)+2)/4;
 b4 = _AC4 * (uint32_t)(x3+32768)/pow(2,15);
 b7 = ((uint32_t)_P_bits-b3) * (50000 >> _oss);
 P_val = (b7 < 0x80000000) ? (b7*2)/b4 : (b7/b4)*2;
 x1 = (P_val/256) * (P_val/256);
 x1 = (x1*3038)/pow(2,16);
 x2 = (-7537*P_val)/pow(2,16);
 P_val += (x1+x2+3791)/16; 

 // 5. result
 P = double(P_val) / 100.0;
}

bool BMP180::readUInt(char reg, uint16_t &coeff_bits)
{
 uint8_t hi, lo;
 Wire.beginTransmission(_addr);
 Wire.write(reg);
 Wire.endTransmission();

 Wire.requestFrom(_addr, 2);
 hi = Wire.read();
 lo = Wire.read();

 coeff_bits = ( (uint16_t)hi<<8 ) | lo ;

 return ((coeff_bits == 0xFFFF) | coeff_bits==0 ) ? false : true;
}

bool BMP180::readInt(char reg, int16_t &coeff_bits)
{
 uint8_t hi, lo;
 Wire.beginTransmission(_addr);
 Wire.write(reg);
 Wire.endTransmission();

 Wire.requestFrom(_addr, 2);
 hi = Wire.read();
 lo = Wire.read();

 coeff_bits = ( (int16_t)hi<<8 ) | lo ;

 return ((coeff_bits == 0xFFFF) | coeff_bits==0 ) ? false : true;
}
