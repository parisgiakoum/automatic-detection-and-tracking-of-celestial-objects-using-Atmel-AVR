
#include <avr/io.h>
 
#define F_CPU 118916UL

 
int main()
{
	DDRB = 0xFF;		// Configure port B as output
 	DDRD = 0x00;
	PORTB=0x00;
	PORTD=0xFF;

	while(1)
	{
		PORTB=PIND;
	}
 
	return 0;
}


/*		if(0b11111111==~0b00000000){
			PORTB=0xFE;
		}
		else{
			PORTB=0xFF;
		}
*/
