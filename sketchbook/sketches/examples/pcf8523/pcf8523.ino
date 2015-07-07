/**
 test for PCF8523.
 Datasheet
 http://www.nxp.com/documents/data_sheet/PCF8523.pdf
 ! PCF8523 provides interrupt function but it is not 
   electrically wired to the chip (PCINT19).
**/

#include <SoftwareSerial.h>
#define RX_DEBUG_PIN A0
#define TX_DEBUG_PIN A1
SoftwareSerial portA(RX_DEBUG_PIN, TX_DEBUG_PIN);

#include <Wire.h>

// Register Control_1
#define RTC_CAP_SEL 7
#define RTC_STOP    5
#define RTC_SR      4
#define RTC_12_24   3
#define RTC_SIE     2
#define RTC_AIE     1
#define RTC_CIE     0

// Register Control_2
#define RTC_WTAF    7
#define RTC_CTAF    6
#define RTC_CTBF    5
#define RTC_SF      4
#define RTC_AF      3
#define RTC_WTAIE   2
#define RTC_CTAIE   1
#define RTC_CTBIE   0

// Register Control_3
#define RTC_PM2     7
#define RTC_PM1     6
#define RTC_PM0     5
#define RTC_BSF     3
#define RTC_BLF     2
#define RTC_BSIE    1
#define RTC_BLIE    0

// Oscillator STOP flag
#define RTC_OS 7

// Minute_alarm
#define AEN_M 7

// Hour_alarm
#define AEN_H 7
#define AMPM  5

// Day_alarm
#define AEN_D 7

// Weekday_alarm
#define AEN_W 7

//-------------------------------------

uint8_t RC_1 = _BV(RTC_CAP_SEL) * 0
             | _BV(RTC_STOP)    * 0
             | _BV(RTC_SR)      * 0
             | _BV(RTC_12_24)   * 0
             | _BV(RTC_SIE)     * 0 // second interrupt disabled = 0
             | _BV(RTC_AIE)     * 1 // alarm interrupt disabled = 0
             | _BV(RTC_CIE)     * 0 ;

uint8_t RC_2 = _BV(RTC_WTAF)  * 0 // no watchdog timer A interrupt generated = 0
             | _BV(RTC_CTAF)  * 0 // no countdown timer A interrupt generated = 0
             | _BV(RTC_CTBF)  * 0 // no countdown timer B interrupt generated
             | _BV(RTC_SF)    * 0 // no second interrupt generated = 0
             | _BV(RTC_AF)    * 0 // no alarm interrupt generated = 0
             | _BV(RTC_WTAIE) * 0 // watchdog timer A interrupt is disabled = 0
             | _BV(RTC_CTAIE) * 0 // countdown timer A interrupt is disabled = 0
             | _BV(RTC_CTBIE) * 0 ; // countdown timer B interrupt is disabled = 0

uint8_t RC_3 = _BV(RTC_PM2)  * 1 // Power management 
             | _BV(RTC_PM1)  * 1 // Power management 
             | _BV(RTC_PM0)  * 1 // Power management 
             | _BV(RTC_BSF)  * 0 
             | _BV(RTC_BLF)  * 0 
             | _BV(RTC_BSIE) * 0
             | _BV(RTC_BLIE) * 0 ;

// all alarms are disabled.
uint8_t minute_alarm  = _BV(AEN_M) | dtob(0) ;
uint8_t hour_alarm    = _BV(AEN_M) | dtob(0) ;
uint8_t day_alarm     = _BV(AEN_D) | dtob(1) ;
uint8_t weekday_alarm = _BV(AEN_W) | dtob(0) ;

void setup()
{
 portA.begin(9600);
 Wire.begin();

 uint8_t *p = new uint8_t[20];

 // Control registers
 p[0] = RC_1;
 p[1] = RC_2;
 p[2] = RC_3;

 // Time and date registers
 p[3] = 50; // sec 0-59
 p[4] = 59; // minutes 0-59
 p[5] = 23; // hours 1-12 or 0-23
 p[6] = 31; // days 1-31
 p[7] = 6;  // weekdays 0-6
 p[8] = 5;  // months 1-12
 p[9] = 15; // years 0-99

 // Alarm registers
 p[10] = minute_alarm;
 p[11] = hour_alarm;
 p[12] = day_alarm;
 p[13] = weekday_alarm;

 // Offset register
 p[14] = 0;

 // CLOCKOUT and timer registers
 p[15] = 0b00111011;
 p[16] = 0b00000010; // 1 Hz
 p[17] = 0x0A; // T_A[7:0] timer value 00 to FF
 p[18] = 0b00000010;
 p[19] = 0x03;

 //set_date(p);
 delete[] p;
}

void loop()
{
 print_date();
 //clear_AF();
 clear_CTAF();
 clear_CTBF();
 delay(1000);
}

//-------------------------------------

uint8_t btod(uint8_t bcd) {
  return (bcd & 0x0f) + 10 * ((bcd >> 4) & 0x0f);
}

uint8_t dtob(uint8_t decimal) {
  uint8_t tens = decimal / 10;
  return (tens << 4) + ((decimal - tens * 10) & 0x0f);
}

