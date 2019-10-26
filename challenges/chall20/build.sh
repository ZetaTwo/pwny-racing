#!/bin/sh

echo "Building challenge and container"
docker build -t chall20 .

echo "Extracting build artifacts"
mkdir -p bin
docker container create --name extract chall20:latest
docker cp extract:/home/ctf/chall bin/chall20
docker cp --follow-link extract:/lib/i386-linux-gnu/libc.so.6 bin/libc.so.6
docker container rm -f extract

echo "Packaging dist.tgz"
mkdir tmp
cp package/* bin/* tmp/
tar czvf chall20-dist.tgz -C tmp .
rm -rf tmp
