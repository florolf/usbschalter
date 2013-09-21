#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <ftdi.h>
#include <signal.h>

#define SCHALTER_VID 0x0403
#define SCHALTER_PID 0x6001

enum action {
	ON, OFF, CYCLE, IDLE
};

struct ftdi_context ftdi;
volatile enum action action = IDLE;

void handler(int sig) {
	switch(sig) {
		case SIGUSR1:
			action = ON;
			break;
		case SIGUSR2:
			action = OFF;
			break;
		case SIGHUP:
			action = CYCLE;
			break;
	}
}

void open_ftdi(void) {
	int f = ftdi_usb_open(&ftdi, SCHALTER_VID, SCHALTER_PID);
	if(f < 0) {
		fprintf(stderr, "ftdi_usb_open failed: %s\n", ftdi_get_error_string(&ftdi));
		exit(1);
	}
}

void reopen_ftdi(void) {
	int f = ftdi_usb_close(&ftdi);
	if(f < 0) {
		fprintf(stderr, "ftdi_usb_close failed: %s\n", ftdi_get_error_string(&ftdi));
		exit(1);
	}

	open_ftdi();
}

void set_usb(bool state) {
	int bitmask = 0x10 | (state ? 0 : 1);

	fprintf(stderr, "setting output to %s\n", state ? "ON" : "OFF");

	while(ftdi_set_bitmode(&ftdi, bitmask, BITMODE_CBUS) < 0) {
		fprintf(stderr, "ftdi_set_bitmode failed: %s\n", ftdi_get_error_string(&ftdi));
		reopen_ftdi();

		sleep(1);
	}
}

int main(int argc, char **argv) {
	fprintf(stderr, "usbschalter starting up\n");

	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);

	ftdi_init(&ftdi);
	open_ftdi();

	set_usb(true);

	while(1) {
		pause();

		switch(action) {
			case OFF:
				set_usb(false);
				break;
			case ON:
				set_usb(true);
				break;
			case CYCLE:
				set_usb(false);
				sleep(2);
				set_usb(true);
				break;
			default:
				fprintf(stderr, "unhandled signal received\n");
				break;
		}

		action = IDLE;
	}

}
