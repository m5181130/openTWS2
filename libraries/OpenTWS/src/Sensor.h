#ifndef SENSOR_h
#define SENSOR_h

#include "Arduino.h"
#include "i2c_interface.h"

#define HIH9131_I2C_ADDRESS 0x27
#define TMP102_I2C_ADDRESS  0x48
#define ADT7420_I2C_ADDRESS 0x4B
#define BMP280_I2C_ADDRESS  0x76

//--- HIH9131 ----------------------------------
class HIH9131 : public I2CI
{
public:
 HIH9131();
 void read(double &T,double &H);

private:
 void measurementRequest();
 void dataFetch(uint8_t *data);
};

//--- TMP102 ----------------------------------
class TMP102 : public I2CI
{
public:
 TMP102();
 void read(double &T);
};

//--- ADT7420 ----------------------------------
class ADT7420 : public I2CI
{
public:
 ADT7420();
 void read(double &T);
};

//--- BMP280 ----------------------------------
class BMP280 : public I2CI
{
public:
 BMP280();
 bool begin();
 void read(double &T, double &P);
private:
 int32_t compensate_T(int32_t adc_T);
 uint32_t compensate_P(int32_t adc_P);

uint16_t dig_T1;
 int16_t dig_T2;
 int16_t dig_T3;
uint16_t dig_P1;
 int16_t dig_P2;
 int16_t dig_P3;
 int16_t dig_P4;
 int16_t dig_P5;
 int16_t dig_P6;
 int16_t dig_P7;
 int16_t dig_P8;
 int16_t dig_P9;
 int32_t t_fine;
 uint8_t ctrl_meas_reg;
};

#endif
