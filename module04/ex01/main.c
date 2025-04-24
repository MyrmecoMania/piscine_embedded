#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t up = 1;
uint8_t overflowcounter = 0;
int main() {

  DDRB = 1 << PB1;
  // Timer0 en normal avec un prescaler de 64
  TCCR0B = (1<<CS00) | (1<<CS01);
  // pour arriver a 0.05 secondes: 
  // 16000000/64 = 250 000
  // 250 000 /20 = 12500
  // 125000 / 255 = 49 
  // interrupt lors de overflow
  TIMSK0 = (1<<TOIE0);
  // timer1 en fast PWM va toggle la led
  TCCR1B = (1<<CS10) | (1<<WGM12) | (1<<WGM13);
  TCCR1A = (1<<WGM11 | 1<<COM1A1 );
  ICR1 = 10;
  OCR1A = 0;
	sei();
  while(1)
  {
  }
}

ISR(TIMER0_OVF_vect) {
  if (overflowcounter == 49) {
    if (OCR1A == 0)
      up = 1;
    else if (OCR1A == 10)
      up = 0;
    if (up == 1)
      OCR1A++;
    else
      OCR1A--;
    TCNT0 = 0;
  }
  overflowcounter++;
  if (overflowcounter > 49)
    overflowcounter = 0;
}