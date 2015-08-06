#ifndef I2C_PROTOCOL_h
#define I2C_PROTOCOL_h

#include <Arduino.h>

class I2C_PROTOCOL
{
public:
 I2C_PROTOCOL(uint8_t addr);
 bool begin();

protected:
 void readByte(uint8_t sreg, uint8_t data);
 void writeByte(uint8_t sreg, uint8_t data);
 void readBytes(uint8_t sreg, uint8_t treg, uint8_t *data);
 void writeBytes(uint8_t sreg, uint8_t treg, uint8_t *data);

 uint8_t _addr;
};
#endif
