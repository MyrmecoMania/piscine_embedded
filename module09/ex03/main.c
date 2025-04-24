#include <avr/io.h>
#include <util/delay.h>
#define zero 0b00111111
#define un 0b00000110
#define deux 0b01011011
#define trois 0b01001111
#define quatre 0b01100110
#define cinq 0b01101101
#define six 0b01111101
#define sept 0b00000111
#define huit 0b01111111
#define neuf 0b01100111

uint8_t numbers[10] = {
  zero, un, deux, trois, quatre, cinq, six, sept, huit, neuf };

void i2c_init(void) {
  TWBR = 72; // bit frequency, 16000000/16 + 2 * 72 = 100000;
  TWCR = (1<<TWEN); // enable TWI
}

void i2c_start(void) {
  TWCR |= (1<<TWSTA) | (1<<TWINT); // start condition bit + flag
  while (!(TWCR & (1<<TWINT))); // on attend que le flag repasse a 0
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


uint8_t i2c_read(void) {
  TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWEA) ; // je toggle TWINT
  while (!(TWCR & (1<<TWINT)));
  return TWDR; // je recupere l'octet dans le registre
}

int main() {
  uint8_t value = 0;
  i2c_init();
  i2c_start();
  i2c_write(0b01000000);
  i2c_write(6);
  i2c_write(0b01111111);
  i2c_stop();
  i2c_start();
  i2c_write(0b01000000);
  i2c_write(7);
  i2c_write(0b00000000);
  i2c_stop();
  while(1)
  {
     i2c_start();
    i2c_write(0b01000000);
    i2c_write(2);
    i2c_write(0b01111111);
    i2c_stop();
    i2c_start();
    i2c_write(0b01000000);
    i2c_write(3);
    i2c_write(0b00000000);
    i2c_stop();
    if (value == 9)
      value = 0;
    else
      value++;
    i2c_start();
    i2c_write(0b01000000);
    i2c_write(3);
    i2c_write(numbers[value]);
    i2c_stop();
    _delay_ms(1000);
  }
}
