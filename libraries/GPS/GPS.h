#ifndef GPS_h
#define GPS_h

#include <Arduino.h>

static const char *NMEA_RMC = "$PORZB,RMC,1*38\r\n";
static const char *NMEA_SLEEP = "$POPWR,1111*66\r\n";
static const char *NMEA_CLEAR = "$PORZB*55\r\n";
static const char *BINR_F4    = "\x10\xF4\x0A\x10\x03";
static const char *BINR_CLEAR = "\x10\x0E\x10\x03";

class GPS
{
public:
 GPS(HardwareSerial *serial);
 bool begin(long baud, uint8_t config);
 int read();
 int available();
 void flush_input();
 bool isConnected();
 inline bool isSleeping();
protected:
 virtual bool testRequest()=0;
 HardwareSerial *_hardSerial;
};

class NMEA : public GPS
{
public:
 NMEA(HardwareSerial *serial);
 bool requestSleep();
 bool requestWake();
 void requestClear();
 void requestRMC();
private:
 bool testRequest();
};

class BINR : public GPS
{
public:
 BINR(HardwareSerial *serial);
 bool requestF5();
 void requestClear();
private:
 bool testRequest();
};

#endif
