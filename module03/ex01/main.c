#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define RED 0b00100000
#define GREEN 0b01000000
#define BLUE 0b00001000
#define YELLOW 0b01100000
#define CYAN 0b01001000
#define MAGENTA 0b00101000
#define WHITE 0b01101000

int main() {
  DDRD = (1<<PD3) | (1<<PD5) | (1<<PD6);
  TCCR1B = (1 << CS12) | (1 << CS10) | (1<<WGM12);
  OCR1A = 15625;
  TIMSK1 = (1 << OCIE1A);
  PORTD = 1<<PD5;
	sei();
  while(1)
  {
  }
}

ISR(TIMER1_COMPA_vect) {
  switch (PORTD) {
    case RED:
      PORTD = GREEN;
      break;
    case GREEN:
      PORTD = BLUE;
      break;
    case BLUE:
      PORTD = YELLOW;
      break;
    case YELLOW:
      PORTD = CYAN;
      break;
    case CYAN:
      PORTD = MAGENTA;
      break;
    case MAGENTA:
      PORTD = WHITE;
      break;
    case WHITE:
      PORTD = RED;
      break;
  }
}