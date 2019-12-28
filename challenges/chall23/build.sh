#!/bin/sh

CHALL="chall23"

DOCKER="sudo docker"

echo "Building challenge and container"
$DOCKER build -t ${CHALL} .

echo "Compiling..."
rm -rf src/build
mkdir src/build
$DOCKER run -t --volume="$PWD/src/:/src/:rw" chall23 make -C /src/build -f /src/Makefile

echo "Packaging dist.tgz"
mkdir tmp
cp src/build/chall23.hex tmp/
tar czvf ${CHALL}-dist.tgz -C tmp .

echo "Packaging secret-for-organizer.tgz"
cp src/build/writeFlagToEEPROM.hex tmp/
tar czvf ${CHALL}-secret-for-organizer.tgz -C tmp .
rm -rf tmp

