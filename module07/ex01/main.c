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
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
}

void uart_tx(char c) {
  // On envoie notre charactere
  // a laide du buffer de transmition
  while(!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

unsigned char uart_rx(void)
{
  while(!(UCSR0A & (1 << RXC0)));
  return UDR0;
}

unsigned char EEPROM_read(unsigned int address) {
  while (EECR & (1<<EEPE));
  EEAR = address;
  EECR |= (1<<EERE);
  return EEDR;
}

void EEPROM_write(unsigned int address, unsigned char data)
{
  while(EECR & (1<<EEPE));
  EEAR = address;
  EEDR = data;
  EECR |= (1<<EEMPE);
  EECR |= (1<<EEPE);
}

void uart_print_str(const char *str)
{
  while (*str)
  {
    uart_tx(*str);
    str++;
  }
}

void  changeBase(uint8_t number, int color) {
  int number1 = number >> 4;
  int number2 = number & 0x0F;
  if (number1 > 9)
    number1 += 39;
  if (number2 > 9)
    number2 += 39;
  if (color) {
    uart_print_str("\x1b[31m");
    uart_tx(number1 + '0');
    uart_tx(number2 + '0');
    uart_print_str("\x1b[0m");
  }
  else {
    uart_tx(number1 + '0');
    uart_tx(number2 + '0');
  }
}

int isPresent(char a) {
  char hexa[] = "0123456789abcdef";
  int i = 0;
  while (hexa[i]) {
    if (hexa[i] == a)
      return 1;
    i++;
  }
  return 0;
}

int checkChar(char *str) {
  while (*str) {
    if (!isPresent(*str))
      return 1;
    str++;
  }
  return 0;
}

int ft_pow(int n, int p) {
  if (p == 0)
    return 1;
  while (p > 1) {
    n *= n;
    p--;
  }
  return n;
}

void  printNumber(unsigned int number) {
  char numberChar[4];
  uint8_t i = 0;
  if (number == 0)
    return uart_tx('0');
  while ((number / 10) > 0) {
    numberChar[i++] = (number % 10) + '0';
    number /= 10;
  }
  numberChar[i] = number + '0';
  while (i > 0) {
    uart_tx(numberChar[i--]);
  }
  uart_tx(numberChar[0]);
}

uint32_t hextodec(char *str, int len) {
  unsigned int final = 0;
  int i = 0;
  while(str[i]) {
    if (str[i] >= '0' && str[i] <= '9')
      final += ((str[i] - '0') * ft_pow(16, len));
    if (str[i] >= 'a' && str[i] <= 'f')
      final += ((str[i] - 87) * ft_pow(16, len));
    i++;
    len--;
  }
  return final;
}

void hexdump(unsigned int address) {
  int i = 0;
  while (i <= E2END) {
    if (!(i % 16)) {
      uart_print_str("0000");
      if (i < 16)
        uart_print_str("00");
      else if (i >= 16 && i < 1000)
        changeBase(i >> 4, 0);
      else if (i > 1000)
        changeBase(i >> 4, 0);
      uart_print_str("0 ");
    }
    if (address == i)
      changeBase(EEPROM_read(i), 1);
    else
      changeBase(EEPROM_read(i), 0);
    if ((i + 1) % 16 && (i % 2))
      uart_tx(' ');
    else if (!((i + 1) % 16))
      uart_print_str("\r\n");
    i++;
  }
}

int main() {
  char address[4];
  char value[3];
  char a;
  int i;
  uart_init(MYUBBR);
  while(1) {
    i = 0;
    uart_print_str("Address: ");
    while (i < 3) {
      a = uart_rx();
      address[i++] = a;
      uart_tx(a);
    }
    address[3] = 0;
    uart_print_str("   :   ");
    uart_print_str(address);
    uart_print_str("\r\n");
    i = 0;
    uart_print_str("Value: ");
    while (i < 2) {
      a = uart_rx();
      value[i++] = a;
      uart_tx(a);
    }
    value[2] = 0;
    uart_print_str("   :   ");
    uart_print_str(value);
    uart_print_str("\r\n");
    if (!checkChar(address) && !checkChar(value)) {
      if (hextodec(address, 2) > 1023)
        uart_print_str("Address outside of range\r\n");
      else {
        EEPROM_write(hextodec(address, 2), hextodec(value, 1));
        hexdump(hextodec(address, 2));
      }
    }
    else
      uart_print_str("Wrong address or value\r\n");
    address[0] = 0;
    value[0] = 0;
  }
}
