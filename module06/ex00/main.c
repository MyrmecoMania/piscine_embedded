#include <avr/io.h>
#include <util/twi.h>
#define BAUD 115200
#define MYUBBR ((16000000/(8UL*BAUD))-1UL)

void uart_init(unsigned int ubrr) {
  // On initialize UART
  UBRR0L = (unsigned char)ubrr;
  UBRR0H = (unsigned char)(ubrr >> 8);
  // On active le double speed, la reception et l'envoi
  UCSR0A = (1<<U2X0);
  UCSR0B |= (1 << TXEN0);
}

void uart_tx(char c) {
  // On envoie notre charactere
  // a laide du buffer de transmition
  while(!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

void  changeBase(uint8_t number) {
  int number1 = number >> 4;
  int number2 = number & 0x0F;
  if (number1 > 9)
    number1 += 39;
  uart_tx(number1 + '0');
  if (number2 > 9)
    number2 += 39;
  uart_tx(number2 + '0');
}

void i2c_init(void) {
  TWBR = 72; // bit frequency, 16000000/16 + 2 * 72 = 100000;
  TWCR = (1<<TWEN); // enable TWI
}

void i2c_start(void) {
  TWCR |= (1<<TWSTA) | (1<<TWINT); // start condition bit + flag
  while (!(TWCR & (1<<TWINT))); // on attend que le flag repasse a 0
  uint8_t status = TWSR & 0xF8; // on enregistre le status 5 derier bit de TWSR
  changeBase(status);
  uart_tx('\r');
  uart_tx('\n');
}

void i2c_stop(void) {
  TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO); // on stop condition bit
  while (!(TWCR & (1<<TWSTO))); // on attend le bit soit deset
}

int main() {
  uart_init(MYUBBR);
  i2c_init();
  i2c_start();
  i2c_stop();
  while(1)
  {
  }
}
