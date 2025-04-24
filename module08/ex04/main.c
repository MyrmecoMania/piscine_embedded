#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD 115200
#define MYUBBR ((16000000/(8UL*BAUD))-1UL)

uint8_t rgbLed[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
int y = 0, position = 0;

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

unsigned char uart_rx(void)
{
  while(!(UCSR0A & (1 << UDRE0)));
  return UDR0;
}

void uart_print_str(const char *str)
{
  while (*str)
  {
    uart_tx(*str);
    str++;
  }
}

void SPI_init() {
  DDRB =  (1<<PB2) | (1<<PB3) | (1<<PB5);
  SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void SPI_MasterTransmit(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1<<SPIF)));
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

int ft_strcmp(char *str1, char *str2) {
  while (*str1 && *str2) {
    if (*str1 != *str2)
      return 1;
    str1++;
    str2++;
  }
  if ((*str1 && !*str2) || (!*str1 && *str2))
    return 1;
  return 0;
}

void ledControl() {
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  // LED FRAME 1
  SPI_MasterTransmit(0b11100001);
  SPI_MasterTransmit(rgbLed[0][0]);
  SPI_MasterTransmit(rgbLed[0][1]);
  SPI_MasterTransmit(rgbLed[0][2]);
  // LED FRAME 2
  SPI_MasterTransmit(0b11100001);
  SPI_MasterTransmit(rgbLed[1][0]);
  SPI_MasterTransmit(rgbLed[1][1]);
  SPI_MasterTransmit(rgbLed[1][2]);
  // LED FRAME 3
  SPI_MasterTransmit(0b11100001);
  SPI_MasterTransmit(rgbLed[2][0]);
  SPI_MasterTransmit(rgbLed[2][1]);
  SPI_MasterTransmit(rgbLed[2][2]);
  //END FRAME 
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < 3; i++) {
    rgbLed[i][0] = b;
    rgbLed[i][1] = g;
    rgbLed[i][2] = r;
  }
  ledControl();
}

void wheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    set_rgb(255 - pos * 3, 0, pos * 3);
  } else if (pos < 170) {
    pos = pos - 85;
    set_rgb(0, pos * 3, 255 - pos * 3);
  } else {
    pos = pos - 170;
    set_rgb(pos * 3, 255 - pos * 3, 0);
  }
}

int main() {
  int i = 0;
  int up = 1;
  SPI_init();
  uart_init(MYUBBR);
  while(1) {

  }
}

ISR(USART_RX_vect) {
  int led;
  uint8_t r, g, b;
  char line[13];
  unsigned char received = uart_rx();
  if (received != 13) {
    uart_tx(received);
    line[y++] = received;
  }
  if (y == 13 || received == 13) {
    uart_print_str("\r\n");
    line[y] = 0;
    if (!ft_strcmp("#FULLRAINBOW", line)) {
      TCCR1B = (1 << CS12) | (1 << CS10) | (1<<WGM12);
      OCR1A = 255;
      TIMSK1 = (1 << OCIE1A);
    }
    else {
      TCCR1B = 0;
      if (line[7] != 'D') {
        uart_print_str("Wrong led\r\n");
      }
      else {
        led = line[8] - '0' - 6;
        if (led < 0 || led > 2)
          uart_print_str("Wrong led\r\n");
        else {
          r |= numberCast(line[1]) << 4;
          r |= numberCast(line[2]);
          g |= numberCast(line[3]) << 4;
          g |= numberCast(line[4]);
          b |= numberCast(line[5]) << 4;
          b |= numberCast(line[6]);
          rgbLed[led][0] = b;
          rgbLed[led][1] = g;
          rgbLed[led][2] = r;
          ledControl();
        }
      }
    }
    y = 0;
    line[0] = 0;
  }
}

ISR(TIMER1_COMPA_vect) {
  if (position == 255)
    position = 0;
  position++;
  wheel(position);
}