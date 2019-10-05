#!/bin/sh

echo "Building challenge and container"
docker build -t chall14 .

echo "Extracting build artifacts"
mkdir -p bin
docker container create --name extract chall14:latest
docker cp extract:/home/ctf/chall bin/chall14
#docker cp --follow-link extract:/lib/x86_64-linux-gnu/libc.so.6 bin/libc.so.6
docker container rm -f extract

echo "Packaging dist.tgz"
mkdir tmp
cp package/* bin/* tmp/
tar czvf chall14-dist.tgz -C tmp .
rm -rf tmp
