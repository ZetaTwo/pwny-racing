#!/bin/sh
CHALLENGE_NAME=$1
cd $CHALLENGE_NAME
./test.sh
cd -
