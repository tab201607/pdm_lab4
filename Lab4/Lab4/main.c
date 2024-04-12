/******************************************************************************
; Universidad del Valle de Guatemala
; 1E2023: Programacion de Microcontroladores
; main.c
; Autor: Jacob Tabush
; Proyecto: Prelaboratorio 4
; Hardware: ATMEGA328P
; Creado: 9/04/2024
; Ultima modificacion: 10/04/2024
*******************************************************************************/

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

void initADC(void);
char changesomebits(char oldvalue, char bitstochange, char newvalue);
 
    // Definimos el tiempo que utilizaremos para reiniciar el timer0
const char timer0restart = 245;
	//definimos la tabla de display de 7 segmentos
const char tabla7seg[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18, 0x08, 0x03, 0x27, 0x21, 0x06, 0x0E};
char ADCResult = 0;
char decenas = 0;
char unidades = 0;
char mux = 0; // Corr 
char counter = 0;
char debouncetimer = 0;

void setup(void)
{
	cli();

	DDRB = 0b00000111; //Colocamos a B0-2 como salidas, B3y4 como entrads
	PORTB = 0b00011000; //Habilitamos pullups en 3 y 4
	
	PCICR = 0x01; 
	PCMSK0 = 0b00011000; // Habilitamos pin interrupt en PB3 y PB4
	
	DDRD = 0xFF; //Colocamos a todo D como salidas
	PORTD = 0;
	
	UCSR0B = 0; // Deshablitamos serial en D0 y D1
	
	TCCR0B = 0b00000101; // Le colocamos un prescaler de 1024 a Timer 0
	TCNT0 = timer0restart;
	
	TIMSK0 = 0x01; // habilitamos interrupt en TIMR0 overflow
	
	initADC();
	
	sei();
}

int main(void)
{
	setup();
	while (1)
	{
		ADCSRA |= (1<<ADSC); //reiniciamos el ADC
		_delay_ms(100);
		
	}
}

void initADC(void) //Funcion para inicializar el ADC
{
	ADMUX = 0; 
	
	ADMUX |= (1<<REFS0); //conectamos a AVcc
	ADMUX &= ~(1<<REFS1);
	
	ADMUX |= 0b00000110; //Seleccionamos A6
	
	ADMUX |= (1<<ADLAR); // Justificado a la izquierda
	
	ADCSRA |= (1<<ADEN); //Encendemos el ADC
	ADCSRA |= (1<<ADIE); // Encendemos el interrupt
	ADCSRA |= (0b00000111); //Prescaler de 128
}

ISR(ADC_vect){
	ADCResult = ADCH;
	ADCSRA |= (1<<ADIF);
	
	unidades = (ADCResult & 0x0F); // grabamos los valores de unidades y decenas en sus lugares respectivos
	decenas = (ADCResult >> 4); 
	
	return;
	
}

ISR(PCINT0_vect){
	
	if (debouncetimer != 0) {return;}
	
	if ((PINB & 0b00011000) == 0b00010000) { //Aumentamos si el bit de incrementos esta apachado
		counter--;
		debouncetimer = 0xFF;

	}
	
	else if ((PINB & 0b00011000) == 0b00001000) {
		counter++;
		debouncetimer = 0xFF;

	}
	
	return;
	
}

ISR(TIMER0_OVF_vect){
	TCNT0 = timer0restart; // reiniciamos el MUX
	mux++; //cambiamos el mux
	mux %= 3; //aseguramos que no pasa 3
	
	if (mux == 0x00) 
	{PORTD = tabla7seg[unidades];
		PORTB = changesomebits(PORTB, 0b00000111, 0x01);} //desplegamos unidades
	else if (mux == 0x01) 
	{PORTD = tabla7seg[decenas];
		PORTB = changesomebits(PORTB, 0b00000111, 0x02);} //desplegamos decenas
	else if (mux == 0x02) 
	{PORTD = counter; 
	PORTB = changesomebits(PORTB, 0b00000111, 0x04);} //desplegamos el contador
	
	 if(debouncetimer != 0) {
		 debouncetimer--; //Cambiamos el clock de debounce
	 }
	
	TIFR0 |= (1 << TOV0);
	return;
}

char changesomebits(char oldvalue, char bitstochange, char newvalue) //Funcion para solo cambiar algunos bits en un registro
{	char result = (newvalue & bitstochange) | (oldvalue & ~bitstochange);
	return result; 
}
