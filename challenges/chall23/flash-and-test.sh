#!/bin/sh -v
CHALL=chall23
SERIALPORT=/dev/ttyACM0
FLASH="avrdude -p m328p -P $SERIALPORT -c arduino"

# Write flag to eeprom
$FLASH -U flash:w:bin/${CHALL}-flash-flag.hex
echo "Waiting for flag to be written..."
#grep -l "Flag written to EEPROM" $SERIALPORT

sleep 5

# Write challenge to eeprom
$FLASH -U flash:w:bin/${CHALL}.hex
grep -l "RESET" $SERIALPORT

python3 solve/solve.py
