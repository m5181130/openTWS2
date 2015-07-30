#include <OpenTWS.h>
#include <GPS.h>
#include <Parser.h>
#include <Wire.h>
#include <PCF8523.h> 
#include <Sensor.h>
#include <SdFat.h>
#include <Time.h>

NMEA nmea(&Serial);
BINR binr(&Serial1);
Parser parser;

TMP102 tmp102;
HIH9131 hih9131;
ADT7420 adt7420;
BMP280 bmp280;

PCF8523 rtc;

tmElements_t tm;

const uint8_t spiSpeed = SPI_HALF_SPEED;
const int chipSelect = SD_CS;
SdFile file;
SdFat sd;
char fname[13];

int led = led1;
int redled = led2;

int sample_N = 5;
int n = 0;

void setup() {
 pinMode(led, OUTPUT);
 pinMode(redled, OUTPUT);
 
 // 1. connection check
 if (!nmea.begin(9600,SERIAL_8N1))
  blinkLED(redled, 1);
  
 if (!binr.begin(19200, SERIAL_8O1))
  blinkLED(redled, 2);
  
 if (!rtc.begin())
  blinkLED(redled, 3);
  
 if (!tmp102.begin())
  blinkLED(redled, 4);
  
 if (!hih9131.begin())
  blinkLED(redled, 4);
  
 if (!adt7420.begin())
  blinkLED(redled, 4);
 
 if (!bmp280.begin())
  blinkLED(redled, 4);
  
 if (!sd.begin(chipSelect, spiSpeed))
  blinkLED(redled, 5);
 
 // 2. receive current time from the gps.
 nmea.requestRMC();
 while (!parser.parse( nmea.read() )) ;
 parser.crack_datetime(tm);
 
 // 3. clear all request
 nmea.requestClear();
 binr.requestClear();
 
 // 4. set time
 rtc.set(makeTime(tm) + 9 * SECS_PER_HOUR);
 
 // 5. set timer
 while (tm.Second % 30) rtc.read(tm);
 rtc.enableTmrA(0x1E, RTC_TQS); // 30 sec
 
 // 6. start the logging cycle
 while (n < sample_N)
 {
  while ( !rtc.getTmrAFlag() ) ;
  rtc.clearTmrAFlag();
  
  digitalWrite(led, HIGH);
  
  if (!log_sensor_val())
   blinkLED(redled, 6);

  if (!log_gps_data())
   blinkLED(redled, 6);

  digitalWrite(led, LOW);
  n++;
 }
 
 while (true)
 {
  digitalWrite(led, !digitalRead(led));
  delay(300);
 }
}

void loop() {}

bool log_sensor_val()
{
 double p0;
 double h0;
 double t0,t1,t2,t3;
 
 if (!rtc.read(tm)) return false;
 
 file.open("sensor.log", O_WRITE | O_APPEND | O_CREAT);
 int i;
 for (i=0;i<5;i++)
 {
  tmp102.read(t0);
  adt7420.read(t1);
  hih9131.read(t2, h0);
  bmp280.read(t3, p0);
  
  file.print(n);  file.write(',');
  file.print(t0); file.write(',');
  file.print(t1); file.write(',');
  file.print(t2); file.write(',');
  file.print(t3); file.write(',');
  file.print(h0); file.write(',');
  file.print(p0); file.write(',');
  file.print(tm.Month);  file.write(',');
  file.print(tm.Day);    file.write(',');
  file.print(tm.Hour);   file.write(',');
  file.print(tm.Minute); file.write(',');
  file.print(tm.Second); file.write(',');
  file.println();
 }
 file.close();
 
 return true;
}

bool log_gps_data()
{
 sprintf(fname,"%d.bin",n);
 file.open(fname, O_WRITE | O_TRUNC | O_CREAT);
 
 // 1. clear buffer
 binr.requestClear();
 delay(100);
 binr.flush_input();
 
 // 2. request F5
 binr.requestF5();
 
 // 3. read binr message
 rtc.enableTmrB(0x06, RTC_TQS);
 while ( !rtc.getTmrBFlag() ) {
  while ( binr.available() )
   file.write( binr.read() );
 }
 rtc.clearTmrBFlag();
 rtc.disableTmrB();

 // 4. error check and file close
 bool err = (file.fileSize() < 1024) ? true : false;
 file.close();
 if (err) return false;
 
 return true;
}

void blinkLED(int led, int n) {
 int i;
 while (true) {
  for(i=0; i<n; i++) {
   digitalWrite(led, HIGH);
   delay(300);
   digitalWrite(led, LOW);
   delay(300);
  }
  delay(1000);
 }
}

