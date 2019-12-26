#!/bin/sh

echo "                            pwny.racing presents...\n\n"\
"\e[32;1m\e[47m▄\e[0m\e[32;1m█████████▄\e[0m \e[32;1m\e[47m▄\e[0m\e[32;1m███     \e[32;1m\e[47m▄\e[0m\e[32;1m███\e[0m \e[32;1m\e[47m▄\e[0m\e[32;1m███   \e[32;1m\e[47m▄\e[0m\e[32;1m███\e[0m \e[32;1m▄\e[47m▄\e[0m\e[32;1m███\e[0m \e[32;1m▄\e[47m▄\e[0m\e[32;1m███\e[0m\n"\
"\e[32;1m████\e[42;1m▀▀▀████\e[0m \e[32;1m████     ████\e[0m \e[32;1m█████▄ ████\e[0m \e[32;1m▀████\e[0m \e[32;1m▀████\e[0m\n"\
"\e[32;1m████▄▄▄████\e[0m \e[32;1m████  ▄  ████\e[0m \e[32;1m███████████\e[0m  \e[32;1m████\e[0m  \e[32;1m████\e[0m\n"\
"\e[32;1m██████████\e[42;1m▀\e[0m \e[32;1m████▄███▄████\e[0m \e[32;1m████\e[0m\e[32m▀\e[32;1m\e[42;1m▀█████\e[0m  \e[32;1m████\e[0m  \e[32;1m████\e[0m\n"\
"\e[32;1m████\e[0m\e[32m▀▀▀▀▀▀\e[0m  \e[32;1m\e[42;1m██████▀██████\e[0m \e[32;1m████  \e[0m\e[32m▀\e[32;1m████\e[0m  \e[32;1m████\e[0m  \e[32;1m████\e[0m\n"\
"\e[32;1m████\e[0m        \e[32;1m████\e[42;1m▀\e[0m\e[32m▀\e[0m \e[32m▀\e[0m\e[32;1m\e[42;1m▀████\e[0m \e[32;1m████   ████\e[0m  \e[32;1m████\e[0m  \e[32;1m████\e[0m\n"\
"\e[32m▀▀▀▀\e[0m        \e[32m▀▀▀▀     ▀▀▀▀\e[0m \e[32m▀▀▀▀   ▀▀▀▀\e[0m  \e[32m▀▀▀▀\e[0m  \e[32m▀▀▀▀\e[0m\n"\
"\n"


echo "Connect to the device, for example with one of these methods:"
echo "minicom -D /dev/ttyACM0 -b 9600"
echo "python -c 'from pwn import *; r = serialtube(\"/dev/ttyACM0\", baudrate=9600); r.recv(...)'"
echo ""
echo "To convert the challenge to a binary format for loading in some RE tools, you can run:"
echo "objcopy -I ihex -O binary infile.hex outfile.dat"
