#include <avr/io.h>

int main() {
    DDRB = 1 << PB1;
   while(1)
   {
        unsigned long i = 0;
        while (i < 236500)
            i++;
        PORTB ^= 1 << PB1;
   }	
}