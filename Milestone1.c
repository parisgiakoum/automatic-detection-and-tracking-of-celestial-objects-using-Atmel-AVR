
#include <avr/io.h>
 
#define F_CPU 1228800UL
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

ISR(INT0_vect){
	PORTB=0xFB;		//OPEN LED 2
	TCCR1B = (1<<CS12) | (1<<CS10);		//Prescaler 1024, Start Counter
	wdt_reset();          
}



ISR(INT1_vect){
	TCCR1B = 0;		//Close simple Counter
	PORTB=0xFB;		//Open Led 2
	WDTCR=0x0F;		//Enable watchdog, 2 second reset. Can be delayed stopped with Int0!
}

ISR(TIMER1_COMPA_vect){
	PORTB =0xFF;			//If INT0 is pressed, after 1 second the Led closes
	//TIMSK = (0<<OCIE1A);
	//TCNT1 = 0;
	
}

//ISR(TIMER1_COMPB_vect){
//	PORTB =0x00;
//}

//AFTER ISR(COMP), TIMER STOPS AND RESETS ?



int main(){
	DDRB = 0xFF;		// Configure port B as output
	DDRD = 0x00;	
	PORTD=0x00;
	sei();

	TIMSK = (1<<OCIE1A) | (1<<OCIE1B);
	TCNT1 = 0;

	GIMSK=0b11000000;

	OCR1A = 1200;	//1 second
	//OCR1B = 1800;

	MCUCR=0x0F;			//Interrupt at rising edge
	
	
	while(1){
		if((~PIND) & 0b00000001==1 ){
			PORTB=0xFF;
		}
	}
}
