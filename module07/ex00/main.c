#include <avr/io.h>
#include <avr/eeprom.h>
#define BAUD 115200
#define MYUBBR ((16000000/(8UL*BAUD))-1UL)


void uart_init(unsigned int ubrr) {
  // On initialize UART
  UBRR0L = (unsigned char)ubrr;
  UBRR0H = (unsigned char)(ubrr >> 8);
  // On active le double speed, la reception et l'envoi
  UCSR0A = (1<<U2X0);
  UCSR0B |= (1 << TXEN0);
}

void uart_tx(char c) {
  // On envoie notre charactere
  // a laide du buffer de transmition
  while(!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

unsigned char EEPROM_read(unsigned int address) {
  while (EECR & (1<<EEPE));
  EEAR = address; // l'addresse ou je veux aller 
  EECR |= (1<<EERE); // enable read
  return EEDR; // EEPROM data reguistery
}

void uart_print_str(const char *str)
{
  while (*str)
  {
    uart_tx(*str);
    str++;
  }
}

void  changeBase(uint8_t number) {
  int number1 = number >> 4;
  int number2 = number & 0x0F;
  if (number1 > 9)
    number1 += 39;
  uart_tx(number1 + '0');
  if (number2 > 9)
    number2 += 39;
  uart_tx(number2 + '0');
}

int main() { 
  int i = 0;
  uart_init(MYUBBR);
  while (i <= E2END) {
    if (!(i % 16)) {
      uart_print_str("0000");
      if (i < 16)
        uart_print_str("00");
      else if (i >= 16 && i < 1000)
        changeBase(i >> 4);
      else if (i > 1000)
        changeBase(i >> 4);
      uart_print_str("0 ");
    }
    changeBase(EEPROM_read(i));
    if ((i + 1) % 16 && (i % 2))
      uart_tx(' ');
    else if (!((i + 1) % 16))
      uart_print_str("\r\n");
    i++;
  }
  while(1) {
  }
}

