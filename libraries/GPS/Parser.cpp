
#include "Parser.h"

Parser::Parser()
 : _term_offset(0)
 , _term_number(0)
 , _time(0)
 , _date(0)
 , _checksum(0)
 , _inpayload(false)
 , _sentence_type(_GPS_OTHER)
 , _valid_sentence(false)
{
 _term[0] = '\0';
}

bool Parser::parse(char c)
{
 if (c < 0) return false;

 switch (c)
 {
  case ',':
   _checksum ^= c;
   _term[_term_offset] = '\0';
   term_complete();
   _term_number++;
   _term_offset = 0;
   break;
  case '*':
   _inpayload = false;
   break;
  case '\r':
   break;
  case '\n':
   if (_valid_sentence)
   return _checksum & (htoi(_term[1]) << 4) + htoi(_term[2]);
  case '$':
   _term_offset = _term_number = 0;
   _sentence_type = _GPS_OTHER;
   _checksum = 0;
   _inpayload = true;
   _valid_sentence = false;
   break;
  default:
   if ( _inpayload ) _checksum ^= c;
   _term[_term_offset++] = c;
   break;
 }

 return false;
}

#define COMBINE(sentence_type, term_number) (((sentence_type) << 5) | term_number)

bool Parser::term_complete()
{
 if (_term_number == 0)
 {
  if (strcmp(_term, "GPRMC") == 0)
   _sentence_type = _GPS_GPRMC;
 }

 switch( COMBINE(_sentence_type, _term_number) )
 {
  case COMBINE(_GPS_GPRMC, 1):
   _time = atol(_term);
   break;
  case COMBINE(_GPS_GPRMC, 2):
   _valid_sentence = _term[0] == 'A' ? true : false;
   break;
  case COMBINE(_GPS_GPRMC, 9):
   _date = atol(_term);
   break;
 }

 return false;
}

void Parser::crack_datetime(tmElements_t &tm)
{
 tm.Year   = y2kYearToTm( _date % 100 );
 tm.Month  = (_date / 100) % 100;
 tm.Day    = _date / 10000;
 tm.Hour   = _time/10000;
 tm.Minute = (_time/100) % 100;
 tm.Second = _time % 100;
}

long Parser::atol(const char *str)
{
 long ret = 0;
 while (isdigit(*str))
  ret = (10 * ret) + (*str++ - '0');
 return ret;
}

int Parser::htoi(const char h)
{
 if (isdigit(h))
  return h - '0';
 else if (isxdigit(h))
  return toupper(h) - 'A' + 10;
 else
  return -1;
}

