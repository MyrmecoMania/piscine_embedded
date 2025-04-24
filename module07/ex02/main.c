#include <avr/io.h>
#include <avr/eeprom.h>
#define BAUD 115200
#define MYUBBR ((16000000/(8UL*BAUD))-1UL)
#define STARTKEY 0x7f
#define STARTVALUE 0x83
#define ENDDATA 0x89


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

int ft_strcmp(char *str1, char *str2) {
  while (*str1 && *str2) {
    if (*str1 != *str2)
      return 1;
    str1++;
    str2++;
  }
  if ((*str1 && !*str2) || (!*str1 && *str2))
    return 1;
  return 0;
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
  if (number2 > 9)
    number2 += 39;
  uart_tx(number1 + '0');
  uart_tx(number2 + '0');
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

void hexdump() {
  int i = 0, y = 0;
  while (i <= E2END) {
    if (!(i % 16)) {
      y = i;
      uart_print_str("0000");
      if (i < 16)
        uart_print_str("00");
      else
        changeBase(i >> 4);
      uart_print_str("0 ");
    }
    changeBase(EEPROM_read(i));
    if (!((i + 1) % 16)) {
      uart_print_str(" |");
      while ((y + 1) % 16) {
        if (EEPROM_read(y) >= 33 && EEPROM_read(y) <= 126)
          uart_tx(EEPROM_read(y));
        else
          uart_tx(' ');
        y++;
      }
      uart_print_str("|");
      uart_print_str("\r\n");
    }
    if ((i + 1) % 16 && (i % 2))
      uart_tx(' ');
    i++;
  }
}

void strInput(int f, char *str) {
  int i = 0;
  char a;
  if (f == 0)
    uart_print_str("Key: ");
  else
    uart_print_str("Value: ");
  while (i < 32) {
    a = uart_rx();
    if (a == 13)
      break ;
    str[i] = a;
    uart_tx(a);
    i++;
  }
  str[i] = 0;
  uart_print_str("  :");
  uart_print_str(str);
  uart_print_str("\r\n");
}

int findKey(char *key) {
  int i = 0, y, r;
  char foundKey[33];
  while (i <= E2END ) {
    if (EEPROM_read(i) == STARTKEY) {
      y = 0;
      i++;
      r = i;
      while ((EEPROM_read(i) != STARTVALUE) && (i <= E2END)) {
        foundKey[y++] = EEPROM_read(i++);
      }
      foundKey[y] = 0;
      if (!ft_strcmp(foundKey, key))
        return r;
      foundKey[0] = 0;
    }
    i++;
  }
  return -1;
}

void readKey(char *key) {
  int i = findKey(key);
  if (i >= 0) {
    while(EEPROM_read(i) != STARTVALUE)
      i++;
    i++;
    while(EEPROM_read(i) != ENDDATA)
      uart_tx(EEPROM_read(i++));
    uart_print_str("\r\n");
  }
  else
    uart_print_str("empty\r\n");
}

void forgetKey(char *key) {
  int i = findKey(key);
  if (i >= 0) {
    i--;
    EEPROM_write(i, 0xff);
  }
  else
    uart_print_str("Not found\r\n");
}

int findSpace(int len) {
  int i = 0, y = 0, r;
  while (i <= E2END) {
    if (EEPROM_read(i) == STARTKEY) {
      while (EEPROM_read(i) != ENDDATA)
        i++;
    } else {
      r = i;
      while((EEPROM_read(i) != STARTKEY) && (y < len)) {
        i++;
        y++;
      }
      if (y == len)
        return r;
    }
  }
  return (-1);
}

int ft_strlen(char *str) {
  int i = 0;
  while (*str) {
    str++;
    i++;
  }
  return i;
}

void writeKey(char *key, char *value) {
  int i = findKey(key);
  if (i != -1)
    return uart_print_str("Already exist\r\n");
  i = findSpace(ft_strlen(key) + ft_strlen(value) + 3);
  if (i == -1)
    return uart_print_str("no space left\r\n");
  else {
    EEPROM_write(i++, STARTKEY);
    while(*key) {
      EEPROM_write(i++, *key);
      key++;
    }
    EEPROM_write(i++, STARTVALUE);
    while(*value) {
      EEPROM_write(i++, *value);
      value++;
    }
    EEPROM_write(i++, ENDDATA);
  }
}

int main() {
  char key[33];
  char value[33];
  char command[7];
  char a;
  int i;
  uart_init(MYUBBR);
  while(1) {
    i = 0;
    a = 0;
    uart_print_str(">");
    while (i < 6) {
      a = uart_rx();
      if (a == 13)
        break ;
      command[i] = a;
      uart_tx(a);
      i++;
    }
    command[i] = 0;
    uart_print_str("  :");
    uart_print_str(command);
    uart_print_str("\r\n");
    if (!ft_strcmp(command, "READ")) {
      strInput(0, key);
      readKey(key);
    }
    else if (!ft_strcmp(command, "WRITE")) {
      strInput(0, key);
      strInput(1, value);
      writeKey(key, value);
    }
    else if (!ft_strcmp(command, "FORGET")) {
      strInput(0, key);
      forgetKey(key);
    }
    else if (!ft_strcmp(command, "PRINT"))
      hexdump();
    else
      uart_print_str("Unknown command!\r\n");
    command[0] = 0;
  }
}

// while (i < 32) {
//   a = uart_rx();
//   if (a == 13)
//     break ;
//   address[i++] = a;
//   uart_tx(a);
// }
// address[i] = 0;
// uart_print_str("   :   ");
// uart_print_str(address);
// uart_print_str("\r\n");
// i = 0;
// uart_print_str("Value: ");
// while (i < 32) {
//   a = uart_rx();
//   if (a == 13)
//     break ;
//   value[i++] = a;
//   uart_tx(a);
// }
// value[i] = 0;
// uart_print_str("   :   ");
// uart_print_str(value);
// uart_print_str("\r\n");
// if (!checkChar(address) && !checkChar(value)) {
//   if (hextodec(address, 2) > 1023)
//     uart_print_str("Address outside of range\r\n");
//   else {
//     EEPROM_write(hextodec(address, 2), hextodec(value, 1));
//     hexdump(hextodec(address, 2));
//   }
// }
// else
//   uart_print_str("Wrong address or value\r\n");
// address[0] = 0;
// value[0] = 0;
