#!/bin/sh

CHALLENGE_NAME=$1

sudo docker save $CHALLENGE_NAME | gzip > ${CHALLENGE_NAME}.tgz
