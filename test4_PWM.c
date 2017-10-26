//PUTS

#define F_CPU 118916UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


double doublec=0;

int main()
{
	DDRD = 0b00100000;
	TCCR1A = 0b10000001;
	TCCR1B = 0x05;
	TIMSK=(1<<TOIE1);

	OCR1A=(doublec/100.0)*255.0;
	sei();

	while (1) {
		_delay_ms(100);
		doublec+=10;

		if (doublec>100) {
			doublec = 0;}
	}
}

ISR(TIMER1_OVF_vect)
{
	OCR1A=(doublec/100.0)*255.0;
}
