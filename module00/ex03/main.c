#include <avr/io.h>
#include <util/delay.h>

int main()
{
    DDRB = 1 << PB0;
    while(1)
    {
        // On regarde si le bit 3 du PORT D est nulle;
        // si oui le swicth est appuye, on change letat de la led;
        // puis on attend que le bouton soit lache
        if (!(PIND & 1 << 2))
        {
            if (PORTB == 0)
                PORTB = 1;
            else
                PORTB = 0;
        }
        while (!(PIND & 1 << 2)) {}
        _delay_ms(25);
    }
}