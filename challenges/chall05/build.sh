#!/bin/sh

echo "Building challenge and container"
docker build -t chall5 .

echo "Extracting build artifacts"
mkdir -p bin
docker container create --name extract chall5:latest
docker cp extract:/home/ctf/chall bin/chall5
docker cp --follow-link extract:/lib/i386-linux-gnu/libc.so.6 bin/libc.so.6
docker container rm -f extract

echo "Packaging dist.tgz"
$TMP=$(mktemp -d)
cp package/* bin/* $TMP/
tar czvf chall5-dist.tgz -C $TMP .
rm -rf $TMP
