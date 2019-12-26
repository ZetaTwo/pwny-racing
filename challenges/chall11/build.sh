#!/bin/sh

echo "Building challenge and container"
docker build -t chall11 .

echo "Extracting build artifacts"
mkdir -p bin
docker container create --name extract chall11:latest
docker cp extract:/home/ctf/chall bin/chall11
docker cp --follow-link extract:/usr/arm-linux-gnueabihf/lib/libc.so.6 bin/libc.so.6
docker container rm -f extract

echo "Packaging dist.tgz"
$TMP=$(mktemp -d)
cp package/* bin/* $TMP/
tar czvf chall11-dist.tgz -C $TMP .
rm -rf $TMP
