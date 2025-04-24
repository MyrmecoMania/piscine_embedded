#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD 115200
#define MYUBBR ((16000000/(8UL*BAUD))-1UL)


void uart_init(unsigned int ubrr) {
  // On initialize UART
  UBRR0L = (unsigned char)ubrr;
  UBRR0H = (unsigned char)(ubrr >> 8);
  // On active le double speed, la reception et l'envoi
  UCSR0A = (1<<U2X0);
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void adc_init() {
  // ADEN enable adc, ADPS0 et ADPS1 pour 8 bits p218
  ADCSRA |= (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
  // REFs0 pour lire AVcc et ADLAR pour lire ADCH p217
  ADMUX	=	(1<<REFS0) | (1<<ADLAR);
}

uint16_t adc_read_potent() {
  // ON commence la conversion puis attendont le resultat
  ADMUX	=	(1<<REFS0) | (1<<ADLAR);
  ADCSRA |= (1<<ADSC);
  while ( (ADCSRA & (1<<ADSC)) );
  return ADCH;
}

uint16_t adc_read_ldr() {
  // ON commence la conversion puis attendont le resultat
  ADMUX	=	(1<<REFS0) | (1<<ADLAR) | (1<<MUX0);
  ADCSRA |= (1<<ADSC);
  while ( (ADCSRA & (1<<ADSC)) );
  return ADCH;
}

uint16_t adc_read_ntc() {
  // ON commence la conversion puis attendont le resultat
  ADMUX	=	(1<<REFS0) | (1<<ADLAR) | (1<<MUX1);
  ADCSRA |= (1<<ADSC);
  while ( (ADCSRA & (1<<ADSC)) );
  return ADCH;
}

void uart_tx(char c) {
  // On envoie notre charactere
  // a laide du buffer de transmition
  while(!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

char  changeBase(int number) {
  if (number > 9)
    number += 39;
  return (number + '0');
}


ISR(TIMER1_COMPA_vect) {
  
  uart_tx(changeBase(adc_read_potent() >> 4));
  uart_tx(changeBase(adc_read_potent() & 0x0F));
  uart_tx(',');
  uart_tx(' ');
  uart_tx(changeBase(adc_read_ldr() >> 4));
  uart_tx(changeBase(adc_read_ldr() & 0x0F));
  uart_tx(',');
  uart_tx(' ');
  uart_tx(changeBase(adc_read_ntc() >> 4));
  uart_tx(changeBase(adc_read_ntc() & 0x0F));
  uart_tx('\r');
  uart_tx('\n');
}

int main() {
  uart_init(MYUBBR);
  adc_init();
  // on initialise un compteur en mode CTC, avec un prescaler de 64
  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS11);
  // interrupt de comparaison
  TIMSK1 = (1<<OCIE1A);
  // 16000000/64 = 250 000
  // 250 000/50 = 5000;
  OCR1A = 5000;
	sei();
  while(1)
  {
  }
}
