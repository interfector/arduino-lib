#include <stdio.h>
#include <arduino/arduino.h>

#define IPIN 13
#define OPIN 12

int
main( int argc, char** argv )
{
	int fd = initSerial("/dev/ttyUSB0", 9600 );
	uchar byte;
	int i = 0, j = 1;
	int state;

	if( fd < 0 )
		return -1;

//	setSerialDelay( 100 );
	pinMode( IPIN, INPUT );
	pinMode( OPIN, OUTPUT );
/*
	while(1)
	{
		i += j;

		pinSet( OPIN, ANALOG, i );

		state = pinGet( IPIN, DIGIT );

		if( state == HIGH )
			break;

		if( i == 255 )
			j = -1;

		if(i == 0 )
			j = 1;
	}
*/	

	while(1)
	{
		i = pinGet( IPIN, DIGIT );

		_sleep(0.1);

		if( i == HIGH)
			break;
	}

	while(1)
	{
		for(i = 0;i <= 255;i++)
		{
			pinSet( OPIN, ANALOG, i );
			_sleep( 0.02 );
		}

		for(i = 255;i > 0;i--)
		{
			pinSet( OPIN, ANALOG, i );
			_sleep( 0.02 );
		}
	}
	
	closeSerial();

	return 0;
}
