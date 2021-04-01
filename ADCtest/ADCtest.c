
#define F_CPU 16000000UL


#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include "usart.h"

#define control_bus PORTB
#define controlbus_direction DDRB
#define data_bus PORTD
#define databus_direction DDRD

#define rs 2
#define en 3
#define d4 4
#define d5 5
#define d6 6
#define d7 7


char y[100];

void LCD_CmdWrite( char a)
{
	if(a & 0x80) data_bus|=(1<<d7); else data_bus&= ~(1<<d7);
	if(a & 0x40) data_bus|=(1<<d6); else data_bus&= ~(1<<d6);
	if(a & 0x20) data_bus|=(1<<d5); else data_bus&= ~(1<<d5);
	if(a & 0x10) data_bus|=(1<<d4); else data_bus&= ~(1<<d4);
	control_bus &=~(1<<rs);control_bus |=(1<<en);
	_delay_ms(2);
	control_bus &=~(1<<en);
	
	_delay_ms(2);
	
	if(a & 0x08) data_bus|=(1<<d7); else data_bus&= ~(1<<d7);
	if(a & 0x04) data_bus|=(1<<d6); else data_bus&= ~(1<<d6);
	if(a & 0x02) data_bus|=(1<<d5); else data_bus&= ~(1<<d5);
	if(a & 0x01) data_bus|=(1<<d4); else data_bus&= ~(1<<d4);
	control_bus &=~(1<<rs);control_bus |=(1<<en);
	_delay_ms(2);
	control_bus &=~(1<<en);
	
	_delay_ms(2);
}


void LCD_DataWrite( char a)
{
	
	if(a & 0x80) data_bus|=(1<<d7); else data_bus&= ~(1<<d7);
	if(a & 0x40) data_bus|=(1<<d6); else data_bus&= ~(1<<d6);
	if(a & 0x20) data_bus|=(1<<d5); else data_bus&= ~(1<<d5);
	if(a & 0x10) data_bus|=(1<<d4); else data_bus&= ~(1<<d4);
	control_bus |=(1<<rs)|(1<<en);
	_delay_ms(2);
	control_bus &=~(1<<en);
	_delay_ms(2);
	
	if(a & 0x08) data_bus|=(1<<d7); else data_bus&= ~(1<<d7);
	if(a & 0x04) data_bus|=(1<<d6); else data_bus&= ~(1<<d6);
	if(a & 0x02) data_bus|=(1<<d5); else data_bus&= ~(1<<d5);
	if(a & 0x01) data_bus|=(1<<d4); else data_bus&= ~(1<<d4);
	control_bus |=(1<<rs)|(1<<en);
	_delay_ms(2);
	control_bus &=~(1<<en);
	_delay_ms(2);
	
}


void LCD_Init()
{
	controlbus_direction |= ((1<<rs)|(1<<en));
	databus_direction |= ((1<<d7)|(1<<d6)|(1<<d5)|(1<<d4));
	_delay_ms(2);
	LCD_CmdWrite(0x01); // clear display
	LCD_CmdWrite(0x02); // back to home	
	LCD_CmdWrite(0x28); // 4bit,2line,5x7 pixel
	LCD_CmdWrite(0x06); // entry mode,cursor increments by cursor shift
	LCD_CmdWrite(0x0c); // display ON,cursor OFF
	LCD_CmdWrite(0x80); // force cursor to begin at line1

}

void LCD_Disp(const char *p)
{
	while(*p!='\0')
	{
		LCD_DataWrite(*p);
		p++; _delay_ms(2);
	}
}

void LCD_setCursor(int a, int b)
{
	int i=0;
	switch(b){
		case 0:LCD_CmdWrite(0x80);break;
		case 1:LCD_CmdWrite(0xC0);break;
		case 2:LCD_CmdWrite(0x94);break;
	case 3:LCD_CmdWrite(0xd4);break;}
	for(i=0;i<a;i++)
	LCD_CmdWrite(0x14);
}


unsigned int ReadChannel(unsigned char mux)
{
	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0); // frequency prescaler
	ADMUX = mux; // channel select
	ADMUX |= (1<<REFS1) | (1<<REFS0); 
	ADCSRA |= (1<<ADSC); // Start conversion
	while ( ADCSRA & (1<<ADSC) ) ;
	ADCSRA |= (1<<ADSC); // a transformation �single conversion�
	while ( ADCSRA & (1<<ADSC) );
	ADCSRA &= ~(1<<ADEN); // Disable ADC
	return ADCW;
}

int main( void )
{
	unsigned int adc;
	float x;
	char y[100];
	
	
	LCD_Init();
	usart_init (); // configure the usart and baudrate
	DDRB |= 0x01;
	PORTB |= 0x01;

	printf("\nTemprature Reader\n");

	while(1)
	{	
		LCD_setCursor(0,0);
		//LCD_Disp("Temprature:");
		//LCD_setCursor(0,1);
		adc=ReadChannel(0);
		x=(adc*5.0)/1023.0*100.0-273.15;
		printf("%7.5f\n\r",x);
		sprintf(y,"%7.5f",x);
		LCD_Disp(y);
		_delay_ms(2000);
		PORTB ^= 0x01;
		_delay_ms(500);
	}
}
