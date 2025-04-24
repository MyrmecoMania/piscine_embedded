#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD 115200
#define MYUBBR ((16000000/(8UL*BAUD))-1UL)

char line[8];
int i = 0;

void init_rgb() {
  DDRD = (1<<PD3) | (1<<PD5) | (1<<PD6);
  TCCR0A = (1<<COM0A1) | (1<<COM0B1) | (1<<WGM00);
  TCCR0B = (1<<CS00);
  TCCR2A = (1<<COM2B1) | (1<<WGM20);
  TCCR2B = (1<<CS20);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
  OCR0A = g;
  OCR0B = r;
  OCR2B = b;
}


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
  // UDR is the transmit buffer
  while(!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

void uart_print_str(char *str)
{
  while (*str)
  {
    uart_tx(*str);
    str++;
  }
}

unsigned char uart_rx(void)
{
  while(!(UCSR0A & (1 << UDRE0)));
  return UDR0;
}

int main() {
  uart_init(MYUBBR);
  init_rgb();
	sei();
  while(1)
  {
  }
}

int numberCast(char a) {
  if (a >= '0' && a <= '9')
    return (a - '0');
  else if (a >= 'A' && a <= 'F')
    return (a - '7');
  else if (a >= 'a' && a <= 'f')
    return (a - 'W');
  else
    return 0;
}

ISR(USART_RX_vect) {
  uint8_t r, g, b;
  unsigned char received = uart_rx();
  uart_tx(received);
  line[i++] = received;
  if (i == 7) {
    r |= numberCast(line[1]) << 4;
    r |= numberCast(line[2]);
    g |= numberCast(line[3]) << 4;
    g |= numberCast(line[4]);
    b |= numberCast(line[5]) << 4;
    b |= numberCast(line[6]);
    set_rgb(r, g, b);
    i = 0;
    uart_print_str("\r\n");
  }
}