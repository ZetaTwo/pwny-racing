#!/bin/sh

CHALL="chall22"

echo "Building challenge and container"
docker build -t ${CHALL} .

echo "Extracting build artifacts"
mkdir -p bin
docker container create --name extract ${CHALL}:latest
docker cp extract:/home/ctf/chall bin/${CHALL}
#docker cp --follow-link extract:/lib/i386-linux-gnu/libc.so.6 bin/libc.so.6
docker container rm -f extract

echo "Packaging dist.tgz"
$TMP=$(mktemp -d)
cp package/* bin/* $TMP/
tar czvf ${CHALL}-dist.tgz -C $TMP .
rm -rf $TMP
