#ifndef SENSOR_h
#define SENSOR_h

#include "Arduino.h"

#define HIH9131_I2C_ADDRESS 0x27
#define TMP102_I2C_ADDRESS  0x48
#define ADT7420_I2C_ADDRESS 0x4B
#define BMP280_I2C_ADDRESS  0x76

//--- Base Class ----------------------------------
class Sensor
{
public:
 Sensor(uint8_t addr);
 bool begin();
 // virtual void read(double &sensorValue) =0;
protected:
 uint8_t _addr;
};

//--- HIH9131 ----------------------------------
class HIH9131 : public Sensor 
{
public:
 HIH9131();
 void read(double &T,double &H);
};

//--- TMP102 ----------------------------------
class TMP102 : public Sensor 
{
public:
 TMP102();
 void read(double &T);
};

//--- ADT7420 ----------------------------------
class ADT7420 : public Sensor 
{
public:
 ADT7420();
 void read(double &T);
};

//--- BMP280 ----------------------------------
class BMP280 : public Sensor
{
public:
 BMP280();
 bool begin();
 void read(double &T, double &P);
private:
 int32_t compensate_T(int32_t adc_T);
 uint32_t compensate_P(int32_t adc_P);
 void readInt(char reg, int16_t &coeff_bits);
 void readUInt(char reg, uint16_t &coeff_bits);

 int16_t dig_T2,dig_T3,dig_T4,dig_P2,dig_P3,dig_P4,dig_P5,dig_P6,dig_P7,dig_P8,dig_P9; 
 uint16_t dig_P1,dig_T1;
 int32_t t_fine;
 uint8_t osrs_p, osrs_t;
 uint8_t mode;
};

#endif
