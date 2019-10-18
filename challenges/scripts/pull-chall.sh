#!/bin/sh

CHALL=$1
REPO="928148438546.dkr.ecr.eu-west-1.amazonaws.com/pwny-racing/challenges/${CHALL}"

$(aws ecr get-login --no-include-email --region eu-west-1)
docker pull "${REPO}:latest"
docker tag "${REPO}:latest" "${CHALL}:latest" 
