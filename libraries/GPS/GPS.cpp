
#include "GPS.h"

GPS::GPS(HardwareSerial *serial)
 : _hardSerial(serial)
{
 pinMode(23, INPUT); // GPS SLEEP FLAG PIN
}

bool GPS::begin(long baud, uint8_t config)
{
 _hardSerial->begin(baud, config);
 return isConnected();
}

bool GPS::isConnected()
{
 bool connected = false;
 uint8_t buf;
 _hardSerial->setTimeout(1000);

 testRequest();
 if ( _hardSerial->readBytes(&buf,1) )
  connected = true;

 return connected;
}

void GPS::flush_input()
{
 while ( _hardSerial->available() )
  _hardSerial->read();
}

int GPS::read()
{
 return _hardSerial->read();
}

int GPS::available()
{
 return _hardSerial->available();
}

inline bool GPS::isSleeping()
{
 return digitalRead(23) ? false : true;
}

// NMEA CLASS ----------------------
NMEA::NMEA(HardwareSerial *serial)
 : GPS(serial)
{}

bool NMEA::testRequest()
{
 requestRMC();
}

void NMEA::requestClear()
{
 _hardSerial->write( NMEA_CLEAR );
}

bool NMEA::requestSleep()
{
 _hardSerial->write( NMEA_SLEEP );
 delay(1000); 

 return isSleeping();
}

bool NMEA::requestWake()
{
 _hardSerial->write( 0 );
 delay(1000);

 return !isSleeping();
}

void NMEA::requestRMC()
{
 _hardSerial->write( NMEA_RMC );
}

// BINR CLASS ----------------------
BINR::BINR(HardwareSerial *serial)
 : GPS(serial)
{}

bool BINR::testRequest()
{
 requestF5();
}

bool BINR::requestF5()
{
 _hardSerial->write( BINR_F4 );
}

void BINR::requestClear()
{
 _hardSerial->write( BINR_CLEAR );
}

