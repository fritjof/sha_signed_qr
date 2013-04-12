const unsigned char neccblk1 = 2;
const unsigned char neccblk2 = 0;
const unsigned char datablkw = 24;
const unsigned char eccblkwid = 26;
const unsigned char VERSION = 4;
const unsigned char ECCLEVEL = 3;
const unsigned char WD = 33;
const unsigned char WDB = 5;
unsigned char strinbuf[165];
unsigned char qrframe[600];
unsigned char rlens[34];
#ifndef __AVR__
#define PROGMEM
#define memcpy_P memcpy
#define __LPM(x) *x
#else
#include <avr/pgmspace.h>
#endif
const unsigned char framebase[] PROGMEM = {
0xfe,0x00,0x00,0x3f,0x80,
0x82,0x00,0x00,0x20,0x80,
0xba,0x00,0x00,0x2e,0x80,
0xba,0x00,0x00,0x2e,0x80,
0xba,0x00,0x00,0x2e,0x80,
0x82,0x00,0x00,0x20,0x80,
0xfe,0xaa,0xaa,0xbf,0x80,
0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x00,0xf8,0x00,
0x00,0x80,0x00,0x88,0x00,
0xfe,0x00,0x00,0xa8,0x00,
0x82,0x00,0x00,0x88,0x00,
0xba,0x00,0x00,0xf8,0x00,
0xba,0x00,0x00,0x00,0x00,
0xba,0x00,0x00,0x00,0x00,
0x82,0x00,0x00,0x00,0x00,
0xfe,0x00,0x00,0x00,0x00,
};

const unsigned char framask[] PROGMEM = {
0xff,0xff,0xff,0xff,0xff,
0xf8,0x10,0x04,0x00,0x80,
0x08,0x00,0x40,0x01,0x00,
0x02,0x00,0x02,0x00,0x01,
0x00,0x00,0x40,0x00,0x08,
0x00,0x00,0x80,0x00,0x04,
0x00,0x00,0x10,0x00,0x00,
0x20,0x00,0x00,0x20,0x00,
0x0f,0xfc,0x00,0x07,0xff,
0x00,0x01,0xff,0xe0,0x00,
0x3f,0xfe,0x00,0x03,0xff,
0xf0,0x00,0x00,0x7f,0xc0,
0x00,0x00,0xff,0x80,0x00,
0x00,0xff,0x80,0x00,0x00,
0x00,
};
