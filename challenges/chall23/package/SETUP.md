# Setup Instructions for Challenge 23

This challenge is an AVR pwnable and is meant to be flashed onto an AVR processor.
The board that was used in the episode was an Arduino Nano (ATMega328p) but should work on a wide range of processors.


To setup the board for this challenge you first need to flash the board with the `chall23-flash-flag.hex` firmware.
After this is done wait a few seconds and then flash the actual challenge firmware `chall23.hex` onto the board.
This can be done using avrdude and the following commands:

```
avrdude -p m328p -b 9600 -c arduino -P /dev/ttyUSB0 -U flash:w:chall23-flash-flag.hex
sleep 5
avrdude -p m328p -b 9600 -c arduino -P /dev/ttyUSB0 -U flash:w:chall23.hex
```

Now, delete `chall23-flash-flag.hex` as it should not be used while solving the challenge.
It is only for putting the flag in the memory of the board.

The goal of the challenge is to get the board to send the flag over the serial interface using only serial communication.
The intended solution does not involve any hardware attacks. Finally, simply using a AVR flasher to read the memory is not a valid solution.

Good luck!
