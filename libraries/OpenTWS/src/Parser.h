#ifndef PARSER_h
#define PARSER_h

#include <Time.h>
#include <string.h>
#include <ctype.h>

class Parser {
public:
 Parser();
 bool parse(char c);
 bool term_complete();
 void crack_datetime(tmElements_t &tm);

private:
 char _term[15];
 uint8_t _term_offset, _term_number;
 unsigned long _time, _date;
 uint8_t _checksum;
 bool _inpayload;
 uint8_t _sentence_type;
 bool _valid_sentence;
 enum { _GPS_GPRMC, _GPS_OTHER};

 long atol(const char *str);
 int htoi(const char h);
} ;

#endif
