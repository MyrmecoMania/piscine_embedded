#include <avr/io.h>

int main() {
    DDRB= 1 << PB1;
    // on utilise le timer1 en mode Fast PWM
    // OC1A est clear a 7812, puis set a bottom 0 
    // 16000000/1024 = 15625
    TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12) | (1<<WGM13);
    TCCR1A = (1<<WGM11 | 1<<COM1A1 );
    ICR1 = 15625;
    OCR1A = 7812;
  	while(1);
}