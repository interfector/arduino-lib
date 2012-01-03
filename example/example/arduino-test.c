#include <stdio.h>
#include <arduino/arduino.h>

//#define TEST1

#define IPIN 13
#define OPIN 12

int
main( int argc, char** argv )
{
	int fd = initSerial("/dev/ttyUSB0", 115200 );
	uchar byte;
	int i = 0, j = 1;
	int state;

	if( fd < 0 )
		return -1;

	pinMode( IPIN, INPUT );
	pinMode( OPIN, OUTPUT );

#ifdef TEST1
	pinSet( OPIN, ANALOG, 255 );

	while(1)
	{
		state = pinGet( IPIN, DIGIT );

		if( state == HIGH )
			break;
	}

	pinSet( OPIN, ANALOG, 0 );

#else

	while(1)
	{
		i = pinGet( IPIN, DIGIT );

		if( i == HIGH)
			break;
	}

	_sleep(0.05);

	while(1)
	{
		for(i = 0;i <= 255;i++)
		{
			state = pinGet( IPIN, DIGIT );

			if( state == HIGH )
				goto exit;
				

			pinSet( OPIN, ANALOG, i );
		}

		for(i = 255;i > 0;i--)
		{
			state = pinGet( IPIN, DIGIT );

			if( state == HIGH )
				goto exit;

			pinSet( OPIN, ANALOG, i );
		}

	}
#endif

exit:

	closeSerial();

	return 0;
}
