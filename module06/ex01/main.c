#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
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


void i2c_write(unsigned char data) {
  TWDR = data; // on mets le byte a envoyer dans le registre de comunication
  TWCR = (1<<TWEN) | (1<<TWINT); // on lance la transmission
  while (!(TWCR & (1<<TWINT))); // on attend que le flag soit 0
}

void print_hex_value(char c) {
  uint8_t number = c;
  int number1 = number >> 4;
  int number2 = number & 0x0F;
  if (number1 > 9)
    number1 += 7;
  uart_tx(number1 + '0');
  if (number2 > 9)
    number2 += 7;
  uart_tx(number2 + '0');
}

void i2c_read(void) {
  TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWEA) ; // je toggle TWINT
  while (!(TWCR & (1<<TWINT)));
  print_hex_value(TWDR); // je recupere l'octet dans le registre
}

int main() {
  uart_init(MYUBBR);
  i2c_init();
  i2c_start();
  _delay_ms(40);
  i2c_write(0b01110000); // addresse du capteur
  i2c_write(0b10111110); // commande d'initialization
  i2c_write(0x08);       // params
  i2c_write(0);          // params
  i2c_stop();
  _delay_ms(20);
  while(1)
  {
    i2c_start();
    i2c_write(0b01110000); // addresse du capteur + bit de write
    i2c_write(0b10101100); // commande de releve
    i2c_write(0b00110011); // params
    i2c_write(0b00000000); // params
    i2c_stop();
    _delay_ms(1000);
    i2c_start();
    i2c_write(0b01110001); // addresse + bit de lecture
    i2c_read();
    uart_tx(' ');
    i2c_read();
    uart_tx(' ');
    i2c_read();
    uart_tx(' ');
    i2c_read();
    uart_tx(' ');
    i2c_read();
    uart_tx(' ');
    i2c_read();
    uart_tx(' ');
    i2c_read();
    i2c_stop();
    uart_tx('\r');
    uart_tx('\n');
    _delay_ms(500);
  }
}
