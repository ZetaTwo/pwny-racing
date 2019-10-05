#!/bin/sh
CHALLENGE_NAME=$1

docker run --name challenge_test --rm -v '/etc/ssl/certs/ca-certificates.crt:/etc/ssl/certs/ca-certificates.crt' -d -p 30000:1337/tcp ${CHALLENGE_NAME}:latest
