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
  // ADEN enable adc
  ADCSRA |= (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
}

uint16_t adc_read_potent() {
  // ON commence la conversion puis attendont le resultat
  ADMUX	=	(1<<REFS0);
  ADCSRA |= (1<<ADSC);
  while ( (ADCSRA & (1<<ADSC)) );
  return ADC;
}

uint16_t adc_read_ldr() {
  // ON commence la conversion puis attendont le resultat
  ADMUX	=	(1<<REFS0) | (1<<MUX0);
  ADCSRA |= (1<<ADSC);
  while ( (ADCSRA & (1<<ADSC)) );
  return ADC;
}

uint16_t adc_read_ntc() {
  // ON commence la conversion puis attendont le resultat
  ADMUX	=	(1<<REFS0) | (1<<MUX1);
  ADCSRA |= (1<<ADSC);
  while ( (ADCSRA & (1<<ADSC)) );
  return ADC;
}

void uart_tx(char c) {
  // On envoie notre charactere
  // a laide du buffer de transmition
  while(!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

void  printNumber(uint16_t number) {
  char numberChar[4];
  uint8_t i = 0;
  if (number == 0)
    return uart_tx('0');
  while ((number / 10) > 0) {
    numberChar[i++] = (number % 10) + '0';
    number /= 10;
  }
  numberChar[i] = number + '0'; 
  while (i > 0) {
    uart_tx(numberChar[i--]);
  }
  uart_tx(numberChar[0]);
}

ISR(TIMER1_COMPA_vect) {
  printNumber(adc_read_potent());
  uart_tx(',');
  uart_tx(' ');
  printNumber(adc_read_ldr());
  uart_tx(',');
  uart_tx(' ');
  printNumber(adc_read_ntc());
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
