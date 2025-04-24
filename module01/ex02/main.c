#include <avr/io.h>

int main() {
  // meme chose qu'avant, mais cette fois ci la les
  // est clear arrive a OCR1A
    DDRB= 1 << PB1;        
    TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12) | (1<<WGM13);
    TCCR1A = (1<<WGM11 | 1<<COM1A1 );
    ICR1 = 15625;
    OCR1A = 1562;
  	while(1);
}