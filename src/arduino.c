#include <stdio.h>
#include <arduino.h>
#include <time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>

int serial_fd = -1;

/* Arduino Function */

uint
pinSet( uint pin, uint type, uint mode )
{
	uchar bytes[4];
	uchar result = -1;

	bytes[0] = '\xA0';    /* pinSet */
	bytes[1] = (char)pin; /* pinSet */
	bytes[2] = (char)type;/* pinSet */
	bytes[3] = (char)mode;/* pinSet */

#ifdef _DEBUG
	printf("%x %x %x %x\n", bytes[0], bytes[1], bytes[2], bytes[3] );
#endif

	(void)sendSerialRaw( serial_fd, bytes, 4 );

	(void)getSerialRaw( serial_fd, &result, 1 );

	if( result < 0 )
	{
		perror("getSerialRaw");
		exit(1);
	}

	return result;
}

uint
pinGet( uint pin, uint type )
{
	uchar bytes[3];
	int result = 0;

	bytes[0] = '\xA1';    /* pinGet */
	bytes[1] = (char)pin; /* pinGet */
	bytes[2] = (char)type;/* pinGet */

	(void)sendSerialRaw( serial_fd, bytes, 3 );

	(void)getSerialRaw( serial_fd, &result, 2 );

#ifdef _DEBUG
	printf("result: %x\n", result );
#endif

	return result;
}

void
tone( uint pin, uint freq, uint duration )
{
	uchar bytes[4];

	bytes[0] = '\xA2';        /* tone */
	bytes[0] = (char)pin;     /* tone */
	bytes[0] = (char)freq;    /* tone */
	bytes[0] = (char)duration;/* tone */

	(void)sendSerialRaw( serial_fd, bytes, 4 );
}

void
noTone( uint pin )
{
	uchar bytes[2];

	bytes[0] = '\xA3';    /* noTone */
	bytes[1] = (char)pin; /* noTone */

	(void)sendSerialRaw( serial_fd, bytes, 2 );
}

void
writeMem( void* ptr, uint size )
{
	uchar bytes[2];

	bytes[0] = '\xA4';     /* writeMem */
	bytes[1] = (char)size; /* writeMem */

	(void)sendSerialRaw( serial_fd, bytes, 2 );

	_sleep( 0.02 );

	(void)sendSerialRaw( serial_fd, ptr, size );
}

uchar*
readMem( uint size )
{
	uchar bytes[2];
	uchar* result; /* [0xff] */

	bytes[0] = '\xA5';     /* readMem */
	bytes[1] = (char)size; /* readMem */

	(void)sendSerialRaw( serial_fd, bytes, 2 );

	_sleep( 0.02 );

	result = malloc( size );

	(void)getSerialRaw( serial_fd, result, size );

	return result;
}

uint
goSleep( uint delay )
{
	uchar bytes[3];
	uint ret;

	bytes[0] = '\xA6';     /* goSleep */
	bytes[1] = (char)((delay & 0xFF00) >> 8); /* goSleep */
	bytes[2] = (char)(delay & 0x00FF); /* goSleep */

	(void)sendSerialRaw( serial_fd, bytes, 3 );

	_sleep( 0.02 );

	(void)getSerialRaw( serial_fd, &ret, 2 );

	return ret;
}

void
pinMode( uint pin, uint mode )
{
	uchar bytes[3];

	bytes[0] = '\xA7';    /* pinMode */
	bytes[1] = (char)pin; /* pinMode */
	bytes[2] = (char)mode;/* pinMode */

	(void)sendSerialRaw( serial_fd, bytes, 3 );
}

void
setSerialDelay( uint delay )
{
	uchar bytes[2];

	bytes[0] = '\xA8';     /* setSerialDelay */
	bytes[1] = (char)delay;/* setSerialDelay */

	(void)sendSerialRaw( serial_fd, bytes, 2 );
}

void
resetAll( )
{
	int i;

	for(i = 0;i <= 54;i++)
		pinSet( i, DIGIT, LOW );
}

void
setStandBy( )
{
	uchar byte;

	byte = '\xA9';

	(void)sendSerialRaw( serial_fd, &byte, 1 );
}

/* Low level function */

/* Getted from internet */
int
initSerial( const char* serialport, int baud )
{
    struct termios toptions;
    int fd;
    speed_t brate = baud;
    
    fd = open(serialport, O_RDWR | O_NOCTTY/* | O_NDELAY */);

    if (fd < 0)
    {
        perror("serial_init");
        return -1;
    }
    
    if (tcgetattr(fd, &toptions) < 0)
    {
        perror("serial_init");
        return -1;
    }

    switch(baud)
    {
	    case 4800:
		    brate=B4800;
		    break;
	    case 9600:
		    brate=B9600;
		    break;
	    case 38400:
		    brate=B38400;
		    break;
	    case 57600:
		    brate=B57600;
		    break;
	    case 115200:
		    brate=B115200;
		    break;
    }

    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;
    
    if( tcsetattr(fd, TCSANOW, &toptions) < 0)
    {
        perror("serial_init");
        return -1;
    }

    serial_fd = fd;

    return fd;
}

void
closeSerial(  )
{
	close( serial_fd );
}

void
_sleep (float seconds)
{
    struct timeval timeout = {
        (long) seconds,
        (long) ((seconds - (long) seconds) * 1000000)
    };

    select(NULL, NULL, NULL, 0, &timeout);
}

int
map( int val, int min, int max, int min2, int max2 )
{
	return (((max2-min2)*val)/(max-min))+min2;
}
