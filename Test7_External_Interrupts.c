

#define F_CPU 118916UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

int main(){
DDRB = 0xFF;
//PORTB=0xFF; //WHYYYYYYYYYYYYYYYYYY????

sei();

GIMSK=0b11000000;
MCUCR=0x0F;


while(1){
PORTC=0x00;
}


}

ISR(INT0_vect){
	PORTB=0xFF;
}


ISR(INT1_vect){
	PORTB=0x00;
}
