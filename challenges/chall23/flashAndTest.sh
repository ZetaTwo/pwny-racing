#!/bin/sh -v
CHALL=chall23
SERIALPORT=/dev/ttyACM0
FLASH="avrdude -p m328p -P $SERIALPORT -c arduino"
mkdir tmp
cd tmp || exit
tar xzvf ../${CHALL}-secret-for-organizer.tgz

# Write flag to eeprom
$FLASH -U flash:w:writeFlagToEEPROM.hex
echo "Waiting for flag to be written..."
grep -l "Flag written to EEPROM" $SERIALPORT

# Write challenge to eeprom
$FLASH -U flash:w:chall23.hex
grep -l "RESET" $SERIALPORT

cd ..
rm -rf tmp
python solve/solve.py
