#!/bin/sh

echo "Building challenge and container"
docker build -t chall18 .

echo "Extracting build artifacts"
mkdir -p bin
docker container create --name extract chall18:latest
docker cp extract:/home/ctf/chall bin/chall18
#docker cp --follow-link extract:/lib/i386-linux-gnu/libc.so.6 bin/libc.so.6
docker container rm -f extract

echo "Packaging dist.tgz"
mkdir tmp
cp package/* tmp/
tar czvf chall18-dist.tgz -C tmp .
rm -rf tmp
