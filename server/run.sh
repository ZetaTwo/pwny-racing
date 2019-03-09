#!/bin/sh
docker run --rm -ti -p 9090:9090 --network pwnyracing --name pwnyracing-nodecg pwnyracing/nodecg
docker run --rm -ti -p 5080:80 -p 5443:443 --network pwnyracing --name pwnyracing-nginx pwnyracing/nginx
