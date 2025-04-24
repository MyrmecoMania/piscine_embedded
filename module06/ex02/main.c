#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdlib.h>
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

void uart_print_str(const char *str)
{
  while (*str)
  {
    uart_tx(*str);
    str++;
  }
}

void i2c_init(void) {
  TWBR = 72;
  TWCR = (1<<TWEN);
}

void i2c_start(void) {
  TWCR = (1<<TWEN) | (1<<TWSTA) | (1<<TWINT);
  while (!(TWCR & (1<<TWINT)));
}

void i2c_stop(void) {
  TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO);
  while (!(TWCR & (1<<TWSTO)));
}


void i2c_write(unsigned char data) {
  TWDR = data;
  TWCR = (1<<TWEN) | (1<<TWINT);
  while (!(TWCR & (1<<TWINT)));
}

uint8_t i2c_read(void) {
  TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWEA);
  while (!(TWCR & (1<<TWINT)));
  return TWDR;
}

void print_hex_value(uint32_t *temperature, uint32_t *humidity) {
  char str[4];
  float medtemp = (temperature[0] + temperature[1] + temperature[2]) / 3;
  float medhumi = (humidity[0] + humidity[1] + humidity[2]) / 3;
  uart_print_str("Temperature: ");
  uart_print_str(dtostrf(((float)temperature[0] / 1048576.0) * 200 - 50, 10, 1, str));
  uart_print_str("C, Humidity: ");
  uart_print_str(dtostrf(((float)humidity[0] / 1048576.0) * 100, 10, 2, str));
  uart_print_str("%\r\n");
}

void checkConverstion(void) {
  uint8_t status = (1<<7);
  while (status & (1<<7)) {
    _delay_ms(80);
    i2c_write(0b01110001);
    status = i2c_read();
  }
}

int main() { 
  uint8_t data[7];
  uint8_t i, y;
  uint32_t temperature[3];
  uint32_t humidity[3];
  uart_init(MYUBBR);
  i2c_init();
  i2c_start();
  _delay_ms(40);
  i2c_write(0b01110000);
  i2c_write(0xbe);
  i2c_write(0x08);
  i2c_write(0);
  _delay_ms(20);
  while(1)
  {
    i = 0;
    while (i < 3) {
      y = 0;
      i2c_start();
      i2c_write(0b01110000);
      i2c_write(0xAC);
      i2c_write(0x08);
      i2c_write(0);
      _delay_ms(100);
      i2c_start();
      i2c_write(0b01110001);
      while (y < 7) {
        data[y++] = i2c_read();
      }
      // on recoit 40 bits de donnees, 20 pour chaque
      // 2 octets et demie chacun, on doit couper celui du millieu
      humidity[i] = ((uint32_t)data[1] << 12 | (uint32_t)data[2] << 4 | data[3] >> 4);
      temperature[i] = (((uint32_t)data[3] & (0x0F)) << 16 | (uint32_t)data[4] << 8 | (uint32_t)data[5]);
      i++;
    }
    print_hex_value(temperature, humidity);
  }
}

