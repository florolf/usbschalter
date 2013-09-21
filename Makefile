all: main.c
	gcc -Wall -Werror -o usbschalter main.c -lftdi

clean:
	rm -f usbschalter
