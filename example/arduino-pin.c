#include <stdio.h>
#include <arduino/arduino.h>

int
main( int argc, char** argv )
{
	int fd = initSerial("/dev/ttyUSB0", 115200 );
	uchar byte;
	int i = 0, j = 1;
	int state;

	if( fd < 0 )
		return -1;

	if( argc < 4 )
		return -1;

	pinSet( atoi(argv[1]), (argv[2][0] != 'D'), atoi(argv[3]) );
	
	closeSerial();

	return 0;
}
