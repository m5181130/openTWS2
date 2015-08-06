#include <I2C_PROTOCOL.h>
#include <Wire.h>

I2C_PROTOCOL::I2C_PROTOCOL(uint8_t addr)
 : _addr(addr)
{}

bool I2C_PROTOCOL::begin()
{
 Wire.begin();
 Wire.beginTransmission(_addr);
 if ( Wire.endTransmission() != 0 ) return false;
 return true;
}

void I2C_PROTOCOL::readByte(uint8_t sreg, uint8_t data)
{
 readBytes(sreg, sreg, &data);
}

void I2C_PROTOCOL::writeByte(uint8_t sreg, uint8_t data)
{
 writeBytes(sreg, sreg, &data);
}

void I2C_PROTOCOL::readBytes(uint8_t sreg,uint8_t treg, uint8_t *data)
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

void I2C_PROTOCOL::writeBytes(uint8_t sreg,uint8_t treg, uint8_t *data)
{
 int i=0;
 int len = treg-sreg+1;
 Wire.beginTransmission(_addr);
 Wire.write(sreg);
 while (i<len)
  Wire.write(data[i++]);

 Wire.endTransmission();
}


