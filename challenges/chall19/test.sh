#!/bin/sh

docker run --rm --name challenge_test -v '/etc/ssl/certs/ca-certificates.crt:/etc/ssl/certs/ca-certificates.crt' -d -p 30000:1337/tcp chall19:latest
cd solve
HEALTH_CHECK=1 python3 solve.py localhost 30000
CHALLENGE_HEALTH=$?
cd -
docker stop challenge_test

if [ $CHALLENGE_HEALTH -eq 0 ]
then
    echo "Test passed"
    exit 0
else
    echo "Test failed"
    exit 1
fi
