#include <avr/io.h>

int main()
{
    DDRB = 1 << PB0;
    while(1)
    {
        // on change le premier bit du port B a 1
        // on passe de 00000000 avec aucune led allumee
        // a 00000001 avec premiere led allumee
        // la led R1 etant pin au premier bit du port B
        PORTB =  1;
    }
}