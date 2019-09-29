#!/bin/sh

echo "Building challenge and container"
docker build -t chall2 .

echo "Extracting build artifacts"
mkdir -p bin
docker container create --name extract chall2:latest
docker cp extract:/home/ctf/chall bin/chall2
#docker cp --follow-link extract:/lib/i386-linux-gnu/libc.so.6 bin/libc.so.6
docker container rm -f extract

echo "Packaging dist.tgz"
mkdir tmp
cp package/* bin/* tmp/
tar czvf chall2-dist.tgz -C tmp .
rm -rf tmp
