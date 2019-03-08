#!/bin/sh

$(aws --profile pwnyracing ecr get-login --no-include-email --region eu-west-1)
docker tag pwnyracing/nodecg:latest 928148438546.dkr.ecr.eu-west-1.amazonaws.com/pwnyracing/nodecg:latest
docker push 928148438546.dkr.ecr.eu-west-1.amazonaws.com/pwnyracing/nodecg:latest
