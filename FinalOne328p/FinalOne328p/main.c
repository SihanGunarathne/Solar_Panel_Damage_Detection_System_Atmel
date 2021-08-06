/*
 * Description : Damage Detection System for Solar Panel
 * main.c
 * Created: 4/3/2021 3:44:16 PM
 * Author: SIHAN
 */ 


#include <avr/io.h>

#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "LCD16x2_4bit.h"


// initialize adc
void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
	// ADC Enable and prescaler of 128
	// 8000000/128 = 62500
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

// read adc value
uint16_t adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with '7' will always keep the value
	// of 'ch' between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing
	
	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes '0' again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}


// reverses a string 'str' of length 'len'
void reverse(char *str, int len)
{
	int i=0, j=len-1, temp;
	while (i<j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}

// Converts a given integer x to string str[].  d is the number
// of digits required in output. If d is more than the number
// of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x)
	{
		str[i++] = (x%10) + '0';
		x = x/10;
	}
	
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
	str[i++] = '0';
	
	reverse(str, i);
	str[i] = ' ';
	return i;
}

// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;
	
	// Extract floating part
	float fpart = n - (float)ipart;
	
	// convert integer part to string
	int i = intToStr(ipart, res, 0);
	
	// check for display option after point
	if (afterpoint != 0)
	{
		res[i] = '.';  // add dot
		
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}


int main()
{
	DDRB |= 0b00000010; //Data Direction Register B: writing a 1 to the bit enables output
	DDRB |= 0b00000100; //Data Direction Register B: writing a 1 to the bit enables output


 	float adc_result0, adc_result2, adc_result1, adc_result3;
	char int_buffer1[10];
	char int_buffer2[10];
	char int_buffer3[10];
	char int_buffer4[10];

	// initialize adc and lcd
	adc_init();
	lcdinit();
	
	lcd_gotoxy(0,0);
	lcd_print("  SOLAR  PANEL ");
	lcd_gotoxy(0,1);
	lcd_print("DAMAGE DETECTION");
	_delay_ms(2000);
			
	lcd_clear();

	
	lcd_gotoxy(0,0);
	lcd_print("  GROUP NO : 17 ");
	lcd_gotoxy(0,1);
	_delay_ms(2000);

	lcd_clear();


	while(1)
	{
 		adc_result0 = ((adc_read(0)*0.00488) - 2.5)/0.066;      // read adc value at PA0 | Calculate the current
		adc_result1 = (adc_read(1)*0.00488)*6;    // read adc value at PA1 | Calculate the voltage
		adc_result2 = adc_result0*adc_result1;	//Calculate the power
 		adc_result3 =(((adc_read(2)*0.00488) - 2.5)/0.066) *((adc_read(3)*0.00488)*6);    // read adc value at PA2 and PA3 | Calculate the reference power
		
		// condition for led to glow
		
		
		// now display current on lcd
		lcd_gotoxy(0,0);
		lcd_print("Current=");
		ftoa(adc_result0, int_buffer1, 2 );
// 		sprintf (int_buffer1, "%f", adc_result0);
 		lcd_gotoxy(8,0);
		lcd_print(int_buffer1);
		lcd_gotoxy(13,0);
		lcd_print("A");
		_delay_ms(1000);

		
		
		// now display voltage on lcd
		lcd_gotoxy(0,1);
		lcd_print("Voltage=");
 		ftoa(adc_result1, int_buffer2, 4 );
 		lcd_gotoxy(8,1);
 		lcd_print(int_buffer2);
		lcd_gotoxy(13,1);
		lcd_print("V");
		_delay_ms(3000);
		
		lcd_clear();
		
		// now display power of domestic panel on lcd
 		ftoa(adc_result2, int_buffer3, 2 );
		lcd_gotoxy(0,0);
		lcd_print("Power=");
		lcd_gotoxy(6,0);
		lcd_print(int_buffer3);
		lcd_gotoxy(12,0);
		lcd_print("W");
		_delay_ms(1000);
		
		// now display power of reference panel on lcd
		// 		itoa(adc_result3, int_buffer4, 10);
 		ftoa(adc_result3, int_buffer4, 2 );
		lcd_gotoxy(0,1);
		lcd_print("RefPower=");
		lcd_gotoxy(9,1);
		lcd_print(int_buffer4);
		lcd_gotoxy(14,1);
		lcd_print("W");
		_delay_ms(3000);
		
		lcd_clear();
		
		// Compare the generated power of reference and domestic panels
		if (adc_result2 != adc_result3)
		{
			lcd_gotoxy(0,0);
			lcd_print("THERE IS DAMAGE");

			PORTB= 0b00000010; //turns on LED attached to port PB0 (Red)
			_delay_ms(3000);

			lcd_clear();
		}
		
		
		else
		{
			lcd_gotoxy(0,0);
			lcd_print("THERE IS NO");
			lcd_gotoxy(0,1);
			lcd_print("DAMAGE");
			
			PORTB= 0b00000000; //Turns off all B pins, including PB0
			PORTB= 0b00000100; //turns on LED attached to port PB0 (Green)
			_delay_ms(3000);

			lcd_clear();
		}
		
		
		
	}
}