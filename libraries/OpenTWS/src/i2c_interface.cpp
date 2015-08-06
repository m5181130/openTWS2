#include <i2c_interface.h>
#include <Wire.h>

I2CI::I2CI(uint8_t addr)
 : _addr(addr)
{}

bool I2CI::begin()
{
 Wire.begin();
 Wire.beginTransmission(_addr);
 if ( Wire.endTransmission() != 0 ) return false;
 return true;
}

void I2CI::readByte(uint8_t sreg, uint8_t data)
{
 readBytes(sreg, sreg, &data);
}

void I2CI::writeByte(uint8_t sreg, uint8_t data)
{
 writeBytes(sreg, sreg, &data);
}

void I2CI::readBytes(uint8_t sreg,uint8_t treg, uint8_t *data)
{
 int i=0;
 int len = treg-sreg+1;
 Wire.beginTransmission(_addr);
 Wire.write(sreg);
 Wire.endTransmission();
 
 Wire.requestFrom(_addr, len);
 while(Wire.available())
  data[i++] = Wire.read();
}

void I2CI::writeBytes(uint8_t sreg,uint8_t treg, uint8_t *data)
{
 int i=0;
 int len = treg-sreg+1;
 Wire.beginTransmission(_addr);
 Wire.write(sreg);
 while (i<len)
  Wire.write(data[i++]);

 Wire.endTransmission();
}


