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

void adc_init() {
  // ADEN eable adc, prescaler de 128 pour 125kHz
  ADCSRA |= (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
  // REFs0 pour lire AVcc et ADLAR vu quon a que 8bit ADCH
  ADMUX	=	(1<<REFS0);
}

uint16_t adc_read() {
  // ON commence la conversion puis attendont le resultat
  ADCSRA |= (1<<ADSC);
  while ( (ADCSRA & (1<<ADSC)) );
  return ADC;
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
  while (i < 4) {
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
  uint8_t value = 0;
  i2c_init();
  adc_init();
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
  while(1)
  {
    uint16_t a = adc_read();
    printNumber(a);
  }
}
