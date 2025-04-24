#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t toggle = 0;
int main()
{
    DDRB = 1 << PB0;
    // J'active les PCINT 23-16
    PCICR = (1<<PCIE2);
    // j'avoue que jai pas encore compris
    EIMSK = (1<<INT0);
    // J'active le PCINT18, liee au swicth SW1
    PCMSK2 = (1<<PCINT18);
    // Timer1 va remettre l'etat de mon bouton a 0
    TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS11);
    // interrupt de comparaison
    TIMSK1 = (1<<OCIE1A);
    OCR1A = 12500;
    sei();
    while(1)
    {
    }
}

ISR(TIMER1_COMPA_vect) {
    toggle = 0;
}

ISR(PCINT2_vect) {
    if (toggle == 0)
    {
        PORTB ^= 1 << PB0;
        toggle = 1;
    }
}