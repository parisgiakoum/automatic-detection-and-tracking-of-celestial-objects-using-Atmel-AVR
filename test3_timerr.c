
#include <avr/io.h>
 
#define F_CPU 118916UL

 
int main()
{
	DDRB = 0xFF;		// Configure port B as output
 	
	PORTB=0xFF;
	OCR1A=2323;
	OCR1B=4646;
	TIMSK=0b01000000;
	TCCR1B = 0x04; 		//1024 div

	while(1)
	{		
		if (!(TIFR & 0b01000000) == 0) {
			PORTB=0xFE;
			TIFR=0b01000000;
		}
		if (!(TIFR & 0b00100000)==0) {
			PORTB=0xFF;
			TIFR=0b0010000;
		}
	}
 }
