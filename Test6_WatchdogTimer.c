

#define F_CPU 118916UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>


int main(){
DDRB = 0xFF;
PORTB=0x00;

WDTCR=0b00011111;	//Enables Watchdog

while(1){


//_delay_ms(2500);

//wdt_reset();	//resets to all zero

PORTC=0x00;

	if(WDTCR==0b00011111){		// disables Watchdog
	WDTCR=0b00011111;
	WDTCR= (0<<WDE);
	}
}


}
