#!/bin/sh

HOST=$1
PORT=$2
CHALL="chall6"

if [ -z "$HOST" ]
then
    docker run --rm --name challenge_test -v '/etc/ssl/certs/ca-certificates.crt:/etc/ssl/certs/ca-certificates.crt' -d -p 30000:1337/tcp $CHALL:latest
    cd solve
    HEALTH_CHECK=1 python3 solve.py localhost 30000
    CHALLENGE_HEALTH=$?
    cd -
    docker stop challenge_test
else
    cd solve
    HEALTH_CHECK=1 python3 solve.py $HOST $PORT
    CHALLENGE_HEALTH=$?
    cd -
fi

if [ $CHALLENGE_HEALTH -eq 0 ]
then
    echo "Test passed"
    exit 0
else
    echo "Test failed"
    exit 1
fi
