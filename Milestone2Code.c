/*************************************************************************************
University: 			Technical University of Crete
School:					School of Electrical & Computer Engineering
Course:					Embedded Systems - HRY 411
Professor:				A. Dollas
Author:					S. Apostolakis
Modified by: 			P. Giakoumakis, N. Ghionis, P.Portokalakis 
Create Date: 			24-11-2017
Project Name: 			Automatic Detection of Celestial Bodies with Telescope
Target Device:			ATMEL AVR ATmega16L
Gimbal:					AS20-RS485
Development Platform:	AtmelStudio 7
Version:				0.2

Additional Comments:	The purpose of this file is to show the basic functions
						of a gimbal.
						This version implements all the functions needed to tilt and pan
						the gimbal.
						pan/tilt for specific ammount of degrees supported
						A test program is shown at the main function, that initializes the
						gimbal to a specific position, and then makes a custom move.
						
						
*************************************************************************************/

#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay.h>

//Declare functions
void stop(void);
void USART_Transmit(unsigned char data);
void gimbal_transmit(int byte1,int byte2,int byte3,int byte4,int byte5,int byte6,int byte7,int byte8);
void down(void);
void up(void);
void left(void);
void left_deg(int deg);
void right(void);
void right_deg(int deg);
void down_deg(int deg);
void USART_Init(int baud);
void gimbal_init(void);


void USART_Transmit(unsigned char data)
{
	// Wait for empty transmit buffer
	while (!( UCSRA & (1<<UDRE)));

	// Put data into buffer, sends the data
	UDR = data;
}

void USART_Init(int baud) {
		// Set baud register
		UBRRH=(baud>>8);
		UBRRL=baud;

		// Enable UART transmit
		UCSRB=(1<<TXEN)| (0<<UCSZ2);
		// Enable transmit: data 8 bytes, 1 stop bit and parity: none
		UCSRC=(1<<URSEL) |(0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ0) | (1<<UCSZ1);
}

// Initialize gimbal on starting position (pan left, tilt down)
void gimbal_init(void) {
	left();
	_delay_ms(40000);
	down();
	_delay_ms(19000);
}

// Transmits each byte separately
void gimbal_transmit(int byte1,int byte2,int byte3,int byte4,int byte5,int byte6,int byte7,int byte8)
{
	USART_Transmit(byte1);
	USART_Transmit(byte2);
	USART_Transmit(byte3);
	USART_Transmit(byte4);
	USART_Transmit(byte5);
	USART_Transmit(byte6);
	USART_Transmit(byte7);
	USART_Transmit(byte8);
}

// Tilt down
void down(void)
{
	gimbal_transmit(0xA0,0x00,0x00,0x10,0x00,0x20,0xAF,0x3F);
}

// Tilt down on specific degrees
void down_deg(int deg)
{
	gimbal_transmit(0xA0,0x00,0x00,0x10,0x00,0x20,0xAF,0x3F);

	while(deg != 0)
	{
		_delay_us(190000);		// Tilt down one degree
		deg--;
	}

	stop();
}

// Tilt up
void up(void)
{
	gimbal_transmit(0xA0,0x00,0x00,0x08,0x00,0x20,0xAF,0x27);
}

// Tilt up on specific degrees
void up_deg(int deg) 
{	
		gimbal_transmit(0xA0,0x00,0x00,0x08,0x00,0x20,0xAF,0x27);
		
		while(deg != 0)
		{
			_delay_us(190000);		// Tilt up one degree
			deg--;
		}

		stop();
}

// Pan left
void left(void)
{
	gimbal_transmit(0xA0,0x00,0x00,0x04,0x20,0x00,0xAF,0x2B);
}


// Pan left on specific degrees
void left_deg(int deg)
{
	gimbal_transmit(0xA0,0x00,0x00,0x04,0x20,0x00,0xAF,0x2B);
	
	while(deg != 0)
	{
		_delay_us(114000); // Pan left one degree
		deg--;
	}
	
	stop();
}

// Pan right
void right(void)
{
	gimbal_transmit(0xA0,0x00,0x00,0x02,0x20,0x00,0xAF,0x2D);
}

// Pan right on specific degrees
void right_deg(int deg)
{
	gimbal_transmit(0xA0,0x00,0x00,0x02,0x20,0x00,0xAF,0x2D);

	while(deg != 0)
	{
		_delay_us(114000); // Pan right one degree
		deg--;
	}

	stop();
}

// Stop movement
void stop(void)
{
	gimbal_transmit(0xA0,0x00,0x00,0x00,0x00,0x00,0xAF,0x0F);
}


void test(void) {
	right_deg(90);
	up_deg(49);
}

int main(void)
{
	
	USART_Init(55);	//set at 55 for a baud rate of 9600
	
	
	gimbal_init();
	test();
	
	while (1) 
    {
		
    }
}