#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD 115200
#define MYUBBR ((16000000/(8UL*BAUD))-1UL)

uint8_t rgbLed[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
int currentLed = 0, currentColor = 0;

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


void ledControl() {
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  SPI_MasterTransmit(0);
  // LED FRAME 1
  SPI_MasterTransmit(0b11100001);
  SPI_MasterTransmit(rgbLed[0][0]);
  SPI_MasterTransmit(rgbLed[0][1]);
  SPI_MasterTransmit(rgbLed[0][2]);
  // LED FRAME 2
  SPI_MasterTransmit(0b11100001);
  SPI_MasterTransmit(rgbLed[1][0]);
  SPI_MasterTransmit(rgbLed[1][1]);
  SPI_MasterTransmit(rgbLed[1][2]);
  // LED FRAME 3
  SPI_MasterTransmit(0b11100001);
  SPI_MasterTransmit(rgbLed[2][0]);
  SPI_MasterTransmit(rgbLed[2][1]);
  SPI_MasterTransmit(rgbLed[2][2]);
  //END FRAME 
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
  SPI_MasterTransmit(0xFF);
}

int main() {
  int i = 0;
  int up = 1;
  SPI_init();
  adc_init();
  EIMSK = (1<<INT0);
  PCICR = (1<<PCIE2);
  PCMSK2 = (1<<PCINT20);
  EICRA = (1<<ISC01) | (1<<ISC11);
  sei();
  while(1) {
    uint8_t a = adc_read();
    rgbLed[currentLed][currentColor] = a;
    cli();
    ledControl();
    sei();
  }
}


// interrupt de INT0 SW1
ISR(INT0_vect) {
  _delay_ms(50);
  if (!(PIND & 1 << PD2)) {
    _delay_ms(50);
    if (currentLed == 2)
      currentLed = 0;
    else
      currentLed++;
  }
  while (!(PIND & 1 << PD2));
}

//  interrupt de PCINT20 SW2
ISR(PCINT2_vect) {
  _delay_ms(50);
  if (!(PIND & 1 << PD4)) {
    _delay_ms(50);
    if (currentColor == 2)
      currentColor = 0;
    else
      currentColor++;
  }
  while (!(PIND & 1 << PD4));
}