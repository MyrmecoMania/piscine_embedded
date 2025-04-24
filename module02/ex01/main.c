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
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void uart_tx(char c)
{
  // UDR is the transmit buffer
  while(!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

void uart_print_str(const char *str)
{
  while (*str)
  {
    uart_tx(*str);
    str++;
  }
}

int main() {
  uart_init(MYUBBR);
  TCCR1B = (1 << CS12) | (1 << CS10) | (1<<WGM12);
  OCR1A = 31250;
  TIMSK1 = (1 << OCIE1A);  // Enable timer1 overflow interrupt(TOIE1)
	sei();        // E
  while(1)
  {
  }
}

ISR(TIMER1_COMPA_vect) {
  uart_print_str("Hello World!\n\r");
}