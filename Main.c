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

// #define F_CPU 8000000L is defined in .h files included
// Define global variables
#define PAN_DEG_TIME			114000				// Pan time for one degree
#define TILT_DEG_TIME			190000				// Tilt time for one degree
#define NEXT_SAMPLE				120000				//	Time interval(min)*60000ms
#define Device_Write_address	0xD0				// Define RTC DS1307 slave address for write operation //
#define Device_Read_address		0xD1				// Make LSB bit high of slave address for read operation //
#define TimeFormat12			0x40				// Define 12 hour format //
#define AMPM					0x20
#define OBS_START_HOUR			10					//Time samples start hour
#define	OBS_START_MINUTES		0					//Time samples start minute
#define TIME_INTERVAL			2					//Time between samples in minutes
#define uint8 unsigned char
#define BAUD 55										//set at 55 for a baud rate of 9600


//include needed libraries
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "I2C_Master_H_file.h"
#include "LCD16x2_4Bit.h"
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
bool IsItPM(char hour_);
void RTC_Read_Clock(char read_clock_address);
void RTC_Read_Calendar(char read_calendar_address);
void ShowTime();
int HexToDec(int arg0);
void StartTime_ToLEDs();
void ConvertTimeToDecimal();
int IntervalCalculation();
void FillAz_El(int16_t* azimuth,int16_t* elevation);
void TrackOrbit(int16_t* azimuth,int16_t* elevation,int full_intervals);

// Fill matrix with data from Horizons. The values are separated in pairs (azimuth,elevation) - Next pair=next value
// Data inserted - Orbit: Voyager I, Date: 15/12/2017, Time: 10:00 - 16:00, Time interval: 2 minutes
	int16_t eedata[] = {
		134,59,135,59,136,60,137,60,137,60,138,61,
		139,61,140,61,141,61,142,62,143,62,143,62,
		144,62,145,63,146,63,147,63,148,63,149,63,
		150,64,151,64,152,64,153,64,154,64,155,65,
		156,65,158,65,159,65,160,65,161,65,162,65,
		163,66,164,66,165,66,167,66,168,66,169,66,
		170,66,171,66,173,66,174,66,175,66,176,66,
		178,66,179,66,180,66,181,66,182,66,184,66,
		185,66,186,66,187,66,189,66,190,66,191,66,
		192,66,193,66,194,66,196,66,197,66,198,65,
		199,65,200,65,201,65,202,65,203,65,205,65,
		206,64,207,64,208,64,209,64,210,64,211,63,
		212,63,213,63,214,63,215,63,216,62,216,62,
		217,62,218,62,219,61,220,61,221,61,222,61,
		223,60,223,60,224,60,225,59,226,59,226,59,
		227,59,228,58,229,58,229,58,230,57,231,57,
		231,57,232,56,233,56,233,56,234,55,235,55,
		235,55,236,54,237,54,237,54,238,53,238,53,
		239,53,239,52,240,52,241,52,241,51,242,51,
		242,51,243,50,243,50,244,49,244,49,245,49,
		245,48,246,48,246,48,247,47,247,47,248,46,
		248,46
	};
	
// Initialize USART connection
void USART_Init(int baud) {
		// Set baud register
		UBRRH=(baud>>8);
		UBRRL=baud;

		// Enable UART transmit
		UCSRB=(1<<TXEN)| (0<<UCSZ2);
		// Enable transmit: data 8 bytes, 1 stop bit and parity: none
		UCSRC=(1<<URSEL) |(0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ0) | (1<<UCSZ1);
}

// Send data using the USART protocol
void USART_Transmit(unsigned char data)
{
	// Wait for empty transmit buffer
	while (!( UCSRA & (1<<UDRE)));

	// Put data into buffer, sends the data
	UDR = data;
}

