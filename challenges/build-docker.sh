#!/bin/sh

set -e

CHALLENGE_NAME=$1
DST_DIR=${CHALLENGE_NAME}_docker

mkdir $DST_DIR
cp $CHALLENGE_NAME/$CHALLENGE_NAME $DST_DIR/chall
cp $CHALLENGE_NAME/.config.json $DST_DIR/.config.json
cp docker-pwn/* $DST_DIR
cp $CHALLENGE_NAME/flag $DST_DIR
cp ../submission/client/client $DST_DIR/flag_submitter
cd $DST_DIR
docker build -t $CHALLENGE_NAME .
echo $DST_DIR
rm -rf $DST_DIR
