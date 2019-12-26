#!/bin/sh

CHALL="chall23"

echo "Building challenge and container"
docker build -t ${CHALL} .

echo "Extracting build artifacts"
mkdir -p bin
docker container create --name extract ${CHALL}:latest
docker cp extract:/build/chall.hex bin/${CHALL}.hex
docker cp extract:/build/flash-flag.hex bin/${CHALL}-flash-flag.hex
docker container rm -f extract

echo "Packaging dist.tgz"
TMP=$(mktemp -d)
cp bin/${CHALL}.hex package/* $TMP/
tar czvf ${CHALL}-dist.tgz -C $TMP .
rm -rf $TMP
