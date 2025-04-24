F_CPU = 16000000UL

all: hex flash

hex:
	avr-gcc -mmcu=atmega328p -Os -DF_CPU=${F_CPU} main.c -o main.bin
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
flash:
	avrdude -c arduino -P /dev/ttyUSB0 -p m328p -b 115200 -U flash:w:main.hex
	screen /dev/ttyUSB0 115200
clean:
	rm main.bin
	rm main.hex