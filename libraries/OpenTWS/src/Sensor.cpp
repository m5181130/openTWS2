#include <Sensor.h>
#include <Wire.h>

//--- HIH9131 ----------------------------------
HIH9131::HIH9131()
 : I2CI(HIH9131_I2C_ADDRESS)
{}

void HIH9131::read(double &T,double &H)
{
 uint16_t adc_H, adc_T;
 uint8_t data[4];

 measurementRequest();
 delay(55); // no stale data
 dataFetch(data);

 adc_H = ((uint16_t)data[0] << 8) | data[1];
 adc_H &= 0x3FFF;
 adc_T = ((uint16_t)data[2] << 8) | data[3];
 adc_T >>= 2;

 H = double(adc_H) * 6.10e-3;
 T = double(adc_T) * 1.007e-2 - 40.0;
}

void HIH9131::measurementRequest()
{
 Wire.beginTransmission(_addr);
 Wire.endTransmission();
}

void HIH9131::dataFetch(uint8_t *data)
{
 int i=0;
 Wire.requestFrom(_addr, (uint8_t)4);
 while(Wire.available())
  data[i++] = Wire.read();
}

//--- TMP102 ----------------------------------
TMP102::TMP102()
 : I2CI(TMP102_I2C_ADDRESS)
{}

void TMP102::read(double &T)
{
 uint16_t adc_T;
 uint8_t data[2];
 uint8_t sign;

 readBytes(0x00, 0x01, data);
 adc_T = ((uint16_t)data[0] << 8) | data[1];
 adc_T >>= 4;
 sign = adc_T >> 11;

 if (sign == 0)
  T = double(adc_T) * 0.0625;
 else
  T = -double((~adc_T+1) & 0x0FFF) * 0.0625;
}

//--- ADT7420 ----------------------------------
ADT7420::ADT7420()
 : I2CI(ADT7420_I2C_ADDRESS)
{}

void ADT7420::read(double &T)
{
 uint16_t adc_T;
 uint8_t data[2];
 uint8_t sign;

 readBytes(0x00, 0x01, data);
 adc_T = ((uint16_t)data[0] << 8 ) | data[1];
 adc_T >>= 3;
 sign = adc_T>>12;

 if (sign == 0)
  T = double(adc_T) * 0.0625;
 else
  T = -double((~adc_T+1) & 0x0FFF) * 0.0625;
}

//--- BMP280 ----------------------------------
BMP280::BMP280()
 : I2CI(BMP280_I2C_ADDRESS)
 , dig_T1(0)
 , dig_T2(0)
 , dig_T3(0)
 , dig_P1(0)
 , dig_P2(0)
 , dig_P3(0)
 , dig_P4(0)
 , dig_P5(0)
 , dig_P6(0)
 , dig_P7(0)
 , dig_P8(0)
 , dig_P9(0)
 , t_fine(0)
 , ctrl_meas_reg(0)
{}

bool BMP280::begin()
{
 if ( !I2CI::begin() ) return false;

// config
 uint8_t osrs_t = 1;
 uint8_t osrs_p = 1;
 uint8_t mode = 3;
 uint8_t t_sb = 5;
 uint8_t filter = 0;
 uint8_t spi3w_en = 0;
 uint8_t config_reg = (t_sb << 5) | (filter << 2) | spi3w_en;
 ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;

 writeByte(0xF5, config_reg);

// param
 uint8_t data[24];

 readBytes(0x88, 0x9F, data);
 dig_T1 = (data[1] << 8) | data[0];
 dig_T2 = (data[3] << 8) | data[2];
 dig_T3 = (data[5] << 8) | data[4];
 dig_P1 = (data[7] << 8) | data[6];
 dig_P2 = (data[9] << 8) | data[8];
 dig_P3 = (data[11]<< 8) | data[10];
 dig_P4 = (data[13]<< 8) | data[12];
 dig_P5 = (data[15]<< 8) | data[14];
 dig_P6 = (data[17]<< 8) | data[16];
 dig_P7 = (data[19]<< 8) | data[18];
 dig_P8 = (data[21]<< 8) | data[20];
 dig_P9 = (data[23]<< 8) | data[22];

 return true;
}

void BMP280::read(double &T, double &P)
{
 int32_t adc_T, adc_P;
 uint8_t data[6];

 // 1. start messurement
 writeByte(0xF4, ctrl_meas_reg);
 _delay_ms(6.4); // wait for measurement
 
 // 2. read ADC bits
 readBytes(0xF7, 0xFC, data);
 adc_P = (( (uint32_t)data[0]<<16 ) | ( (uint32_t)data[1]<<8 ) | data[2] ) >> 4;
 adc_T = (( (uint32_t)data[3]<<16 ) | ( (uint32_t)data[4]<<8 ) | data[5] ) >> 4;

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