#define PCF8523_I2C_ADDRESS 0xD1

void set_date(uint8_t *p)
{
  Wire.beginTransmission(PCF8523_I2C_ADDRESS);  
  Wire.write(0);
  Wire.write(p[0]);
  Wire.write(p[1]);
  Wire.write(p[2]);
  Wire.write(dtob(p[3]));
  Wire.write(dtob(p[4]));
  Wire.write(dtob(p[5]));
  Wire.write(dtob(p[6]));
  Wire.write(dtob(p[7]));
  Wire.write(dtob(p[8]));
  Wire.write(dtob(p[9]));
  Wire.write(p[10]);
  Wire.write(p[11]);
  Wire.write(p[12]);
  Wire.write(p[13]);
  Wire.write(p[14]);
  Wire.write(p[15]);
  Wire.write(p[16]);
  Wire.write(p[17]);
  Wire.write(p[18]);
  Wire.write(p[19]);
  Wire.endTransmission();
}

void get_date(uint8_t *p)
{
  Wire.beginTransmission(PCF8523_I2C_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.requestFrom(PCF8523_I2C_ADDRESS, 20);
  int i;
  for (i=0;i<20;i++)
   p[i] = Wire.read();
}

void print_date()
{
 uint8_t hi;
 uint8_t lo;
 uint8_t *p = new uint8_t[20];
 get_date(p);

 portA.print(p[0], BIN); portA.print(" ");
 portA.print(p[1], BIN); portA.print(" ");
 portA.print(p[2], BIN); portA.print(" ");
 portA.print(btod(p[3] & 0x7F), DEC); portA.print(" ");
 portA.print(btod(p[4] & 0x7F), DEC); portA.print(" ");
 portA.print(btod(p[5] & 0x3F), DEC); portA.print(" ");
 portA.print(btod(p[6] & 0x3F), DEC); portA.print(" ");
 portA.print(btod(p[7] & 0x07), DEC); portA.print(" ");
 portA.print(btod(p[8] & 0x1F), DEC); portA.print(" ");
 portA.print(btod(p[9] & 0xFF), DEC); portA.print(" ");
 hi = btod( (p[10] & 0b01110000) >> 4);
 lo = btod(  p[10] & 0b00001111 );
 portA.print( hi * 10 + lo, DEC); portA.print(" ");
 hi = btod( (p[11] & 0b00110000) >> 4);
 lo = btod(  p[11] & 0b00001111 );
 portA.print( hi * 10 + lo, DEC); portA.print(" ");
 hi = btod( (p[12] & 0b00110000) >> 4);
 lo = btod(  p[12] & 0b00001111 );
 portA.print( hi * 10 + lo, DEC); portA.print(" ");
 hi = 0;
 lo = btod(  p[13] & 0b00000111 );
 portA.print( hi * 10 + lo, DEC); portA.print(" ");
 portA.print(p[14], BIN); portA.print(" ");
 portA.print(p[15], BIN); portA.print(" ");
 portA.print(p[16], BIN); portA.print(" ");
 portA.print(p[17], BIN); portA.print(" ");
 portA.print(p[18], BIN); portA.print(" ");
 portA.print(p[19], BIN); portA.print(" ");    

 portA.println();
 delete[] p;
}

bool clear_AF() {
 bool flag = false;
 
 Wire.beginTransmission(PCF8523_I2C_ADDRESS);  
 Wire.write(0x01);
 Wire.endTransmission();
 
 Wire.requestFrom(PCF8523_I2C_ADDRESS, 1);
 flag = ((Wire.read() & _BV(RTC_AF) ) >> RTC_AF) & 0b1 ? true : false ; 
 
 if (flag) {
  Wire.beginTransmission(PCF8523_I2C_ADDRESS);
  Wire.write(0x01);
  Wire.write(0b01110000);
  Wire.endTransmission();
 }
 return flag;
}

bool clear_CTAF() {
 bool flag = false;
 
 Wire.beginTransmission(PCF8523_I2C_ADDRESS);  
 Wire.write(0x01);
 Wire.endTransmission();
 
 Wire.requestFrom(PCF8523_I2C_ADDRESS, 1);
 flag = (Wire.read() & _BV(RTC_CTAF) ) ? true : false ; 
 
 if (flag) {
  Wire.beginTransmission(PCF8523_I2C_ADDRESS);
  Wire.write(0x01);
  Wire.write(0b00111000);
  Wire.endTransmission();
 }
 return flag;
}

bool clear_CTBF() {
 bool flag = false;
 
 Wire.beginTransmission(PCF8523_I2C_ADDRESS);  
 Wire.write(0x01);
 Wire.endTransmission();
 
 Wire.requestFrom(PCF8523_I2C_ADDRESS, 1);
 flag = (Wire.read() & _BV(RTC_CTBF) ) ? true : false ; 
 
 if (flag) {
  Wire.beginTransmission(PCF8523_I2C_ADDRESS);
  Wire.write(0x01);
  Wire.write(0b01011000);
  Wire.endTransmission();
 }
 return flag;
}
