#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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
  if (mode == 1) {
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
  if (mode == 2) {
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
  if (mode == 3) {
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
  while(1) {
    sendRed(0);
    _delay_ms(83);
    sendRed(1);
    _delay_ms(250);
    sendRed(0);
    _delay_ms(83);
    sendRed(2);
    _delay_ms(250);
    sendRed(0);
    _delay_ms(83);
    sendRed(3);
    _delay_ms(250);
  }
}
