#define F_CPU 921600UL

#include <avr/io.h>
#include <util/delay.h>

void left();
void gimbal_transmit(int byte1,int byte2,int byte3,int byte4,int byte5,int byte6,int byte7);
void UART_Transmit(unsigned char data);


int main(){


	UCR = (1<<RXEN)|(1<<TXEN);
	UBRR= 12;
	while(1){
		left();
	}
}


void UART_Transmit(unsigned char data)
{
	// Wait for empty transmit buffer 
	while (!( USR & (1<<UDRE)));

	// Put data into buffer, sends the data 
	UDR = data;
}


void gimbal_transmit(int byte1,int byte2,int byte3,int byte4,int byte5,int byte6,int byte7)
{
	UART_Transmit(byte1);
	UART_Transmit(byte2);
	UART_Transmit(byte3);
	UART_Transmit(byte4);
	UART_Transmit(byte5);
	UART_Transmit(byte6);
	UART_Transmit(byte7);
	
	/*
	USART1_Transmit(byte1);
	USART1_Transmit(byte2);
	USART1_Transmit(byte3);
	USART1_Transmit(byte4);
	USART1_Transmit(byte5);
	USART1_Transmit(byte6);
	USART1_Transmit(byte7);	
	*/
}

void left(void)
{
	gimbal_transmit(0xFF,0x01,0x00,0x04,0x20,0x00,0x25);
}
