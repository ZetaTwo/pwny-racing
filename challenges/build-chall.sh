#!/bin/sh
CHALLENGE_NAME=$1
cd $CHALLENGE_NAME
./build.sh
cd -
