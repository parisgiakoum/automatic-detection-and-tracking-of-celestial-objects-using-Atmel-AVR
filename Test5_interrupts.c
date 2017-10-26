

#define F_CPU 118916UL

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_OVF_vect){
PORTB ^= 0xFF;
TCNT1=65304;
}


int main(){

DDRB = 0xFF;
PORTB=0x00;
sei();
TIMSK |= (1<<TOIE1);
TCCR1B=0x05;
TCNT1=65304;

while(1){
}



}
