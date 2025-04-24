#include <avr/io.h>
#include <util/delay.h>

int main()
{
    DDRB = 1 << PB0;
    while(1)
    {
        // On verifie le 3eme bit du registery d'entree du port D,
        // le switch 1 etant assigne au bit 2 du PORT D, si le bit est 0,
        // on allume sinon on eteint
        // pull up resistor, le signal est bas quand le switch est appuye
        if (PIND & 1 << 2) {
            // on reset les bits du port B a 0;
            PORTB = 0;
        }
        else {
            // on change le premier bit du registery de sortie B a 1
            // on passe de 00000000 avec aucune led eteinte
            // a 00000001 avec premiere led allumee
            PORTB = 1;
        }
    }
}