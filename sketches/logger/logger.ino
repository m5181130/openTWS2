#include <OpenTWS.h>
#include <GPS.h>
#include <Parser.h>
#include <Wire.h>
#include <PCF8523.h> 
#include <Sensor.h>
#include <savepower.h>
#include <SdFat.h>
#include <Time.h>

NMEA nmea(&Serial);
BINR binr(&Serial1);
Parser parser;

TMP102 tmp102;
HIH9131 hih9131;
ADT7420 adt7420;
BMP180 bmp180;

PCF8523 rtc;

const uint8_t spiSpeed = SPI_HALF_SPEED;
const int chipSelect = SD_CS;
SdFile file;
SdFat sd;

int led = led1;
int redled = led2;

int sample_N = 3; // 1008; //6 * 24 * 7; // for 7 days
int n = 0;

char fname[13];

tmElements_t tm;

void setup() {
 pinMode(led, OUTPUT);
 pinMode(redled, OUTPUT);
 setSleepInterrupt();
 
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
  
 if (!sd.begin(chipSelect, spiSpeed))
  blinkLED(redled, 5);
 
 // receive the current time from the gps.
 nmea.requestRMC();
 while (!parser.parse( nmea.read() )) ;
 nmea.requestClear();
 parser.crack_datetime(tm);
 rtc.set(makeTime(tm) + 9 * SECS_PER_HOUR);
 
 // set and enable timer A
 rtc.enableTmrA(0x01, RTC_TQM);
 
 bool err = false;
 while (n < sample_N)
 {
  /* there are intervals in real observation.
  while ( !rtc.getTmrAFlag() ) {
   digitalWrite(led, HIGH);
   delay(50);
   digitalWrite(led, LOW);   
   enterSleep(); // sleep 8.0 sec
  }
  rtc.clearTmrAFlag();
  // */
  
  err |= !log_sensor_val();
  err |= !log_gps_data();
  
  if (err)
   blinkLED(redled, 6);

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
 double h0;
 double t0,t1,t2;
 
 if (!rtc.read(tm)) return false;
 
 file.open("sensor.log", O_WRITE | O_APPEND | O_CREAT);
 int i;
 for (i=0;i<5;i++)
 {
  hih9131.read(h0, t0);
  tmp102.read(t1);
  adt7420.read(t2);
  file.print(n);  file.write(',');
  file.print(t2); file.write(',');
  file.print(t1); file.write(',');
  file.print(t0); file.write(',');
  file.print(h0); file.write(',');
  file.print(tm.Hour);   file.write(',');
  file.print(tm.Minute); file.write(',');
  file.print(tm.Second); file.write(',');
  file.print(tm.Day);    file.write(',');
  file.println();
 }
 file.close();
 
 return true;
}

bool log_gps_data()
{
 if (!nmea.requestWake()) return false;
 delay(60000); // ttff

 binr.requestClear();
 binr.flush_input();
 binr.requestF5();
 
 sprintf(fname,"%d.bin",n);
 file.open(fname, O_WRITE | O_TRUNC | O_CREAT);
 
 rtc.enableTmrB(0x06, RTC_TQS);
 while ( !rtc.getTmrBFlag() ) {
  while ( binr.available() )
   file.write( binr.read() );
 }
 rtc.clearTmrBFlag();
 rtc.disableTmrB();
 // bool err = (file.fileSize() < 1024) ? true : false;
 file.close();
 // if (err) return false;
 
 if (!nmea.requestSleep()) return false;
 
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

