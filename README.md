# Purpose

In our hackerspace [RFID dooropener project](https://github.com/entropia/tuerd),
we encountered the problem that the RFID reader would randomly lock up so badly
that only disconnecting and reconnecting it to USB would help. While locking
up could be detected in software, nothing could be done except sending out an
alert and hoping that someone would manually powercycle the reader. This project
attempts to fix this. It allows another piece of software to physically
disconnect and reconnect an USB device.

# Usage

The daemon is meant to be run by some service supervision scheme like
daemontools or [runit](http://smarden.org/runit/). It attaches itself to the
FTDI device (should you have more than one FTDI device present, you'll probably
have to change the VID/PID of one of them to disambiguate) and waits for one of
the following signals in a loop:

* <code>SIGUSR1</code> will turn the device on
* <code>SIGUSR2</code> will turn the device off
* <code>SIGHUP</code> will turn the device off, wait 2 seconds and turn the
  device on again

When the daemon is started, the device will be turned on.
