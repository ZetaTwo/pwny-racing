#!/bin/sh

CHALLENGE_NAME=$1
PORT=$2

sudo docker run -d -p ${PORT}:1337/tcp ${CHALLENGE_NAME}:latest
