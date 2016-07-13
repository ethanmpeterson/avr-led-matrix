#! /bin/sh
avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o main.o main.c -std=gnu99 \
		&& avr-gcc -mmcu=atmega328p main.o -o main \
		&& avr-objcopy -O ihex -R .eeprom main main.hex \
		&& sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:main.hex
