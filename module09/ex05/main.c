#include <avr/io.h>
#include <avr/interrupt.h>
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

uint16_t value = 0;

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

void toZero() {
  i2c_start();
  i2c_write(0b01000000);
  i2c_write(2);
  i2c_write(0b00001111);
  i2c_stop();
  i2c_start();
  i2c_write(0b01000000);
  i2c_write(3);
  i2c_write(0);
  i2c_stop();
}

void  ft_atoi(unsigned int number, char *str) {
  uint8_t i = 0;
  while ((number / 10) > 0) {
    str[i++] = (number % 10) + '0';
    number /= 10;
  }
  str[i++] = number + '0';
  str[i] = 0;
}

void printNumber(uint16_t number) {
  int i = 0;
  uint8_t oct;
  if (number == 0) {
    i2c_start();
    i2c_write(0b01000000);
    i2c_write(2);
    i2c_write(oct);
    i2c_stop();
    i2c_start();
    i2c_write(0b01000000);
    i2c_write(3);
    i2c_write(numbers[0]);
    i2c_stop();
    _delay_ms(1);
  }
  while (number > 0) {
    if (i == 0)
      oct = 0b01111111;
    else if (i == 1)
      oct = 0b10111111;
    else if (i == 2)
      oct = 0b11011111;
    else
      oct = 0b11101111;
    i2c_start();
    i2c_write(0b01000000);
    i2c_write(2);
    i2c_write(oct);
    i2c_stop();
    i2c_start();
    i2c_write(0b01000000);
    i2c_write(3);
    i2c_write(numbers[number % 10]);
    i2c_stop();
    _delay_ms(1);
    i2c_start();
    i2c_write(0b01000000);
    i2c_write(2);
    i2c_write(oct);
    i2c_stop();
    i2c_start();
    i2c_write(0b01000000);
    i2c_write(3);
    i2c_write(0);
    i2c_stop();
    _delay_ms(1);
    i++;
    number /= 10;
  }
}

int main() {
  TCCR1B = (1 << CS12) | (1 << CS10) | (1<<WGM12);
  OCR1A = 15625;
  TIMSK1 = (1 << OCIE1A);
  i2c_init();
  i2c_start();
  i2c_write(0b01000000);
  i2c_write(6);
  i2c_write(0b00001111);
  i2c_stop();
  i2c_start();
  i2c_write(0b01000000);
  i2c_write(7);
  i2c_write(0b00000000);
  i2c_stop();
  sei();
  while(1)
  {
    printNumber(value);
  }
}

ISR(TIMER1_COMPA_vect) {
  if (value == 9999)
      value = 0;
    else
      value++;
}