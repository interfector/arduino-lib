#include <EEPROM.h>
#include <avr/power.h>
#include <avr/sleep.h>

#define DIGIT  0
#define ANALOG 1

#define RAW_OK 0x00
#define STD_DELAY 10

int fbyte;         // incoming serial byte
int dbytes[0xFF] = { 0 };
volatile int ndata;
volatile int tmp;
volatile int serial_delay = STD_DELAY;

struct ICode {
  int ndata;
  void (*function)(int*,int);
};

typedef struct ICode ICode;

ICode ICodes[] = {
  { 3, pinSet },
  { 2, pinGet },
  { 3, gTone  },
  { 1, nTone  },
  { 1, writeMem },
  { 1, readMem },
  { 2, goSleep },
  { 2, spinMode },
  { 1, setSDelay },
  { 0, setStandBy },
};

void setup() {      
  //Serial.begin(9600);
  Serial.begin(115200);
// establishContact();
}

void loop() {
    if (Serial.available() > 0) {
    // get incoming byte:
      serialReadBytes( &fbyte, 1 );
      
      ndata = ICodes[fbyte - 0xA0].ndata;
      
      if( ndata > 0 )
        serialReadBytes( dbytes, ndata );
      
      ICodes[fbyte - 0xA0].function( dbytes, ndata );
      
      Serial.flush();
    }
}

void
sleepNow()
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and 
     * wake up sources are available in which sleep modus.
     *
     * In the avr/sleep.h file, the call names of these sleep modus are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings 
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     *  the power reduction management <avr/power.h>  is described in 
     *  http://www.nongnu.org/avr-libc/user-manual/group__avr__power.html
     */  
     
  set_sleep_mode(SLEEP_MODE_IDLE);   // sleep mode is set here

  sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 
  
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();
  
  
  sleep_mode();            // here the device is actually put to sleep!!
 
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();         // first thing after waking from sleep:
                            // disable sleep...

  power_all_enable();
   
}

void establishContact() {
  while (Serial.available() <= 0) {
    delay(300);
  }
}

void serialReadBytes(int* bytes, int nsize)
{
  int i;
  
  while( Serial.available() > 0 && i < nsize )
  {
    bytes[i++] = Serial.read();
    delay(serial_delay);
  }
}

void
pinSet( int* dbytes, int ndata )
{  
  if( dbytes[1] == DIGIT ) /* Digital */
    digitalWrite( dbytes[0], dbytes[2] );
  else
    analogWrite( dbytes[0], dbytes[2] );
    
  Serial.print(RAW_OK,BYTE);
}

void
pinGet( int* dbytes, int ndata )
{
  if( dbytes[1] == DIGIT ) /* Digital */
  {
    Serial.print(digitalRead( dbytes[0] ), BYTE);
  } else {
    int val = analogRead( dbytes[0] );
    Serial.print( val & 0x00FF, BYTE );
    Serial.print( (val & 0xFF00) >> 8, BYTE );
/*    Serial.print(analogRead( dbytes[0] ) / 4, BYTE); */
  }
}

void
gTone( int* dbytes, int ndata )
{
  if( dbytes[2] == 0 ) /* Digital */
  {
    tone( dbytes[0], dbytes[1] );
  } else {
    tone( dbytes[0], dbytes[1], dbytes[2] );
  }
}

void
nTone( int* dbytes, int ndata )
{
  noTone( dbytes[0] );
}

void
writeMem( int* dbytes, int ndata )
{
  int i;
  int length = dbytes[0];
  
  serialReadBytes( dbytes, length );
  
  for(i = 0;i < length;i++)
    EEPROM.write( i, dbytes[i] );
}

void
readMem( int* dbytes, int ndata )
{
  int i;
  int length = dbytes[0];
  
  for(i = 0;i < length;i++)
    Serial.print( EEPROM.read( i ), BYTE );
}

void
goSleep( int* dbytes, int ndata )
{
  int val = 0;
  
  val = (dbytes[0] << 8) | dbytes[1];
  
  Serial.print( (val & 0xFF00) >> 8, BYTE );
  Serial.print( val & 0x00FF, BYTE );
  
  delay( val );
}

void
spinMode( int* dbytes, int ndata )
{
  pinMode( dbytes[0], dbytes[1] );
}

void
setSDelay( int* dbytes, int ndata )
{
  serial_delay = dbytes[0];
}

void
setStandBy( int* dbytes, int ndata )
{
  sleepNow();
}
