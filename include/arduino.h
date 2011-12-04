#ifndef _ARDUINO_LIB_H_
#define _ARDUINO_LIB_H_

#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>

/* Type */
#define DIGIT	0
#define ANALOG	1

/* Mode */
#define ON	1
#define OFF	0
#define HIGH	1
#define LOW	0

#define INPUT	0x0
#define OUTPUT	0x1

#define RAW_OK	0x00

#define A0 54
#define A1 55
#define A2 56
#define A3 57
#define A4 58
#define A5 59
#define A6 60
#define A7 61
#define A8 62
#define A9 63
#define A10 64
#define A11 65
#define A12 66
#define A13 67
#define A14 68
#define A15 69

#define sendSerialRaw(fd,ptr,size) write(fd,ptr,size)
#define getSerialRaw(fd,ptr,size)  read(fd,ptr,size)

typedef unsigned char	uchar;
typedef unsigned int	uint;

/*
	== Output ==
	{ '\xA0', 3 },  pinSet( pin, type, mode ) 
	{ '\xA1', 2 },  pinGet( pin, type )
	{ '\xA2', 3 },  tone( pin, freq, duration ) 
	{ '\xA3', 1 },  noTone( pin ) 
	{ '\xA4', 2 },  writeMem( val, size )
	{ '\xA5', 2 },  readMem( val, size )
	{ '\xA6', 1 },  goSleep( delay )
	{ '\xA7', 2 },  pinMode( pin, mode )
	{ '\xA8', 1 },  setSerialDelay( delay )
	{ '\xA9', 0 },  setStandBy( )
	== Input ==
*/

char* raw_errors[] = {
	"Success",
	"Generic error",
};

/* Low level function */

int  initSerial( const char*, int );
void closeSerial( void );
void _sleep ( float );

/* Arduino function */

uint pinSet( uint, uint, uint );
uint pinGet( uint, uint );
void tone( uint, uint, uint );
void noTone( uint );
void writeMem( void*, uint );
uchar* readMem( uint  );
uint goSleep( uint );
void resetAll( void );
void setStandBy( void );

#endif /*_ARDUINO_LIB_H_*/
