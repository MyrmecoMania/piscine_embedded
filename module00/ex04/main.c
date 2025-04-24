#include <avr/io.h>
#include <util/delay.h>

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

int main()
{
    DDRB = 1 << PB0 | 1 << PB1 | 1 << PB2 | 1 << PB4;  
    int value = 0;
    int newValue = 0;
    while(1)
    {
        // j'incremente si le bit 3 du port D est de 0
        if (!(PIND & 1 << 2))
        {
            if (value == 15)
                value = 0;
            else
                value++;
            while (!(PIND & 1 << 2)) {}
        }
        // je decremente si le bit 5 du port D est de 0
        if (!(PIND & 1 << 4))
        {
            if (value == 0)
                value = 15;
            else
                value--;
            while (!(PIND & 1 << 4)) {}
        }
        // si la valeure est en dessous de 7 pas besoin de la led 4
        if (value <= 7)
            PORTB = value;
        // je saute le bit 4, je le remet a 0, et change le bit 5
        // La led 4 etant pin au bit 5 du port b
        else {
            newValue = modifyBit(value, 3, 0);
            newValue = modifyBit(newValue, 4, 1);
            PORTB = newValue;
        }
        _delay_ms(25);
    }
}