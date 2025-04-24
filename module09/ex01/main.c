#include <avr/io.h>
#include <util/delay.h>


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

int sw3() {
  uint8_t a;
  i2c_start();
  i2c_write(0b01000000);
  i2c_write(0);
  i2c_start();
  i2c_write(0b01000001);
  a = i2c_read();
  i2c_stop();
  if (!(a & 1))
    return 1;
  return 0;
}

int main() {
  uint8_t value = 0;
  i2c_init();
  i2c_start();
  i2c_write(0b01000000);
  i2c_write(6);
  i2c_write(0b11110001);
  i2c_stop();
  while(1)
  {
    if (sw3()) {
      if (value == 7)
        value = 0;
      else
        value++;
      while (sw3());
    }
    i2c_start();
    i2c_write(0b01000000);
    i2c_write(2);
    i2c_write(~value<<1);
    i2c_stop();
  }
}
