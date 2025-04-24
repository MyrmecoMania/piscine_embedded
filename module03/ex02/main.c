#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int up = 1;
int position = 0;

void init_rgb() {
  DDRD = (1<<PD3) | (1<<PD5) | (1<<PD6);
  // On lance le timer0 en PWM phase correct avec toggle de
  // OCOB et OCOA pour les led PD5 (rouge) et PD6 (vert)
  TCCR0A = (1<<COM0A1) | (1<<COM0B1) | (1<<WGM00);
  TCCR0B = (1<<CS00);
  // ET timer2 pareil, avec toggle de OC2B (bleu)
  TCCR2A = (1<<COM2B1) | (1<<WGM20);
  TCCR2B = (1<<CS20);
  TCCR1B = (1 << CS12) | (1 << CS10) | (1<<WGM12);
  OCR1A = 255;
  TIMSK1 = (1 << OCIE1A);
  sei();
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

int main() {
  init_rgb();
	while(1)
	{
  }
}

ISR(TIMER1_COMPA_vect) {
  if (position == 255)
    position = 0;
  position++;
  wheel(position);
}