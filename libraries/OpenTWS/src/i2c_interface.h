#ifndef i2c_interface_h
#define i2c_interface_h

#include <Arduino.h>

class I2CI
{
public:
 I2CI(uint8_t addr);
 bool begin();

protected:
 void readByte(uint8_t sreg, uint8_t data);
 void writeByte(uint8_t sreg, uint8_t data);
 void readBytes(uint8_t sreg, uint8_t treg, uint8_t *data);
 void writeBytes(uint8_t sreg, uint8_t treg, uint8_t *data);

 uint8_t _addr;
};
#endif
