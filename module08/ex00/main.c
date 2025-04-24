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

int main() {
  SPI_init();
  // START
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  // LED FRAME 1
  SPI_MasterTransmit(0b11100001);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0xFF);
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
  while(1) {

  }
}
