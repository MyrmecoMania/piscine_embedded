#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD 115200
#define MYUBBR ((16000000/(8UL*BAUD))-1UL)


void uart_init(unsigned int ubrr)
{
  //  16000000/(16*115200) - 1
  UBRR0L = (unsigned char)ubrr;
  UBRR0H = (unsigned char)(ubrr >> 8);
  UCSR0A = (1<<U2X0);
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
  sei();
}

void uart_tx(char c)
{
  while(!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

unsigned char uart_rx(void)
{
  while(!(UCSR0A & (1 << UDRE0)));
  return UDR0;
}

int main() {
  uart_init(MYUBBR);
	sei();
  while(1)
  {
  }
}

ISR(USART_RX_vect) {
  unsigned char received = uart_rx();
  uart_tx(received);
}