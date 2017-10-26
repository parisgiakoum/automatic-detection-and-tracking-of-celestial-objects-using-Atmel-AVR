
#include <avr/io.h>
 
#define F_CPU 118916UL
#include <util/delay.h>
 
int main()
{
	DDRB = 0b11111111;		// Configure port B as output
 	

	while(1)
	{
		PORTB = 0xFF;	// Turn OFF LEDs
		_delay_ms(1000); // Wait 250ms
		PORTB = 0xAA;	// Turn ON LEDs
		_delay_ms(1000);	// Wail 250ms
	}
 
	return 0;
}
