#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void adc_init() {
  // ADEN eable adc, prescaler de 128 pour 125kHz
  ADCSRA |= (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
  // REFs0 pour lire AVcc et ADLAR vu quon a que 8bit ADCH
  ADMUX	=	(1<<REFS0) | (1<<ADLAR);
}

uint16_t adc_read() {
  // ON commence la conversion puis attendont le resultat
  ADCSRA |= (1<<ADSC);
  while ( (ADCSRA & (1<<ADSC)) );
  return ADCH;
}

void SPI_init() {
  DDRB =  (1<<PB2) | (1<<PB3) | (1<<PB5);
  SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void SPI_MasterTransmit(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1<<SPIF)));
}

void sendRed(int mode) {
  // START
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  // LED FRAME 1
  if (mode <= 3) {
    SPI_MasterTransmit(0b11100001);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
  } else {
    SPI_MasterTransmit(0b11100001);
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0);
  }
  // LED FRAME 2
  if (mode <= 2) {
    SPI_MasterTransmit(0b11100001);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
  } else {
    SPI_MasterTransmit(0b11100001);
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0);
  }
  // LED FRAME 3
  if (mode <= 1) {
    SPI_MasterTransmit(0b11100001);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
  } else {
    SPI_MasterTransmit(0b11100001);
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0);
  }
  // END FRAME
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
}

int main() {
  SPI_init();
  adc_init();
  uint8_t a;
  while(1) {
    a = adc_read();
    if (a < 85)
      sendRed(5);
    if (a > 85)
      sendRed(3);
    if (a > 170)
      sendRed(2);
    if (a == 255)
      sendRed(1);
    _delay_ms(50);
  }
}
