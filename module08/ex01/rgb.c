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
  SPI_MasterTransmit(0b11100001);
  if (mode <= 1) {
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0xff);
  } else if (mode <= 2) {
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0);
  } else if (mode == 3) {
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0);
  } else if (mode == 4) {
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
  } else if (mode == 5) {
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0);
  } else if (mode == 6) {
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0);
    SPI_MasterTransmit(0xff);
  } else if (mode == 7) {
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
    SPI_MasterTransmit(0xff);
  }
  // LED FRAME 2
  SPI_MasterTransmit(0xff);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  // LED FRAME 3
  SPI_MasterTransmit(0xff);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  // END FRAME
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
}

int main() {
  SPI_init();
  while(1) {
    sendRed(1);
    _delay_ms(1000);
    sendRed(2);
    _delay_ms(1000);
    sendRed(3);
    _delay_ms(1000);
    sendRed(4);
    _delay_ms(1000);
    sendRed(5);
    _delay_ms(1000);
    sendRed(6);
    _delay_ms(1000);
    sendRed(7);
    _delay_ms(1000);
  }
}
