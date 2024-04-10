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

const char timer0restart = 200;
char debounceon = 0;

void setup(void)
{
	cli();
	
	DDRD = 0xFF; //Colocamos a todo D como salidas
	PORTD = 0;
	
	UCSR0B = 0; // Deshablitamos serial en D0 y D1
	
	DDRB = 0x00; //Colocamos a B como entradas, utilizaremos PB2 y PB3
	PORTB = 0x0C; // Habilitamos pullups
	
	PCICR = 0x01; // Habilitamos pin interrupt en PB2 y PB3
	PCMSK0 = 0x0C;
	
	TCCR0B = 0b00000101; // Le colocamos un prescaler de 1024 a Timer 0 
	TCNT0 = timer0restart;
	
	TIMSK0 = 0x01; // habilitamos interrupt en TIMR0 overflow
	
	sei();
}

int main(void)
{
	setup();
    /* Replace with your application code */
    while (1) 
    {
		_delay_ms(100);
    }
}

ISR(PCINT0_vect){
	
	if ((debounceon & 0x80) == 0b10000000) {return;}
	
	if ((PINB & 0b00001100) == 0b00001000) { //Aumentamos si el bit de incrementos esta apachado
		PORTD++;
		debounceon = 0xFF;
		//while(PINB = 0xF7); //antirebote
	}
	
	else if ((PINB & 0b00001100) == 0b00000100) {
		PORTD--;
		debounceon = 0xFF;
		//while(PINB = 0xFB); //antirebote
	}
	
	
	
	return;
	
}

ISR(TIMER0_OVF_vect){
 TCNT0 = timer0restart;
 if((debounceon & 0b10000000) == 0b10000000) {
	 debounceon--;
 }
 
}

