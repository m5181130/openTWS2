#ifndef SENSOR_h
#define SENSOR_h

#include "Arduino.h"

#define HIH9131_I2C_ADDRESS 0x27
#define TMP102_I2C_ADDRESS  0x48
#define ADT7420_I2C_ADDRESS 0x4B
#define BMP180_I2C_ADDRESS  0x77

//--- Base Class ----------------------------------
class Sensor
{
public:
 Sensor(uint8_t addr);
 bool begin();
 virtual void read(double &sensorValue) =0;
protected:
 uint8_t _addr;
};

//--- HIH9131 ----------------------------------
class HIH9131 : public Sensor 
{
public:
 HIH9131();
 void read(double &H);
 void read(double &H, double &T);
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

//--- BMP180 ----------------------------------
class BMP180 : public Sensor
{
public:
 BMP180();
 bool begin();
 void read(double &T);
 void read(double &T, double &P);
private:
 int16_t _AC1, _AC2, _AC3, _B1, _B2, _MB, _MC, _MD;
 uint16_t _AC4, _AC5, _AC6;
 int32_t _T_bits, _P_bits;
 uint8_t _oss;

 bool readInt(char reg, int16_t &coeff_bits);
 bool readUInt(char reg, uint16_t &coeff_bits);
};

#endif
