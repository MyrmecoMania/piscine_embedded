#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main() {
  DDRD = (1<<PD3) | (1<<PD5) | (1<<PD6);
  TCCR1B = (1 << CS12) | (1 << CS10) | (1<<WGM12);
  OCR1A = 15625;
  // J'active l'interupt a la comparaison avec OCR1A
  TIMSK1 = (1 << OCIE1A);
  // J'alume a led rouge
  PORTD = 1<<PD5;
	sei();
  while(1)
  {
  }
}

ISR(TIMER1_COMPA_vect) {
  if (PORTD & 1<<PD6) {
    PORTD = 0;
    PORTD = 1<<PD3;
  }
  else if (PORTD & 1<<PD3) {
    PORTD = 0;
    PORTD = 1<<PD5;
  }
  else {
    PORTD = 0;
    PORTD = 1<<PD6;
  }
}