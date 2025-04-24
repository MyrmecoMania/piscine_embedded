#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int number = 0;

int modifyBit(int n, int p, int b) 
{ 
    // je change le bit a la position souhaitee a 0
    // 00001011, 00001000, 
    // 11110111, 00001011,
    // 00000011, 00000000,
    // 00000011
    // 00000011, 00010000,
    // 11101111, 00000011,
    // 00000011, 00010000,
    // 00010011,
    int mask = 1 << p; 
    return ((n & ~mask) | (b << p)); 
}

void    display() {
    int newValue = 0;
    if (number <= 7)
        PORTB = number;
    else {
        newValue = modifyBit(number, 3, 0);
        newValue = modifyBit(newValue, 4, 1);
        PORTB = newValue;
    }
    _delay_ms(50);
}

// interrupt de INT0 SW1
ISR(INT0_vect) {
    if (number == 15)
        number = 0;
    else
        number++;
    while (!(PIND & 1 << PD2));
    display();
}

//  interrupt de PCINT20 SW2
ISR(PCINT2_vect) {
    if (PIND & 1 << PD4) {
        if (number == 0)
            number = 15;
        else
            number--;
    }
    while (!(PIND & 1 << PD4));
    display();
}

int main()
{
    DDRB = 1 << PB0 | 1 << PB1 | 1<< PB2 | 1 << PB4;
    // j active interrupt lorsque le bouton SW1 est appuye
    EIMSK = (1<<INT0);
    // j'active les interrupts quil y a un chagement detat dans les pcint de 16-23
    PCICR = (1<<PCIE2);
    PCMSK2 = (1<<PCINT20);
    sei();
    while(1)
    {
    }
}