/*************************************************************************************
University: 			Technical University of Crete
School:					School of Electrical & Computer Engineering
Course:					Embedded Systems - HRY 411
Professor:				A. Dollas
Authors: 				P. Giakoumakis, N. Ghionis, P.Portokalakis 
Create Date: 			27-10-2017
Project Name: 			Automatic Detection of Celestial Bodies with Telescope
Target Device:			ATMEL AVR AT90S8515 
Development Platform:	AVR Studio 4
Version:				0.2

Additional Comments:	The purpose of this file is to show the basic capabilities
						of an AVR microcontroller. For more info, check the report.
*************************************************************************************/

#define F_CPU 1228800UL 

#include <avr/io.h>  
#include <avr/interrupt.h> 
#include <util/delay.h> 
#include <avr/wdt.h> 


ISR(INT0_vect){ 
	PORTB=0xFB;									//OPEN LED 2 
	TCCR1B = (1<<CS12) | (1<<CS10)| (1<<CTC1); 	//Prescaler 1024, Start Counter 
	wdt_reset();          						//and clear on compare 
}


ISR(INT1_vect){ 
	TCCR1B = 0;									//Close simple Counter 
	PORTB=0xFB;									//Open Led 2 
	WDTCR=0x0F;									//Enable watchdog, 2 second reset. Can be delayed stopped with Int0! 
} 

 

ISR(TIMER1_COMPA_vect){ 
	PORTB =0xFF;								//If INT0 is pressed, after 1 second the Led closes 
	TCCR1B = 0;									//Stop Counter 
} 

 
void switchPolling(){

		if((~PIND) & (0b00000001==1) ){ 
			PORTB=0xFF;
			}

}

void init(){

	DDRB = 0xFF;								// Configure port B as output 
	DDRD = 0x00; 
	sei(); 
	TIMSK = (1<<OCIE1A) | (1<<OCIE1B); 
	GIMSK = 0b11000000; 
	OCR1A = 1200;								//1 second 
	MCUCR = 0x0F;									//Interrupt at rising edge

}




int main(){ 

	init();

	while(1){ 
		switchPolling();
	} 
	return 0;

}
