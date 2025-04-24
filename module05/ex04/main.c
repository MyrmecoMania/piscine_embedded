#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_rgb() {
  DDRD = (1<<PD3) | (1<<PD5) | (1<<PD6);
  DDRB = (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB4);
  // On lance le timer0 en PWM phase correct avec toggle de
  // OCOB et OCOA pour les led PD5 (rouge) et PD6 (vert)
  TCCR0A = (1<<COM0A1) | (1<<COM0B1) | (1<<WGM00);
  TCCR0B = (1<<CS00);
  // ET timer2 pareil, avec toggle de OC2B (bleu)
  TCCR2A = (1<<COM2B1) | (1<<WGM20);
  TCCR2B = (1<<CS20);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
  OCR0A = g;
  OCR0B = r;
  OCR2B = b;
}

void wheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    set_rgb(255 - pos * 3, 0, pos * 3);
  } else if (pos < 170) {
    pos = pos - 85;
    set_rgb(0, pos * 3, 255 - pos * 3);
  } else {
    pos = pos - 170;
    set_rgb(pos * 3, 255 - pos * 3, 0);
  }
}

void adc_init() {
  // ADEN eable adc, ADPS0 et ADPS1 pour 8 bits
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

char  changeBase(int number) {
  if (number > 9)
    number += 39;
  return (number + '0');
}


ISR(TIMER1_COMPA_vect) {
  uint8_t read = adc_read();
  wheel(read);
  PORTB = 0;
  if (read >= 64)
    PORTB |= (1<<PB0);
  if (read >= 128)
    PORTB |= (1<<PB1);
  if (read >= 192)
    PORTB |= (1<<PB2);
  if (read >= 254)
    PORTB |= (1<<PB4);
}

int main() {
  adc_init();
  init_rgb();
  // on initialise un compteur en mode CTC, avec un prescaler de 64
  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS11);
  // interrupt de comparaison
  TIMSK1 = (1<<OCIE1A);
  // 16000000/64 = 250 000
  // 250 000/50 = 5000;
  OCR1A = 5000;
	sei();
  while(1)
  {
  }
}
