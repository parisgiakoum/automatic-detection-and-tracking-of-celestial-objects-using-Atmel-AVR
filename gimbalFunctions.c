/*************************************************************************************
University: 			Technical University of Crete
School:					School of Electrical & Computer Engineering
Course:					Embedded Systems - HRY 411
Professor:				A. Dollas
Author:					S. Apostolakis
Modified by: 			P. Giakoumakis, N. Ghionis, P.Portokalakis 
Create Date: 			27-10-2017
Project Name: 			Automatic Detection of Celestial Bodies with Telescope
Target Device:			ATMEL AVR AT90S8515
Gimbal:					AS20-RS485
Development Platform:	AVR Studio 4
Version:				0.1

Additional Comments:	The purpose of this file is to show the basic functions
						of a gimbal. 
						
						NOTE: The following functions have not been checked, because
						we do not have the corresponding power supply for the gimbal.
						Newer versions to be uploaded.
*************************************************************************************/


//Function to place the byte to transmit, at UART data register, waiting for it to get empty
void UART_Transmit(unsigned char data) 
{ 
	// Wait for empty transmit buffer  
	while (!( USR & (1<<UDRE))); 
	// Put data into buffer, sends the data  
	UDR = data; 
} 


//Function that uses UART_Transmit to transmit 8 bytes
void gimbal_transmit(int byte1,int byte2,int byte3,int byte4,int byte5,int byte6,int byte7) 

{ 
	UART_Transmit(byte1); 
	UART_Transmit(byte2); 
	UART_Transmit(byte3); 
	UART_Transmit(byte4); 
	UART_Transmit(byte5); 
	UART_Transmit(byte6); 
	UART_Transmit(byte7); 
} 

//Function to turn left. (same logic for right etc.)
void left(void) 

{ 

	gimbal_transmit(0xFF,0x01,0x00,0x04,0x20,0x00,0x25); 

} 

//Basic example of the gimbal usage
int main(){ 

	UCR = (1<<RXEN) | (1<<TXEN); 

	UBRR= 12; 

		while(1){ 
			left( ); 
		} 

} 
