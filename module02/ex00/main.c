#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD 115200
#define MYUBBR ((16000000/(8UL*BAUD))-1UL)


void uart_init(unsigned int ubrr)
{
  // On initialize UART
  UBRR0L = (unsigned char)ubrr;
  UBRR0H = (unsigned char)(ubrr >> 8);
  // On active le double speed, la reception et l'envoi
  UCSR0A = (1<<U2X0);
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void uart_tx(char c)
{
  // On envoie notre charactere
  // a laide du buffer de transmition
  while(!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

int main() {
  uart_init(MYUBBR);
  // on initialise un compteur en mode CTC, avec un prescaler de 1024
  TCCR1B = (1 << CS12) | (1 << CS10) | (1<<WGM12);
  OCR1A = 15625;
  TIMSK1 = (1 << OCIE1A);  // On met en place l'interupt de comparaison
	sei();
  while(1)
  {
  }
}

ISR(TIMER1_COMPA_vect) {
  uart_tx('Z');
}