// Initialize gimbal on starting position (pan left, tilt down)
void gimbal_init(void) {
	PORTA = 0xFB;		// Open LED2
	left();				// Pan left
	_delay_ms(40000);	// Delay time for worst case scenario
	PORTA = 0xF7;		// Open LED3
	down();				// Tilt down
	_delay_ms(19000);	// Delay time for worst case scenario
	PORTA = 0xFF;
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

	// Count down degrees
	while(deg != 0)
	{
		_delay_us(TILT_DEG_TIME);		// Tilt down one degree
		deg--;							// One degree less
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
		
		// Count down degrees
		while(deg != 0)
		{
			_delay_us(TILT_DEG_TIME);		// Tilt up one degree
			deg--;							// One degree less
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
	
	// Count down degrees
	while(deg != 0)
	{
		_delay_us(PAN_DEG_TIME);		// Pan left one degree
		deg--;							// One degree less
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

	// Count down degrees
	while(deg != 0)
	{
		_delay_us(PAN_DEG_TIME);		// Pan right one degree
		deg--;							// One degree less
	}
	stop();
}

// Stop movement
void stop(void)
{
	gimbal_transmit(0xA0,0x00,0x00,0x00,0x00,0x00,0xAF,0x0F);
}

//***********************************	RTC FUNCTIONS	***********************************//
int second,minute,hour,day,date,month,year;

// Check if it's AM or PM
bool IsItPM(char hour_)
{
	if(hour_ & (AMPM))
	return 1;
	else
	return 0;
}

void RTC_Read_Clock(char read_clock_address)
{
	I2C_Start(Device_Write_address);				// Start I2C communication with RTC
	I2C_Write(read_clock_address);					// Write address to read
	I2C_Repeated_Start(Device_Read_address);		// Repeated start with device read address

	second = I2C_Read_Ack();						// Read second
	minute = I2C_Read_Ack();						// Read minute
	hour = I2C_Read_Nack();							// Read hour with Nack
	I2C_Stop();										// Stop i2C communication
}

void RTC_Read_Calendar(char read_calendar_address)
{
	I2C_Start(Device_Write_address);
	I2C_Write(read_calendar_address);
	I2C_Repeated_Start(Device_Read_address);

	day = I2C_Read_Ack();							// Read day
	date = I2C_Read_Ack();							// Read date
	month = I2C_Read_Ack();							// Read month
	year = I2C_Read_Nack();							// Read the year with Nack
	I2C_Stop();										// Stop i2C communication
}

// Read time from RTC and print it on LCD screen
void ShowTime()
{
	char buffer[20];
	char* days[7]= {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
		
	RTC_Read_Clock(0);							/* Read the clock with second address i.e location is 0 */
	if (hour & TimeFormat12)
	{
		sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
		if(IsItPM(hour))
		strcat(buffer, "PM");
		else
		strcat(buffer, "AM");
		lcd_print_xy(0,0,buffer);
	}
	else
	{
		sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
		lcd_print_xy(0,0,buffer);
	}
	RTC_Read_Calendar(3);						/* Read the calender with day address i.e location is 3 */
	sprintf(buffer, "%02x/%02x/%02x %3s ", date, month, year,days[day-1]);
	lcd_print_xy(1,0,buffer);
}

//*******************************************************************************************//

// Hours, Minutes, Seconds are read at BCD format. They are converted to decimals
int HexToDec(int arg0)
{
		int LB = (arg0 & 0b00001111);
		int HB = (arg0 & 0b11110000);
		HB = HB>>4;
		
		return (LB + HB*10);
	}

// Used primarily for testing and debugging. Lights LEDs on appropriate values corresponding to starting time (binary format)
void StartTime_ToLEDs()
{
	PORTA = 0xFF;
	_delay_ms(1000);
	
	PORTA = ~(hour);			// Light LEDs to show hours
	_delay_ms(2000);
	PORTA = 0xFF;				// Turn off LEDs
	_delay_ms(2000);
	PORTA = ~(minute);			// Light LEDs to show minutes
	_delay_ms(2000);
}

// Converts time from BCD to decimal
void ConvertTimeToDecimal()
{
	hour = hour & 0b00011111;
	hour = HexToDec(hour);
	minute = HexToDec(minute);
}

// Allign and count intervals
int IntervalCalculation()
{
	int cur_hour = (hour - OBS_START_HOUR);				// Hours passed since observation started
	int cur_min;										// Minutes passed since observation started

	// Find the remainder between present minutes and observation's start
	if(minute - OBS_START_MINUTES>=0){					
		cur_min = (minute - OBS_START_MINUTES);
	}
	else {												// example: time now: 3.35 - time when observation started: 3.45
		cur_hour--;
		cur_min = 60-abs(minute-OBS_START_MINUTES);
	}

	int cur_time_sample = (cur_hour*60 + cur_min);		// Total time difference between start time and current time, to minute format
	
	int full_intervals = cur_time_sample/TIME_INTERVAL;	// Find full intervals completed (matrix starting point)
	int interval_remainder = cur_time_sample%TIME_INTERVAL;	// Find remaining minutes in current interval
	
	for(int i=0; i<(TIME_INTERVAL - interval_remainder); i++)  {		// Align with intervals of HORIZONS
		_delay_ms(60000);			// wait for one minute
	}
	
	return full_intervals;			// return full intervals
}

// Fill azimuth and elevation tables
void FillAz_El(int16_t* azimuth,int16_t* elevation)
{
	int j=0;
	for(int k=0; k<254; k=k+2)  {
		// Break pairs and put them in separate matrices
		azimuth[j] = eedata[k];
		elevation[j] = eedata[k+1];
		j++;
	}
}

// Main functionality - track and follow an orbit
void TrackOrbit(int16_t* azimuth, int16_t* elevation, int full_intervals)
{
	
	int flag=0;											// Flag
	
	//The sample I want to find is at full_intervals+1!!!
	//First Sample
	while(flag==0){												
		if(elevation[full_intervals+1]-41 >=0)  {		// Check if elevation to be done is valid (the gimbal tilts -41 to 41 degrees - before the first sample it will be down as optimized by gimbal_init())
			PORTA = 0xFF;								// Turn off LEDs
			right_deg(azimuth[full_intervals+1]);		// Pan right to the azimuth degrees of the first sample
			up_deg(elevation[full_intervals+1]-41);		// Tilt up to the elevation degrees of the first sample
			flag=1;										// First sample - done
		}
		else {											// First sample is at wrong coordinates - check next coordinates
			PORTA = 0xFE;								// Turn on first LED for error checking
			full_intervals++;							// Go to next coordinates
		}
		_delay_ms(NEXT_SAMPLE);							// wait for the next sample
	}
	
	for(int l=full_intervals+1; l<126; l++)  {			// Next samples - run until we run out of coordinates
		if((int)elevation[l+1]-41 >= 0)  {				// Check if elevation to be done is valid (the gimbal tilts -41 to 41 degrees)
			if(azimuth[l+1] - azimuth[l] >= 0)  {		// find remaining degrees to match next value
				right_deg(azimuth[l+1] - azimuth[l]);
			}
			else  {
				left_deg(azimuth[l] - azimuth[l+1]);
			}
			if((int)elevation[l+1] - (int)elevation[l] >= 0)  {
				up_deg(elevation[l+1] - elevation[l]);
			}
			else  {
				down_deg(elevation[l] - elevation[l+1]);
			}
		}
		else  {											// Sample is at wrong coordinates - Observation is finished
			PORTA = 0x00;
			_delay_ms(5000);
			gimbal_init();								// Initialize gimbal in starting position
			PORTA = 0x00;
			break;										// The target is lost
		}
		
		_delay_ms(NEXT_SAMPLE);							// Wait for next sample
	}
}

int main(void)
{
	DDRA = 0xFF;
	PORTA = 0xFF;

	int16_t azimuth[127];
	int16_t elevation[127];

	I2C_Init();										// Initialize I2C connection (RTC)
	lcdinit();										// Initialize LCD
	
	ShowTime();										// Show time at start
	
	USART_Init(BAUD);								// Initialize USART Connection
	gimbal_init();									// Initialize gimbal position (Left and Down)
	
	ShowTime();										// Show time for starting interval
	
	ConvertTimeToDecimal();
	
	StartTime_ToLEDs();								// For Testing, Debugging
	
	int full_intervals = IntervalCalculation();		// Count completed intervals
	
	FillAz_El(&azimuth[127], &elevation[127]);		// Fill azimuth and elevation tables
	
	TrackOrbit(&azimuth[127], &elevation[127], full_intervals);	// Start tracking
	
	
	while(1) { }
		
}