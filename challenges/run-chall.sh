#!/bin/sh

CHALLENGE_NAME=$1
PORT=$2

sudo docker run -v '/etc/ssl/certs/ca-certificates.crt:/etc/ssl/certs/ca-certificates.crt' -d -p ${PORT}:1337/tcp ${CHALLENGE_NAME}:latest
