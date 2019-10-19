#!/bin/sh

set -e

HOST=$1

./test-chall.sh chall01 $HOST
./test-chall.sh chall02 $HOST
./test-chall.sh chall03 $HOST

./test-chall.sh chall05 $HOST
./test-chall.sh chall06 $HOST
./test-chall.sh chall07 $HOST
./test-chall.sh chall08 $HOST
./test-chall.sh chall09 $HOST
./test-chall.sh chall10 $HOST
./test-chall.sh chall11 $HOST
./test-chall.sh chall12 $HOST
./test-chall.sh chall13 $HOST
./test-chall.sh chall14 $HOST
./test-chall.sh chall15 $HOST
#./test-chall.sh chall16 $HOST
./test-chall.sh chall17 $HOST
#./test-chall.sh chall18 # Takes a lot of time (passes)
./test-chall.sh chall19 $HOST
